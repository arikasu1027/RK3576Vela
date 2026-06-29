/****************************************************************************
 * smart_car_headunit/apps/i2c_test/i2c_test_main.c
 *
 * I2C test application for KICKPI-K7 (RK3576)
 * Tests I2C master communication with various devices.
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

#include "rk3576_i2c.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Test I2C bus and addresses */

#define TEST_I2C_BUS            0
#define TEST_I2C_ADDR_7BIT      0x50   /* Common EEPROM address */
#define TEST_I2C_ADDR扫描起始   0x03
#define TEST_I2C_ADDR扫描结束   0x77

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_speed_config(int bus)
{
  int ret;

  printf("=== Speed Configuration Test (I2C%d) ===\n", bus);

  /* Test 100 kHz */

  ret = rk3576_i2c_set_speed(bus, I2C_CLK_100KHZ);
  if (ret < 0)
    {
      printf("Failed to set 100kHz: %d\n", ret);
      return ret;
    }

  printf("  100 kHz: OK\n");

  /* Test 400 kHz */

  ret = rk3576_i2c_set_speed(bus, I2C_CLK_400KHZ);
  if (ret < 0)
    {
      printf("Failed to set 400kHz: %d\n", ret);
      return ret;
    }

  printf("  400 kHz: OK\n");

  /* Test 1 MHz */

  ret = rk3576_i2c_set_speed(bus, I2C_CLK_1MHZ);
  if (ret < 0)
    {
      printf("Failed to set 1MHz: %d\n", ret);
      return ret;
    }

  printf("  1 MHz: OK\n");

  /* Reset to standard speed */

  rk3576_i2c_set_speed(bus, I2C_CLK_100KHZ);
  printf("\n");

  return OK;
}

