/****************************************************************************
 * smart_car_headunit/apps/i2s_test/i2s_test_main.c
 *
 * I2S audio test application for KICKPI-K7 (RK3576)
 * Tests I2S master output and input.
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
#include <math.h>

#include "rk3576_i2s.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TEST_I2S_BUS            0
#define SAMPLE_RATE             44100
#define TEST_DURATION_MS        1000
#define SINE_FREQ_HZ            1000

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_config(int bus)
{
  int ret;

  printf("=== I2S Configuration Test (I2S%d) ===\n", bus);

  /* Test sample rates */

  uint32_t rates[] = {8000, 16000, 44100, 48000, 96000};
  int count = sizeof(rates) / sizeof(rates[0]);

  for (int i = 0; i < count; i++)
    {
      ret = rk3576_i2s_set_samplerate(bus, rates[i]);
      if (ret < 0)
        {
          printf("Failed to set rate %u: %d\n", rates[i], ret);
          return ret;
        }

      printf("  Rate %u Hz: OK\n", rates[i]);
    }

  /* Test word sizes */

  int sizes[] = {16, 24, 32};
  count = sizeof(sizes) / sizeof(sizes[0]);

  for (int i = 0; i < count; i++)
    {
      ret = rk3576_i2s_set_bits(bus, sizes[i]);
      if (ret < 0)
        {
          printf("Failed to set %d bits: %d\n", sizes[i], ret);
          return ret;
        }

      printf("  %d bits: OK\n", sizes[i]);
    }

  /* Test channel count */

  ret = rk3576_i2s_set_channels(bus, 1);
  if (ret < 0)
    {
      printf("Failed to set mono: %d\n", ret);
      return ret;
    }

  printf("  Mono: OK\n");

  ret = rk3576_i2s_set_channels(bus, 2);
  if (ret < 0)
    {
      printf("Failed to set stereo: %d\n", ret);
      return ret;
    }

  printf("  Stereo: OK\n");

  /* Reset to default */

  rk3576_i2s_set_samplerate(bus, SAMPLE_RATE);
  rk3576_i2s_set_bits(bus, 16);
  rk3576_i2s_set_channels(bus, 2);

  printf("\n");
  return OK;
}

static int test_role(int bus)
{
  int ret;

  printf("=== I2S Role Test (I2S%d) ===\n", bus);

  ret = rk3576_i2s_set_role(bus, RK3576_I2S_ROLE_MASTER);
  if (ret < 0)
    {
      printf("Failed to set master: %d\n", ret);
      return ret;
    }

  printf("  Master: OK\n");

  ret = rk3576_i2s_set_role(bus, RK3576_I2S_ROLE_SLAVE);
  if (ret < 0)
    {
      printf("Failed to set slave: %d\n", ret);
      return ret;
    }

  printf("  Slave: OK\n");

  /* Reset to master */

  rk3576_i2s_set_role(bus, RK3576_I2S_ROLE_MASTER);

  printf("\n");
  return OK;
}

static int test_sine_wave(int bus)
{
  uint16_t buf[256];
  int samples;
  int ret;

  printf("=== I2S Sine Wave Test (I2S%d) ===\n", bus);
  printf("Generate 1kHz sine wave at %d Hz sample rate\n\n", SAMPLE_RATE);

  /* Configure */

  rk3576_i2s_set_samplerate(bus, SAMPLE_RATE);
  rk3576_i2s_set_bits(bus, 16);
  rk3576_i2s_set_channels(bus, 2);
  rk3576_i2s_set_role(bus, RK3576_I2S_ROLE_MASTER);

  /* Generate sine wave samples */

  samples = SAMPLE_RATE * TEST_DURATION_MS / 1000;
  if (samples > sizeof(buf) / sizeof(buf[0]))
    {
      samples = sizeof(buf) / sizeof(buf[0]);
    }

  printf("  Generating %d samples (%d ms)...\n", samples, TEST_DURATION_MS);

  for (int i = 0; i < samples; i++)
    {
      /* 16-bit signed amplitude */

      int16_t val = (int16_t)(32767.0 * sin(2.0 * M_PI * SINE_FREQ_HZ * i / SAMPLE_RATE));

      /* Interleave L/R channels */

      buf[i * 2] = (uint16_t)val;      /* Left */
      buf[i * 2 + 1] = (uint16_t)val;  /* Right */
    }

  /* Send */

  ret = rk3576_i2s_send(bus, buf, samples * 2);
  if (ret < 0)
    {
      printf("  Send failed: %d\n\n", ret);
      return ret;
    }

  printf("  Sent %d samples\n\n", samples * 2);

  return OK;
}

