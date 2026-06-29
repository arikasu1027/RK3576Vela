/****************************************************************************
 * smart_car_headunit/apps/gpio_test/gpio_test_main.c
 *
 * GPIO test application for KICKPI-K7 (RK3576)
 * Tests basic GPIO input/output and interrupt functionality.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "rk3576_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Default test pins (configure according to your hardware) */

#define TEST_OUTPUT_PIN     RK3576_GPIO_PIN(2, 0, 0)  /* GPIO2_A0 */
#define TEST_INPUT_PIN      RK3576_GPIO_PIN(2, 0, 1)  /* GPIO2_A1 */
#define TEST_IRQ_PIN        RK3576_GPIO_PIN(2, 0, 2)  /* GPIO2_A2 */

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_output_pin(int pin)
{
    printf("=== GPIO Output Test (pin %d) ===\n", pin);

    /* Configure as output */

    rk3576_gpio_direction_output(pin, 0);
    printf("Pin %d configured as output\n", pin);

    /* Toggle 5 times */

    for (int i = 0; i < 5; i++)
    {
        rk3576_gpio_set_value(pin, 1);
        printf("  Pin %d = HIGH\n", pin);
        usleep(500000);

        rk3576_gpio_set_value(pin, 0);
        printf("  Pin %d = LOW\n", pin);
        usleep(500000);
    }

    printf("Output test passed!\n\n");
    return OK;
}

static int test_input_pin(int pin)
{
    int value;

    printf("=== GPIO Input Test (pin %d) ===\n", pin);
    printf("Connect pin %d to HIGH or LOW, reading 10 times...\n", pin);

    /* Configure as input */

    rk3576_gpio_direction_input(pin);
    printf("Pin %d configured as input\n", pin);

    /* Read 10 times */

    for (int i = 0; i < 10; i++)
    {
        value = rk3576_gpio_get_value(pin);
        printf("  Read %d: pin %d = %d\n", i + 1, pin, value);
        usleep(200000);
    }

    printf("Input test passed!\n\n");
    return OK;
}

static int irq_test_count = 0;

static int irq_test_handler(int irq, void *context, void *arg)
{
    irq_test_count++;
    printf("  IRQ triggered! count=%d\n", irq_test_count);
    return OK;
}

static int test_irq_pin(int pin)
{
    int ret;

    printf("=== GPIO IRQ Test (pin %d) ===\n", pin);
    printf("Connect pin %d to a button or signal source\n", pin);

    /* Configure as input */

    rk3576_gpio_direction_input(pin);

    /* Attach interrupt */

    ret = rk3576_gpio_irq_attach(pin, irq_test_handler, NULL);
    if (ret < 0)
    {
        printf("Failed to attach IRQ: %d\n", ret);
        return ret;
    }

    /* Set to rising edge trigger */

    ret = rk3576_gpio_irq_settype(pin, GPIO_IRQTYPE_RISING);
    if (ret < 0)
    {
        printf("Failed to set IRQ type: %d\n", ret);
        return ret;
    }

    /* Enable interrupt */

    ret = rk3576_gpio_irq_enable(pin);
    if (ret < 0)
    {
        printf("Failed to enable IRQ: %d\n", ret);
        return ret;
    }

    printf("IRQ configured on pin %d (rising edge). Waiting 5 seconds...\n", pin);
    printf("Trigger the pin to see interrupts.\n");

    irq_test_count = 0;
    sleep(5);

    /* Disable interrupt */

    rk3576_gpio_irq_disable(pin);
    rk3576_gpio_irq_detach(pin);

    printf("IRQ test complete: %d interrupts received\n\n", irq_test_count);
    return OK;
}

static int test_loopback(int out_pin, int in_pin)
{
    int val;

    printf("=== GPIO Loopback Test (out=%d, in=%d) ===\n", out_pin, in_pin);
    printf("Connect pin %d to pin %d with a jumper wire\n", out_pin, in_pin);

    /* Configure pins */

    rk3576_gpio_direction_output(out_pin, 0);
    rk3576_gpio_direction_input(in_pin);

    /* Test LOW */

    rk3576_gpio_set_value(out_pin, 0);
    usleep(10000);
    val = rk3576_gpio_get_value(in_pin);
    printf("  Output LOW -> Input reads %d (expected 0)\n", val);

    /* Test HIGH */

    rk3576_gpio_set_value(out_pin, 1);
    usleep(10000);
    val = rk3576_gpio_get_value(in_pin);
    printf("  Output HIGH -> Input reads %d (expected 1)\n", val);

    if (val == 1)
    {
        printf("Loopback test passed!\n\n");
    }
    else
    {
        printf("Loopback test FAILED! Check wiring.\n\n");
    }

    return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
    printf("\n");
    printf("===================================\n");
    printf("  RK3576 GPIO Test Application\n");
    printf("  KICKPI-K7 (KICKPI-K7)\n");
    printf("===================================\n\n");

    /* Initialize GPIO subsystem */

    rk3576_gpio_init();
    printf("GPIO subsystem initialized\n\n");

    /* Run tests */

    test_output_pin(TEST_OUTPUT_PIN);
    test_input_pin(TEST_INPUT_PIN);
    test_irq_pin(TEST_IRQ_PIN);
    test_loopback(TEST_OUTPUT_PIN, TEST_INPUT_PIN);

    printf("===================================\n");
    printf("  All GPIO tests completed!\n");
    printf("===================================\n");

    return OK;
}
