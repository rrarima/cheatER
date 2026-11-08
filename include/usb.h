#ifndef CHEATER_USB_H
#define CHEATER_USB_H

/*
 * USB mass storage mode.
 */

int load_start_module(char *path);
int start_usb(void);
void stop_usb(void);

#endif // CHEATER_USB_H
