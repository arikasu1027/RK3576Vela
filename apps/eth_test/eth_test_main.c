/****************************************************************************
 * smart_car_headunit/apps/eth_test/eth_test_main.c
 *
 * Ethernet test application for KICKPI-K7 (RK3576)
 * Tests Ethernet MAC controller and MDIO PHY access.
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

#include "rk3576_eth.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TEST_ETH_PORT           0
#define TEST_PHY_ADDR           1

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_init(int port)
{
  printf("=== Ethernet Init Test (ETH%d) ===\n", port);

  rk3576_eth_init(port);
  printf("  Init: OK\n");

  int status = rk3576_eth_is_link_up(port);
  printf("  Link: %s\n", status == 1 ? "UP" : "DOWN");

  printf("\n");
  return OK;
}

static int test_interface(int port)
{
  int ret;

  printf("=== Ethernet Interface Test (ETH%d) ===\n", port);

  int ifaces[] = {RK3576_ETH_IF_MII, RK3576_ETH_IF_RMII, RK3576_ETH_IF_RGMII};
  const char *names[] = {"MII", "RMII", "RGMII"};

  for (int i = 0; i < 3; i++)
    {
      ret = rk3576_eth_set_interface(port, ifaces[i]);
      if (ret < 0)
        {
          printf("Failed to set %s: %d\n", names[i], ret);
          return ret;
        }

      printf("  %s: OK\n", names[i]);
    }

  /* Reset to RGMII */

  rk3576_eth_set_interface(port, RK3576_ETH_IF_RGMII);

  printf("\n");
  return OK;
}

static int test_speed_duplex(int port)
{
  int ret;

  printf("=== Ethernet Speed/Duplex Test (ETH%d) ===\n", port);

  /* Test speeds */

  int speeds[] = {RK3576_ETH_SPEED_10M, RK3576_ETH_SPEED_100M, RK3576_ETH_SPEED_1000M};
  const char *speed_names[] = {"10 Mbps", "100 Mbps", "1000 Mbps"};

  for (int i = 0; i < 3; i++)
    {
      ret = rk3576_eth_set_speed(port, speeds[i]);
      if (ret < 0)
        {
          printf("Failed to set %s: %d\n", speed_names[i], ret);
          return ret;
        }

      printf("  %s: OK\n", speed_names[i]);
    }

  /* Test duplex */

  ret = rk3576_eth_set_duplex(port, RK3576_ETH_HALF_DUPLEX);
  if (ret < 0)
    {
      printf("Failed to set half duplex: %d\n", ret);
      return ret;
    }

  printf("  Half Duplex: OK\n");

  ret = rk3576_eth_set_duplex(port, RK3576_ETH_FULL_DUPLEX);
  if (ret < 0)
    {
      printf("Failed to set full duplex: %d\n", ret);
      return ret;
    }

  printf("  Full Duplex: OK\n");

  printf("\n");
  return OK;
}

static int test_macaddr(int port)
{
  int ret;

  printf("=== Ethernet MAC Address Test (ETH%d) ===\n", port);

  /* Set custom MAC address */

  uint8_t mac[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};

  ret = rk3576_eth_set_macaddr(port, mac);
  if (ret < 0)
    {
      printf("Failed to set MAC: %d\n", ret);
      return ret;
    }

  printf("  MAC: 02:00:00:00:00:01 OK\n");

  printf("\n");
  return OK;
}

static int test_mdio(int port)
{
  int ret;
  uint16_t val;

  printf("=== Ethernet MDIO Test (ETH%d) ===\n", port);

  /* Read PHY basic registers */

  printf("  Reading PHY %d registers:\n", TEST_PHY_ADDR);

  for (int reg = 0; reg < 8; reg++)
    {
      val = rk3576_eth_mdio_read(port, TEST_PHY_ADDR, reg);
      if (val < 0)
        {
          printf("    REG %d: ERROR (%d)\n", reg, val);
        }
      else
        {
          printf("    REG %d: 0x%04x", reg, val);

          /* Decode some common registers */

          switch (reg)
            {
              case 0:
                printf(" (BMCR: %s%s%s)",
                       (val & 0x8000) ? "RESET " : "",
                       (val & 0x4000) ? "LOOPBACK " : "",
                       (val & 0x2000) ? "100MB " : "10MB ");
                break;
              case 1:
                printf(" (BMSR: %s%s%s)",
                       (val & 0x0004) ? "LINK " : "",
                       (val & 0x0020) ? "AUTO-NEG " : "",
                       (val & 0x0008) ? "FD " : "HD ");
                break;
              case 2:
                printf(" (PHY ID High)");
                break;
              case 3:
                printf(" (PHY ID Low)");
                break;
            }

          printf("\n");
        }
    }

  /* Test PHY address range */

  printf("\n  Scanning for PHYs:\n");

  for (int phy = 0; phy < 32; phy++)
    {
      val = rk3576_eth_mdio_read(port, phy, 0);
      if (val >= 0)
        {
          printf("    PHY %d: Found (BMCR=0x%04x)\n", phy, val);
        }
    }

  printf("\n");
  return OK;
}

static int test_start_stop(int port)
{
  printf("=== Ethernet Start/Stop Test (ETH%d) ===\n", port);

  /* Start */

  rk3576_eth_start(port);
  printf("  Started\n");

  /* Check link */

  int link = rk3576_eth_is_link_up(port);
  printf("  Link: %s\n", link ? "UP" : "DOWN");

  /* Stop */

  rk3576_eth_stop(port);
  printf("  Stopped\n");

  printf("\n");
  return OK;
}

static int test_both_ports(void)
{
  printf("=== Ethernet Dual Port Test ===\n");

  /* Init both ports */

  rk3576_eth_init(0);
  printf("  ETH0: OK\n");

  rk3576_eth_init(1);
  printf("  ETH1: OK\n");

  /* Start both */

  rk3576_eth_start(0);
  printf("  ETH0: Started\n");

  rk3576_eth_start(1);
  printf("  ETH1: Started\n");

  /* Stop both */

  rk3576_eth_stop(0);
  rk3576_eth_stop(1);

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
  printf("  RK3576 Ethernet MAC Test\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Run tests */

  test_init(TEST_ETH_PORT);
  test_interface(TEST_ETH_PORT);
  test_speed_duplex(TEST_ETH_PORT);
  test_macaddr(TEST_ETH_PORT);
  test_mdio(TEST_ETH_PORT);
  test_start_stop(TEST_ETH_PORT);
  test_both_ports();

  printf("===================================\n");
  printf("  All Ethernet tests completed!\n");
  printf("  Connect cable to verify link\n");
  printf("===================================\n");

  return OK;
}
