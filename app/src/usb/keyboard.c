#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_hid.h>

LOG_MODULE_REGISTER(zippy_keyboard_hid);

static const uint8_t __aligned(32) keyboard_hid_report_desc[] = HID_KEYBOARD_REPORT_DESC();

static uint32_t kb_dev_duration = 0;

static void kb_iface_ready(const struct device *dev, const bool ready)
{
	LOG_INF("HID device %p interface is %s", dev, ready ? "ready" : "not ready");
}

static int kb_get_report(const struct device *dev,
			 const uint8_t type, const uint8_t id, const uint16_t len,
			 uint8_t *const buf)
{
	return 0;
}

static int kb_set_report(const struct device *dev,
			 const uint8_t type, const uint8_t id, const uint16_t len,
			 const uint8_t *const buf)
{
	return 0;
}

static void kb_set_idle(const struct device *dev,
			const uint8_t id, const uint32_t duration)
{
	LOG_INF("Set Idle %u to %u", id, duration);
	kb_dev_duration = duration;
}

static uint32_t kb_get_idle(const struct device *dev, const uint8_t id)
{
	LOG_INF("Get Idle to %u", id);
    return kb_dev_duration;
}

static void kb_set_protocol(const struct device *dev, const uint8_t proto)
{
	LOG_INF("Protocol changed to %s",
		proto == 0U ? "Boot Protocol" : "Report Protocol");
}

static void kb_output_report(const struct device *dev, const uint16_t len,
			     const uint8_t *const buf)
{
	LOG_HEXDUMP_INF(buf, len, "o.r.");
	kb_set_report(dev, 2U, 0U, len, buf);
}

struct hid_device_ops kb_ops = {
	.iface_ready = kb_iface_ready,
	.get_report = kb_get_report,
	.set_report = kb_set_report,
	.set_idle = kb_set_idle,
	.get_idle = kb_get_idle,
	.set_protocol = kb_set_protocol,
	.output_report = kb_output_report,
};

static int hid_init_keyboard(void) {
    int err;

    const struct device *keyboard_dev;
    keyboard_dev = DEVICE_DT_GET(DT_NODELABEL(hid_keyboard));
	if (!device_is_ready(keyboard_dev)) {
		LOG_ERR("HID Device is not ready");
		return -EIO;
	}

	err = hid_device_register(keyboard_dev,
				  keyboard_hid_report_desc, sizeof(keyboard_hid_report_desc),
				  &kb_ops);
    if (err) {
        LOG_ERR("Failed to register HID device: %d", err);
        return err;
    }

    return 0;
}

SYS_INIT(hid_init_keyboard, APPLICATION, CONFIG_ZIPPY_USBD_HID_KEYBOARD_INIT_PRIORITY);