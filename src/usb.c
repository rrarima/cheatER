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
    else
        start_result =
                sceKernelStartModule(load_result, 0, NULL, &status, NULL);

    if (load_result != start_result)
        return -2;

    return 0;
}

void start_usb(void) {
    load_start_module("flash0:/kd/semawm.prx");
    load_start_module("flash0:/kd/usbstor.prx");
    load_start_module("flash0:/kd/usbstormgr.prx");
    load_start_module("flash0:/kd/usbstorms.prx");
    load_start_module("flash0:/kd/usbstorboot.prx");
    sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
    sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
    sceUsbstorBootSetCapacity(0x800000);
    sceUsbActivate(0x1c8);
}

void stop_usb(void) {
    sceUsbDeactivate(0x1c8);
    sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);
    sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);
    sceIoDevctl("fatms0:", 0x0240D81E, NULL, 0, NULL, 0);
}
