/****************************************************************************
 * smart_car_headunit/apps/hdmi_test/hdmi_test_main.c
 *
 * HDMI test application for KICKPI-K7 (RK3576)
 * Tests HDMI display output and hotplug detection.
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

#include "rk3576_hdmi.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define HDMI_RESOLUTION_COUNT  6

static const char *g_resolution_names[] =
{
  "640x480",
  "800x600",
  "1024x768",
  "1280x720",
  "1280x1024",
  "1920x1080",
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_init(void)
{
  printf("=== HDMI Init Test ===\n");

  rk3576_hdmi_init();
  printf("  Init: OK\n");

  /* Check connection */

  int connected = rk3576_hdmi_is_connected();
  printf("  Display: %s\n", connected ? "Connected" : "Not connected");

  printf("\n");
  return OK;
}

static int test_resolution(void)
{
  int ret;

  printf("=== HDMI Resolution Test ===\n");

  for (int i = 0; i < HDMI_RESOLUTION_COUNT; i++)
    {
      ret = rk3576_hdmi_set_resolution(i);
      if (ret < 0)
        {
          printf("Failed to set %s: %d\n", g_resolution_names[i], ret);
          return ret;
        }

      printf("  %s: OK\n", g_resolution_names[i]);
    }

  /* Reset to 1280x720 */

  rk3576_hdmi_set_resolution(RK3576_HDMI_RES_1280x720);

  printf("\n");
  return OK;
}

static int test_format(void)
{
  int ret;

  printf("=== HDMI Format Test ===\n");

  int formats[] =
  {
    RK3576_HDMI_FMT_RGB888,
    RK3576_HDMI_FMT_RGB565,
    RK3576_HDMI_FMT_YCBCR444,
    RK3576_HDMI_FMT_YCBCR422,
  };

  const char *format_names[] =
  {
    "RGB888",
    "RGB565",
    "YCbCr444",
    "YCbCr422",
  };

  for (int i = 0; i < 4; i++)
    {
      ret = rk3576_hdmi_set_format(formats[i]);
      if (ret < 0)
        {
          printf("Failed to set %s: %d\n", format_names[i], ret);
          return ret;
        }

      printf("  %s: OK\n", format_names[i]);
    }

  /* Reset to RGB888 */

  rk3576_hdmi_set_format(RK3576_HDMI_FMT_RGB888);

  printf("\n");
  return OK;
}

static int test_mode(void)
{
  int ret;

  printf("=== HDMI Mode Test ===\n");

  ret = rk3576_hdmi_set_mode(RK3576_HDMI_MODE_HDMI);
  if (ret < 0)
    {
      printf("Failed to set HDMI mode: %d\n", ret);
      return ret;
    }

  printf("  HDMI mode: OK\n");

  ret = rk3576_hdmi_set_mode(RK3576_HDMI_MODE_DVI);
  if (ret < 0)
    {
      printf("Failed to set DVI mode: %d\n", ret);
      return ret;
    }

  printf("  DVI mode: OK\n");

  /* Reset to HDMI */

  rk3576_hdmi_set_mode(RK3576_HDMI_MODE_HDMI);

  printf("\n");
  return OK;
}

static int test_enable_disable(void)
{
  printf("=== HDMI Enable/Disable Test ===\n");

  /* Set resolution */

  rk3576_hdmi_set_resolution(RK3576_HDMI_RES_1920x1080);

  /* Enable */

  rk3576_hdmi_enable();
  printf("  Enabled (1920x1080)\n");

  /* Wait 2 seconds */

  sleep(2);

  /* Disable */

  rk3576_hdmi_disable();
  printf("  Disabled\n");

  printf("\n");
  return OK;
}

static int test_hotplug(void)
{
  printf("=== HDMI Hotplug Test ===\n");
  printf("Connect/disconnect HDMI cable during this test\n\n");

  for (int i = 0; i < 10; i++)
    {
      int connected = rk3576_hdmi_is_connected();
      printf("  [%d] Display: %s\n", i, connected ? "Connected" : "Not connected");
      sleep(1);
    }

  printf("\n");
  return OK;
}

static int test_edid(void)
{
  uint8_t edid[128];
  int ret;

  printf("=== HDMI EDID Read Test ===\n");

  ret = rk3576_hdmi_read_edid(edid, sizeof(edid));
  if (ret < 0)
    {
      printf("Failed to read EDID: %d\n", ret);
      return ret;
    }

  /* Print EDID header */

  printf("  EDID Header: ");
  for (int i = 0; i < 8; i++)
    {
      printf("%02x ", edid[i]);
    }

  printf("\n");

  /* Print version */

  printf("  EDID Version: %d.%d\n", edid[18], edid[19]);

  printf("\n");
  return OK;
}

static int test_resolution_cycle(void)
{
  printf("=== HDMI Resolution Cycle Test ===\n");
  printf("Cycle through all resolutions (2 seconds each)\n\n");

  for (int i = 0; i < HDMI_RESOLUTION_COUNT; i++)
    {
      printf("  Setting %s...\n", g_resolution_names[i]);

      rk3576_hdmi_set_resolution(i);
      rk3576_hdmi_enable();

      sleep(2);

      rk3576_hdmi_disable();
    }

  /* Reset to default */

  rk3576_hdmi_set_resolution(RK3576_HDMI_RES_1280x720);
  rk3576_hdmi_enable();

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
  printf("  RK3576 HDMI Display Test\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Run tests */

  test_init();
  test_resolution();
  test_format();
  test_mode();
  test_enable_disable();
  test_hotplug();
  test_edid();
  test_resolution_cycle();

  printf("===================================\n");
  printf("  All HDMI tests completed!\n");
  printf("  Connect monitor to verify\n");
  printf("===================================\n");

  return OK;
}
