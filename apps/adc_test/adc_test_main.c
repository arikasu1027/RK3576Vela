/****************************************************************************
 * smart_car_headunit/apps/adc_test/adc_test_main.c
 *
 * ADC test application for KICKPI-K7 (RK3576)
 * Tests SARADC input on all available channels.
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

#include "rk3576_adc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TEST_ADC_CHANNEL       0
#define AVG_SAMPLES            16

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_single_read(int channel)
{
  int raw;
  int mv;

  printf("=== Single Read Test (ADC CH%d) ===\n", channel);

  raw = rk3576_adc_read(channel);
  if (raw < 0)
    {
      printf("Failed to read ADC: %d\n", raw);
      return raw;
    }

  mv = (raw * SARADC_VREF_MV) / ((1 << SARADC_RESOLUTION) - 1);

  printf("  Raw:  %d (0x%03X)\n", raw, raw);
  printf("  Volts: %d mV (%d.%03d V)\n",
         mv, mv / 1000, mv % 1000);
  printf(" %%Range: %d%%\n\n", (raw * 100) / 4095);

  return OK;
}

static int test_averaged_read(int channel)
{
  int raw;
  int mv;

  printf("=== Averaged Read Test (ADC CH%d, %d samples) ===\n",
         channel, AVG_SAMPLES);

  raw = rk3576_adc_read_avg(channel, AVG_SAMPLES);
  if (raw < 0)
    {
      printf("Failed to read ADC: %d\n", raw);
      return raw;
    }

  mv = (raw * SARADC_VREF_MV) / ((1 << SARADC_RESOLUTION) - 1);

  printf("  Avg Raw:  %d (0x%03X)\n", raw, raw);
  printf("  Avg Volts: %d mV (%d.%03d V)\n\n",
         mv, mv / 1000, mv % 1000);

  return OK;
}

static int test_all_channels(void)
{
  int ch;
  int raw;
  int mv;

  printf("=== All Channels Scan ===\n");

  for (ch = 0; ch < g_saradc_channels; ch++)
    {
      raw = rk3576_adc_read(ch);
      if (raw < 0)
        {
          printf("  CH%d: ERROR (%d)\n", ch, raw);
          continue;
        }

      mv = (raw * SARADC_VREF_MV) / ((1 << SARADC_RESOLUTION) - 1);

      printf("  CH%d: %4d (0x%03X) = %d.%03d V\n",
             ch, raw, raw, mv / 1000, mv % 1000);
    }

  printf("\n");
  return OK;
}

static int test_continuous_read(int channel, int seconds)
{
  int count = 0;
  int raw;
  int mv;
  int total = 0;
  int min_val = 4095;
  int max_val = 0;

  printf("=== Continuous Read Test (ADC CH%d, %d seconds) ===\n",
         channel, seconds);

  printf("Connect a variable voltage source to see changes.\n");
  printf("Press Ctrl+C to stop.\n\n");

  printf("  Time   Raw    Voltage    Bar\n");
  printf("  ----   ---    -------    ---\n");

  while (count < seconds * 10)
    {
      raw = rk3576_adc_read(channel);
      if (raw < 0)
        {
          usleep(100000);
          continue;
        }

      mv = (raw * SARADC_VREF_MV) / ((1 << SARADC_RESOLUTION) - 1);

      /* Track min/max */

      if (raw < min_val)
        {
          min_val = raw;
        }

      if (raw > max_val)
        {
          max_val = raw;
        }

      total += raw;
      count++;

      /* Print every 100ms */

      printf("\r  %3d.%d  %4d  %d.%03d V  [",
             count / 10, count % 10,
             raw, mv / 1000, mv % 1000);

      /* Draw bar graph */

      int bar_len = (raw * 32) / 4095;
      for (int i = 0; i < 32; i++)
        {
          if (i < bar_len)
            {
              printf("#");
            }
          else
            {
              printf(".");
            }
        }

      printf("]");

      fflush(stdout);
      usleep(100000);
    }

  printf("\n\n");

  /* Print statistics */

  printf("  Samples:  %d\n", count);
  printf("  Average:  %d mV\n",
         ((total / count) * SARADC_VREF_MV) / 4095);
  printf("  Min:      %d mV\n",
         (min_val * SARADC_VREF_MV) / 4095);
  printf("  Max:      %d mV\n",
         (max_val * SARADC_VREF_MV) / 4095);
  printf("  Range:    %d mV\n\n",
         ((max_val - min_val) * SARADC_VREF_MV) / 4095);

  return OK;
}

