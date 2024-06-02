#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

LOG_MODULE_REGISTER(zippy_usb);

#define ZEPHYR_PROJECT_USB_VID		0x2FE3

USBD_DEVICE_DEFINE(zippy_usbd,
		   DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
		   ZEPHYR_PROJECT_USB_VID, CONFIG_ZIPPY_USBD_PID);

USBD_DESC_LANG_DEFINE(zippy_lang);
USBD_DESC_MANUFACTURER_DEFINE(zippy_mfr, CONFIG_ZIPPY_USBD_MANUFACTURER);
USBD_DESC_PRODUCT_DEFINE(zippy_product, CONFIG_ZIPPY_USBD_PRODUCT);
USBD_DESC_SERIAL_NUMBER_DEFINE(zippy_sn);

static const uint8_t attributes = (IS_ENABLED(CONFIG_ZIPPY_USBD_SELF_POWERED) ?
				   USB_SCD_SELF_POWERED : 0) |
				  (IS_ENABLED(CONFIG_ZIPPY_USBD_REMOTE_WAKEUP) ?
				   USB_SCD_REMOTE_WAKEUP : 0);

USBD_CONFIGURATION_DEFINE(zippy_config,
			  attributes,
			  CONFIG_ZIPPY_USBD_MAX_POWER);

int usbd_init_device(void)
{
	int err;

	err = usbd_add_descriptor(&zippy_usbd, &zippy_lang);
	if (err) {
		LOG_ERR("Failed to initialize language descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&zippy_usbd, &zippy_mfr);
	if (err) {
		LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&zippy_usbd, &zippy_product);
	if (err) {
		LOG_ERR("Failed to initialize product descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&zippy_usbd, &zippy_sn);
	if (err) {
		LOG_ERR("Failed to initialize SN descriptor (%d)", err);
		return err;
	}

	err = usbd_add_configuration(&zippy_usbd, USBD_SPEED_HS, &zippy_config);
	if (err) {
		LOG_ERR("Failed to add configuration (%d)", err);
		return err;
	}

	STRUCT_SECTION_FOREACH_ALTERNATE(usbd_class_fs, usbd_class_node, c_nd) {
		/* Pull everything that is enabled in our configuration. */
		err = usbd_register_class(&zippy_usbd, c_nd->c_data->name,
					  USBD_SPEED_FS, 1);
		if (err) {
			LOG_ERR("Failed to register %s (%d)", c_nd->c_data->name, err);
			return err;
		}

		LOG_DBG("Register %s", c_nd->c_data->name);
	}

	err = usbd_device_set_code_triple(&zippy_usbd, USBD_SPEED_FS, 0, 0, 0);
	if (err) {
		LOG_ERR("Failed to set code triple (%d)", err);
		return err;
	}

	err = usbd_init(&zippy_usbd);
	if (err) {
		LOG_ERR("Failed to initialize device support");
		return err;
	}

    err = usbd_enable(&zippy_usbd);
    if (err) {
        LOG_ERR("Failed to enable USB device");
        return err;
    }

	return err;
}

SYS_INIT(usbd_init_device, APPLICATION, CONFIG_ZIPPY_USBD_INIT_PRIORITY);
