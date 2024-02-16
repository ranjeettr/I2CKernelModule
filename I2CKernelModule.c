#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>

#define I2C_DEVICE_ADDRESS 0x50  // Example I2C device address
#define I2C_BUS_ID 1             // Example I2C bus ID

static struct i2c_adapter *adapter;
static struct i2c_client *client;

static int i2c_read_data(struct i2c_client *client, char *buf, int len)
{
    struct i2c_msg msgs[1];
    int ret;

    // Set up message for read operation
    msgs[0].addr = client->addr;
    msgs[0].flags = 0;      // Write
    msgs[0].len = len;
    msgs[0].buf = buf;

    ret = i2c_transfer(client->adapter, msgs, 1);
    if (ret < 0) {
        pr_err("I2C read failed: %d\n", ret);
        return ret;
    }

    return 0;
}

static int i2c_write_data(struct i2c_client *client, char *buf, int len)
{
    struct i2c_msg msgs[1];
    int ret;

    // Set up message for write operation
    msgs[0].addr = client->addr;
    msgs[0].flags = I2C_M_WR;      // Write
    msgs[0].len = len;
    msgs[0].buf = buf;

    ret = i2c_transfer(client->adapter, msgs, 1);
    if (ret < 0) {
        pr_err("I2C write failed: %d\n", ret);
        return ret;
    }

    return 0;
}

static int __init i2c_module_init(void)
{
    int ret;

    // Get the I2C adapter
    adapter = i2c_get_adapter(I2C_BUS_ID);
    if (!adapter) {
        pr_err("I2C adapter not found.\n");
        return -ENODEV;
    }

    // Create an I2C client
    client = i2c_new_client_device(adapter, &(struct i2c_board_info){
        I2C_BOARD_INFO("example-i2c-device", I2C_DEVICE_ADDRESS),
    });
    if (!client) {
        pr_err("Failed to create I2C device.\n");
        i2c_put_adapter(adapter);
        return -ENODEV;
    }

    // Perform I2C read and write operations
    char write_data[] = "Hello I2C Device!";
    char read_data[20];
    ret = i2c_write_data(client, write_data, sizeof(write_data));
    if (ret < 0) {
        pr_err("Failed to write data to I2C device.\n");
        i2c_unregister_device(client);
        i2c_put_adapter(adapter);
        return ret;
    }

    ret = i2c_read_data(client, read_data, sizeof(read_data));
    if (ret < 0) {
        pr_err("Failed to read data from I2C device.\n");
        i2c_unregister_device(client);
        i2c_put_adapter(adapter);
        return ret;
    }

    // Print the read data
    pr_info("Read data from I2C device: %s\n", read_data);

    pr_info("I2C module initialized.\n");

    return 0;
}

static void __exit i2c_module_exit(void)
{
    // Unregister the I2C device
    i2c_unregister_device(client);

    // Release the I2C adapter
    i2c_put_adapter(adapter);

    pr_info("I2C module exited.\n");
}

module_init(i2c_module_init);
module_exit(i2c_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RTR");
MODULE_DESCRIPTION("I2C kernel module example with read/write operations");

