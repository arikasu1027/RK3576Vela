/***************************************************************************
 * apps/lvgl_demo/lvgl_demo_main.c
 *
 * LVGL Demo Application for RK3576 KICKPI-K7
 *
 * Creates a basic dashboard UI with:
 * - System status display
 * - Network status
 * - Time display
 *
 ***************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <unistd.h>
#include <nuttx/arch.h>

#include "rk3576_fb.h"
#include "rk3576_lvgl.h"

#ifdef CONFIG_RK3576_LVGL

static void create_dashboard(void)
{
  lv_obj_t *scr = lv_screen_active();

  lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "KICKPI-K7 Dashboard");
  lv_obj_set_style_text_color(title, lv_color_hex(0x00d4ff), 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

  lv_obj_t *status = lv_label_create(scr);
  lv_label_set_text(status, "System Ready");
  lv_obj_set_style_text_color(status, lv_color_hex(0x00ff88), 0);
  lv_obj_set_style_text_font(status, &lv_font_montserrat_48, 0);
  lv_obj_align(status, LV_ALIGN_CENTER, 0, -40);

  lv_obj_t *info = lv_label_create(scr);
  lv_label_set_text(info, "RK3576 + NuttX RTOS");
  lv_obj_set_style_text_color(info, lv_color_hex(0xaaaaaa), 0);
  lv_obj_align(info, LV_ALIGN_CENTER, 0, 10);

  lv_obj_t *net = lv_label_create(scr);
  lv_label_set_text(net, LV_SYMBOL_WIFI " Network: Active");
  lv_obj_set_style_text_color(net, lv_color_hex(0x4488ff), 0);
  lv_obj_align(net, LV_ALIGN_BOTTOM_LEFT, 20, -20);

  lv_obj_t *time_label = lv_label_create(scr);
  lv_label_set_text(time_label, "12:34");
  lv_obj_set_style_text_color(time_label, lv_color_hex(0xffffff), 0);
  lv_obj_align(time_label, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
}

int main(int argc, char *argv[])
{
  int ret;

  UNUSED(argc);
  UNUSED(argv);

  ret = rk3576_lvgl_init();
  if (ret < 0)
    {
      fprintf(stderr, "Failed to init LVGL: %d\n", ret);
      return ret;
    }

  create_dashboard();

  printf("LVGL Demo: Dashboard running\n");

  for (; ; )
    {
      rk3576_lvgl_task();
      usleep(5000);
    }

  return OK;
}

#endif /* CONFIG_RK3576_LVGL */
