/**
 * @file ui_obj_tree.c
 * @brief 演示1：对象树模型
 *
 * 本页面演示 LVGL 对象树的核心概念：
 *   - 父子关系与嵌套创建
 *   - 对象遍历（child_cnt, get_child）
 *   - 对象标志（flags）的动态切换
 *   - 屏幕层级（scr_act, layer_top, layer_sys）
 *   - 删除父对象时子对象自动销毁
 *
 * 交互方式：
 *   - 点击"添加子对象"按钮 → 在容器中动态创建子对象
 *   - 点击"遍历打印"按钮 → 在日志区打印对象树结构
 *   - 点击"清空容器"按钮 → lv_obj_clean() 删除所有子对象
 *   - 切换 HIDDEN 开关 → 演示对象隐藏/显示
 */

#include "ui.h"
#include <stdio.h>

/* ==================== 模块变量 ==================== */

static lv_obj_t *container    = NULL;  /* 演示用容器 */
static lv_obj_t *log_label    = NULL;  /* 日志输出标签 */
static lv_obj_t *child_count_label = NULL;  /* 子对象计数 */
static int       child_index  = 0;     /* 子对象编号计数器 */

/* ==================== 辅助函数 ==================== */

/**
 * 向日志区追加一行文字
 */
static void log_append(const char *text)
{
    /* 获取当前文本 */
    const char *cur = lv_label_get_text(log_label);

    /* 拼接新行（限制总长度防止溢出） */
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s\n%s", cur, text);
    lv_label_set_text(log_label, buf);

    /* 滚动到底部 */
    lv_obj_scroll_to_y(lv_obj_get_parent(log_label), LV_COORD_MAX, LV_ANIM_ON);
}

/**
 * 更新子对象计数显示
 */
static void update_count(void)
{
    char buf[32];
    uint32_t cnt = lv_obj_get_child_cnt(container);
    snprintf(buf, sizeof(buf), "Children: %u", cnt);
    lv_label_set_text(child_count_label, buf);
}

/* ==================== 事件回调 ==================== */

/**
 * 子对象点击回调
 * 演示：lv_event_get_target() 获取触发事件的对象
 */
static void child_click_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *lbl = lv_obj_get_child(obj, 0);
    char msg[64];
    snprintf(msg, sizeof(msg), "Clicked: %s (pos: %d,%d)",
             lv_label_get_text(lbl),
             lv_obj_get_x(obj), lv_obj_get_y(obj));
    log_append(msg);
}

/**
 * "添加子对象"按钮回调
 * 演示：lv_obj_create(parent) 在指定父对象上创建子对象
 */
