/****************************************************************************
 * smart_car_headunit/apps/spi_test/spi_test_main.c
 *
 * SPI test application for KICKPI-K7 (RK3576)
 * Tests SPI master communication with various devices.
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

#include "rk3576_spi.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TEST_SPI_BUS            0

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_mode_config(int bus)
{
  int ret;

  printf("=== SPI Mode Configuration Test (SPI%d) ===\n", bus);

  /* Test all 4 SPI modes */

  for (int mode = 0; mode <= 3; mode++)
    {
      ret = rk3576_spi_set_mode(bus, mode);
      if (ret < 0)
        {
          printf("Failed to set mode %d: %d\n", mode, ret);
          return ret;
        }

      printf("  Mode %d (CPOL=%d, CPHA=%d): OK\n",
             mode, (mode >> 1) & 1, mode & 1);
    }

  /* Reset to mode 0 */

  rk3576_spi_set_mode(bus, RK3576_SPI_MODE_0);
  printf("\n");

  return OK;
}

static int test_word_size(int bus)
{
  int ret;

  printf("=== SPI Word Size Test (SPI%d) ===\n", bus);

  /* Test different word sizes */

  int sizes[] = {8, 16};
  int count = sizeof(sizes) / sizeof(sizes[0]);

  for (int i = 0; i < count; i++)
    {
      ret = rk3576_spi_set_bits(bus, sizes[i]);
      if (ret < 0)
        {
          printf("Failed to set %d bits: %d\n", sizes[i], ret);
          return ret;
        }

      printf("  %d bits: OK\n", sizes[i]);
    }

  /* Reset to 8-bit */

  rk3576_spi_set_bits(bus, 8);
  printf("\n");

  return OK;
}

static int test_frequency(int bus)
{
  int ret;

  printf("=== SPI Frequency Test (SPI%d) ===\n", bus);

  uint32_t freqs[] = {1000000, 5000000, 10000000, 20000000};
  int count = sizeof(freqs) / sizeof(freqs[0]);

  for (int i = 0; i < count; i++)
    {
      ret = rk3576_spi_set_frequency(bus, freqs[i]);
      if (ret < 0)
        {
          printf("Failed to set %u Hz: %d\n", freqs[i], ret);
          return ret;
        }

      printf("  %u MHz: OK\n", freqs[i] / 1000000);
    }

  /* Reset to 1 MHz */

  rk3576_spi_set_frequency(bus, 1000000);
  printf("\n");

  return OK;
}

static int test_loopback(int bus)
{
  uint8_t tx_buf[16];
  uint8_t rx_buf[16];
  int ret;

  printf("=== SPI Loopback Test (SPI%d) ===\n", bus);
  printf("Connect MOSI to MISO with a jumper wire\n\n");

  /* Configure: 8-bit, mode 0, 1 MHz */

  rk3576_spi_set_mode(bus, RK3576_SPI_MODE_0);
  rk3576_spi_set_bits(bus, 8);
  rk3576_spi_set_frequency(bus, 1000000);

  /* Prepare test pattern */

  for (int i = 0; i < sizeof(tx_buf); i++)
    {
      tx_buf[i] = i;
    }

  memset(rx_buf, 0, sizeof(rx_buf));

  /* Exchange */

  ret = rk3576_spi_exchange(bus, tx_buf, rx_buf, sizeof(tx_buf));
  if (ret < 0)
    {
      printf("Exchange failed: %d\n\n", ret);
      return ret;
    }

  /* Verify */

  int pass = 1;
  for (int i = 0; i < sizeof(tx_buf); i++)
    {
      if (tx_buf[i] != rx_buf[i])
        {
          printf("  Mismatch at byte %d: TX=0x%02x, RX=0x%02x\n",
                 i, tx_buf[i], rx_buf[i]);
          pass = 0;
        }
    }

  if (pass)
    {
      printf("  Verify: PASS (sent=received)\n\n");
    }
  else
    {
      printf("  Verify: FAIL\n\n");
    }

  return OK;
}

