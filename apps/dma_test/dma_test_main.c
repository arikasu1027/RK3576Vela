/****************************************************************************
 * smart_car_headunit/apps/dma_test/dma_test_main.c
 *
 * DMA test application for KICKPI-K7 (RK3576)
 * Tests DMA controller functionality.
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

#include "rk3576_dmac.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int test_init(void)
{
  printf("=== DMA Init Test ===\n");

  rk3576_dmac_init();
  printf("  Init: OK\n");

  printf("\n");
  return OK;
}

static int test_alloc_free(void)
{
  int ch;

  printf("=== DMA Alloc/Free Test ===\n");

  /* Allocate channels */

  for (int i = 0; i < 8; i++)
    {
      ch = rk3576_dmac_alloc();
      if (ch < 0)
        {
          printf("Failed to allocate channel %d: %d\n", i, ch);
          return ch;
        }

      printf("  Allocated channel: %d\n", ch);
    }

  /* Try to allocate one more (should fail) */

  ch = rk3576_dmac_alloc();
  if (ch < 0)
    {
      printf("  Overflow correctly rejected\n");
    }
  else
    {
      printf("  ERROR: Should have failed\n");
    }

  /* Free all channels */

  for (int i = 0; i < 8; i++)
    {
      rk3576_dmac_free(i);
    }

  printf("  Freed all channels\n");

  printf("\n");
  return OK;
}

static int test_mem2mem(void)
{
  /* Use static buffers to ensure they're in accessible memory */

  static uint8_t src_buf[256];
  static uint8_t dst_buf[256];
  int ch;
  int ret;

  printf("=== DMA Memory-to-Memory Test ===\n");

  /* Initialize source buffer with pattern */

  for (int i = 0; i < sizeof(src_buf); i++)
    {
      src_buf[i] = (uint8_t)i;
    }

  /* Clear destination buffer */

  memset(dst_buf, 0, sizeof(dst_buf));

  /* Allocate channel */

  ch = rk3576_dmac_alloc();
  if (ch < 0)
    {
      printf("Failed to allocate channel: %d\n", ch);
      return ch;
    }

  printf("  Using channel %d\n", ch);

  /* Perform transfer */

  ret = rk3576_dmac_mem2mem(ch,
                            (uint32_t)(uintptr_t)dst_buf,
                            (uint32_t)(uintptr_t)src_buf,
                            sizeof(src_buf));
  if (ret < 0)
    {
      printf("Failed to setup transfer: %d\n", ret);
      rk3576_dmac_free(ch);
      return ret;
    }

  /* Start transfer */

  rk3576_dmac_start(ch);

  /* Wait for completion */

  rk3576_dmac_wait(ch);

  /* Verify */

  int pass = 1;
  for (int i = 0; i < sizeof(src_buf); i++)
    {
      if (src_buf[i] != dst_buf[i])
        {
          printf("  Mismatch at byte %d: src=0x%02x dst=0x%02x\n",
                 i, src_buf[i], dst_buf[i]);
          pass = 0;
          if (i > 10)
            {
              break;
            }
        }
    }

  printf("  Verify: %s\n", pass ? "PASS" : "FAIL");

  /* Free channel */

  rk3576_dmac_free(ch);

  printf("\n");
  return OK;
}

static int test_different_sizes(void)
{
  static uint8_t src_buf[1024];
  static uint8_t dst_buf[1024];
  int ch;
  int ret;

  printf("=== DMA Different Sizes Test ===\n");

  /* Initialize source */

  for (int i = 0; i < sizeof(src_buf); i++)
    {
      src_buf[i] = (uint8_t)(i * 3 + 7);
    }

  /* Test different transfer sizes */

  uint32_t sizes[] = {1, 4, 16, 64, 256, 512, 1024};
  int count = sizeof(sizes) / sizeof(sizes[0]);

  for (int i = 0; i < count; i++)
    {
      memset(dst_buf, 0, sizeof(dst_buf));

      ch = rk3576_dmac_alloc();
      if (ch < 0)
        {
          printf("Failed to allocate: %d\n", ch);
          return ch;
        }

      ret = rk3576_dmac_mem2mem(ch,
                                (uint32_t)(uintptr_t)dst_buf,
                                (uint32_t)(uintptr_t)src_buf,
                                sizes[i]);
      if (ret < 0)
        {
          printf("Failed to setup %u bytes: %d\n", sizes[i], ret);
          rk3576_dmac_free(ch);
          continue;
        }

      rk3576_dmac_start(ch);
      rk3576_dmac_wait(ch);

      /* Verify */

      int pass = 1;
      for (int j = 0; j < sizes[i]; j++)
        {
          if (src_buf[j] != dst_buf[j])
            {
              pass = 0;
              break;
            }
        }

      printf("  %4u bytes: %s\n", sizes[i], pass ? "PASS" : "FAIL");

      rk3576_dmac_free(ch);
    }

  printf("\n");
  return OK;
}

