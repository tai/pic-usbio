#ifndef SYSTEM_H
#define SYSTEM_H

#include "xc.h"
#include "usb.h"
#include "usb_device_hid.h"

// helper for VSCode parser
#if defined(__clang__)
#include "pic16f1459.h"
#include "usb_hal_pic16f1.h"
#endif

typedef enum {
    SYSTEM_STATE_USB_START,
    SYSTEM_STATE_USB_SUSPEND,
    SYSTEM_STATE_USB_RESUME
} SYSTEM_STATE;

extern void sys_init(SYSTEM_STATE state);

#endif
