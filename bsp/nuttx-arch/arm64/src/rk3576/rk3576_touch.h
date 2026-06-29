/****************************************************************************
 * arch/arm64/src/rk3576/rk3576_touch.h - Touch screen driver
 ****************************************************************************/

#ifndef __ARCH_ARM64_SRC_RK3576_RK3576_TOUCH_H
#define __ARCH_ARM64_SRC_RK3576_RK3576_TOUCH_H

#include <nuttx/config.h>
#include <stdint.h>

struct rk3576_touch_point_s
{
  uint16_t x;
  uint16_t y;
  uint8_t  pressure;
  bool     touched;
};

void rk3576_touch_init(void);
int  rk3576_touch_read(struct rk3576_touch_point_s *point);
int  rk3576_touch_get_max_points(void);

#endif
