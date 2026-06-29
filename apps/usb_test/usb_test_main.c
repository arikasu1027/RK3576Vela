/****************************************************************************
 * smart_car_headunit/apps/usb_test/usb_test_main.c
 *
 * USB test application for KICKPI-K7 (RK3576)
 * Tests USB controller initialization and mode switching.
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

#include "rk3576_usb.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TEST_USB_PORT           0

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_init_deinit(int port)
{
  printf("=== USB Init/Deinit Test (USB%d) ===\n", port);

  /* Initialize */

  rk3576_usb_init(port);
  printf("  Init: OK\n");

  /* Check status */

  int status = rk3576_usb_get_status(port);
  printf("  Status: %s\n", status == 0 ? "OK" : "ERROR");

  /* Deinitialize */

  rk3576_usb_deinit(port);
  printf("  Deinit: OK\n");

  printf("\n");
  return OK;
}

static int test_mode_host(int port)
{
  int ret;

  printf("=== USB Host Mode Test (USB%d) ===\n", port);

  /* Initialize */

  rk3576_usb_init(port);

  /* Set host mode */

  ret = rk3576_usb_set_mode(port, RK3576_USB_MODE_HOST);
  if (ret < 0)
    {
      printf("Failed to set host mode: %d\n\n", ret);
      return ret;
    }

  printf("  Mode: Host\n");

  /* Simulate device connection */

  rk3576_usb_host_connect(port);
  printf("  Device connected\n");

  /* Get speed */

  int speed = rk3576_usb_host_get_speed(port);
  printf("  Speed: %s\n", speed == 0 ? "High" : "Full");

  /* Simulate device disconnect */

  rk3576_usb_host_disconnect(port);
  printf("  Device disconnected\n");

  /* Deinit */

  rk3576_usb_deinit(port);

  printf("\n");
  return OK;
}

static int test_mode_device(int port)
{
  int ret;

  printf("=== USB Device Mode Test (USB%d) ===\n", port);

  /* Initialize */

  rk3576_usb_init(port);

  /* Set device mode */

  ret = rk3576_usb_set_mode(port, RK3576_USB_MODE_DEVICE);
  if (ret < 0)
    {
      printf("Failed to set device mode: %d\n\n", ret);
      return ret;
    }

  printf("  Mode: Device\n");

  /* Set address */

  ret = rk3576_usb_device_set_address(port, 1);
  if (ret < 0)
    {
      printf("Failed to set address: %d\n\n", ret);
      return ret;
    }

  printf("  Address: 1\n");

  /* Connect */

  ret = rk3576_usb_device_connect(port);
  if (ret < 0)
    {
      printf("Failed to connect: %d\n\n", ret);
      return ret;
    }

  printf("  Device connected\n");

  /* Disconnect */

  ret = rk3576_usb_device_disconnect(port);
  if (ret < 0)
    {
      printf("Failed to disconnect: %d\n\n", ret);
      return ret;
    }

  printf("  Device disconnected\n");

  /* Deinit */

  rk3576_usb_deinit(port);

  printf("\n");
  return OK;
}

static int test_mode_otg(int port)
{
  int ret;

  printf("=== USB OTG Mode Test (USB%d) ===\n", port);

  /* Initialize */

  rk3576_usb_init(port);

  /* Set OTG mode */

  ret = rk3576_usb_set_mode(port, RK3576_USB_MODE_OTG);
  if (ret < 0)
    {
      printf("Failed to set OTG mode: %d\n\n", ret);
      return ret;
    }

  printf("  Mode: OTG (auto-detect)\n");

  /* Deinit */

  rk3576_usb_deinit(port);

  printf("\n");
  return OK;
}

static int test_speed(int port)
{
  printf("=== USB Speed Test (USB%d) ===\n", port);

  /* Initialize */

  rk3576_usb_init(port);

  /* Set host mode */

  rk3576_usb_set_mode(port, RK3576_USB_MODE_HOST);

  /* Test different speeds */

  printf("  High Speed (480 Mbps): ");
  g_usb_dev[port].speed = RK3576_USB_SPEED_HIGH;
  printf("%s\n", rk3576_usb_host_get_speed(port) == 0 ? "OK" : "FAIL");

  printf("  Full Speed (12 Mbps):  ");
  g_usb_dev[port].speed = RK3576_USB_SPEED_FULL;
  printf("%s\n", rk3576_usb_host_get_speed(port) == 1 ? "OK" : "FAIL");

  /* Deinit */

  rk3576_usb_deinit(port);

  printf("\n");
  return OK;
}

static int test_both_ports(void)
{
  printf("=== USB Dual Port Test ===\n");

  /* Initialize both ports */

  rk3576_usb_init(0);
  printf("  USB0: OK\n");

  rk3576_usb_init(1);
  printf("  USB1: OK\n");

  /* Set modes */

  rk3576_usb_set_mode(0, RK3576_USB_MODE_HOST);
  printf("  USB0: Host\n");

  rk3576_usb_set_mode(1, RK3576_USB_MODE_DEVICE);
  printf("  USB1: Device\n");

  /* Deinit both */

  rk3576_usb_deinit(0);
  rk3576_usb_deinit(1);

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
  printf("  RK3576 USB Test Application\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Run tests */

  test_init_deinit(TEST_USB_PORT);
  test_mode_host(TEST_USB_PORT);
  test_mode_device(TEST_USB_PORT);
  test_mode_otg(TEST_USB_PORT);
  test_speed(TEST_USB_PORT);
  test_both_ports();

  printf("===================================\n");
  printf("  All USB tests completed!\n");
  printf("  Connect USB device to verify\n");
  printf("===================================\n");

  return OK;
}
