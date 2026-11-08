#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <string.h>

#include "common.h"
#include "hooks.h"
#include "module.h"


//Mac Address hooking module
unsigned char cfg[] = {'C', 'F', 'G', 0x88, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80, 0x00, 0x00,
                       0x00, 0x30, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00};

unsigned int hook_mac(unsigned char *mac) {
    memcpy(mac, cfg + 5, 6);
    return 0;
}

Hook hook_a[1] =
        {
                {{0, NULL}, "sceWlan_Driver", "sceWlanDrv", 0x0c622081, hook_mac},
};

void mac_hook_install(void) {
    if (cfg[4]) {
        SceModule *mod;
        while (1) {
            mod = sceKernelFindModuleByName(hook_a[0].modname);
            if (mod == NULL) {
                sceKernelDelayThread(100);
                continue;
            }
            break;
        }
        module_hook_func(&hook_a[0].modfunc, sceKernelSearchModuleByName(hook_a[0].modname), hook_a[0].libname, hook_a[0].nid,
                       hook_a[0].func);
    }
}

Hook hook_b[1] =
        {
                {{0, NULL}, "sceOpenPSID_Service", "sceOpenPSID_driver", 0xc69bebce, NULL},
};

char psid[16];

int sceOpenPSIDGetOpenPSID(char *openpsid) {
    memcpy(openpsid, psid, 16);
    return 0;
}

static unsigned char patch_a[] = {0x21, 0x88, 0x02, 0x3c,//lui v0, $8822
                                 0x21, 0x10, 0x42, 0x34,//ori v0, v0, $0008
                                 0x08, 0x00, 0x40, 0x00,//jr v0
                                 0x00, 0x00, 0x00, 0x00};

void psid_patch_apply(void) {
    signed int fd;

    //Generate the patch
    *((unsigned short *) (&patch_a[0])) = (unsigned short) (((unsigned int) sceOpenPSIDGetOpenPSID) >> 16);
    *((unsigned short *) (&patch_a[4])) = (unsigned short) ((unsigned int) &sceOpenPSIDGetOpenPSID);

    //Find the function we want!
    while (1) {
        SceModule *mod = sceKernelFindModuleByName(hook_b[0].modname);
        if (mod == NULL) {
            sceKernelDelayThread(100);
            continue;
        }
        break;
    }
    unsigned int hook_address = (unsigned int) module_find_func(
            module_find_library(sceKernelSearchModuleByName(hook_b[0].modname), hook_b[0].libname), hook_b[0].nid);
    hook_address = *((unsigned int *) hook_address);

    //Open the PSID file
    fd = sceIoOpen("ms0:/seplugins/cheatER/cheatERimportant.bin", PSP_O_RDONLY, 0777);
    if (fd > 0) {
        sceIoRead(fd, psid, 16);
        sceIoClose(fd);
    } else {
        fd = sceIoOpen("ms0:/seplugins/cheatER/cheatERimportant.bin", PSP_O_WRONLY | PSP_O_CREAT, 0777);
        if (fd > 0) {
            ((int (*)(char *)) hook_address)(psid);
            sceIoWrite(fd, psid, 16);
            sceIoClose(fd);
        } else {
            return;//Skip the evil patch
        }
    }

    //Apply the memcpy
    sceKernelDcacheWritebackAll();
    sceKernelIcacheInvalidateAll();

    //Apply the patch
    memcpy((void *) hook_address, patch_a, 16);

    //Apply the memcpy
    sceKernelDcacheWritebackAll();
    sceKernelIcacheInvalidateAll();
}
