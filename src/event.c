#include "system.h"
#include "app.h"

#if defined(__clang__)
#define INTERRUPT /*helper for VSCode parser*/
#elif __XC8_VERSION < 2000
#define INTERRUPT interrupt
#else
#define INTERRUPT __interrupt()
#endif

/**
 * High-priority interrupt handler.
 * 
 * @see https://microchipdeveloper.com/faq:31
 */
void INTERRUPT
on_interrupt(void) {
    USBDeviceTasks();

    // timer1 interrupt
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;
        TMR1H = 0x00;
        TMR1L = 0x00;
#ifdef DEBUG_LED
        static int count;

        if (count++ > 500) {
            count = 0;
            app_blink();
        }
#endif
    }
}

/**
 * USB event handler.
 * 
 * Notes:
 * - Runs in interrupt context.
 * - Function name MUST be kept as-is, as this is called from the MLA framework.
 *
 * @param event - the type of event
 * @param pdata - pointer to the event data
 * @param size - size of the event data
 */
bool
USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, uint16_t size) {
    switch((int)event) {
    case EVENT_TRANSFER:
        break;

    case EVENT_SOF:
#ifdef DEBUG_LED
        app_blink();
#endif
        break;

    case EVENT_SUSPEND:
        //Call the hardware platform specific handler for suspend events for
        //possible further action (like optionally going reconfiguring the application
        //for lower power states and going to sleep during the suspend event).  This
        //would normally be done in USB compliant bus powered applications, although
        //no further processing is needed for purely self powered applications that
        //don't consume power from the host.
        sys_init(SYSTEM_STATE_USB_SUSPEND);
        break;

    case EVENT_RESUME:
        //Call the hardware platform specific resume from suspend handler (ex: to
        //restore I/O pins to higher power states if they were changed during the 
        //preceding sys_init(SYSTEM_STATE_USB_SUSPEND) call at the start
        //of the suspend condition.
        sys_init(SYSTEM_STATE_USB_RESUME);
        break;

    case EVENT_CONFIGURED:
        // init app once USB initialization is complete
        app_init();
        break;

    case EVENT_SET_DESCRIPTOR:
        break;

    case EVENT_EP0_REQUEST:
        /* We have received a non-standard USB request.  The HID driver
            * needs to check to see if the request was for it. */
        USBCheckHIDRequest();
        break;

    case EVENT_BUS_ERROR:
        break;

    case EVENT_TRANSFER_TERMINATED:
        break;

    default:
        break;
    }
    return true;
}
