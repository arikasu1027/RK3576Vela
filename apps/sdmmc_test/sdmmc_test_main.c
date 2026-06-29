/****************************************************************************
 * smart_car_headunit/apps/sdmmc_test/sdmmc_test_main.c
 *
 * SDMMC test application for KICKPI-K7 (RK3576)
 * Tests SD card and eMMC functionality.
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

#include "rk3576_sdmmc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TEST_SDMMC_CTRL     0
#define TEST_SECTOR         0
#define TEST_SECTOR_COUNT   4
#define TEST_SECTOR_SIZE    512

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_init(void)
{
  int ret;

  printf("=== SDMMC Init Test ===\n");

  ret = rk3576_sdmmc_init(TEST_SDMMC_CTRL);
  if (ret < 0)
    {
      printf("Failed to init SDMMC%d: %d\n", TEST_SDMMC_CTRL, ret);
      return ret;
    }

  printf("  Init: OK\n");

  /* Check card type */

  int type = rk3576_sdmmc_card_type(TEST_SDMMC_CTRL);
  const char *type_str = "None";

  switch (type)
    {
      case CARD_TYPE_MMC:
        type_str = "MMC";
        break;
      case CARD_TYPE_SD:
        type_str = "SD";
        break;
      case CARD_TYPE_SDIO:
        type_str = "SDIO";
        break;
    }

  printf("  Card type: %s\n", type_str);

  printf("\n");
  return OK;
}

static int test_card_detect(void)
{
  int detected;

  printf("=== SDMMC Card Detect Test ===\n");

  detected = rk3576_sdmmc_card_detect(TEST_SDMMC_CTRL);
  printf("  Card detected: %s\n", detected ? "YES" : "NO");

  printf("\n");
  return OK;
}

static int test_read(void)
{
  uint8_t buf[TEST_SECTOR_SIZE * TEST_SECTOR_COUNT];
  int ret;

  printf("=== SDMMC Read Test ===\n");

  /* Read sectors */

  memset(buf, 0, sizeof(buf));

  ret = rk3576_sdmmc_read(TEST_SDMMC_CTRL, TEST_SECTOR,
                           TEST_SECTOR_COUNT, buf);
  if (ret < 0)
    {
      printf("Failed to read: %d\n", ret);
      return ret;
    }

  printf("  Read %d bytes from sector %d\n", ret, TEST_SECTOR);

  /* Print first 64 bytes */

  printf("  First 64 bytes:\n  ");
  for (int i = 0; i < 64 && i < ret; i++)
    {
      printf("%02x ", buf[i]);
      if ((i + 1) % 16 == 0)
        {
          printf("\n  ");
        }
    }

  printf("\n");

  /* Check for common boot signatures */

  if (buf[510] == 0x55 && buf[511] == 0xaa)
    {
      printf("  MBR signature found (0x55AA)\n");
    }
  else
    {
      printf("  No MBR signature\n");
    }

  printf("\n");
  return OK;
}

static int test_write_read(void)
{
  uint8_t write_buf[TEST_SECTOR_SIZE];
  uint8_t read_buf[TEST_SECTOR_SIZE];
  int ret;

  printf("=== SDMMC Write/Read Test ===\n");
  printf("  WARNING: This will write to sector 1000!\n");
  printf("  Make sure the card doesn't contain important data.\n\n");

  /* Fill write buffer with pattern */

  for (int i = 0; i < TEST_SECTOR_SIZE; i++)
    {
      write_buf[i] = (uint8_t)(i & 0xff);
    }

  /* Write to sector 1000 */

  ret = rk3576_sdmmc_write(TEST_SDMMC_CTRL, 1000, 1, write_buf);
  if (ret < 0)
    {
      printf("Failed to write: %d\n", ret);
      return ret;
    }

  printf("  Written %d bytes to sector 1000\n", ret);

  /* Read back */

  memset(read_buf, 0, sizeof(read_buf));
  ret = rk3576_sdmmc_read(TEST_SDMMC_CTRL, 1000, 1, read_buf);
  if (ret < 0)
    {
      printf("Failed to read back: %d\n", ret);
      return ret;
    }

  printf("  Read back %d bytes\n", ret);

  /* Verify */

  int pass = 1;
  for (int i = 0; i < TEST_SECTOR_SIZE; i++)
    {
      if (write_buf[i] != read_buf[i])
        {
          printf("  Mismatch at byte %d: wrote 0x%02x, read 0x%02x\n",
                 i, write_buf[i], read_buf[i]);
          pass = 0;
          if (i > 10)
            {
              break;
            }
        }
    }

  printf("  Verify: %s\n", pass ? "PASS" : "FAIL");

  printf("\n");
  return OK;
}

static int test_performance(void)
{
  uint8_t buf[512 * 256];   /* 128KB */
  int ret;

  printf("=== SDMMC Performance Test ===\n");

  /* Read 128KB (256 sectors) */

  printf("  Reading 128KB...\n");

  uint32_t start = up_perf_gettime();

  ret = rk3576_sdmmc_read(TEST_SDMMC_CTRL, 0, 256, buf);

  uint32_t elapsed = up_perf_gettime() - start;

  if (ret < 0)
    {
      printf("Failed to read: %d\n", ret);
      return ret;
    }

  printf("  Read %d bytes in %u ticks\n", ret, elapsed);
  printf("  Throughput: %u KB/s\n",
         (ret / 1024) * up_perf_convert(1, 1000) / (elapsed ? elapsed : 1));

  printf("\n");
  return OK;
}

static int test_multi_controllers(void)
{
  printf("=== SDMMC Multi-Controller Test ===\n");

  /* Init SDMMC0 (SD card) */

  int ret = rk3576_sdmmc_init(0);
  printf("  SDMMC0: %s\n", ret == 0 ? "OK" : "FAIL");

  /* Init SDMMC1 (SD card) */

  ret = rk3576_sdmmc_init(1);
  printf("  SDMMC1: %s\n", ret == 0 ? "OK" : "FAIL");

  /* Init eMMC */

  ret = rk3576_sdmmc_init(2);
  printf("  eMMC:   %s\n", ret == 0 ? "OK" : "FAIL");

  /* Check card types */

  for (int i = 0; i < 3; i++)
    {
      int type = rk3576_sdmmc_card_type(i);
      const char *type_str = "None";

      switch (type)
        {
          case CARD_TYPE_MMC:
            type_str = "MMC";
            break;
          case CARD_TYPE_SD:
            type_str = "SD";
            break;
          case CARD_TYPE_SDIO:
            type_str = "SDIO";
            break;
        }

      printf("  Controller %d: %s\n", i, type_str);
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
  printf("  RK3576 SDMMC/eMMC Test\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Run tests */

  test_init();
  test_card_detect();
  test_read();
  test_write_read();
  test_performance();
  test_multi_controllers();

  printf("===================================\n");
  printf("  All SDMMC tests completed!\n");
  printf("===================================\n");

  return OK;
}
