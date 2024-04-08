#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_hid.h>
#include <zephyr/net/buf.h>
#include <zephyr/zbus/zbus.h>

#include "usb/raw_hid.h"

LOG_MODULE_REGISTER(zippy_raw_hid);

#define HID_USAGE_GEN_ZIPPY 0xFF
#define HID_USAGE_GEN_ZIPPY_PAGE 0x69

static const uint8_t __aligned(32) raw_report_desc[] = {
	HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),			
	HID_USAGE(HID_USAGE_GEN_DESKTOP_UNDEFINED),	
	HID_COLLECTION(HID_COLLECTION_APPLICATION),	
		HID_USAGE_PAGE(0x6A),
		HID_USAGE_MIN8(0x00),
		HID_USAGE_MAX16(0xFF, 0xFF),
		HID_REPORT_SIZE(8),
		HID_REPORT_COUNT(32),
		HID_INPUT(0x02),
		HID_USAGE_PAGE(0x6B),
		HID_USAGE_MIN8(0x00),
		HID_USAGE_MAX16(0xFF, 0xFF),
		HID_REPORT_SIZE(8),
		HID_REPORT_COUNT(32),
		HID_OUTPUT(0x02),
	HID_END_COLLECTION,
};

static uint32_t raw_dev_duration = 0;

static void raw_iface_ready(const struct device *dev, const bool ready)
{
	LOG_INF("HID device %p interface is %s", dev, ready ? "ready" : "not ready");
}

static int raw_get_report(const struct device *dev,
			 const uint8_t type, const uint8_t id, const uint16_t len,
			 uint8_t *const buf)
{
	LOG_INF("Get Report %u %u %u", type, id, len);
	LOG_HEXDUMP_INF(buf, len, "Buffer");
	return 0;
}

static int raw_set_report(const struct device *dev,
			 const uint8_t type, const uint8_t id, const uint16_t len,
			 const uint8_t *const buf)
{
	LOG_INF("Set Report %u %u %u", type, id, len);
	LOG_HEXDUMP_INF(buf, len, "Buffer");
	return 0;
}

static void raw_set_idle(const struct device *dev,
			const uint8_t id, const uint32_t duration)
{
	LOG_INF("Set Idle %u to %u", id, duration);
	raw_dev_duration = duration;
}

static uint32_t raw_get_idle(const struct device *dev, const uint8_t id)
{
	LOG_INF("Get Idle to %u", id);
    return raw_dev_duration;
}

static void raw_set_protocol(const struct device *dev, const uint8_t proto)
{
	LOG_INF("Protocol changed to %s",
		proto == 0U ? "Boot Protocol" : "Report Protocol");
}

#define HID_BUF_COUNT 16 
#define HID_BUF_SIZE 32
#define HID_BUF_USER_DATA_SIZE 4

NET_BUF_POOL_DEFINE(raw_hid_pool, HID_BUF_COUNT, HID_BUF_SIZE, HID_BUF_USER_DATA_SIZE, NULL);

K_FIFO_DEFINE(raw_hid_fifo);

static void raw_output_report(const struct device *dev, const uint16_t len,
			     const uint8_t *const buf)
{
	struct net_buf *nb = net_buf_alloc(&raw_hid_pool, K_FOREVER);
	net_buf_add_mem(nb, buf, len);

	net_buf_put(&raw_hid_fifo, nb);
}

struct hid_device_ops raw_ops = {
	.iface_ready = raw_iface_ready,
	.get_report = raw_get_report,
	.set_report = raw_set_report,
	.set_idle = raw_set_idle,
	.get_idle = raw_get_idle,
	.set_protocol = raw_set_protocol,
	.output_report = raw_output_report,
};

static int hid_init_raw(void) {
    int err;

	const struct device *raw_dev;
	raw_dev = DEVICE_DT_GET(DT_NODELABEL(hid_raw));
	if (!device_is_ready(raw_dev)) {
		LOG_ERR("HID Device is not ready");
		return -EIO;
	}

	err = hid_device_register(raw_dev,
				  raw_report_desc, sizeof(raw_report_desc),
				  &raw_ops);
    if (err) {
        LOG_ERR("Failed to register HID device: %d", err);
        return err;
    }

    return 0;
}

SYS_INIT(hid_init_raw, APPLICATION, CONFIG_ZIPPY_USBD_HID_RAW_INIT_PRIORITY);

int raw_hid_send_packet(struct raw_hid_packet *packet) {
	int err;

	uint8_t buffer[32];
	buffer[0] = packet->report_id;
	buffer[1] = packet->seq_id & 0xFF;
	buffer[2] = (packet->seq_id >> 8) & 0xFF;
	memcpy(&buffer[RAW_HID_HEADER_SIZE], packet->data, packet->data_len);

	const struct device *raw_dev;
	raw_dev = DEVICE_DT_GET(DT_NODELABEL(hid_raw));
	err = hid_device_submit_report(raw_dev,
								   RAW_HID_HEADER_SIZE + packet->data_len,
								   buffer,
								   true
	);
	if (err) {
		LOG_ERR("Failed to submit raw HID report: %d", err);
		return err;
	}

	return 0;
}

static int parse_raw_hid_packet(struct net_buf *nb) {

	int err;

    uint8_t buffer[32];

    size_t packet_length = net_buf_frags_len(nb);
    memcpy(&buffer, net_buf_pull_mem(nb, packet_length), packet_length);

    struct raw_hid_packet packet = {0};
    packet.report_id = buffer[0];
    packet.seq_id = buffer[2] << 8 | buffer[1];
    packet.data_len = packet_length - RAW_HID_HEADER_SIZE;
    memcpy(packet.data, &buffer[RAW_HID_HEADER_SIZE], packet.data_len);

    switch(packet.report_id) {
        case REPORT_ID_TIME:
			err = time_set(&packet);
        case REPORT_ID_AUDIO_TITLE:
			err = song_set(&packet);
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
            break;
        default:
            LOG_ERR("Received raw HID packet with unknown report ID %d", packet.report_id);
            return -EINVAL;
    }

	if (err) {
		LOG_ERR("Failed to process raw HID packet: %d", err);
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