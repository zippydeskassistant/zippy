#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/buf.h>

LOG_MODULE_REGISTER(zippy_raw_hid_parse);

extern struct k_fifo raw_hid_fifo;

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
    uint8_t report_id;
    uint16_t seq_id;
    uint8_t data[29];
    size_t data_len;
};

static int parse_raw_hid_packet(struct net_buf *nb) {

    uint8_t buffer[32];
    size_t packet_length = net_buf_frags_len(nb);
    memcpy(&buffer, net_buf_pull_mem(nb, packet_length), packet_length);

    struct raw_hid_packet packet = {0};
    packet.report_id = buffer[0];
    packet.seq_id = (buffer[1] << 8) | buffer[2];
    memcpy(&packet.data, &buffer[RAW_HID_HEADER_SIZE], packet_length - RAW_HID_HEADER_SIZE);
    packet.data_len = packet_length - RAW_HID_HEADER_SIZE;

    switch(packet.report_id) {
        case REPORT_ID_TIME:
        case REPORT_ID_AUDIO_TITLE:
        case REPORT_ID_AUDIO_ARTIST:
        case REPORT_ID_AUDIO_ALBUM:
        case REPORT_ID_AUDIO_TIME:
        case REPORT_ID_AUDIO_PROCESS:
        case REPORT_ID_MACRO:
        case REPORT_ID_SET_IMAGE_PATH:
        case REPORT_ID_GET_IMAGE_PATH:
        case REPORT_ID_IMAGE_DATA:
        case REPORT_ID_IMAGE_CRC_REQUEST:
        case REPORT_ID_IMAGE_CONFIRM:
            LOG_INF("Received raw HID packet with report ID %d and seq ID %d", packet.report_id, packet.seq_id);
            LOG_HEXDUMP_INF(packet.data, packet.data_len, "Data");
            break;
        default:
            LOG_ERR("Received raw HID packet with unknown report ID %d", packet.report_id);
            return -EINVAL;
    }

    return 0;
}

static void raw_hid_thread(void *p1, void *p2, void *p3) {
    int err;

	while (1) {
		struct net_buf *nb = net_buf_get(&raw_hid_fifo, K_FOREVER);

        err = parse_raw_hid_packet(nb);
        if (err) {
            LOG_ERR("Failed to parse raw HID packet: %d", err);
        }

		net_buf_reset(nb);
		net_buf_unref(nb);
	}
}

K_THREAD_DEFINE(raw_hid_thread_id, 1024, raw_hid_thread, NULL, NULL, NULL, 5, 0, 0);