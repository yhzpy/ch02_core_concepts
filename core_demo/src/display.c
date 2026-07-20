/**
 * @file display.c
 * @brief Framebuffer 显示驱动实现（与第1章相同）
 */

#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

static int       fb_fd       = -1;
static void     *fb_buf      = NULL;
static uint32_t  fb_size     = 0;
static int       scr_width   = 0;
static int       scr_height  = 0;
static int       line_length = 0;

static lv_disp_drv_t      disp_drv;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t        *buf1 = NULL;
static lv_color_t        *buf2 = NULL;

static void fb_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area,
                         lv_color_t *color_p)
{
    int32_t w = area->x2 - area->x1 + 1;
    for (int32_t y = area->y1; y <= area->y2; y++) {
        uint8_t *dst = (uint8_t *)fb_buf + y * line_length +
                       area->x1 * sizeof(lv_color_t);
        uint8_t *src = (uint8_t *)color_p +
                       (y - area->y1) * w * sizeof(lv_color_t);
        memcpy(dst, src, w * sizeof(lv_color_t));
    }
    lv_disp_flush_ready(drv);
}

int display_init(const char *fb_dev)
{
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    fb_fd = open(fb_dev, O_RDWR);
    if (fb_fd < 0) { perror("[display] open"); return -1; }

    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

    scr_width   = vinfo.xres;
    scr_height  = vinfo.yres;
    line_length = finfo.line_length;
    fb_size     = finfo.smem_len;

    printf("[display] %dx%d, bpp=%d\n", scr_width, scr_height,
           vinfo.bits_per_pixel);

    fb_buf = mmap(NULL, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_buf == MAP_FAILED) { close(fb_fd); return -1; }
    memset(fb_buf, 0, fb_size);

    uint32_t buf_px = scr_width * 40;
    buf1 = malloc(buf_px * sizeof(lv_color_t));
    buf2 = malloc(buf_px * sizeof(lv_color_t));
    if (!buf1 || !buf2) return -1;

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, buf_px);
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = fb_flush_cb;
    disp_drv.hor_res  = scr_width;
    disp_drv.ver_res  = scr_height;
    lv_disp_drv_register(&disp_drv);

    return 0;
}

void display_deinit(void)
{
    free(buf1); free(buf2);
    if (fb_buf && fb_buf != MAP_FAILED) munmap(fb_buf, fb_size);
    if (fb_fd >= 0) close(fb_fd);
}

int display_get_width(void)  { return scr_width;  }
int display_get_height(void) { return scr_height; }
