/**
 * @file ui_events.c
 * @brief 演示2：事件机制
 *
 * 本页面演示 LVGL 事件系统的核心概念：
 *   - 事件回调注册（lv_obj_add_event_cb）
 *   - 事件码区分（CLICKED / PRESSED / RELEASED / LONG_PRESSED）
 *   - 事件冒泡（EVENT_BUBBLE flag）
 *   - user_data 传递自定义数据
 *   - 多事件处理器（同一对象注册多个回调）
 *   - lv_event_send() 手动发送事件
 *
 * 交互方式：
 *   - 点击/长按按钮观察不同事件码
 *   - 点击子按钮观察事件冒泡到父容器
 *   - 观察 user_data 如何传递
 */

#include "ui.h"
#include <stdio.h>
#include <stdarg.h>

/* ==================== 模块变量 ==================== */

static lv_obj_t *event_log = NULL;     /* 事件日志标签 */
static int       event_seq = 0;        /* 事件序号 */

/* ==================== 辅助函数 ==================== */

static void elog(const char *fmt, ...)
{
    va_list args;
    char line[128];
    va_start(args, fmt);
    vsnprintf(line, sizeof(line), fmt, args);
    va_end(args);

    const char *cur = lv_label_get_text(event_log);
    char buf[2048];
    snprintf(buf, sizeof(buf), "[%03d] %s\n%s", ++event_seq, line, cur);
    lv_label_set_text(event_log, buf);
}

/* ==================== 事件回调 ==================== */

/**
 * 多事件处理器：一个回调处理多种事件码
 * 演示：通过 lv_event_get_code() 区分事件类型
 */
static void multi_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    switch (code) {
    case LV_EVENT_PRESSED:
        elog("PRESSED - finger down");
        break;
    case LV_EVENT_RELEASED:
        elog("RELEASED - finger up");
        break;
    case LV_EVENT_CLICKED:
        elog("CLICKED - press+release on same obj");
        break;
    case LV_EVENT_LONG_PRESSED:
        elog("LONG_PRESSED - held > 400ms");
        break;
    case LV_EVENT_LONG_PRESSED_REPEAT:
        elog("LONG_PRESSED_REPEAT - auto repeat");
        break;
    default:
        elog("Other event: %d", (int)code);
        break;
    }
}

/**
 * 事件冒泡演示：子按钮的回调
 * 演示：LV_OBJ_FLAG_EVENT_BUBBLE 让事件传递到父对象
 */
static void bubble_child_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    const char *name = (const char *)lv_event_get_user_data(e);
    elog("Child '%s' CLICKED (target=%p)", name, (void *)target);
}

/**
 * 事件冒泡演示：父容器的回调
 * 当子对象的事件冒泡上来时，这个回调也会被触发
 */
static void bubble_parent_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);   /* 原始触发者 */
    lv_obj_t *cur    = lv_event_get_current_target(e);  /* 当前处理者 */

    /* 只有当 target != current_target 时，说明是冒泡上来的 */
    if (target != cur) {
        elog("Parent received BUBBLED event from child");
    }
}

/**
 * user_data 演示：通过 user_data 传递按钮编号
 */
static void userdata_cb(lv_event_t *e)
{
    int btn_id = (int)(intptr_t)lv_event_get_user_data(e);
    elog("Button #%d clicked (via user_data)", btn_id);
}

/**
 * 手动发送事件演示
 */
static void send_event_cb(lv_event_t *e)
{
    (void)e;
    elog("Manually sending CLICKED to Button A...");

    /* 找到 Button A 并手动发送事件 */
    lv_obj_t *scr = lv_scr_act();
    /* 遍历找到名为 "BtnA" 的对象（简化演示） */
    lv_event_send(scr, LV_EVENT_CLICKED, NULL);
    elog("lv_event_send() done");
}

/**
 * 阻止事件冒泡演示
 */
static void stop_bubble_cb(lv_event_t *e)
{
    elog("STOP_BUBBLE: event will NOT propagate to parent");
    lv_event_stop_bubbling(e);  /* 阻止继续冒泡 */
}

/* ==================== 页面创建 ==================== */