static int test_addr_scan(int bus)
{
  int addr;
  int found = 0;
  uint8_t dummy;
  int ret;

  printf("=== I2C Address Scan (I2C%d) ===\n", bus);
  printf("Scanning addresses 0x03-0x77...\n\n");

  printf("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

  for (addr = 0x03; addr <= 0x77; addr++)
    {
      if ((addr & 0x0f) == 0)
        {
          printf("%02x: ", addr);
        }

      /* Try to read 1 byte */

      rk3576_i2c_set_address(bus, addr);
      ret = rk3576_i2c_read(bus, &dummy, 1);

      if (ret >= 0)
        {
          printf("%02x ", addr);
          found++;
        }
      else
        {
          printf("-- ");
        }

      if ((addr & 0x0f) == 0x0f)
        {
          printf("\n");
        }

      /* Small delay between probes */

      usleep(1000);
    }

  printf("\nFound %d device(s)\n\n", found);

  return OK;
}

static int test_eeprom_write_read(int bus)
{
  uint8_t wr_buf[8];
  uint8_t rd_buf[8];
  int ret;

  printf("=== EEPROM Write/Read Test (I2C%d, addr 0x%02x) ===\n",
         bus, TEST_I2C_ADDR_7BIT);

  /* Prepare test data */

  for (int i = 0; i < 8; i++)
    {
      wr_buf[i] = 0xA0 + i;
    }

  /* Write to EEPROM at address 0x00 */

  printf("  Writing 8 bytes to address 0x00...\n");
  ret = rk3576_i2c_write_reg(bus, 0x00, wr_buf, 8);
  if (ret < 0)
    {
      printf("  Write failed: %d\n", ret);
      printf("  (No EEPROM at 0x%02x?)\n\n", TEST_I2C_ADDR_7BIT);
      return ret;
    }

  printf("  Write OK\n");

  /* Wait for EEPROM write cycle */

  usleep(10000);

  /* Read back */

  printf("  Reading 8 bytes from address 0x00...\n");
  memset(rd_buf, 0, sizeof(rd_buf));
  ret = rk3576_i2c_read_reg(bus, 0x00, rd_buf, 8);
  if (ret < 0)
    {
      printf("  Read failed: %d\n\n", ret);
      return ret;
    }

  /* Verify */

  printf("  Written: ");
  for (int i = 0; i < 8; i++)
    {
      printf("%02x ", wr_buf[i]);
    }

  printf("\n  Read:    ");
  for (int i = 0; i < 8; i++)
    {
      printf("%02x ", rd_buf[i]);
    }

  printf("\n");

  if (memcmp(wr_buf, rd_buf, 8) == 0)
    {
      printf("  Verify: PASS\n\n");
    }
  else
    {
      printf("  Verify: FAIL\n\n");
    }

  return OK;
}

static int test_sensor_read(int bus)
{
  uint8_t whoami;
  int ret;

  printf("=== Sensor WHO_AM_I Test (I2C%d) ===\n", bus);
  printf("Common sensor addresses:\n");
  printf("  0x68: MPU6050/MPU9250/ICM20948\n");
  printf("  0x76: BME280/BMP280\n");
  printf("  0x3C: SSD1306 OLED\n");
  printf("  0x27: PCF8574 I2C GPIO\n\n");

  /* Try MPU6050 */

  uint8_t addrs[] = {0x68, 0x76, 0x3C, 0x27};
  const char *names[] = {"MPU6050", "BME280", "SSD1306", "PCF8574"};

  for (int i = 0; i < 4; i++)
    {
      rk3576_i2c_set_address(bus, addrs[i]);
      ret = rk3576_i2c_read_reg(bus, 0x75, &whoami, 1);

      if (ret >= 0)
        {
          printf("  Found %s at 0x%02x, WHO_AM_I=0x%02x\n",
                 names[i], addrs[i], whoami);
        }
    }

  printf("\n");
  return OK;
}

static int test_multi_byte(int bus)
{
  uint8_t buf[32];
  int ret;

  printf("=== Multi-Byte Transfer Test (I2C%d) ===\n", bus);

  /* Fill buffer with pattern */

  for (int i = 0; i < sizeof(buf); i++)
    {
      buf[i] = i;
    }

  /* Write 32 bytes */

  printf("  Writing 32 bytes...\n");
  ret = rk3576_i2c_write_reg(bus, 0x10, buf, sizeof(buf));
  if (ret < 0)
    {
      printf("  Write failed: %d\n\n", ret);
      return ret;
    }

  printf("  Write OK\n");

  /* Read back */

  memset(buf, 0, sizeof(buf));
  printf("  Reading 32 bytes...\n");
  ret = rk3576_i2c_read_reg(bus, 0x10, buf, sizeof(buf));
  if (ret < 0)
    {
      printf("  Read failed: %d\n\n", ret);
      return ret;
    }

  /* Verify */

  int pass = 1;
  for (int i = 0; i < sizeof(buf); i++)
    {
      if (buf[i] != (uint8_t)i)
        {
          printf("  Mismatch at byte %d: expected 0x%02x, got 0x%02x\n",
                 i, i, buf[i]);
          pass = 0;
          break;
        }
    }

  printf("  Verify: %s\n\n", pass ? "PASS" : "FAIL");

  return OK;
}

static int test_reset(int bus)
{
  int ret;

  printf("=== I2C Bus Reset Test (I2C%d) ===\n", bus);

  ret = rk3576_i2c_reset(bus);
  if (ret < 0)
    {
      printf("Reset failed: %d\n\n", ret);
      return ret;
    }

  printf("Reset OK\n\n");
  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
  printf("\n");
  printf("===================================\n");
  printf("  RK3576 I2C Test Application\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Initialize I2C bus */

  rk3576_i2c_init(TEST_I2C_BUS);
  printf("I2C%d initialized\n\n", TEST_I2C_BUS);

  /* Run tests */

  test_speed_config(TEST_I2C_BUS);
  test_addr_scan(TEST_I2C_BUS);
  test_sensor_read(TEST_I2C_BUS);
  test_eeprom_write_read(TEST_I2C_BUS);
  test_multi_byte(TEST_I2C_BUS);
  test_reset(TEST_I2C_BUS);

  printf("===================================\n");
  printf("  All I2C tests completed!\n");
  printf("===================================\n");

  return OK;
}
