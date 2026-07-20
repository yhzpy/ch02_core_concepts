/**
 * @file ui_lifecycle.c
 * @brief 演示4：对象生命周期与定时器
 *
 * 本页面演示：
 *   - 对象创建 → 使用 → 销毁的完整生命周期
 *   - lv_obj_del() vs lv_obj_del_async() vs lv_obj_clean()
 *   - lv_timer 创建、暂停、恢复、删除
 *   - 在事件回调中安全删除对象
 *   - 内存监控（lv_mem_monitor）
 *
 * 交互方式：
 *   - 点击"创建对象"→ 动态创建带定时器的对象
 *   - 点击"删除"→ 演示不同删除方式
 *   - 观察定时器计数和内存使用变化
 */

#include "ui.h"
#include <stdio.h>
#include <stdlib.h>

/* ==================== 模块变量 ==================== */

static lv_obj_t  *status_label  = NULL;   /* 状态信息 */
static lv_obj_t  *mem_label     = NULL;   /* 内存信息 */
static lv_obj_t  *timer_label   = NULL;   /* 定时器计数 */
static lv_obj_t  *dynamic_cont  = NULL;   /* 动态对象容器 */
static lv_timer_t *demo_timer   = NULL;   /* 演示定时器 */
static int        timer_count   = 0;      /* 定时器触发次数 */
static int        obj_counter   = 0;      /* 创建的对象总数 */

/* ==================== 定时器回调 ==================== */

/**
 * 周期性定时器回调
 * 演示：lv_timer 的基本用法
 *
 * 注意：定时器回调在 lv_timer_handler() 中被调用，
 *       运行在 UI 线程上下文中，可以安全操作 LVGL 对象。
 */
static void timer_cb(lv_timer_t *timer)
{
    (void)timer;
    timer_count++;

    char buf[64];
    snprintf(buf, sizeof(buf), "Timer fired: %d times", timer_count);
    lv_label_set_text(timer_label, buf);

    /* 每 5 次更新一次内存信息 */
    if (timer_count % 5 == 0) {
        lv_mem_monitor_t mon;
        lv_mem_monitor(&mon);
        snprintf(buf, sizeof(buf), "Mem: %u%% used (%u KB free)",
                 mon.used_pct, mon.free_size / 1024);
        lv_label_set_text(mem_label, buf);
    }
}

/* ==================== 事件回调 ==================== */

/**
 * "创建对象"按钮
 * 演示：动态创建对象 + 为对象关联定时器
 */
static void btn_create_cb(lv_event_t *e)
{
    (void)e;
    obj_counter++;

    /* 在容器中创建一个新对象 */
    lv_obj_t *obj = lv_obj_create(dynamic_cont);
    lv_obj_set_size(obj, 80, 40);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x42A5F5), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(obj, 6, 0);

    /* 使用 flex 布局自动排列 */
    lv_obj_set_flex_flow(dynamic_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_gap(dynamic_cont, 8, 0);

    lv_obj_t *lbl = lv_label_create(obj);
    char text[16];
    snprintf(text, sizeof(text), "Obj#%d", obj_counter);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
    lv_obj_center(lbl);

    /* 让对象可点击，点击后自我删除（演示 lv_obj_del_async） */
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(obj, obj_self_delete_cb, LV_EVENT_CLICKED, NULL);

    char buf[64];
    snprintf(buf, sizeof(buf), "Created Obj#%d (click to delete)", obj_counter);
    lv_label_set_text(status_label, buf);
}

/**
 * 动态对象点击回调：安全地删除自身
 * 演示：在事件回调中删除触发事件的对象
 *
 * 关键：必须用 lv_obj_del_async() 而不是 lv_obj_del()！
 * 因为 lv_obj_del() 会立即释放对象内存，
 * 但事件处理函数返回后 LVGL 还会访问该对象，导致 use-after-free。
 */
static void obj_self_delete_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);

    /* 获取对象上的标签文字用于日志 */
    lv_obj_t *lbl = lv_obj_get_child(obj, 0);
    const char *name = lv_label_get_text(lbl);

    char buf[64];
    snprintf(buf, sizeof(buf), "%s: lv_obj_del_async() called", name);
    lv_label_set_text(status_label, buf);

    /*
     * 异步删除：LVGL 会在当前事件处理完毕后安全删除对象
     * 这是唯一安全的"在回调中删除自身"的方式
     */
    lv_obj_del_async(obj);
}

/**
 * "lv_obj_del"按钮：立即删除容器第一个子对象
 */
static void btn_del_cb(lv_event_t *e)
{
    (void)e;
    uint32_t cnt = lv_obj_get_child_cnt(dynamic_cont);
    if (cnt == 0) {
        lv_label_set_text(status_label, "No objects to delete");
        return;
    }

    lv_obj_t *first = lv_obj_get_child(dynamic_cont, 0);
    lv_obj_t *lbl = lv_obj_get_child(first, 0);
    char buf[64];
    snprintf(buf, sizeof(buf), "lv_obj_del(%s) - immediate",
             lv_label_get_text(lbl));
    lv_label_set_text(status_label, buf);

    lv_obj_del(first);  /* 立即删除（不在事件回调中删除自身时可用） */
}

