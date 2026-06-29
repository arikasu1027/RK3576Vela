/****************************************************************************
 * smart_car_headunit/apps/pwm_test/pwm_test_main.c
 *
 * PWM test application for KICKPI-K7 (RK3576)
 * Tests PWM output with backlight, buzzer, and servo scenarios.
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

#include "rk3576_pwm.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Test PWM channels (configure according to your hardware) */

#define TEST_PWM_CONTROLLER    0   /* PWM0 */
#define TEST_PWM_CHANNEL       0   /* PWM0_CH0 */

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_basic_pwm(int ctrl, int ch)
{
  int ret;

  printf("=== Basic PWM Test (PWM%d_CH%d) ===\n", ctrl, ch);

  /* Configure 1kHz, 50% duty */

  ret = rk3576_pwm_set_frequency(ctrl, ch, 1000, 50);
  if (ret < 0)
    {
      printf("Failed to configure PWM: %d\n", ret);
      return ret;
    }

  printf("Configured: 1kHz, 50%% duty\n");

  /* Enable */

  ret = rk3576_pwm_enable(ctrl, ch);
  if (ret < 0)
    {
      printf("Failed to enable PWM: %d\n", ret);
      return ret;
    }

  printf("Enabled, running 2 seconds...\n");
  sleep(2);

  /* Disable */

  rk3576_pwm_disable(ctrl, ch);
  printf("Disabled\n\n");

  return OK;
}

static int test_duty_cycle_sweep(int ctrl, int ch)
{
  int ret;
  int duty;

  printf("=== Duty Cycle Sweep Test (PWM%d_CH%d, 1kHz) ===\n", ctrl, ch);

  /* Sweep duty from 0% to 100% in 10% steps */

  for (duty = 0; duty <= 100; duty += 10)
    {
      ret = rk3576_pwm_set_frequency(ctrl, ch, 1000, duty);
      if (ret < 0)
        {
          printf("Failed at %d%%: %d\n", duty, ret);
          continue;
        }

      ret = rk3576_pwm_enable(ctrl, ch);
      if (ret < 0)
        {
          printf("Failed to enable at %d%%: %d\n", duty, ret);
          continue;
        }

      printf("  Duty: %3d%%\n", duty);
      usleep(500000);

      rk3576_pwm_disable(ctrl, ch);
    }

  printf("Sweep complete!\n\n");
  return OK;
}

static int test_backlight(int ctrl, int ch)
{
  int ret;
  int brightness;

  printf("=== Backlight Simulation (PWM%d_CH%d, 20kHz) ===\n", ctrl, ch);
  printf("Simulating brightness levels 0-100%%\n");

  for (brightness = 0; brightness <= 100; brightness += 5)
    {
      ret = rk3576_pwm_set_frequency(ctrl, ch, 20000, brightness);
      if (ret < 0)
        {
          printf("Failed at brightness %d: %d\n", brightness, ret);
          continue;
        }

      ret = rk3576_pwm_enable(ctrl, ch);
      if (ret < 0)
        {
          printf("Failed to enable: %d\n", ret);
          continue;
        }

      printf("  Brightness: %3d%%\n", brightness);
      usleep(300000);

      rk3576_pwm_disable(ctrl, ch);
    }

  printf("Backlight test complete!\n\n");
  return OK;
}

static int test_buzzer(int ctrl, int ch)
{
  int ret;

  printf("=== Buzzer Test (PWM%d_CH%d, 1kHz) ===\n", ctrl, ch);

  /* Beep pattern: 3 short beeps */

  for (int i = 0; i < 3; i++)
    {
      ret = rk3576_pwm_set_frequency(ctrl, ch, 1000, 50);
      if (ret < 0)
        {
          printf("Failed: %d\n", ret);
          return ret;
        }

      ret = rk3576_pwm_enable(ctrl, ch);
      if (ret < 0)
        {
          printf("Failed: %d\n", ret);
          return ret;
        }

      printf("  Beep %d\n", i + 1);
      usleep(200000);

      rk3576_pwm_disable(ctrl, ch);
      usleep(200000);
    }

  printf("Buzzer test complete!\n\n");
  return OK;
}

static int test_frequency_sweep(int ctrl, int ch)
{
  int ret;
  uint32_t freq;

  printf("=== Frequency Sweep Test (PWM%d_CH%d, 50%% duty) ===\n", ctrl, ch);

  /* Sweep frequency: 100Hz, 1kHz, 10kHz, 20kHz, 50kHz */

  uint32_t freqs[] = {100, 1000, 10000, 20000, 50000};
  int count = sizeof(freqs) / sizeof(freqs[0]);

  for (int i = 0; i < count; i++)
    {
      freq = freqs[i];

      ret = rk3576_pwm_set_frequency(ctrl, ch, freq, 50);
      if (ret < 0)
        {
          printf("Failed at %u Hz: %d\n", freq, ret);
          continue;
        }

      ret = rk3576_pwm_enable(ctrl, ch);
      if (ret < 0)
        {
          printf("Failed to enable: %d\n", ret);
          continue;
        }

      printf("  Frequency: %u Hz\n", freq);
      usleep(500000);

      rk3576_pwm_disable(ctrl, ch);
    }

  printf("Frequency sweep complete!\n\n");
  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
  printf("\n");
  printf("===================================\n");
  printf("  RK3576 PWM Test Application\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Initialize PWM subsystem */

  rk3576_pwm_init();
  printf("PWM subsystem initialized\n\n");

  /* Run tests */

  test_basic_pwm(TEST_PWM_CONTROLLER, TEST_PWM_CHANNEL);
  test_duty_cycle_sweep(TEST_PWM_CONTROLLER, TEST_PWM_CHANNEL);
  test_frequency_sweep(TEST_PWM_CONTROLLER, TEST_PWM_CHANNEL);
  test_backlight(TEST_PWM_CONTROLLER, TEST_PWM_CHANNEL);
  test_buzzer(TEST_PWM_CONTROLLER, TEST_PWM_CHANNEL);

  printf("===================================\n");
  printf("  All PWM tests completed!\n");
  printf("  Connect oscilloscope to verify\n");
  printf("  PWM%d_CH%d output waveform\n",
         TEST_PWM_CONTROLLER, TEST_PWM_CHANNEL);
  printf("===================================\n");

  return OK;
}
