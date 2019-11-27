#include "system.h"

#include <string.h>

typedef enum {
    COMMAND_WRITE_PORT0 = 0x01,
    COMMAND_WRITE_PORT1 = 0x02,
    COMMAND_READ_PORT0 = 0x03,
    COMMAND_READ_PORT1 = 0x04,
    COMMAND_TOGGLE_LED = 0x80,
    COMMAND_GET_BUTTON_STATUS = 0x81,
    COMMAND_READ_POTENTIOMETER = 0x37
} CUSTOM_HID_DEMO_COMMANDS;

// USB-RAM buffers with fixed-address allocation for some chips
#if defined(__XC8)
unsigned char ReceivedDataBuffer[64] HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS;
unsigned char ToSendDataBuffer[64] HID_CUSTOM_IN_DATA_BUFFER_ADDRESS;
#else
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];
#endif

volatile USB_HANDLE USBOutHandle;    
volatile USB_HANDLE USBInHandle;

/**
 *
 */
void
app_init(void) {
    // initialize the variable holding the handle for the last transmission
    USBInHandle = 0;

    // enable the HID endpoint
    USBEnableEndpoint(CUSTOM_DEVICE_HID_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    // Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&ReceivedDataBuffer[0],8);
}

/**
 * Run app-specific tasks
 */
void
app_run(void) {
#ifdef DEBUG_UART
    static uint8_t c = 'a';

    if (TXSTAbits.TRMT) {
        if (c > 'z') c = 'a';
        TXREG = c++;
    }
#endif

    /* If the USB device isn't configured yet, we can't really do anything
     * else since we don't have a host to talk to.  So jump back to the
     * top of the while loop. */
    if (USBGetDeviceState() < CONFIGURED_STATE) {
        return;
    }

    /* If we are currently suspended, then we need to see if we need to
     * issue a remote wakeup.  In either case, we shouldn't process any
     * keyboard commands since we aren't currently communicating to the host
     * thus just continue back to the start of the while loop. */
    if (USBIsDeviceSuspended() == true) {
        return;
    }

    // Check if we got request from HID.write()
    if (HIDRxHandleBusy(USBOutHandle) == false) {
        //We just received a packet of data from the USB host.
        //Check the first uint8_t of the packet to see what command the host
        //application software wants us to fulfill.
        switch (ReceivedDataBuffer[0])				//Look at the data the host sent, to see what kind of application specific command it sent.
        {
            case COMMAND_WRITE_PORT0:
                // write to RC0..7
                PORTC = ReceivedDataBuffer[1];
                break;
            case COMMAND_WRITE_PORT1:
                // write to RB6, RB4, RA5, RA4
		PORTBbits.RB6 = 1 & (ReceivedDataBuffer[1] >> 3);
		PORTBbits.RB4 = 1 & (ReceivedDataBuffer[1] >> 2);
		PORTAbits.RA5 = 1 & (ReceivedDataBuffer[1] >> 1);
		PORTAbits.RA4 = 1 & (ReceivedDataBuffer[1]);
                break;
            case COMMAND_READ_PORT0:
                if (! HIDTxHandleBusy(USBInHandle)) {
                    // reset buffer in compatible manner
                    memcpy(ToSendDataBuffer, ReceivedDataBuffer, 8);
                    
                    // read from RC0..7
                    ToSendDataBuffer[1] = LATC;
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t *)ToSendDataBuffer, 8);
                }
                break;
            case COMMAND_READ_PORT1:
                if (! HIDTxHandleBusy(USBInHandle)) {
                    // reset buffer in compatible manner
                    memcpy(ToSendDataBuffer, ReceivedDataBuffer, 8);

                    // read from RB6, RB4, RA5, RA4
                    ToSendDataBuffer[1] = (
		        (0xF0) |
		        (LATBbits.LATB6 << 3) |
		        (LATBbits.LATB4 << 2) |
		        (LATAbits.LATA5 << 1) |
			(LATAbits.LATA4)
		    );
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t *)ToSendDataBuffer, 8);
                }
                break;
        }
        //Re-arm the OUT endpoint, so we can receive the next OUT data packet 
        //that the host may try to send us.
        USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ReceivedDataBuffer[0], 8);
    }

#if 0 /* FIXME */
    // Allow dummy read to avoid HID.read() hangup without prior HID.write().
    if (! HIDTxHandleBusy(USBInHandle)) {
        memset(ToSendDataBuffer, 0, 8);
        USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t *)ToSendDataBuffer, 8);
    }
#endif
}

void
app_blink(void) {
    static uint8_t led;
    led ^= 1;
    PORTBbits.RB6 = led;
}