static int test_sendrecv(int bus)
{
  uint8_t tx_data[] = {0xAA, 0x55, 0xFF, 0x00, 0xA5, 0x5A};
  uint8_t rx_data[sizeof(tx_data)];

  printf("=== SPI SendRecv Test (SPI%d) ===\n", bus);
  printf("Connect MOSI to MISO with a jumper wire\n\n");

  rk3576_spi_set_mode(bus, RK3576_SPI_MODE_0);
  rk3576_spi_set_bits(bus, 8);
  rk3576_spi_set_frequency(bus, 1000000);

  printf("  Sent:     ");
  for (int i = 0; i < sizeof(tx_data); i++)
    {
      printf("%02x ", tx_data[i]);
    }

  printf("\n");

  /* Send each byte individually */

  for (int i = 0; i < sizeof(tx_data); i++)
    {
      rx_data[i] = rk3576_spi_sendrecv(bus, tx_data[i]);
    }

  printf("  Received: ");
  for (int i = 0; i < sizeof(rx_data); i++)
    {
      printf("%02x ", rx_data[i]);
    }

  printf("\n");

  /* Verify */

  if (memcmp(tx_data, rx_data, sizeof(tx_data)) == 0)
    {
      printf("  Verify: PASS\n\n");
    }
  else
    {
      printf("  Verify: FAIL\n\n");
    }

  return OK;
}

static int test_w25qxx_id(int bus)
{
  uint8_t cmd;
  uint8_t id[3];

  printf("=== W25Qxx SPI Flash ID Read (SPI%d) ===\n", bus);
  printf("Connect W25Qxx flash: CS->GPIO, MOSI/MISO/CLK->SPI\n\n");

  /* Configure: 8-bit, mode 0, 1 MHz */

  rk3576_spi_set_mode(bus, RK3576_SPI_MODE_0);
  rk3576_spi_set_bits(bus, 8);
  rk3576_spi_set_frequency(bus, 1000000);

  /* Send JEDEC ID command (0x9F) */

  cmd = 0x9f;
  rk3576_spi_send(bus, &cmd, 1);

  /* Read 3 ID bytes */

  rk3576_spi_recv(bus, id, 3);

  printf("  JEDEC ID: %02x %02x %02x\n", id[0], id[1], id[2]);

  /* Decode manufacturer */

  switch (id[0])
    {
      case 0xef:
        printf("  Manufacturer: Winbond\n");
        break;
      case 0xc8:
        printf("  Manufacturer: GigaDevice\n");
        break;
      case 0x20:
        printf("  Manufacturer: Micron\n");
        break;
      case 0x01:
        printf("  Manufacturer: Spansion\n");
        break;
      case 0xc2:
        printf("  Manufacturer: Macronix\n");
        break;
      default:
        printf("  Manufacturer: Unknown (0x%02x)\n", id[0]);
        break;
    }

  printf("\n");

  return OK;
}

static int test_performance(int bus)
{
  uint8_t tx_buf[256];
  uint8_t rx_buf[256];
  int ret;
  uint32_t bytes = 0;

  printf("=== SPI Performance Test (SPI%d) ===\n", bus);

  /* Configure: 8-bit, mode 0, 10 MHz */

  rk3576_spi_set_mode(bus, RK3576_SPI_MODE_0);
  rk3576_spi_set_bits(bus, 8);
  rk3576_spi_set_frequency(bus, 10000000);

  memset(tx_buf, 0xa5, sizeof(tx_buf));

  printf("  Transferring 256KB at 10 MHz...\n");

  /* Transfer 256KB (1024 x 256 bytes) */

  uint32_t start = up_perf_gettime();

  for (int i = 0; i < 1024; i++)
    {
      ret = rk3576_spi_exchange(bus, tx_buf, rx_buf, sizeof(tx_buf));
      if (ret < 0)
        {
          printf("  Transfer failed at iteration %d: %d\n", i, ret);
          return ret;
        }

      bytes += sizeof(tx_buf);
    }

  uint32_t elapsed = up_perf_gettime() - start;
  uint32_t elapsed_ms = elapsed / (up_perf_convert(1, 1000));

  printf("  Transferred: %u bytes\n", bytes);
  printf("  Time:        %u ms\n", elapsed_ms);
  printf("  Throughput:  %u KB/s\n\n",
         (bytes / 1024) * 1000 / (elapsed_ms ? elapsed_ms : 1));

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
  printf("\n");
  printf("===================================\n");
  printf("  RK3576 SPI Test Application\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Initialize SPI bus */

  rk3576_spi_init(TEST_SPI_BUS);
  printf("SPI%d initialized\n\n", TEST_SPI_BUS);

  /* Run tests */

  test_mode_config(TEST_SPI_BUS);
  test_word_size(TEST_SPI_BUS);
  test_frequency(TEST_SPI_BUS);
  test_loopback(TEST_SPI_BUS);
  test_sendrecv(TEST_SPI_BUS);
  test_w25qxx_id(TEST_SPI_BUS);
  test_performance(TEST_SPI_BUS);

  printf("===================================\n");
  printf("  All SPI tests completed!\n");
  printf("===================================\n");

  return OK;
}
