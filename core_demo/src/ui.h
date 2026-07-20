/**
 * @file ui.h
 * @brief 第二章核心概念演示 — UI 模块接口
 *
 * 本项目包含 4 个演示页面，通过底部导航栏切换：
 *   1. 对象树 (Object Tree)
 *   2. 事件系统 (Events)
 *   3. 样式系统 (Styles)
 *   4. 生命周期 (Lifecycle)
 */

#ifndef UI_H
#define UI_H

#include "lvgl/lvgl.h"

/**
 * @brief 初始化主题和导航框架，显示第一个演示页面
 */
void ui_init(void);

/* 各演示页面的入口函数（内部使用） */
void ui_show_obj_tree(lv_obj_t *parent);
void ui_show_events(lv_obj_t *parent);
void ui_show_styles(lv_obj_t *parent);
void ui_show_lifecycle(lv_obj_t *parent);

#endif /* UI_H */
