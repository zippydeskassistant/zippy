#ifndef RAW_HID_H__
#define RAW_HID_H__

#include <stdint.h>
#include <string.h>

enum report_id {
    REPORT_ID_TIME = 0x01,
    REPORT_ID_AUDIO_TITLE = 0x02,
    REPORT_ID_AUDIO_ARTIST = 0x03,
    REPORT_ID_AUDIO_ALBUM = 0x04,
    REPORT_ID_AUDIO_TIME = 0x05,
    REPORT_ID_AUDIO_PROCESS = 0x06,
    REPORT_ID_MACRO = 0x07,
    REPORT_ID_SET_IMAGE_PATH = 0x08,
    REPORT_ID_GET_IMAGE_PATH = 0x09,
    REPORT_ID_IMAGE_DATA = 0x0A,
    REPORT_ID_IMAGE_CRC_REQUEST = 0x0B,
    REPORT_ID_IMAGE_CONFIRM = 0x0C,
};

#define RAW_HID_HEADER_SIZE 3

/* A seq_id of 0 means that data transmission has completed */
struct raw_hid_packet {
    uint32_t report_id;
    uint32_t seq_id;
    uint8_t data[32];
    size_t data_len;
};

int raw_hid_send_packet(struct raw_hid_packet *packet);

#endif