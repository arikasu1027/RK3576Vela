/****************************************************************************
 * smart_car_headunit/apps/wdt_test/wdt_test_main.c
 *
 * Watchdog timer test application for KICKPI-K7 (RK3576)
 * Tests watchdog timer configuration and feeding.
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

#include "rk3576_wdt.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_init(void)
{
  printf("=== Watchdog Init Test ===\n");

  rk3576_wdt_init();
  printf("  Init: OK\n");

  int enabled = rk3576_wdt_is_enabled();
  printf("  Enabled: %s\n", enabled ? "YES" : "NO");

  printf("\n");
  return OK;
}

static int test_timeout(void)
{
  int ret;

  printf("=== Watchdog Timeout Test ===\n");

  uint32_t timeouts[] = {1000, 2000, 5000, 10000, 30000};
  int count = sizeof(timeouts) / sizeof(timeouts[0]);

  for (int i = 0; i < count; i++)
    {
      ret = rk3576_wdt_set_timeout(timeouts[i]);
      if (ret < 0)
        {
          printf("Failed to set %u ms: %d\n", timeouts[i], ret);
          return ret;
        }

      printf("  %u ms: OK\n", timeouts[i]);
    }

  /* Reset to 5 seconds */

  rk3576_wdt_set_timeout(5000);

  printf("\n");
  return OK;
}

static int test_mode(void)
{
  int ret;

  printf("=== Watchdog Mode Test ===\n");

  ret = rk3576_wdt_set_mode(RK3576_WDT_MODE_RESET);
  if (ret < 0)
    {
      printf("Failed to set reset mode: %d\n", ret);
      return ret;
    }

  printf("  Reset mode: OK\n");

  ret = rk3576_wdt_set_mode(RK3576_WDT_MODE_INTERRUPT);
  if (ret < 0)
    {
      printf("Failed to set interrupt mode: %d\n", ret);
      return ret;
    }

  printf("  Interrupt mode: OK\n");

  /* Reset to reset mode */

  rk3576_wdt_set_mode(RK3576_WDT_MODE_RESET);

  printf("\n");
  return OK;
}

static int test_feed(void)
{
  printf("=== Watchdog Feed Test ===\n");

  /* Start watchdog with 5 second timeout */

  rk3576_wdt_set_timeout(5000);
  rk3576_wdt_start(5000);

  printf("  Started with 5s timeout\n");

  /* Feed 5 times, 1 second apart */

  for (int i = 0; i < 5; i++)
    {
      uint32_t remaining = rk3576_wdt_get_remaining();
      printf("  Feed %d: remaining %u ms\n", i + 1, remaining);

      rk3576_wdt_feed();
      sleep(1);
    }

  /* Stop watchdog */

  rk3576_wdt_stop();
  printf("  Stopped\n");

  printf("\n");
  return OK;
}

static int test_start_stop(void)
{
  printf("=== Watchdog Start/Stop Test ===\n");

  /* Start */

  rk3576_wdt_set_timeout(10000);
  rk3576_wdt_start(0);

  printf("  Started (10s timeout)\n");

  int enabled = rk3576_wdt_is_enabled();
  printf("  Enabled: %s\n", enabled ? "YES" : "NO");

  /* Wait 2 seconds */

  sleep(2);

  /* Stop */

  rk3576_wdt_stop();

  printf("  Stopped\n");

  enabled = rk3576_wdt_is_enabled();
  printf("  Enabled: %s\n", enabled ? "YES" : "NO");

  printf("\n");
  return OK;
}

static int test_remaining(void)
{
  printf("=== Watchdog Remaining Time Test ===\n");

  /* Start with 5 second timeout */

  rk3576_wdt_set_timeout(5000);
  rk3576_wdt_start(0);

  printf("  Started with 5s timeout\n");

  /* Monitor remaining time */

  for (int i = 0; i < 5; i++)
    {
      uint32_t remaining = rk3576_wdt_get_remaining();
      printf("  [%d] Remaining: %u ms\n", i, remaining);
      sleep(1);
    }

  /* Stop */

  rk3576_wdt_stop();
  printf("  Stopped\n");

  printf("\n");
  return OK;
}

static int test_timeout_overflow(void)
{
  printf("=== Watchdog Timeout Boundary Test ===\n");

  /* Test minimum timeout */

  int ret = rk3576_wdt_set_timeout(1);
  printf("  Min (1 ms): %s\n", ret == 0 ? "OK" : "FAIL");

  /* Test maximum timeout */

  ret = rk3576_wdt_set_timeout(128000);
  printf("  Max (128 s): %s\n", ret == 0 ? "OK" : "FAIL");

  /* Test out of range */

  ret = rk3576_wdt_set_timeout(200000);
  printf("  Out of range: %s\n", ret < 0 ? "OK (rejected)" : "FAIL");

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
  printf("  RK3576 Watchdog Timer Test\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Run tests */

  test_init();
  test_timeout();
  test_mode();
  test_feed();
  test_start_stop();
  test_remaining();
  test_timeout_overflow();

  printf("===================================\n");
  printf("  All watchdog tests completed!\n");
  printf("  WARNING: Do NOT test timeout\n");
  printf("  without feeding - system will\n");
  printf("  reset!\n");
  printf("===================================\n");

  return OK;
}