static int test_silence(int bus)
{
  uint16_t buf[128];
  int ret;

  printf("=== I2S Silence Test (I2S%d) ===\n", bus);

  /* Configure */

  rk3576_i2s_set_samplerate(bus, SAMPLE_RATE);
  rk3576_i2s_set_bits(bus, 16);
  rk3576_i2s_set_channels(bus, 2);

  /* Send silence */

  memset(buf, 0, sizeof(buf));
  int samples = sizeof(buf) / sizeof(buf[0]);

  printf("  Sending %d silent samples...\n", samples);

  ret = rk3576_i2s_send(bus, buf, samples);
  if (ret < 0)
    {
      printf("  Send failed: %d\n\n", ret);
      return ret;
    }

  printf("  Silence sent OK\n\n");
  return OK;
}

static int test_dma(int bus)
{
  printf("=== I2S DMA Configuration Test (I2S%d) ===\n", bus);

  /* Enable DMA for TX */

  rk3576_i2s_dma_enable(bus, true, false);
  printf("  TX DMA: enabled\n");

  /* Enable DMA for RX */

  rk3576_i2s_dma_enable(bus, false, true);
  printf("  RX DMA: enabled\n");

  /* Disable all DMA */

  rk3576_i2s_dma_disable(bus, true, true);
  printf("  All DMA: disabled\n");

  printf("\n");
  return OK;
}

static int test_loopback(int bus)
{
  uint16_t tx_buf[128];
  uint16_t rx_buf[128];
  int ret;

  printf("=== I2S Loopback Test (I2S%d) ===\n", bus);
  printf("Connect I2S TX to I2S RX\n\n");

  /* Configure */

  rk3576_i2s_set_samplerate(bus, SAMPLE_RATE);
  rk3576_i2s_set_bits(bus, 16);
  rk3576_i2s_set_channels(bus, 2);

  /* Generate test pattern */

  for (int i = 0; i < sizeof(tx_buf) / sizeof(tx_buf[0]); i++)
    {
      tx_buf[i] = (uint16_t)i;
    }

  memset(rx_buf, 0, sizeof(rx_buf));

  /* Enable both TX and RX */

  rk3576_i2s_set_mode(bus, RK3576_I2S_MODE_BOTH);
  rk3576_i2s_start(bus);

  /* Send and receive simultaneously */

  int len = sizeof(tx_buf) / sizeof(tx_buf[0]);
  int tx_idx = 0;
  int rx_idx = 0;

  printf("  Transferring %d samples...\n", len);

  while (tx_idx < len || rx_idx < len)
    {
      /* Send if TX FIFO has space */

      if (tx_idx < len)
        {
          uint32_t fifolr = getreg32(g_i2s_base[bus] + I2S_TXFIFOLR);
          if (!(fifolr & (1 << 8)))   /* TXFF bit */
            {
              putreg32(tx_buf[tx_idx], g_i2s_base[bus] + I2S_TXDR);
              tx_idx++;
            }
        }

      /* Receive if RX FIFO has data */

      if (rx_idx < len)
        {
          uint32_t fifolr = getreg32(g_i2s_base[bus] + I2S_RXFIFOLR);
          if (fifolr & 0x3f)   /* RXFL > 0 */
            {
              rx_buf[rx_idx] = (uint16_t)(getreg32(g_i2s_base[bus] + I2S_RXDR) & 0xffff);
              rx_idx++;
            }
        }
    }

  rk3576_i2s_stop(bus);

  /* Verify */

  int pass = 1;
  for (int i = 0; i < len; i++)
    {
      if (tx_buf[i] != rx_buf[i])
        {
          printf("  Mismatch at sample %d: TX=0x%04x, RX=0x%04x\n",
                 i, tx_buf[i], rx_buf[i]);
          pass = 0;
          if (i > 10)
            {
              break;
            }
        }
    }

  printf("  Verify: %s\n\n", pass ? "PASS" : "FAIL");

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
  printf("\n");
  printf("===================================\n");
  printf("  RK3576 I2S Audio Test\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Initialize I2S */

  rk3576_i2s_init(TEST_I2S_BUS);
  printf("I2S%d initialized\n\n", TEST_I2S_BUS);

  /* Run tests */

  test_config(TEST_I2S_BUS);
  test_role(TEST_I2S_BUS);
  test_dma(TEST_I2S_BUS);
  test_silence(TEST_I2S_BUS);
  test_sine_wave(TEST_I2S_BUS);
  test_loopback(TEST_I2S_BUS);

  printf("===================================\n");
  printf("  All I2S tests completed!\n");
  printf("  Connect DAC/ADC to verify audio\n");
  printf("===================================\n");

  return OK;
}
