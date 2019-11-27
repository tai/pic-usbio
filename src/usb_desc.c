//
// # PIC MLA USB framework overview
//
// - App MUST provide usb_config.h for USB stack configuration
//   - Framework headers (usb_*.h) includes this usb_config.h
//   - App MUST compile and link usb_device.c and usb_device_*.c
// - App MUST provide USER_USB_CALLBACK_EVENT_HANDLER() for USB event handling
// - App MUST provide USB descriptors in predefined symbols names
//   - Check "extern" and "extern const" symbols in framework
//   - Use helper macros in usb.h, usb_device.h, usb_device_*.h, and usb_ch9.h
//

#include "usb.h"
#include "usb_device_hid.h"

/**
 * Device Descriptor
 * NOTE:
 * - This varname is required by framework/usb/src/usb_device.c
 * - Can be overridden with USB_USER_DEVICE_DESCRIPTOR in usb_config.h
 */
const USB_DEVICE_DESCRIPTOR device_dsc = {
    0x12,                   // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,  // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0x00,                   // Class Code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0, see usb_config.h
    0x0BFE,                 // Vendor ID: Morphy Planning
    0x1000,                 // Product ID: Morphy USB-IO
    0x0100,                 // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x03,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

/**
 * Configuration 1 Descriptor
 * NOTE: This varname is required by framework/usb/src/usb_device_*.c
 */
const uint8_t configDescriptor1[] = {
    // Configuration Descriptor
    // See USB_CONFIGURATION_DESCRIPTOR in usb_ch9.h
    0x09,                           // bLength
    USB_DESCRIPTOR_CONFIGURATION,   // bDescriptorType
    0x29,0x00,                      // wTotalLength
    1,                              // bNumInterfaces
    1,                              // bConfgurationValue
    0,                              // iConfiguration (string index)
    _DEFAULT,                       // bmAttributes (_DEFAULT, _SELF, _RWU, ...)
    50,                             // bMaxPower (2X mA)
							
    // Interface Descriptor
    // See USB_INTERFACE_DESCRIPTOR in usb_ch9.h
    0x09,                           // bLength
    USB_DESCRIPTOR_INTERFACE,       // bDescriptorType
    0,                              // bInterfaceNumber
    0,                              // bAlternateSetting
    2,                              // bNumEndpoints
    HID_INTF,                       // bInterfaceClass
    0,                              // bInterfaceSubClass
    0,                              // bInterfaceProtocol
    0,                              // iInterface (string index)

    // HID Class-Specific Descriptor
    // See USB_HID_DSC + USB_HID_DSC_HEADER in usb_device_hid.h
    0x09,                           // bLength (= USB_HID_DSC + USB_HID_DSC_HEADER)
    DSC_HID,                        // bDescriptorType
    0x11,0x01,                      // bcdHID (HID spec 1.11)
    0x00,                           // bCountryCode (0x00=NotSupported)
    HID_NUM_OF_DSC,                 // bNumDsc (App-defined in usb_config.h)
    DSC_RPT,                        // bDescriptorType
    HID_RPT01_SIZE,0x00,            // wDscLength
    
    // Endpoint Descriptor
    // See USB_ENDPOINT_DESCRIPTOR in usb_ch9.h
    0x07,                           // bLength
    USB_DESCRIPTOR_ENDPOINT,        // bDescriptorType
    CUSTOM_DEVICE_HID_EP|_EP_IN,    // bEndpointAddress
    _INTERRUPT,                     // bmAttributes (_INTERRUPT, _BULK, _ISO, _NS, ...)
    0x08,0x00,                      // bMaxPacketSize
    0x01,                           // bInterval (interval frame count)

    // Endpoint Descriptor
    0x07,                           // bLength
    USB_DESCRIPTOR_ENDPOINT,        // bDescriptorType
    CUSTOM_DEVICE_HID_EP|_EP_OUT,   // bEndpointAddress
    _INTERRUPT,                     // bmAttributes (_INTERRUPT, _BULK, _ISO, _NS, ...)
    0x08,0x00,                      // bMaxPacketSize
    0x01                            // bInterval (interval frame count)
};

/**
 * Class specific descriptor - HID
 * NOTE: This varname is required by the MLA framework, namely framework/usb/src/usb_device_hid.c
 */
const struct{uint8_t report[HID_RPT01_SIZE];} hid_rpt01 = {{
    //
    // Copied from: usbhid-dump -m 0bfe:1003
    //
    0x06, 0xA0, 0xFF, // USAGE_PAGE (0xFFA0)
    0x09, 0x01, // USAGE(1)
    0xA1, 0x01, // COLLECTION
    0x09, 0x02, // USAGE(2)
    0xA1, 0x00, // COLLECTION

    0x06,       // USAGE_PAGE
    0xA1, 0xFF,

    0x09, 0x03, // USAGE(3)
    0x09, 0x04, // USAGE(4)
    0x15, 0x80,
    0x25, 0x7F,
    0x35, 0x00, 0x45, 0xFF,
    0x75, 0x08, // REPORT_SIZE
    0x95, 0x08,
    0x81, 0x02, // INPUT

    0x09, 0x05, // USAGE(5)
    0x09, 0x06, // USAGE(6)
    0x15, 0x80, // LOGICAL_MIN
    0x25, 0x7F,
    0x35, 0x00, 0x45, 0xFF,
    0x75, 0x08, // REPORT_SIZE
    0x95, 0x08,
    0x91, 0x08, // OUTPUT

    0xC0, 0xC0

#if 0
    0x06, 0x00, 0xff, // USAGE_PAGE (0xFF00 = vendor defined)
    0x09, 0x01, // USAGE (vendor usage #1)
    0xa1, 0x01, // Collection(Application)
    0x15, 0x00, //   LOGICAL_MIN (0x00)
    0x26,       //   LOGICAL_MAX (0x00FF)
    0xFF, 0x00,
    0x75, 0x08, //   REPORT_SIZE (8bits)

    // OUT Message 0x01
    0x85, 0x01, //   REPORT ID(0x01)
    0x98, 0x01, //   REPORT COUNT (# of REPORT_SIZE fields)
    0x19, 0x01, //   USAGE_MIN
    0x19, 0x40, //   USAGE_MAX
    0x91, 0x02, //   OUTPUT (data, array, absolute)

    // IN Message 0x01
    0x85, 0x01, //   REPORT ID(0x01)
    0x98, 0x01, //   REPORT COUNT (# of REPORT_SIZE fields)
    0x19, 0x01, //   USAGE_MIN
    0x19, 0x40, //   USAGE_MAX
    0x81, 0x02, //   INPUT (data, array, absolute)

    0xc0        // END
#endif
}};

// Language code string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[1];} sd000 = {
sizeof(sd000),USB_DESCRIPTOR_STRING,{
    0x0409
}};

// Manufacturer string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[15];} sd001 = {
sizeof(sd001),USB_DESCRIPTOR_STRING,{
    'M','o','r','p','h','y',' ','P','l','a','n','n','i','n','g'
}};

// Product string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[12];} sd002 = {
sizeof(sd002),USB_DESCRIPTOR_STRING,{
    'U','S','B','-','I','O',' ','C','l','o','n','e'
}};

// Serial string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[5];} sd003 = {
sizeof(sd003),USB_DESCRIPTOR_STRING,{
    'h','u','b','0','0'
}};

/**
 * Array of configuration descriptors
 * NOTE:
 * - This varname is required by the MLA framework, namely framework/usb/src/usb_device.c
 * - Can be overridden with USB_USER_CONFIG_DESCRIPTOR in usb_config.h
 */
const uint8_t *const USB_CD_Ptr[] = {
    (const uint8_t *const)configDescriptor1
};

/**
 * Array of string descriptors
 * NOTE: This varname is required by the MLA framework, namely framework/usb/src/usb_device.c
 */
const uint8_t *const USB_SD_Ptr[] = {
    (const uint8_t *const)&sd000,
    (const uint8_t *const)&sd001,
    (const uint8_t *const)&sd002,
    (const uint8_t *const)&sd003
};
