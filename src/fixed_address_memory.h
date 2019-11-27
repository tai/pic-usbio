#ifndef FIXED_ADDRESS_MEMORY_H
#define FIXED_ADDRESS_MEMORY_H

//
// NOTE:
// DO NOT CHANGE THE FILENAME.
// This header is included by the MLA framework, namely framework/usb/inc/usb_hal_pic16f1.h
//

#if __XC8_VERSION < 2000
#define HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS @0x2050
#define HID_CUSTOM_IN_DATA_BUFFER_ADDRESS @0x20A0

#else
#define HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS __at(0x2050)
#define HID_CUSTOM_IN_DATA_BUFFER_ADDRESS __at(0x20A0)

#endif

#endif