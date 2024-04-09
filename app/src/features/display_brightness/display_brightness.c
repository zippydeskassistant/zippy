#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/input/input.h>

LOG_MODULE_REGISTER(zippy_display_brightness);

static const struct pwm_dt_spec pwm_dev = PWM_DT_SPEC_GET(DT_NODELABEL(pwm_led0));

void display_brightness_timer_cb(struct k_timer *timer) {
    // Set the PWM period to 10ms, 20% duty cycle
    pwm_set_dt(&pwm_dev, 10000, 8000);
}

K_TIMER_DEFINE(display_brightness_timer, display_brightness_timer_cb, NULL);

void display_disable_timer_cb(struct k_timer *timer) {
    // Turn off the display
    pwm_set_dt(&pwm_dev, 10000, 10000);
}

K_TIMER_DEFINE(display_disable_timer, display_disable_timer_cb, NULL);

void display_touched_cb(struct input_event *event) {
    if (event->code == INPUT_BTN_TOUCH) {
        k_timer_start(&display_brightness_timer, K_MINUTES(1), K_NO_WAIT);
        k_timer_start(&display_disable_timer, K_MINUTES(5), K_NO_WAIT);
        pwm_set_dt(&pwm_dev, 10000, 2000);
    }
}

INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_NODELABEL(ft5336_adafruit_2_8_tft_touch_v2)), display_touched_cb);

int display_brightness_init(void) {

    if (!pwm_is_ready_dt(&pwm_dev)) {
        LOG_ERR("PWM device is not ready");
        return -ENODEV;
    }

    // Set the PWM period to 10ms, 80% duty cycle
    pwm_set_dt(&pwm_dev, 10000, 2000);

    // Start the timer
    k_timer_start(&display_brightness_timer, K_MINUTES(1), K_NO_WAIT);
    k_timer_start(&display_disable_timer, K_MINUTES(5), K_NO_WAIT);

    return 0;
}

SYS_INIT(display_brightness_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);