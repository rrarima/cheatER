#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <pspmodulemgr.h>
#include <pspusb.h>
#include <pspusbstor.h>

#include "usb.h"


int load_start_module(char *path) {
    u32 load_result;
    u32 start_result;
    int status;

    load_result = sceKernelLoadModule(path, 0, NULL);
    if (load_result & 0x80000000)
        return -1;

    start_result = sceKernelStartModule(load_result, 0, NULL, &status, NULL);
    if (load_result != start_result)
        return -2;

    return 0;
}

static int note_failure(int first_failed, int step, int result) {
    if (first_failed == 0 && result < 0) {
        return step;
    }
    return first_failed;
}

int start_usb(void) {
    int failed = 0;

    failed = note_failure(failed , 1 ,load_start_module("flash0:/kd/semawm.prx"));
    failed = note_failure(failed , 2 ,load_start_module("flash0:/kd/usbstor.prx"));
    failed = note_failure(failed , 3 ,load_start_module("flash0:/kd/usbstormgr.prx"));
    failed = note_failure(failed , 4 ,load_start_module("flash0:/kd/usbstorms.prx"));
    failed = note_failure(failed , 5 ,load_start_module("flash0:/kd/usbstorboot.prx"));
    failed = note_failure(failed , 6 , sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0));
    failed = note_failure(failed , 7 ,sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0));
    failed = note_failure(failed , 8 ,sceUsbstorBootSetCapacity(0x800000));
    failed = note_failure(failed , 9 ,sceUsbActivate(0x1c8));

    return failed;
}

void stop_usb(void) {
    sceUsbDeactivate(0x1c8);
    sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);
    sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);
    sceIoDevctl("fatms0:", 0x0240D81E, NULL, 0, NULL, 0);
}
