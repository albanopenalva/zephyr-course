#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#include <zephyr/device.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int our_driver_counter_increment(const struct device *dev);

#ifdef __cplusplus
}
#endif

#endif