#include <pspctrl.h>
#include <pspctrl_kernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <pspmoduleinfo.h>
#include <pspmodulemgr.h>
#include <pspsdk.h>
#include <pspthreadman.h>
#include <stdio.h>
#include <string.h>

#include "cheat.h"
#include "cheat_file.h"
#include "common.h"
#include "game_pause.h"
#include "hooks.h"
#include "hotkeys.h"
#include "menu.h"
#include "menu_draw.h"
#include "plugin.h"
#include "screenshot.h"
#include "search.h"

//Defines
PSP_MODULE_INFO("cheatER", 0x3007, 1, 0);//0x3007
PSP_MAIN_THREAD_ATTR(0);                 //0 for kernel mode too

//Globals
unsigned char running = 0;
SceUID thid;

//Functions
int module_start(SceSize args, void *argp) __attribute__((alias("_start")));

int module_stop(SceSize args, void *argp) __attribute__((alias("_stop")));

void restart_plugin(const char *plugin_path) {
    SceUID modid;
    const char* plugin_name = "cheatER";
    modid = (SceUID) sceKernelFindModuleByName(plugin_name);

    int stop_result = sceKernelStopModule(modid, 0, NULL, NULL, NULL);
    if (stop_result < 0) {
        snprintf(buffer, sizeof(buffer), "sceKernelStopModule failed: 0x%08X\n", stop_result);
    }

    int unload_result = sceKernelUnloadModule(modid);
    if (unload_result < 0) {
        snprintf(buffer, sizeof(buffer), "sceKernelUnloadModule failed: 0x%08X\n", unload_result);
    }

    modid = sceKernelLoadModule(plugin_path, 0, NULL);
    if (modid < 0) {
        snprintf(buffer, sizeof(buffer), "sceKernelLoadModule failed: 0x%08X\n", modid);
    }

    int start_result = sceKernelStartModule(modid, 0, NULL, NULL, NULL);
    if (start_result < 0) {
        snprintf(buffer, sizeof(buffer), "sceKernelStartModule failed: 0x%08X\n", start_result);
    }
}

// Sleep up to `us`, but wake early when the menu or a screenshot is requested
static void main_loop_wait(unsigned int us) {
    while (us > 0 && !menu_drawn && !screen_time) {
        unsigned int step = us < 10000 ? us : 10000;
        sceKernelDelayThread(step);
        us -= step;
    }
}

int main_thread() {
    signed int fd;
    running = 1;
    unsigned int counter = 0;

    search_history_reset();

    //Wait for the kernel to boot
    sceKernelDelayThread(100000);
    while (!sceKernelFindModuleByName("sceKernelLibrary"))
        sceKernelDelayThread(100000);
    sceKernelDelayThread(100000);

    //Find which screen directory to use
    screenshot_path_init();

    //Patch the OpenPSID service
    psid_patch_apply();

    //Find the GAME ID
    do {
        fd = sceIoOpen("disc0:/UMD_DATA.BIN", PSP_O_RDONLY, 0777);
        sceKernelDelayThread(1000);
    } while (fd <= 0);
    sceIoRead(fd, game_id, 10);
    sceIoClose(fd);
    memcpy(&game_dir[23], game_id, 10);

    //Load the cheats
    cheat_load();

    //Set the VRAM to null, use the current screen
    pspDebugScreenInitEx((void *) 0x44000000, 0, 0);
    vram = NULL;

    //Setup the controller
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    //Register the button callbacks
    sceCtrlRegisterButtonCallback(3, trigger_key | menu_key | screen_key, button_callback, NULL);

    //Do the loop-de-loop
    while (running) {
        if (vram == NULL) {
            //Has the HOME button been pressed?
            unsigned int address = 0;
            unsigned int buffer_width = 0;
            unsigned int pixel_format = 0;
            unsigned int sync;

            sceDisplayGetFrameBufferInternal(0, (void **) &address, &buffer_width, &pixel_format, &sync);

            if (address) {
                vram = (void *) (0xA0000000 | address);
            } else {
                sceDisplayGetMode(&pixel_format, &buffer_width, &buffer_width);
                pspDebugScreenSetColorMode(pixel_format);
                pspDebugScreenSetXY(0, 0);
                pspDebugScreenSetTextColor(0xFFFFFFFF);
                pspDebugScreenPuts("cheatER: Double tap the home button and press vol + & vol - to open menu\n");
            }

            sceKernelDelayThread(1500);
            continue;
        }

        //Handle menu
        if (menu_drawn) {
            //Stop the game from receiving input (USER mode input block)
            sceCtrlSetButtonMasks(0xFFFF, 1); // Mask lower 16bits
            sceCtrlSetButtonMasks(0x10000, 2);// Always return HOME key

            //Setup a custom VRAM
            sceDisplaySetFrameBufferInternal(0, vram, 512, 0, 1);
            pspDebugScreenInitEx(vram, 0, 0);

            //Draw menu
            if (cheat_pause) game_pause(thid);
            menu_back_buffer_open();
            menu_input();
            menu_back_buffer_close();
            game_resume(thid);

            //Return the standard VRAM
            sceDisplaySetFrameBufferInternal(0, 0, 512, 0, 1);

            //Allow the game to receive input
            sceCtrlSetButtonMasks(0x10000, 0);// Unset HOME key
            sceCtrlSetButtonMasks(0xFFFF, 0); // Unset mask
        } else if ((cheat_hz != 0) || (cheat_flash)) {
            //Apply cheats
            cheat_apply(-1);
            if (cheat_flash > 0) cheat_flash--;
        }

        //Handle screenshot
        screenshot_handle_request();

        //Wait a certain amount of seconds before reapplying cheats again
        main_loop_wait(!cheat_hz ? 500000 : cheat_hz);
    }
    return 0;
}

int _start(SceSize args, void *argp) {
    //Load the CFG
    mac_hook_install();
    memcpy(&trigger_key, cfg + 11, 4);
    memcpy(&menu_key, cfg + 15, 4);
    memcpy(&screen_key, cfg + 20, 4);

    //Create thread
    game_threads_snapshot();
    thid = sceKernelCreateThread("cheatERThread", &main_thread, 0x18, 0x1000, 0, NULL);

    //Start thread
    if (thid >= 0) sceKernelStartThread(thid, 0, NULL);

    return 0;
}

int _stop(SceSize args, void *argp) {
    running = 0;
    sceKernelTerminateThread(thid);
    return 0;
}