static int test_voltage_divider(void)
{
  int raw;
  int mv;

  printf("=== Voltage Divider Test ===\n");
  printf("Connect a voltage divider to ADC CH%d:\n", TEST_ADC_CHANNEL);
  printf("  V_IN ---[R1]---+---[R2]--- GND\n");
  printf("                  |\n");
  printf("              ADC CH%d\n", TEST_ADC_CHANNEL);
  printf("\n");
  printf("Configure R1 and R2 values:\n");

  int r1 = 10000;  /* 10k ohm */
  int r2 = 10000;  /* 10k ohm */

  printf("  R1 = %d ohm\n", r1);
  printf("  R2 = %d ohm\n", r2);
  printf("  Divider ratio: R2/(R1+R2) = %d/%d = %d%%\n",
         r2, r1 + r2, (r2 * 100) / (r1 + r2));
  printf("\n");

  raw = rk3576_adc_read(TEST_ADC_CHANNEL);
  if (raw < 0)
    {
      printf("Failed to read ADC: %d\n", raw);
      return raw;
    }

  mv = (raw * SARADC_VREF_MV) / 4095;

  /* Calculate actual input voltage */

  int v_in = (mv * (r1 + r2)) / r2;

  printf("  ADC Raw:   %d\n", raw);
  printf("  ADC Volts: %d mV\n", mv);
  printf("  Input Volts: %d mV (%d.%03d V)\n\n",
         v_in, v_in / 1000, v_in % 1000);

  return OK;
}

static int test_battery_monitor(void)
{
  int raw;
  int mv;

  printf("=== Battery Voltage Monitor ===\n");
  printf("Connect battery through voltage divider to ADC CH0\n");
  printf("Example: 12V battery with 100k/10k divider\n\n");

  raw = rk3576_adc_read(0);
  if (raw < 0)
    {
      printf("Failed to read ADC: %d\n", raw);
      return raw;
    }

  /* Assume 100k/10k divider (ratio = 11) */

  int divider_ratio = 11;
  mv = (raw * SARADC_VREF_MV) / 4095;
  int battery_mv = mv * divider_ratio;
  int battery_v = battery_mv / 1000;

  printf("  ADC Reading: %d mV\n", mv);
  printf("  Battery:     %d mV (%d.%02d V)\n",
         battery_mv, battery_v, (battery_mv % 1000) / 10);

  /* Battery status */

  if (battery_v >= 126)
    {
      printf("  Status: Fully charged (12.6V+)\n");
    }
  else if (battery_v >= 120)
    {
      printf("  Status: Good (12.0-12.6V)\n");
    }
  else if (battery_v >= 115)
    {
      printf("  Status: Fair (11.5-12.0V)\n");
    }
  else if (battery_v >= 110)
    {
      printf("  Status: Low (11.0-11.5V)\n");
    }
  else
    {
      printf("  Status: Critical (<11.0V)\n");
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
  printf("  RK3576 ADC Test Application\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n");
  printf("  Resolution: %d-bit\n", SARADC_RESOLUTION);
  printf("  Reference:  %d mV\n", SARADC_VREF_MV);
  printf("  Channels:   %d\n", g_saradc_channels);
  printf("===================================\n\n");

  /* Initialize ADC subsystem */

  rk3576_adc_init();
  printf("ADC subsystem initialized\n\n");

  /* Run tests */

  test_single_read(TEST_ADC_CHANNEL);
  test_averaged_read(TEST_ADC_CHANNEL);
  test_all_channels();
  test_continuous_read(TEST_ADC_CHANNEL, 5);
  test_battery_monitor();

  printf("===================================\n");
  printf("  All ADC tests completed!\n");
  printf("===================================\n");

  return OK;
}