void ui_show_events(lv_obj_t *parent)
{
    lv_obj_clean(parent);
    event_seq = 0;

    /* ---- 左侧：演示按钮区 ---- */
    lv_obj_t *left = lv_obj_create(parent);
    lv_obj_set_size(left, LV_PCT(50), LV_PCT(100));
    lv_obj_align(left, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(left, 10, 0);
    lv_obj_clear_flag(left, LV_OBJ_FLAG_SCROLLABLE);

    /* 标题 */
    lv_obj_t *title = lv_label_create(left);
    lv_label_set_text(title, "Event System Demo");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    /* --- 多事件按钮 --- */
    lv_obj_t *lbl1 = lv_label_create(left);
    lv_label_set_text(lbl1, "Multi-event button (try click/long-press):");
    lv_obj_align(lbl1, LV_ALIGN_TOP_LEFT, 0, 30);

    lv_obj_t *btn_multi = lv_btn_create(left);
    lv_obj_set_size(btn_multi, 200, 50);
    lv_obj_align_to(btn_multi, lbl1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    /* 注册多个事件码到同一个回调 */
    lv_obj_add_event_cb(btn_multi, multi_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn_multi, multi_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn_multi, multi_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_multi, multi_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    lv_obj_add_event_cb(btn_multi, multi_event_cb, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    lv_obj_t *bl = lv_label_create(btn_multi);
    lv_label_set_text(bl, "Press / Hold Me");
    lv_obj_center(bl);

    /* --- 事件冒泡演示 --- */
    lv_obj_t *lbl2 = lv_label_create(left);
    lv_label_set_text(lbl2, "Event bubbling (click child buttons):");
    lv_obj_align(lbl2, LV_ALIGN_TOP_LEFT, 0, 120);

    /* 父容器 */
    lv_obj_t *bubble_parent = lv_obj_create(left);
    lv_obj_set_size(bubble_parent, 280, 70);
    lv_obj_align_to(bubble_parent, lbl2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_style_bg_color(bubble_parent, lv_color_hex(0xFFF3E0), 0);
    lv_obj_set_style_border_color(bubble_parent, lv_color_hex(0xFF9800), 0);
    lv_obj_set_style_border_width(bubble_parent, 2, 0);
    lv_obj_set_flex_flow(bubble_parent, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bubble_parent, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(bubble_parent, 15, 0);
    /* 父容器注册事件回调（接收冒泡上来的事件） */
    lv_obj_add_event_cb(bubble_parent, bubble_parent_cb, LV_EVENT_CLICKED, NULL);

    /* 子按钮 A（允许冒泡） */
    lv_obj_t *btn_a = lv_btn_create(bubble_parent);
    lv_obj_set_size(btn_a, 100, 40);
    lv_obj_add_flag(btn_a, LV_OBJ_FLAG_EVENT_BUBBLE);  /* 关键：启用冒泡 */
    lv_obj_add_event_cb(btn_a, bubble_child_cb, LV_EVENT_CLICKED, (void *)"A");
    bl = lv_label_create(btn_a);
    lv_label_set_text(bl, "Child A");
    lv_obj_center(bl);

    /* 子按钮 B（阻止冒泡） */
    lv_obj_t *btn_b = lv_btn_create(bubble_parent);
    lv_obj_set_size(btn_b, 100, 40);
    lv_obj_set_style_bg_color(btn_b, lv_color_hex(0x78909C), 0);
    lv_obj_add_flag(btn_b, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(btn_b, stop_bubble_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn_b, bubble_child_cb, LV_EVENT_CLICKED, (void *)"B");
    bl = lv_label_create(btn_b);
    lv_label_set_text(bl, "Child B");
    lv_obj_center(bl);

    /* --- user_data 演示 --- */
    lv_obj_t *lbl3 = lv_label_create(left);
    lv_label_set_text(lbl3, "user_data (3 buttons, same callback):");
    lv_obj_align(lbl3, LV_ALIGN_TOP_LEFT, 0, 230);

    for (int i = 1; i <= 3; i++) {
        lv_obj_t *btn = lv_btn_create(left);
        lv_obj_set_size(btn, 80, 36);
        lv_obj_align_to(btn, lbl3, LV_ALIGN_OUT_BOTTOM_LEFT, (i - 1) * 90, 5);
        /* user_data 传递按钮编号 */
        lv_obj_add_event_cb(btn, userdata_cb, LV_EVENT_CLICKED,
                            (void *)(intptr_t)i);
        bl = lv_label_create(btn);
        char txt[8];
        snprintf(txt, sizeof(txt), "#%d", i);
        lv_label_set_text(bl, txt);
        lv_obj_center(bl);
    }

    /* ---- 右侧：事件日志 ---- */
    lv_obj_t *right = lv_obj_create(parent);
    lv_obj_set_size(right, LV_PCT(48), LV_PCT(95));
    lv_obj_align(right, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_bg_color(right, lv_color_hex(0x1B2631), 0);
    lv_obj_set_style_pad_all(right, 8, 0);

    lv_obj_t *log_title = lv_label_create(right);
    lv_label_set_text(log_title, "Event Log (newest first):");
    lv_obj_set_style_text_color(log_title, lv_color_hex(0x81C784), 0);

    event_log = lv_label_create(right);
    lv_label_set_text(event_log, "");
    lv_obj_set_style_text_color(event_log, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_text_font(event_log, &lv_font_montserrat_12, 0);
    lv_label_set_long_mode(event_log, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(event_log, LV_PCT(100));
    lv_obj_align_to(event_log, log_title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
}
