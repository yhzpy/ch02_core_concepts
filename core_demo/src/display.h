/**
 * @file display.h
 * @brief Framebuffer 显示驱动封装（与第1章相同）
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "lvgl/lvgl.h"

int  display_init(const char *fb_dev);
void display_deinit(void);
int  display_get_width(void);
int  display_get_height(void);

#endif /* DISPLAY_H */
