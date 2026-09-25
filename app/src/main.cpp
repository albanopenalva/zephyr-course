#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "our_driver.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    struct sensor_value val;
    bool led_state = true;

    while (1) {
        // if (led_state) {
        //     sensor_channel_get(dev, SENSOR_CHAN_GREEN, &val);
        // } else {
        //     sensor_sample_fetch(dev);
        // }

        // our_driver_counter_increment(dev);

        // led_state = !led_state;
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS );
    }
    return 0;
}