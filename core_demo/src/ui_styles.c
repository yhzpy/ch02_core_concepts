/**
 * @file ui_styles.c
 * @brief 演示3：样式系统
 *
 * 本页面演示 LVGL 样式系统的核心概念：
 *   - lv_style_t 全局样式 vs 局部样式（lv_obj_set_style_xxx）
 *   - Part（部件）：MAIN / INDICATOR / KNOB
 *   - State（状态）：DEFAULT / PRESSED / CHECKED / FOCUSED / DISABLED
 *   - selector = part | state 的组合
 *   - 样式过渡（transition）
 *   - 样式优先级与覆盖
 *
 * 交互方式：
 *   - 按下按钮观察 PRESSED 状态样式
 *   - 切换开关观察 CHECKED 状态样式
 *   - 拖动滑块观察 INDICATOR 和 KNOB 部件样式
 *   - 观察过渡动画效果
 */

#include "ui.h"
#include <stdio.h>

/* ==================== 全局样式定义 ==================== */

/*
 * 全局样式：用 lv_style_t 定义，可以应用到多个对象
 * 好处：修改一处，所有使用该样式的对象同时变化
 */
static lv_style_t style_card;          /* 卡片容器样式 */
static lv_style_t style_btn_normal;    /* 按钮默认样式 */
static lv_style_t style_btn_pressed;   /* 按钮按下样式 */
static lv_style_t style_title;         /* 标题文字样式 */

/**
 * 初始化全局样式
 * 演示：lv_style_init() + lv_style_set_xxx()
 */
static void init_styles(void)
{
    /* --- 卡片样式 --- */
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, lv_color_white());
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_radius(&style_card, 12);
    lv_style_set_shadow_width(&style_card, 10);
    lv_style_set_shadow_color(&style_card, lv_color_hex(0xBDBDBD));
    lv_style_set_shadow_ofs_y(&style_card, 4);
    lv_style_set_shadow_opa(&style_card, LV_OPA_40);
    lv_style_set_pad_all(&style_card, 15);

    /* --- 按钮默认样式 --- */
    lv_style_init(&style_btn_normal);
    lv_style_set_bg_color(&style_btn_normal, lv_color_hex(0x2196F3));
    lv_style_set_bg_opa(&style_btn_normal, LV_OPA_COVER);
    lv_style_set_radius(&style_btn_normal, 8);
    lv_style_set_text_color(&style_btn_normal, lv_color_white());
    lv_style_set_pad_hor(&style_btn_normal, 20);
    lv_style_set_pad_ver(&style_btn_normal, 10);

    /*
     * 按钮按下样式
     * 演示：selector = LV_STATE_PRESSED
     * 当按钮被按下时，LVGL 自动应用这个样式
     */
    lv_style_init(&style_btn_pressed);
    lv_style_set_bg_color(&style_btn_pressed, lv_color_hex(0x1565C0));
    lv_style_set_shadow_width(&style_btn_pressed, 0);  /* 按下时去掉阴影 */
    lv_style_set_transform_width(&style_btn_pressed, -2);   /* 缩小2px */
    lv_style_set_transform_height(&style_btn_pressed, -2);

    /* --- 标题样式 --- */
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, lv_color_hex(0x1565C0));
    lv_style_set_text_font(&style_title, &lv_font_montserrat_16);
}

/* ==================== 事件回调 ==================== */

/**
 * Switch 状态变化回调：更新旁边的文字
 */
static void sw_changed_cb(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
    bool checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    lv_label_set_text(label, checked ? "ON" : "OFF");
}

/**
 * 样式切换演示：点击按钮切换深色/浅色卡片
 */
static void toggle_theme_cb(lv_event_t *e)
{
    lv_obj_t *card = (lv_obj_t *)lv_event_get_user_data(e);
    static bool dark = false;
    dark = !dark;

    if (dark) {
        /* 局部样式覆盖全局样式 */
        lv_obj_set_style_bg_color(card, lv_color_hex(0x37474F), 0);
        lv_obj_set_style_text_color(card, lv_color_white(), 0);
        lv_obj_set_style_shadow_color(card, lv_color_hex(0x000000), 0);
    } else {
        /* 恢复：移除局部样式，回退到全局样式 */
        lv_obj_remove_style(card, NULL, LV_PART_MAIN | LV_STATE_DEFAULT);
        /* 重新添加全局卡片样式 */
        lv_obj_add_style(card, &style_card, 0);
    }
}

/* ==================== 页面创建 ==================== */

