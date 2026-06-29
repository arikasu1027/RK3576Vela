/****************************************************************************
 * smart_car_headunit/apps/rtc_test/rtc_test_main.c
 *
 * RTC test application for KICKPI-K7 (RK3576)
 * Tests real-time clock functionality.
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
#include <time.h>

#include "rk3576_rtc.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void print_tm(const struct tm *tm, const char *prefix)
{
  printf("  %s: %04d-%02d-%02d %02d:%02d:%02d (wday=%d, yday=%d)\n",
         prefix,
         tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
         tm->tm_hour, tm->tm_min, tm->tm_sec,
         tm->tm_wday, tm->tm_yday);
}

static int test_init(void)
{
  printf("=== RTC Init Test ===\n");

  rk3576_rtc_init();
  printf("  Init: OK\n");

  printf("\n");
  return OK;
}

static int test_get_time(void)
{
  struct tm tm;
  int ret;

  printf("=== RTC Get Time Test ===\n");

  ret = rk3576_rtc_get_time(&tm);
  if (ret < 0)
    {
      printf("Failed to get time: %d\n", ret);
      return ret;
    }

  print_tm(&tm, "Current");

  printf("\n");
  return OK;
}

static int test_set_time(void)
{
  struct tm tm;
  struct tm readback;
  int ret;

  printf("=== RTC Set Time Test ===\n");

  /* Set time to 2026-06-29 12:00:00 */

  memset(&tm, 0, sizeof(tm));
  tm.tm_year = 126;         /* 2026 - 1900 */
  tm.tm_mon = 5;            /* June (0-based) */
  tm.tm_mday = 29;
  tm.tm_hour = 12;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  tm.tm_wday = 1;           /* Monday */
  tm.tm_isdst = 0;

  ret = rk3576_rtc_set_time(&tm);
  if (ret < 0)
    {
      printf("Failed to set time: %d\n", ret);
      return ret;
    }

  printf("  Set: 2026-06-29 12:00:00\n");

  /* Read back and verify */

  usleep(100000);

  ret = rk3576_rtc_get_time(&readback);
  if (ret < 0)
    {
      printf("Failed to read back: %d\n", ret);
      return ret;
    }

  print_tm(&readback, "Read ");

  /* Verify */

  if (readback.tm_hour == 12 && readback.tm_min == 0 &&
      readback.tm_mday == 29)
    {
      printf("  Verify: PASS\n");
    }
  else
    {
      printf("  Verify: FAIL\n");
    }

  printf("\n");
  return OK;
}

static int test_set_various_times(void)
{
  struct tm tm;
  struct tm readback;
  int ret;

  printf("=== RTC Various Times Test ===\n");

  /* Test midnight */

  memset(&tm, 0, sizeof(tm));
  tm.tm_year = 126;
  tm.tm_mon = 0;
  tm.tm_mday = 1;
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;

  ret = rk3576_rtc_set_time(&tm);
  if (ret < 0)
    {
      printf("Failed to set midnight: %d\n", ret);
      return ret;
    }

  printf("  Midnight: OK\n");

  /* Test end of day */

  memset(&tm, 0, sizeof(tm));
  tm.tm_year = 126;
  tm.tm_mon = 11;
  tm.tm_mday = 31;
  tm.tm_hour = 23;
  tm.tm_min = 59;
  tm.tm_sec = 59;

  ret = rk3576_rtc_set_time(&tm);
  if (ret < 0)
    {
      printf("Failed to set end of day: %d\n", ret);
      return ret;
    }

  printf("  End of day: OK\n");

  /* Test leap year */

  memset(&tm, 0, sizeof(tm));
  tm.tm_year = 128;     /* 2028 (leap year) */
  tm.tm_mon = 1;
  tm.tm_mday = 29;
  tm.tm_hour = 12;
  tm.tm_min = 0;
  tm.tm_sec = 0;

  ret = rk3576_rtc_set_time(&tm);
  if (ret < 0)
    {
      printf("Failed to set leap year: %d\n", ret);
      return ret;
    }

  printf("  Leap year 2028-02-29: OK\n");

  /* Reset to 2026-06-29 */

  memset(&tm, 0, sizeof(tm));
  tm.tm_year = 126;
  tm.tm_mon = 5;
  tm.tm_mday = 29;
  tm.tm_hour = 12;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  rk3576_rtc_set_time(&tm);

  printf("\n");
  return OK;
}

