#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static int channel_get_my_impl(const struct device *dev, 
                                enum sensor_channel chan,
                                struct sensor_value *val){
    if (gpio_pin_set_dt(&led, 0) < 0) return 1;
    LOG_INF("LED state: Off");
    return 0;
}

static int sample_fetch_my_impl(const struct device *dev, 
                                enum sensor_channel chan){
    if (gpio_pin_set_dt(&led, 1) < 0) return 1;
    LOG_INF("LED state: On");
    return 0;
}

static DEVICE_API(sensor, our_driver_api) = {
    .channel_get = channel_get_my_impl,
    .sample_fetch = sample_fetch_my_impl,
};

// Init fn
static int init(const struct device *dev){
    if (!gpio_is_ready_dt(&led)) return 1;
    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 1;
    LOG_INF("Device initialized!");
    return 0;
}

#define DEV_INST(inst) DEVICE_DT_INST_DEFINE(inst, init, NULL, NULL, NULL, POST_KERNEL, 80, &our_driver_api);

DT_INST_FOREACH_STATUS_OKAY(DEV_INST);