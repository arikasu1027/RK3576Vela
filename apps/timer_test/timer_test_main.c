/****************************************************************************
 * smart_car_headunit/apps/timer_test/timer_test_main.c
 *
 * Timer test application for KICKPI-K7 (RK3576)
 * Tests general purpose timer functionality.
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

#include "rk3576_timer.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TEST_CONTROLLER      0
#define TEST_CHANNEL         0

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_init(void)
{
  printf("=== Timer Init Test ===\n");

  rk3576_timer_init(TEST_CONTROLLER);
  printf("  TIMER0: OK\n");

  rk3576_timer_init(1);
  printf("  TIMER1: OK\n");

  printf("\n");
  return OK;
}

static int test_mode(void)
{
  int ret;

  printf("=== Timer Mode Test ===\n");

  ret = rk3576_timer_set_mode(TEST_CONTROLLER, TEST_CHANNEL,
                              RK3576_TIMER_MODE_FREE);
  if (ret < 0)
    {
      printf("Failed to set free-running: %d\n", ret);
      return ret;
    }

  printf("  Free-running: OK\n");

  ret = rk3576_timer_set_mode(TEST_CONTROLLER, TEST_CHANNEL,
                              RK3576_TIMER_MODE_USER);
  if (ret < 0)
    {
      printf("Failed to set user-defined: %d\n", ret);
      return ret;
    }

  printf("  User-defined: OK\n");

  /* Reset to free-running */

  rk3576_timer_set_mode(TEST_CONTROLLER, TEST_CHANNEL,
                        RK3576_TIMER_MODE_FREE);

  printf("\n");
  return OK;
}

static int test_count(void)
{
  int ret;

  printf("=== Timer Count Test ===\n");

  /* Set various count values */

  uint32_t counts[] = {1000, 10000, 100000, 24000000};
  int count = sizeof(counts) / sizeof(counts[0]);

  for (int i = 0; i < count; i++)
    {
      ret = rk3576_timer_set_count(TEST_CONTROLLER, TEST_CHANNEL, counts[i]);
      if (ret < 0)
        {
          printf("Failed to set count %u: %d\n", counts[i], ret);
          return ret;
        }

      printf("  Count %u: OK\n", counts[i]);
    }

  printf("\n");
  return OK;
}

static int test_period(void)
{
  int ret;

  printf("=== Timer Period Test ===\n");

  /* Set various periods in microseconds */

  uint32_t periods[] = {100, 1000, 10000, 100000};
  int count = sizeof(periods) / sizeof(periods[0]);

  for (int i = 0; i < count; i++)
    {
      ret = rk3576_timer_set_period_us(TEST_CONTROLLER, TEST_CHANNEL,
                                        periods[i]);
      if (ret < 0)
        {
          printf("Failed to set period %u us: %d\n", periods[i], ret);
          return ret;
        }

      printf("  Period %u us: OK\n", periods[i]);
    }

  printf("\n");
  return OK;
}