static void btn_add_cb(lv_event_t *e)
{
    (void)e;
    child_index++;

    /* 在 container 上创建一个新的子对象 */
    lv_obj_t *child = lv_obj_create(container);
    lv_obj_set_size(child, 120, 50);

    /* 随机位置（在容器范围内） */
    int x = (child_index * 37) % 200;
    int y = (child_index * 23) % 150;
    lv_obj_set_pos(child, x, y);

    /* 设置背景色（根据编号变化） */
    static const lv_color_t colors[] = {
        LV_COLOR_MAKE(0x42, 0xA5, 0xF5),  /* 蓝 */
        LV_COLOR_MAKE(0x66, 0xBB, 0x6A),  /* 绿 */
        LV_COLOR_MAKE(0xFF, 0xA7, 0x26),  /* 橙 */
        LV_COLOR_MAKE(0xAB, 0x47, 0xBC),  /* 紫 */
        LV_COLOR_MAKE(0xEF, 0x53, 0x50),  /* 红 */
    };
    lv_obj_set_style_bg_color(child, colors[child_index % 5], 0);
    lv_obj_set_style_bg_opa(child, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(child, 8, 0);

    /* 在子对象上创建标签（孙对象）— 演示嵌套 */
    lv_obj_t *label = lv_label_create(child);
    char text[16];
    snprintf(text, sizeof(text), "#%d", child_index);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_center(label);

    /* 让子对象可点击，点击后打印自己的信息 */
    lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(child, child_click_cb, LV_EVENT_CLICKED, NULL);

    char msg[64];
    snprintf(msg, sizeof(msg), "Added child #%d at (%d,%d)", child_index, x, y);
    log_append(msg);
    update_count();
}

/**
 * "遍历打印"按钮回调
 * 演示：lv_obj_get_child_cnt() + lv_obj_get_child() 遍历对象树
 */
static void btn_traverse_cb(lv_event_t *e)
{
    (void)e;

    log_append("--- Object Tree ---");

    uint32_t cnt = lv_obj_get_child_cnt(container);
    char msg[128];
    snprintf(msg, sizeof(msg), "Container has %u children:", cnt);
    log_append(msg);

    for (uint32_t i = 0; i < cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(container, i);

        /* 获取子对象的位置和大小 */
        lv_coord_t x = lv_obj_get_x(child);
        lv_coord_t y = lv_obj_get_y(child);
        lv_coord_t w = lv_obj_get_width(child);
        lv_coord_t h = lv_obj_get_height(child);

        /* 检查子对象是否有孙对象（标签） */
        uint32_t sub_cnt = lv_obj_get_child_cnt(child);
        const char *sub_text = "";
        if (sub_cnt > 0) {
            lv_obj_t *sub = lv_obj_get_child(child, 0);
            if (lv_obj_check_type(sub, &lv_label_class)) {
                sub_text = lv_label_get_text(sub);
            }
        }

        snprintf(msg, sizeof(msg), "  [%u] pos=(%d,%d) size=%dx%d %s",
                 i, x, y, w, h, sub_text);
        log_append(msg);
    }

    log_append("--- End ---");
}

/**
 * "清空容器"按钮回调
 * 演示：lv_obj_clean() 删除所有子对象但保留容器本身
 */
static void btn_clear_cb(lv_event_t *e)
{
    (void)e;

    uint32_t cnt = lv_obj_get_child_cnt(container);
    lv_obj_clean(container);  /* 删除所有子对象 */

    char msg[64];
    snprintf(msg, sizeof(msg), "Cleaned %u children (container kept)", cnt);
    log_append(msg);
    update_count();
    child_index = 0;
}

/**
 * HIDDEN 开关回调
 * 演示：LV_OBJ_FLAG_HIDDEN 标志的添加和清除
 */
static void switch_hidden_cb(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    bool checked = lv_obj_has_state(sw, LV_STATE_CHECKED);

    if (checked) {
        lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
        log_append("Container HIDDEN (flag added)");
    } else {
        lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
        log_append("Container VISIBLE (flag cleared)");
    }
}

/* ==================== 页面创建 ==================== */

void ui_show_obj_tree(lv_obj_t *parent)
{
    lv_obj_clean(parent);
    child_index = 0;

    /* ---- 左侧：操作区 ---- */
    lv_obj_t *left = lv_obj_create(parent);
    lv_obj_set_size(left, LV_PCT(45), LV_PCT(100));
    lv_obj_align(left, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(left, 10, 0);
    lv_obj_clear_flag(left, LV_OBJ_FLAG_SCROLLABLE);

    /* 标题 */
    lv_obj_t *title = lv_label_create(left);
    lv_label_set_text(title, "Object Tree Demo");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    /* 子对象计数 */
    child_count_label = lv_label_create(left);
    lv_label_set_text(child_count_label, "Children: 0");
    lv_obj_align_to(child_count_label, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    /* 添加按钮 */
    lv_obj_t *btn_add = lv_btn_create(left);
    lv_obj_set_size(btn_add, 140, 40);
    lv_obj_align(btn_add, LV_ALIGN_LEFT_MID, 0, -60);
    lv_obj_add_event_cb(btn_add, btn_add_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lbl = lv_label_create(btn_add);
    lv_label_set_text(lbl, "Add Child");
    lv_obj_center(lbl);

    /* 遍历按钮 */
    lv_obj_t *btn_trav = lv_btn_create(left);
    lv_obj_set_size(btn_trav, 140, 40);
    lv_obj_align(btn_trav, LV_ALIGN_LEFT_MID, 0, -10);
    lv_obj_add_event_cb(btn_trav, btn_traverse_cb, LV_EVENT_CLICKED, NULL);
    lbl = lv_label_create(btn_trav);
    lv_label_set_text(lbl, "Traverse");
    lv_obj_center(lbl);

    /* 清空按钮 */
    lv_obj_t *btn_clr = lv_btn_create(left);
    lv_obj_set_size(btn_clr, 140, 40);
    lv_obj_align(btn_clr, LV_ALIGN_LEFT_MID, 0, 40);
    lv_obj_set_style_bg_color(btn_clr, lv_color_hex(0xEF5350), 0);
    lv_obj_add_event_cb(btn_clr, btn_clear_cb, LV_EVENT_CLICKED, NULL);
    lbl = lv_label_create(btn_clr);
    lv_label_set_text(lbl, "Clean All");
    lv_obj_center(lbl);

    /* HIDDEN 开关 */
    lv_obj_t *sw_label = lv_label_create(left);
    lv_label_set_text(sw_label, "Toggle HIDDEN:");
    lv_obj_align(sw_label, LV_ALIGN_LEFT_MID, 0, 90);

    lv_obj_t *sw = lv_switch_create(left);
    lv_obj_align_to(sw, sw_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(sw, switch_hidden_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* ---- 右侧上：演示容器 ---- */
    container = lv_obj_create(parent);
    lv_obj_set_size(container, LV_PCT(50), LV_PCT(50));
    lv_obj_align(container, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_set_style_bg_color(container, lv_color_hex(0xE3F2FD), 0);
    lv_obj_set_style_border_color(container, lv_color_hex(0x1976D2), 0);
    lv_obj_set_style_border_width(container, 2, 0);

    /* 容器标题 */
    lv_obj_t *cont_title = lv_label_create(parent);
    lv_label_set_text(cont_title, "Container (children go here)");
    lv_obj_set_style_text_color(cont_title, lv_color_hex(0x757575), 0);
    lv_obj_align_to(cont_title, container, LV_ALIGN_OUT_TOP_LEFT, 0, -2);

    /* ---- 右侧下：日志区 ---- */
    lv_obj_t *log_cont = lv_obj_create(parent);
    lv_obj_set_size(log_cont, LV_PCT(50), LV_PCT(42));
    lv_obj_align(log_cont, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_set_style_bg_color(log_cont, lv_color_hex(0x263238), 0);
    lv_obj_set_style_pad_all(log_cont, 8, 0);

    log_label = lv_label_create(log_cont);
    lv_label_set_text(log_label, "Log output:");
    lv_obj_set_style_text_color(log_label, lv_color_hex(0x4FC3F7), 0);
    lv_obj_set_style_text_font(log_label, &lv_font_montserrat_12, 0);
    lv_label_set_long_mode(log_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(log_label, LV_PCT(100));
}
