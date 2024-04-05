#ifndef USB_MESSAGES_H__
#define USB_MESSAGES_H__
#include <stdint.h>
#include <string.h>

enum usb_hid_message_type {
    USB_HID_MESSAGE_TYPE_KEYBOARD = 0,
    USB_HID_MESSAGE_TYPE_RAW,
};

struct usb_hid_message {
    enum usb_hid_message_type type;
    uint8_t data[32];
    size_t count;
};

enum usb_msc_message_type {
    USB_MSC_MESSAGE_TYPE_READ = 0,
    USB_MSC_MESSAGE_TYPE_WRITE,
    USB_MSC_MESSAGE_TYPE_CREATE,
    USB_MSC_MESSAGE_TYPE_DELETE,
};

struct usb_msc_message {
    enum usb_msc_message_type type;
    uint8_t data[64];
    size_t count;
};

#endif