#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include "our_driver.h"

static int cmd_channel_get_handler(const struct shell *shell, size_t argc, char **argv){
    const struct device *dev = device_get_binding(argv[1]);
    if (!dev) {
        shell_error(shell, "Device not found");
        return -EFAULT;
    }
    struct sensor_value val;
    if (sensor_channel_get(dev, SENSOR_CHAN_ALL, &val) < 0) {
        shell_error(shell, "Failed to get channel value");
        return -EFAULT;
    }
    shell_info(shell, "Channel value: %d", val.val1);
    return 0;
}

static int cmd_sample_fetch_handler(const struct shell *shell, size_t argc, char **argv){
    const struct device *dev = device_get_binding(argv[1]);
    if (!dev) {
        shell_error(shell, "Device not found");
        return -EFAULT;
    }
    if (sensor_sample_fetch(dev) < 0) {
        shell_error(shell, "Failed to fetch sample");
        return -EFAULT;
    }
    shell_info(shell, "Sample fetched successfully");
    return 0;
}

static int cmd_info_handler(const struct shell *shell, size_t argc, char **argv){
    const struct device *dev = device_get_binding(argv[1]);
    if (!dev) {
        shell_error(shell, "Device not found");
        return -EFAULT;
    }
    shell_info(shell, "Device: %s, ready: %s", dev->name,
               device_is_ready(dev) ? "yes" : "no");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(our_driver_subcmd,
    SHELL_CMD_ARG(read, NULL, "Get the channel value", cmd_channel_get_handler, 2, 0),
    SHELL_CMD_ARG(fetch, NULL, "Fetch a sample", cmd_sample_fetch_handler, 2, 0),
    SHELL_CMD_ARG(info, NULL, "Get driver info", cmd_info_handler, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(our_driver, &our_driver_subcmd, "Our driver commands", NULL);