void ui_show_styles(lv_obj_t *parent)
{
    lv_obj_clean(parent);
    init_styles();

    /* 页面背景 */
    lv_obj_set_style_bg_color(parent, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(parent, 10, 0);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(parent, 10, 0);

    /* ---- 标题 ---- */
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "Style System Demo");
    lv_obj_add_style(title, &style_title, 0);  /* 应用全局标题样式 */

    /* ---- 卡片1：按钮状态演示 ---- */
    lv_obj_t *card1 = lv_obj_create(parent);
    lv_obj_set_size(card1, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(card1, &style_card, 0);  /* 应用全局卡片样式 */
    lv_obj_clear_flag(card1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *c1_title = lv_label_create(card1);
    lv_label_set_text(c1_title, "Button States (press me!)");
    lv_obj_set_style_text_font(c1_title, &lv_font_montserrat_14, 0);

    lv_obj_t *btn = lv_btn_create(card1);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 20);
    /* 添加默认样式和按下样式 */
    lv_obj_add_style(btn, &style_btn_normal, 0);
    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    /*
     * 关键：selector = LV_STATE_PRESSED
     * 只有当按钮处于 PRESSED 状态时才应用 style_btn_pressed
     */

    /* 添加过渡动画：颜色变化在 200ms 内平滑过渡 */
    static lv_style_transition_dsc_t trans;
    static const lv_style_prop_t props[] = {
        LV_STYLE_BG_COLOR, LV_STYLE_SHADOW_WIDTH,
        LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT,
        LV_STYLE_PROP_INV
    };
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_ease_out,
                                  200, 0, NULL);
    lv_obj_set_style_transition(btn, &trans, 0);

    lv_obj_t *bl = lv_label_create(btn);
    lv_label_set_text(bl, "Press & Hold");
    lv_obj_center(bl);

    /* 禁用按钮演示 */
    lv_obj_t *btn_dis = lv_btn_create(card1);
    lv_obj_align(btn_dis, LV_ALIGN_CENTER, 120, 20);
    lv_obj_add_style(btn_dis, &style_btn_normal, 0);
    lv_obj_add_state(btn_dis, LV_STATE_DISABLED);  /* 设置禁用状态 */
    lv_obj_set_style_bg_opa(btn_dis, LV_OPA_40, LV_STATE_DISABLED);
    bl = lv_label_create(btn_dis);
    lv_label_set_text(bl, "Disabled");
    lv_obj_center(bl);

    /* ---- 卡片2：Parts 演示（滑块） ---- */
    lv_obj_t *card2 = lv_obj_create(parent);
    lv_obj_set_size(card2, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(card2, &style_card, 0);
    lv_obj_clear_flag(card2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *c2_title = lv_label_create(card2);
    lv_label_set_text(c2_title, "Parts: MAIN / INDICATOR / KNOB (drag slider)");
    lv_obj_set_style_text_font(c2_title, &lv_font_montserrat_14, 0);

    lv_obj_t *slider = lv_slider_create(card2);
    lv_obj_set_width(slider, LV_PCT(90));
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 25);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 60, LV_ANIM_OFF);

    /*
     * Part 样式设置：
     *   LV_PART_MAIN      - 滑轨背景
     *   LV_PART_INDICATOR - 已填充部分
     *   LV_PART_KNOB      - 拖动手柄
     */
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xE0E0E0), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x4CAF50), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x2E7D32), LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider, 6, LV_PART_KNOB);  /*  Knob 大小 */
    lv_obj_set_style_shadow_width(slider, 6, LV_PART_KNOB);
    lv_obj_set_style_shadow_color(slider, lv_color_hex(0x81C784), LV_PART_KNOB);

    /* ---- 卡片3：局部样式 vs 全局样式 ---- */
    lv_obj_t *card3 = lv_obj_create(parent);
    lv_obj_set_size(card3, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(card3, &style_card, 0);
    lv_obj_clear_flag(card3, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *c3_title = lv_label_create(card3);
    lv_label_set_text(c3_title, "Local vs Global Style (click to toggle dark)");
    lv_obj_set_style_text_font(c3_title, &lv_font_montserrat_14, 0);

    lv_obj_t *c3_text = lv_label_create(card3);
    lv_label_set_text(c3_text,
        "This card uses global 'style_card'.\n"
        "Click the button to override with local styles.\n"
        "Local styles have HIGHER priority than global.");
    lv_obj_align(c3_text, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_style_text_font(c3_text, &lv_font_montserrat_12, 0);

    lv_obj_t *btn_toggle = lv_btn_create(card3);
    lv_obj_align(btn_toggle, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_size(btn_toggle, 120, 36);
    lv_obj_add_style(btn_toggle, &style_btn_normal, 0);
    lv_obj_add_event_cb(btn_toggle, toggle_theme_cb, LV_EVENT_CLICKED, card3);
    bl = lv_label_create(btn_toggle);
    lv_label_set_text(bl, "Toggle Dark");
    lv_obj_center(bl);

    /* ---- 卡片4：Switch 的 CHECKED 状态 ---- */
    lv_obj_t *card4 = lv_obj_create(parent);
    lv_obj_set_size(card4, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(card4, &style_card, 0);
    lv_obj_clear_flag(card4, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *c4_title = lv_label_create(card4);
    lv_label_set_text(c4_title, "State: CHECKED (toggle switch)");
    lv_obj_set_style_text_font(c4_title, &lv_font_montserrat_14, 0);

    lv_obj_t *sw = lv_switch_create(card4);
    lv_obj_align(sw, LV_ALIGN_CENTER, 0, 20);
    /* CHECKED 状态的样式 */
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x4CAF50),
                              LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_white(),
                              LV_PART_KNOB | LV_STATE_CHECKED);

    lv_obj_t *sw_label = lv_label_create(card4);
    lv_label_set_text(sw_label, "OFF");
    lv_obj_align_to(sw_label, sw, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

    /* 开关事件：更新文字 */
    lv_obj_add_event_cb(sw, sw_changed_cb, LV_EVENT_VALUE_CHANGED, sw_label);
}
