/****************************************************************************
 * arch/arm64/src/rk3576/rk3576_touch.c - I2C Touch screen driver
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>
#include <string.h>
#include <debug.h>
#include "rk3576_touch.h"
#include "rk3576_i2c.h"

#define TOUCH_I2C_BUS     0
#define TOUCH_I2C_ADDR    0x38
#define TOUCH_REG_XH      0x03
#define TOUCH_REG_MODE    0xa4
#define TOUCH_REG_THGROUP 0x80

static int g_max_points = 5;

void rk3576_touch_init(void)
{
  rk3576_i2c_init(TOUCH_I2C_BUS);
  rk3576_i2c_set_speed(TOUCH_I2C_BUS, RK3576_I2C_SPEED_FAST);
  rk3576_i2c_set_address(TOUCH_I2C_BUS, TOUCH_I2C_ADDR);
  uinfo("Touch: initialized (I2C%d, addr 0x%02x)\n",
        TOUCH_I2C_BUS, TOUCH_I2C_ADDR);
}

int rk3576_touch_read(struct rk3576_touch_point_s *point)
{
  uint8_t buf[8];
  int ret;

  if (point == NULL) return -EINVAL;

  memset(point, 0, sizeof(*point));

  ret = rk3576_i2c_read_reg(TOUCH_I2C_BUS, TOUCH_REG_XH, buf, sizeof(buf));
  if (ret < 0) return ret;

  uint8_t touch_num = buf[5] & 0x0f;
  if (touch_num > 0)
    {
      point->touched = true;
      point->x = ((uint16_t)(buf[1] & 0x0f) << 8) | buf[2];
      point->y = ((uint16_t)(buf[3] & 0x0f) << 8) | buf[4];
      point->pressure = buf[6];
    }

  return OK;
}

int rk3576_touch_get_max_points(void)
{
  return g_max_points;
}
