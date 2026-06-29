/****************************************************************************
 * smart_car_headunit/bsp/drivers/rotary_encoder/rotary_encoder.c
 *
 * Rotary encoder driver for KICKPI-K7 (RK3576)
 * Uses the RK3576 GPIO driver for pin control and interrupts.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <nuttx/arch.h>
#include <nuttx/clock.h>
#include <nuttx/fs/fs.h>
#include <nuttx/wqueue.h>

#include "rk3576_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define ROTARY_DEBOUNCE_MS    50
#define ROTARY_THREAD_STACK   2048
#define ROTARY_THREAD_PRIORITY 100

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct rotary_encoder_s
{
    int pin_a;              /* GPIO pin for phase A */
    int pin_b;              /* GPIO pin for phase B */
    volatile int32_t count; /* Rotary count */
    int32_t last_count;     /* Last reported count */
    bool enabled;           /* Encoder enabled */
    struct work_s work;     /* Work queue for debounce */
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct rotary_encoder_s g_rotary;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void rotary_work_func(void *arg)
{
    struct rotary_encoder_s *dev = (struct rotary_encoder_s *)arg;
    int a;
    int b;

    if (!dev->enabled)
    {
        return;
    }

    /* Read current state */

    a = rk3576_gpio_get_value(dev->pin_a);
    b = rk3576_gpio_get_value(dev->pin_b);

    if (a < 0 || b < 0)
    {
        return;
    }

    /* Determine direction based on phase relationship */

    if (a != b)
    {
        dev->count++;
    }
    else
    {
        dev->count--;
    }

    printf("Rotary count: %d\n", dev->count);
}

static int rotary_interrupt(int irq, void *context, void *arg)
{
    struct rotary_encoder_s *dev = (struct rotary_encoder_s *)arg;

    if (!dev->enabled)
    {
        return OK;
    }

    /* Schedule work for debounce */

    work_queue(HPWORK, &dev->work, rotary_work_func, dev, ROTARY_DEBOUNCE_MS);

    return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int rotary_encoder_init(int pin_a, int pin_b)
{
    int ret;

    /* Configure GPIO pins as inputs */

    rk3576_gpio_direction_input(pin_a);
    rk3576_gpio_direction_input(pin_b);

    /* Enable debounce on both pins */

    rk3576_gpio_set_debounce(pin_a, true);
    rk3576_gpio_set_debounce(pin_b, true);

    /* Attach interrupt on phase A (both edges) */

    ret = rk3576_gpio_irq_attach(pin_a, rotary_interrupt, &g_rotary);
    if (ret < 0)
    {
        printf("Failed to attach interrupt on GPIO %d: %d\n", pin_a, ret);
        return ret;
    }

    ret = rk3576_gpio_irq_settype(pin_a, GPIO_IRQTYPE_BOTHEDGE);
    if (ret < 0)
    {
        printf("Failed to set IRQ type on GPIO %d: %d\n", pin_a, ret);
        return ret;
    }

    ret = rk3576_gpio_irq_enable(pin_a);
    if (ret < 0)
    {
        printf("Failed to enable IRQ on GPIO %d: %d\n", pin_a, ret);
        return ret;
    }

    /* Initialize structure */

    g_rotary.pin_a = pin_a;
    g_rotary.pin_b = pin_b;
    g_rotary.count = 0;
    g_rotary.last_count = 0;
    g_rotary.enabled = true;

    printf("Rotary encoder initialized: A=%d, B=%d\n", pin_a, pin_b);

    return OK;
}

int rotary_encoder_get_count(void)
{
    return g_rotary.count;
}

int rotary_encoder_reset(void)
{
    g_rotary.count = 0;
    g_rotary.last_count = 0;
    return OK;
}

void rotary_encoder_enable(bool enable)
{
    g_rotary.enabled = enable;
}

void rotary_encoder_deinit(void)
{
    rk3576_gpio_irq_detach(g_rotary.pin_a);
    g_rotary.enabled = false;
}