/**
 * "lv_obj_clean"按钮：清空容器所有子对象
 */
static void btn_clean_cb(lv_event_t *e)
{
    (void)e;
    uint32_t cnt = lv_obj_get_child_cnt(dynamic_cont);
    lv_obj_clean(dynamic_cont);  /* 删除所有子对象，保留容器 */

    char buf[64];
    snprintf(buf, sizeof(buf), "lv_obj_clean() removed %u children", cnt);
    lv_label_set_text(status_label, buf);
}

/**
 * 定时器控制：启动/暂停
 */
static void btn_timer_cb(lv_event_t *e)
{
    (void)e;
    if (demo_timer) {
        /* 暂停定时器 */
        lv_timer_pause(demo_timer);
        lv_label_set_text(status_label, "Timer PAUSED (lv_timer_pause)");
        demo_timer = NULL;  /* 标记为已暂停 */
    } else {
        /* 重新创建定时器 */
        demo_timer = lv_timer_create(timer_cb, 1000, NULL);
        lv_label_set_text(status_label, "Timer RESUMED (1000ms period)");
    }
}

/* ==================== 页面创建 ==================== */

void ui_show_lifecycle(lv_obj_t *parent)
{
    lv_obj_clean(parent);
    timer_count = 0;
    obj_counter = 0;

    lv_obj_set_style_pad_all(parent, 10, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    /* ---- 标题 ---- */
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "Lifecycle & Timer Demo");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    /* ---- 状态信息 ---- */
    status_label = lv_label_create(parent);
    lv_label_set_text(status_label, "Ready");
    lv_obj_align_to(status_label, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x757575), 0);

    /* ---- 内存和定时器信息 ---- */
    mem_label = lv_label_create(parent);
    lv_label_set_text(mem_label, "Mem: --");
    lv_obj_align(mem_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_text_font(mem_label, &lv_font_montserrat_12, 0);

    timer_label = lv_label_create(parent);
    lv_label_set_text(timer_label, "Timer fired: 0 times");
    lv_obj_align_to(timer_label, mem_label, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 3);
    lv_obj_set_style_text_font(timer_label, &lv_font_montserrat_12, 0);

    /* ---- 操作按钮行 ---- */
    lv_obj_t *btn_row = lv_obj_create(parent);
    lv_obj_set_size(btn_row, LV_PCT(100), 50);
    lv_obj_align(btn_row, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(btn_row, 10, 0);
    lv_obj_clear_flag(btn_row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_row, 0, 0);

    /* 创建按钮 */
    lv_obj_t *btn1 = lv_btn_create(btn_row);
    lv_obj_set_size(btn1, 100, 36);
    lv_obj_add_event_cb(btn1, btn_create_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *bl = lv_label_create(btn1);
    lv_label_set_text(bl, "Create");
    lv_obj_center(bl);

    /* 删除按钮 */
    lv_obj_t *btn2 = lv_btn_create(btn_row);
    lv_obj_set_size(btn2, 100, 36);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0xFF7043), 0);
    lv_obj_add_event_cb(btn2, btn_del_cb, LV_EVENT_CLICKED, NULL);
    bl = lv_label_create(btn2);
    lv_label_set_text(bl, "Del First");
    lv_obj_center(bl);

    /* 清空按钮 */
    lv_obj_t *btn3 = lv_btn_create(btn_row);
    lv_obj_set_size(btn3, 100, 36);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0xEF5350), 0);
    lv_obj_add_event_cb(btn3, btn_clean_cb, LV_EVENT_CLICKED, NULL);
    bl = lv_label_create(btn3);
    lv_label_set_text(bl, "Clean All");
    lv_obj_center(bl);

    /* 定时器控制按钮 */
    lv_obj_t *btn4 = lv_btn_create(btn_row);
    lv_obj_set_size(btn4, 120, 36);
    lv_obj_set_style_bg_color(btn4, lv_color_hex(0x78909C), 0);
    lv_obj_add_event_cb(btn4, btn_timer_cb, LV_EVENT_CLICKED, NULL);
    bl = lv_label_create(btn4);
    lv_label_set_text(bl, "Pause Timer");
    lv_obj_center(bl);

    /* ---- 动态对象容器 ---- */
    dynamic_cont = lv_obj_create(parent);
    lv_obj_set_size(dynamic_cont, LV_PCT(100), LV_PCT(55));
    lv_obj_align(dynamic_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(dynamic_cont, lv_color_hex(0xE8F5E9), 0);
    lv_obj_set_style_border_color(dynamic_cont, lv_color_hex(0x4CAF50), 0);
    lv_obj_set_style_border_width(dynamic_cont, 2, 0);
    lv_obj_set_style_pad_all(dynamic_cont, 10, 0);
    lv_obj_set_flex_flow(dynamic_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_gap(dynamic_cont, 8, 0);

    /* 容器提示文字 */
    lv_obj_t *hint = lv_label_create(dynamic_cont);
    lv_label_set_text(hint, "Click 'Create' to add objects here.\n"
                            "Click any object to delete it (async).");
    lv_obj_set_style_text_color(hint, lv_color_hex(0x9E9E9E), 0);

    /* ---- 启动演示定时器 ---- */
    demo_timer = lv_timer_create(timer_cb, 1000, NULL);
}
