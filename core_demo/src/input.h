/**
 * @file input.h
 * @brief 触摸屏输入驱动封装（与第1章相同）
 */

#ifndef INPUT_H
#define INPUT_H

#include "lvgl/lvgl.h"

int  input_init(const char *input_dev);
void input_deinit(void);

#endif /* INPUT_H */
