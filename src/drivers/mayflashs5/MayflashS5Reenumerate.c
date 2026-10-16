/*
 * Force a Pico-PIO-USB host-port re-enumeration.
 *
 * After the unlock reply the Mayflash S5 goes quiet (~810 ms) and switches into
 * its signing persona, but the host never notices on its own. Mirror the
 * disconnect path Pico-PIO-USB uses internally (connected=false, suspended,
 * raise the DISCONNECT interrupt) so tuh_task() re-enumerates on the next poll.
 * This is a soft re-enumeration: no SE0 is driven on the bus.
 *
 * Ported from Pelsin/RP2040-DualSense-Mayflash-S5 (src/pio_usb_reenumerate.c).
 */
#include "pio_usb_ll.h"

void mayflash_s5_force_reenumerate(uint8_t root_idx) {
    root_port_t *root = PIO_USB_ROOT_PORT(root_idx);
    root->connected = false;
    root->suspended = true;
    root->ints |= PIO_USB_INTS_DISCONNECT_BITS;
}
