#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gpio.h>

static int output = 0;
static int level = 0;
static int speed = 0;

module_param(output, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(speed, "Output pin");
module_param(level, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(speed, "Status of output");
module_param(speed, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(speed, "Toggling speed");

static struct timer_list blink_timer;

static void blink_timer_func(struct timer_list *t)
{
    printk(KERN_INFO "%s\n", __func__);

    if (output != 0)
    {
        gpio_set_value(output, level);
        level = !level;
    }

    blink_timer.expires = jiffies + (speed * HZ);
    add_timer(&blink_timer);
}

static int __init gpiomod_init(void)
{
    int ret = 0;

    if (output != 0)
    {
        ret = gpio_request_one(output, GPIOF_OUT_INIT_LOW, "OUTPUT");
        if (ret)
        {
            printk(KERN_ERR "Unable to request GPIOs: %d\n", ret);
            return ret;
        }
    }

    gpio_set_value(output, level);

    if (speed != 0)
    {
        timer_setup(&blink_timer, blink_timer_func, 0);

        blink_timer.function = blink_timer_func;
        blink_timer.expires = jiffies + (1 * HZ);
        add_timer(&blink_timer);
    }

    return ret;
}

static void __exit gpiomod_exit(void)
{
    printk(KERN_INFO "%s\n", __func__);

    del_timer_sync(&blink_timer);

    gpio_set_value(output, 0);

    gpio_free(output);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Liem Nguyen");
MODULE_DESCRIPTION("Embedded systems 2 - Labo 5");

module_init(gpiomod_init);
module_exit(gpiomod_exit);
