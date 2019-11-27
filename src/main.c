#include "system.h"
#include "app.h"

void
main(void) {
    sys_init(SYSTEM_STATE_USB_START);

    USBDeviceInit();
    USBDeviceAttach();

    for (;;) {
        app_run();
    }
}