static int test_multiple_channels(void)
{
  static uint8_t src_buf[256];
  static uint8_t dst_buf[4][256];
  int channels[4];
  int ret;

  printf("=== DMA Multiple Channels Test ===\n");

  /* Initialize source */

  for (int i = 0; i < sizeof(src_buf); i++)
    {
      src_buf[i] = (uint8_t)(i + 0xA0);
    }

  /* Allocate 4 channels */

  for (int i = 0; i < 4; i++)
    {
      channels[i] = rk3576_dmac_alloc();
      if (channels[i] < 0)
        {
          printf("Failed to allocate channel %d: %d\n", i, channels[i]);
          return channels[i];
        }

      memset(dst_buf[i], 0, sizeof(dst_buf[i]));

      ret = rk3576_dmac_mem2mem(channels[i],
                                (uint32_t)(uintptr_t)dst_buf[i],
                                (uint32_t)(uintptr_t)src_buf,
                                sizeof(src_buf));
      if (ret < 0)
        {
          printf("Failed to setup channel %d: %d\n", i, ret);
        }
    }

  /* Start all channels */

  for (int i = 0; i < 4; i++)
    {
      rk3576_dmac_start(channels[i]);
    }

  printf("  Started 4 channels\n");

  /* Wait for all to complete */

  for (int i = 0; i < 4; i++)
    {
      rk3576_dmac_wait(channels[i]);
    }

  /* Verify all */

  int all_pass = 1;
  for (int i = 0; i < 4; i++)
    {
      int pass = 1;
      for (int j = 0; j < sizeof(src_buf); j++)
        {
          if (src_buf[j] != dst_buf[i][j])
            {
              pass = 0;
              break;
            }
        }

      if (!pass)
        {
          printf("  Channel %d: FAIL\n", i);
          all_pass = 0;
        }
    }

  printf("  Verify: %s\n", all_pass ? "PASS" : "FAIL");

  /* Free all channels */

  for (int i = 0; i < 4; i++)
    {
      rk3576_dmac_free(channels[i]);
    }

  printf("\n");
  return OK;
}

static int test_abort(void)
{
  static uint8_t src_buf[4096];
  static uint8_t dst_buf[4096];
  int ch;
  int ret;

  printf("=== DMA Abort Test ===\n");

  /* Initialize source */

  for (int i = 0; i < sizeof(src_buf); i++)
    {
      src_buf[i] = (uint8_t)i;
    }

  memset(dst_buf, 0, sizeof(dst_buf));

  ch = rk3576_dmac_alloc();
  if (ch < 0)
    {
      printf("Failed to allocate: %d\n", ch);
      return ch;
    }

  ret = rk3576_dmac_mem2mem(ch,
                            (uint32_t)(uintptr_t)dst_buf,
                            (uint32_t)(uintptr_t)src_buf,
                            sizeof(src_buf));
  if (ret < 0)
    {
      printf("Failed to setup: %d\n", ret);
      rk3576_dmac_free(ch);
      return ret;
    }

  /* Start and immediately abort */

  rk3576_dmac_start(ch);
  rk3576_dmac_abort(ch);

  printf("  Aborted transfer\n");

  /* Free channel */

  rk3576_dmac_free(ch);
  printf("  Channel freed\n");

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
  printf("  RK3576 DMA Controller Test\n");
  printf("  KICKPI-K7 (KICKPI-K7)\n");
  printf("===================================\n\n");

  /* Run tests */

  test_init();
  test_alloc_free();
  test_mem2mem();
  test_different_sizes();
  test_multiple_channels();
  test_abort();

  printf("===================================\n");
  printf("  All DMA tests completed!\n");
  printf("===================================\n");

  return OK;
}