static int test_time_flow(void)
{
  struct tm tm1, tm2;
  int ret;

  printf("=== RTC Time Flow Test ===\n");
  printf("  Monitor time for 5 seconds\n\n");

  /* Read first time */

  ret = rk3576_rtc_get_time(&tm1);
  if (ret < 0)
    {
      printf("Failed: %d\n", ret);
      return ret;
    }

  print_tm(&tm1, "  T1");

  sleep(5);

  /* Read second time */

  ret = rk3576_rtc_get_time(&tm2);
  if (ret < 0)
    {
      printf("Failed: %d\n", ret);
      return ret;
    }

  print_tm(&tm2, "  T2");

  /* Check if 5 seconds elapsed */

  int diff = tm2.tm_sec - tm1.tm_sec;
  if (diff < 0)
    {
      diff += 60;
    }

  printf("  Elapsed: ~%d seconds\n", diff);

  if (diff >= 4 && diff <= 6)
    {
      printf("  Verify: PASS\n");
    }
  else
    {
      printf("  Verify: FAIL (expected ~5s)\n");
    }

  printf("\n");
  return OK;
}

static int test_alarm(void)
{
  struct tm tm;
  int ret;

  printf("=== RTC Alarm Test ===\n");

  /* Set alarm 3 seconds from now */

  ret = rk3576_rtc_get_time(&tm);
  if (ret < 0)
    {
      printf("Failed to get time: %d\n", ret);
      return ret;
    }

  tm.tm_sec += 3;
  if (tm.tm_sec >= 60)
    {
      tm.tm_sec -= 60;
      tm.tm_min++;
      if (tm.tm_min >= 60)
        {
          tm.tm_min = 0;
          tm.tm_hour++;
          if (tm.tm_hour >= 24)
            {
              tm.tm_hour = 0;
              tm.tm_mday++;
            }
        }
    }

  ret = rk3576_rtc_set_alarm(&tm);
  if (ret < 0)
    {
      printf("Failed to set alarm: %d\n", ret);
      return ret;
    }

  printf("  Alarm set to %02d:%02d:%02d\n",
         tm.tm_hour, tm.tm_min, tm.tm_sec);

  /* Wait for alarm */

  printf("  Waiting for alarm...\n");

  for (int i = 0; i < 10; i++)
    {
      /* Check alarm status */

      uint32_t status = getreg32(g_rtc_base + RTC_STATUS);
      if (status & RTC_STATUS_ALARMPENDING)
        {
          printf("  Alarm triggered!\n");
          break;
        }

      sleep(1);
    }

  /* Cancel alarm */

  rk3576_rtc_cancel_alarm();
  printf("  Alarm cancelled\n");

  /* Clear interrupt */

  rk3576_rtc_clear_irq();

  printf("\n");
  return OK;
}

static int test_continuous_read(void)
{
  struct tm tm;
  int ret;

  printf("=== RTC Continuous Read Test ===\n");
  printf("  Read time 10 times, 1 second apart\n\n");

  for (int i = 0; i < 10; i++)
    {
      ret = rk3576_rtc_get_time(&tm);
      if (ret < 0)
        {
          printf("  Read %d: FAILED\n", i);
          return ret;
        }

      printf("  [%d] %02d:%02d:%02d\n", i, tm.tm_hour, tm.tm_min, tm.tm_sec);
      sleep(1);
    }

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
  printf("  RK3576 RTC Real-Time Clock Test\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Run tests */

  test_init();
  test_get_time();
  test_set_time();
  test_set_various_times();
  test_time_flow();
  test_alarm();
  test_continuous_read();

  printf("===================================\n");
  printf("  All RTC tests completed!\n");
  printf("===================================\n");

  return OK;
}
