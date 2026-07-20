/**
 * @file input.c
 * @brief 触摸屏输入驱动实现（与第1章相同）
 */

#include "input.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>

static int input_fd = -1;
static int32_t cur_x = 0, cur_y = 0;
static int pressed = 0;
static int cal_x_min = 0, cal_x_max = 4095;
static int cal_y_min = 0, cal_y_max = 4095;
static int screen_w = 0, screen_h = 0;

static bool touchscreen_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    (void)drv;
    struct input_event ev;

    while (read(input_fd, &ev, sizeof(ev)) == (ssize_t)sizeof(ev)) {
        if (ev.type == EV_ABS) {
            if (ev.code == ABS_X || ev.code == ABS_MT_POSITION_X) {
                int range = cal_x_max - cal_x_min;
                if (range > 0)
                    cur_x = (int32_t)((long)(ev.value - cal_x_min) * screen_w / range);
                if (cur_x < 0) cur_x = 0;
                if (cur_x >= screen_w) cur_x = screen_w - 1;
            } else if (ev.code == ABS_Y || ev.code == ABS_MT_POSITION_Y) {
                int range = cal_y_max - cal_y_min;
                if (range > 0)
                    cur_y = (int32_t)((long)(ev.value - cal_y_min) * screen_h / range);
                if (cur_y < 0) cur_y = 0;
                if (cur_y >= screen_h) cur_y = screen_h - 1;
            }
        } else if (ev.type == EV_KEY && ev.code == BTN_TOUCH) {
            pressed = (ev.value != 0);
        }
    }

    data->point.x = cur_x;
    data->point.y = cur_y;
    data->state = pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    return false;
}

int input_init(const char *input_dev)
{
    input_fd = open(input_dev, O_RDONLY | O_NONBLOCK);
    if (input_fd < 0) {
        printf("[input] Warning: %s unavailable (%s)\n", input_dev, strerror(errno));
        return -1;
    }

    struct input_absinfo abs_x, abs_y;
    if (ioctl(input_fd, EVIOCGABS(ABS_X), &abs_x) == 0) {
        cal_x_min = abs_x.minimum;
        cal_x_max = abs_x.maximum;
    }
    if (ioctl(input_fd, EVIOCGABS(ABS_Y), &abs_y) == 0) {
        cal_y_min = abs_y.minimum;
        cal_y_max = abs_y.maximum;
    }

    lv_disp_t *disp = lv_disp_get_default(NULL);
    if (disp) {
        screen_w = lv_disp_get_hor_res(disp);
        screen_h = lv_disp_get_ver_res(disp);
    }

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchscreen_read_cb;
    lv_indev_drv_register(&indev_drv);

    printf("[input] Touch registered\n");
    return 0;
}

void input_deinit(void)
{
    if (input_fd >= 0) { close(input_fd); input_fd = -1; }
}