static int test_start_stop(void)
{
  printf("=== Timer Start/Stop Test ===\n");

  /* Set 1 second period */

  rk3576_timer_set_period_us(TEST_CONTROLLER, TEST_CHANNEL, 1000000);

  /* Start */

  rk3576_timer_start(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  Started\n");

  /* Check running status */

  int running = rk3576_timer_is_running(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  Running: %s\n", running ? "YES" : "NO");

  /* Wait 500ms */

  usleep(500000);

  /* Get current count */

  uint32_t count = rk3576_timer_get_count(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  Current count: %u\n", count);

  /* Stop */

  rk3576_timer_stop(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  Stopped\n");

  running = rk3576_timer_is_running(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  Running: %s\n", running ? "YES" : "NO");

  printf("\n");
  return OK;
}

static int test_free_running(void)
{
  printf("=== Timer Free-Running Test ===\n");

  /* Configure free-running mode */

  rk3576_timer_set_mode(TEST_CONTROLLER, TEST_CHANNEL,
                        RK3576_TIMER_MODE_FREE);
  rk3576_timer_set_count(TEST_CONTROLLER, TEST_CHANNEL, 0xffffffff);

  /* Start */

  rk3576_timer_start(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  Started free-running\n");

  /* Read count 5 times, 200ms apart */

  for (int i = 0; i < 5; i++)
    {
      uint32_t count = rk3576_timer_get_count(TEST_CONTROLLER, TEST_CHANNEL);
      printf("  [%d] Count: %u\n", i, count);
      usleep(200000);
    }

  /* Stop */

  rk3576_timer_stop(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  Stopped\n");

  printf("\n");
  return OK;
}

static int test_one_shot(void)
{
  printf("=== Timer One-Shot Test ===\n");

  /* Configure user-defined count (one-shot) mode */

  rk3576_timer_set_mode(TEST_CONTROLLER, TEST_CHANNEL,
                        RK3576_TIMER_MODE_USER);
  rk3576_timer_set_period_us(TEST_CONTROLLER, TEST_CHANNEL, 500000);

  /* Start */

  rk3576_timer_start(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  Started one-shot (500ms)\n");

  /* Wait and check if stopped automatically */

  usleep(600000);

  int running = rk3576_timer_is_running(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  After timeout: %s\n", running ? "Still running" : "Stopped");

  /* Clear interrupt */

  rk3576_timer_clear_irq(TEST_CONTROLLER, TEST_CHANNEL);
  printf("  IRQ cleared\n");

  printf("\n");
  return OK;
}

static int test_both_channels(void)
{
  printf("=== Timer Both Channels Test ===\n");

  /* Configure channel 0 */

  rk3576_timer_set_mode(TEST_CONTROLLER, 0, RK3576_TIMER_MODE_FREE);
  rk3576_timer_set_count(TEST_CONTROLLER, 0, 0xffffffff);

  /* Configure channel 1 */

  rk3576_timer_set_mode(TEST_CONTROLLER, 1, RK3576_TIMER_MODE_FREE);
  rk3576_timer_set_count(TEST_CONTROLLER, 1, 0xffffffff);

  /* Start both */

  rk3576_timer_start(TEST_CONTROLLER, 0);
  rk3576_timer_start(TEST_CONTROLLER, 1);
  printf("  Both channels started\n");

  /* Wait 200ms */

  usleep(200000);

  /* Read both counts */

  uint32_t count0 = rk3576_timer_get_count(TEST_CONTROLLER, 0);
  uint32_t count1 = rk3576_timer_get_count(TEST_CONTROLLER, 1);
  printf("  CH0: %u\n", count0);
  printf("  CH1: %u\n", count1);

  /* Stop both */

  rk3576_timer_stop(TEST_CONTROLLER, 0);
  rk3576_timer_stop(TEST_CONTROLLER, 1);
  printf("  Both channels stopped\n");

  printf("\n");
  return OK;
}

static int test_both_controllers(void)
{
  printf("=== Timer Both Controllers Test ===\n");

  /* Init both controllers */

  rk3576_timer_init(0);
  rk3576_timer_init(1);

  /* Configure and start controller 0 */

  rk3576_timer_set_mode(0, 0, RK3576_TIMER_MODE_FREE);
  rk3576_timer_set_count(0, 0, 0xffffffff);
  rk3576_timer_start(0, 0);

  /* Configure and start controller 1 */

  rk3576_timer_set_mode(1, 0, RK3576_TIMER_MODE_FREE);
  rk3576_timer_set_count(1, 0, 0xffffffff);
  rk3576_timer_start(1, 0);

  printf("  Both controllers started\n");

  /* Wait 200ms */

  usleep(200000);

  /* Read counts */

  uint32_t count0 = rk3576_timer_get_count(0, 0);
  uint32_t count1 = rk3576_timer_get_count(1, 0);
  printf("  TIMER0: %u\n", count0);
  printf("  TIMER1: %u\n", count1);

  /* Stop both */

  rk3576_timer_stop(0, 0);
  rk3576_timer_stop(1, 0);
  printf("  Both controllers stopped\n");

  printf("\n");
  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
  printf("\n");
  printf("===================================\n");
  printf("  RK3576 General Purpose Timer Test\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Run tests */

  test_init();
  test_mode();
  test_count();
  test_period();
  test_start_stop();
  test_free_running();
  test_one_shot();
  test_both_channels();
  test_both_controllers();

  printf("===================================\n");
  printf("  All timer tests completed!\n");
  printf("===================================\n");

  return OK;
}
