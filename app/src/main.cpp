#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <stdint.h>

#if CONFIG_LED_SUBSYSTEM
/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
#endif

#if CONFIG_LED_SUBSYSTEM && CONFIG_LED_ADVANCED
static const struct pwm_dt_spec led_pwm =
    PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
#endif

#if CONFIG_LED_ADVANCED && CONFIG_LED_DEBUG
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);
#else
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);
#endif

int main(void)
{
#if !CONFIG_LED_SUBSYSTEM
    LOG_INF("LED subsystem disabled");
    return 0;
#else
    bool led_state = true;

#if CONFIG_LED_ADVANCED
    if (!device_is_ready(led_pwm.dev)) {
        LOG_ERR("LED PWM is not ready");
        return 0;
    }
#else
    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("LED GPIO is not ready");
        return 0;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Could not configure LED GPIO");
        return 0;
    }
#endif

#if CONFIG_LED_ADVANCED && CONFIG_LED_DEBUG
    LOG_DBG("LED brightness: %d%%, fade duration: %d ms",
            CONFIG_LED_BRIGHTNESS, CONFIG_LED_FADE_DURATION);
#endif

    while (1) {
#if CONFIG_LED_ADVANCED && CONFIG_CUSTOM_PATTERN
        const int32_t on_duration_ms = led_state ? 250 : 750;
#else
        const int32_t on_duration_ms = CONFIG_BLINK_SLEEP_TIME_MS;
#endif

        led_state = !led_state;

#if CONFIG_LED_ADVANCED && CONFIG_LED_FADE_DURATION > 0
        const int32_t fade_step_ms = 20;
        const int32_t fade_duration_ms =
            CONFIG_LED_FADE_DURATION < on_duration_ms
                ? CONFIG_LED_FADE_DURATION
                : on_duration_ms;
        const int32_t fade_steps = fade_duration_ms / fade_step_ms;

        if (fade_steps > 0) {
            for (int32_t step = 1; step <= fade_steps; ++step) {
                const int32_t brightness = led_state
                    ? CONFIG_LED_BRIGHTNESS * step / fade_steps
                    : CONFIG_LED_BRIGHTNESS * (fade_steps - step) / fade_steps;
                const uint32_t pulse =
                    led_pwm.period * brightness / 100;

                pwm_set_dt(&led_pwm, led_pwm.period, pulse);
                k_msleep(fade_step_ms);
            }
            pwm_set_dt(&led_pwm, led_pwm.period,
                       led_state
                           ? led_pwm.period * CONFIG_LED_BRIGHTNESS / 100
                           : 0);
            k_msleep(on_duration_ms - fade_duration_ms);
        } else
#endif
        {
#if CONFIG_LED_ADVANCED
            const uint32_t pulse =
                led_pwm.period * CONFIG_LED_BRIGHTNESS / 100;

            pwm_set_dt(&led_pwm, led_pwm.period, led_state ? pulse : 0);
            k_msleep(on_duration_ms);
#else
            gpio_pin_set_dt(&led, led_state);
            k_msleep(on_duration_ms);
            gpio_pin_set_dt(&led, 0);
#endif
        }

#if CONFIG_LED_ADVANCED && CONFIG_LED_DEBUG
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
#endif
    }
#endif
    return 0;
}
