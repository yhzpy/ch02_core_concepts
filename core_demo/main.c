/**
 * @file main.c
 * @brief 第二章核心概念演示 — 程序入口
 *
 * 功能：通过底部导航栏在 4 个演示页面间切换：
 *   1. Object Tree  — 对象树、父子关系、遍历、flags
 *   2. Events       — 事件回调、冒泡、user_data
 *   3. Styles       — 全局/局部样式、Part、State、过渡
 *   4. Lifecycle    — 创建/销毁、定时器、内存监控
 */

#include "lvgl/lvgl.h"
#include "src/display.h"
#include "src/input.h"
#include "src/ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

/* ==================== 配置 ==================== */

#define FB_DEVICE    "/dev/fb0"
#define INPUT_DEVICE "/dev/input/event0"
#define TICK_MS      5

/* ==================== 全局 ==================== */

static volatile int g_running = 1;

static void signal_handler(int sig) { (void)sig; g_running = 0; }

static void *tick_thread(void *arg)
{
    (void)arg;
    struct timespec ts = { 0, TICK_MS * 1000L * 1000L };
    while (g_running) {
        nanosleep(&ts, NULL);
        lv_tick_inc(TICK_MS);
    }
    return NULL;
}

/* ==================== 导航系统 ==================== */

/* 页面枚举 */
enum { PAGE_OBJ_TREE = 0, PAGE_EVENTS, PAGE_STYLES, PAGE_LIFECYCLE, PAGE_COUNT };

static const char *page_names[PAGE_COUNT] = {
    "ObjTree", "Events", "Styles", "Lifecycle"
};

static lv_obj_t *content_area = NULL;   /* 内容区域 */
static int       current_page = 0;

/* 页面显示函数表 */
typedef void (*page_show_fn)(lv_obj_t *parent);
static const page_show_fn page_fns[PAGE_COUNT] = {
    ui_show_obj_tree,
    ui_show_events,
    ui_show_styles,
    ui_show_lifecycle,
};

/**
 * 切换页面
 */
static void show_page(int idx)
{
    if (idx < 0 || idx >= PAGE_COUNT) return;
    current_page = idx;
    page_fns[idx](content_area);
    printf("[nav] Switched to: %s\n", page_names[idx]);
}

/**
 * 导航按钮回调
 */
static void nav_btn_cb(lv_event_t *e)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    show_page(idx);
}

/**
 * 创建导航框架
 */
static void create_nav(void)
{
    lv_obj_t *scr = lv_scr_act();

    /* 底部导航栏 */
    lv_obj_t *nav = lv_obj_create(scr);
    lv_obj_set_size(nav, LV_PCT(100), 50);
    lv_obj_align(nav, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(nav, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(nav, LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(nav, lv_color_hex(0x263238), 0);
    lv_obj_set_style_bg_opa(nav, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(nav, 0, 0);
    lv_obj_set_style_border_width(nav, 0, 0);
    lv_obj_clear_flag(nav, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < PAGE_COUNT; i++) {
        lv_obj_t *btn = lv_btn_create(nav);
        lv_obj_set_size(btn, 100, 36);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x37474F), 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x00BCD4),
                                  LV_STATE_PRESSED);
        lv_obj_add_event_cb(btn, nav_btn_cb, LV_EVENT_CLICKED,
                            (void *)(intptr_t)i);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, page_names[i]);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
        lv_obj_center(lbl);
    }

    /* 内容区域（导航栏上方） */
    content_area = lv_obj_create(scr);
    lv_obj_set_size(content_area, LV_PCT(100), LV_PCT(88));
    lv_obj_align(content_area, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(content_area, lv_color_hex(0xFAFAFA), 0);
    lv_obj_set_style_bg_opa(content_area, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(content_area, 0, 0);
    lv_obj_set_style_border_width(content_area, 0, 0);
    lv_obj_set_style_pad_all(content_area, 5, 0);
    lv_obj_clear_flag(content_area, LV_OBJ_FLAG_SCROLLABLE);
}

/* ==================== 主函数 ==================== */

int main(int argc, char *argv[])
{
    const char *fb_dev    = FB_DEVICE;
    const char *input_dev = INPUT_DEVICE;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--fb") == 0 && i + 1 < argc)
            fb_dev = argv[++i];
        else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc)
            input_dev = argv[++i];
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("=== LVGL Core Concepts Demo (Ch.02) ===\n");

    lv_init();

    if (display_init(fb_dev) < 0) {
        fprintf(stderr, "Display init failed\n");
        return -1;
    }
    input_init(input_dev);

    pthread_t tid;
    pthread_create(&tid, NULL, tick_thread, NULL);

    /* 初始化主题 */
    lv_theme_t *th = lv_theme_default_init(
        lv_disp_get_default(NULL),
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_RED),
        false, &lv_font_montserrat_14);
    lv_disp_set_theme(lv_disp_get_default(NULL), th);

    /* 创建导航框架并显示第一个页面 */
    create_nav();
    show_page(PAGE_OBJ_TREE);

    printf("Entering main loop...\n");
    while (g_running) {
        lv_timer_handler();
        usleep(5000);
    }

    g_running = 0;
    pthread_join(tid, NULL);
    input_deinit();
    display_deinit();
    printf("Bye!\n");
    return 0;
}
