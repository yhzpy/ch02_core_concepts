/**
 * @file lv_conf.h
 * @brief LVGL v8.4 配置文件 — i.MX6ULL 专用
 *
 * 使用方法：
 *   1. 将此文件放在 lvgl/ 目录的同级（即项目根目录）
 *   2. 编译时添加 -DLV_CONF_INCLUDE_SIMPLE 标志
 *   3. 根据需要调整下方各项参数
 *
 * 目标平台：i.MX6ULL (Cortex-A7, 800x480 RGB565 LCD)
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*============================
 * 颜色设置
 *==========================*/

/**
 * 颜色深度：
 *   1  - 单色（电子墨水屏）
 *   8  - 256色
 *  16  - RGB565（i.MX6ULL 推荐）
 *  32  - ARGB8888
 */
#define LV_COLOR_DEPTH 16

/**
 * RGB565 字节序交换：
 *   如果屏幕颜色看起来红蓝颠倒，改为 1
 */
#define LV_COLOR_16_SWAP 0

/**
 * 启用更复杂的图像混合（渐变、透明度混合等）
 * 关闭可节省少量 Flash 和 RAM
 */
#define LV_COLOR_MIX_ROUND_OFS 128

/**
 * 启用大字体和图片的 chroma key 透明色
 */
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)  /* 纯绿色透明 */

/*============================
 * 内存管理
 *==========================*/

/**
 * LVGL 内存池大小（字节）
 * 所有 lv_mem_alloc() 都从这个池里分配
 *
 * i.MX6ULL 有 256MB+ RAM，给 256KB 通常够用
 * 复杂界面可增大到 512KB ~ 1MB
 */
#define LV_MEM_SIZE (256U * 1024U)

/**
 * 使用自定义 malloc/free：
 *   0 = 使用 LVGL 内置内存管理器（推荐）
 *   1 = 使用系统 malloc（适合 RTOS 或需要内存跟踪时）
 */
#define LV_MEM_CUSTOM 0

/**
 * 内存池起始地址对齐（字节）
 */
#define LV_MEM_BUF_MAX_NUM 16

/*============================
 * 显示驱动
 *==========================*/

/**
 * 默认刷新周期（毫秒）
 *   30ms ≈ 33fps（i.MX6ULL 推荐平衡点）
 *   16ms ≈ 60fps（需要 PXP 硬件加速才能稳定达到）
 */
#define LV_DISP_DEF_REFR_PERIOD 30

/**
 * 显示驱动读取输入设备的周期（毫秒）
 */
#define LV_INDEV_DEF_READ_PERIOD 30

/**
 * DPI (dots per inch)，影响控件默认尺寸
 *   800x480 在 7寸屏上约 133 DPI
 *   800x480 在 5寸屏上约 186 DPI
 */
#define LV_DPI_DEF 130

/*============================
 * 绘图引擎
 *==========================*/

/**
 * 复杂绘制特性开关
 * 关闭可减小代码体积、提升性能
 */
#define LV_DRAW_COMPLEX 1

/**
 * 阴影绘制
 */
#define LV_SHADOW_CALC 1

/**
 * 图片变换（缩放、旋转）
 * 如果不需要图片变换可以关闭以节省 Flash
 */
#define LV_IMG_TRANSFORM 1

/**
 * 图片抗锯齿（变换时）
 */
#define LV_IMG_CF_TRUE_COLOR_ALPHA 1
#define LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED 1

/**
 * 圆形绘制精度（0-15）
 * 越大越圆但越慢
 */
#define LV_CIRCLE_MAX_QUERENT 0

/*============================
 * GPU 硬件加速
 *==========================*/

/**
 * i.MX6ULL 的 PXP (Pixel Pipeline) 加速
 * 启用后 blit 操作可提速 5~10 倍
 * 需要内核支持 PXP 驱动
 */
#define LV_USE_GPU_NXP_PXP 0

/**
 * PXP 默认操作超时（毫秒）
 */
#define LV_GPU_NXP_PXP_AUTO_INIT 1

/*============================
 * 日志系统
 *==========================*/

/**
 * 启用日志
 *   开发阶段建议开启
 *   量产时建议关闭或设为 ERROR
 */
#define LV_USE_LOG 1
#if LV_USE_LOG
    /**
     * 日志级别：
     *   LV_LOG_LEVEL_TRACE  - 最详细，性能差
     *   LV_LOG_LEVEL_INFO   - 详细信息
     *   LV_LOG_LEVEL_WARN   - 仅警告（推荐开发用）
     *   LV_LOG_LEVEL_ERROR  - 仅错误（推荐量产用）
     *   LV_LOG_LEVEL_USER   - 用户自定义
     *   LV_LOG_LEVEL_NONE   - 关闭
     */
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

    /**
     * 使用 printf 输出日志（1 = 是, 0 = 否）
     * 设为 0 时需要自己实现 lv_log_register_print_cb
     */
    #define LV_LOG_PRINTF 1

    /**
     * 日志中是否包含函数名
     */
    #define LV_LOG_TRACE_MEM        0
    #define LV_LOG_TRACE_TIMER      0
    #define LV_LOG_TRACE_INDEV      0
    #define LV_LOG_TRACE_DISP_REFR  0
    #define LV_LOG_TRACE_EVENT      0
    #define LV_LOG_TRACE_OBJ_CREATE 0
    #define LV_LOG_TRACE_LAYOUT     0
    #define LV_LOG_TRACE_ANIM       0
#endif /* LV_USE_LOG */

/*============================
 * 断言 (Assert)
 *==========================*/

/**
 * 启用 LVGL 内部断言检查
 * 开发阶段建议开启，量产关闭
 */
#define LV_USE_ASSERT_NULL          1   /* 检查指针是否为 NULL */
#define LV_USE_ASSERT_MALLOC        1   /* 检查内存分配是否成功 */
#define LV_USE_ASSERT_STYLE         0   /* 检查样式合法性（耗性能） */
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /* 检查内存完整性（耗性能） */
#define LV_USE_ASSERT_OBJ           0   /* 检查对象合法性（耗性能） */

/**
 * 自定义断言处理（默认死循环，方便调试）
 */
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);   /* 死循环，调试器可在此设断点 */

/*============================
 * 字体
 *==========================*/

/**
 * 内置英文字体开关
 * 每个字体约增加 10~30KB 二进制体积
 * 只开启你实际使用的字号！
 */
#define LV_FONT_MONTSERRAT_8    0
#define LV_FONT_MONTSERRAT_10   0
#define LV_FONT_MONTSERRAT_12   1   /* 小号文本（状态栏、提示） */
#define LV_FONT_MONTSERRAT_14   1   /* 默认字体（必须至少开一个） */
#define LV_FONT_MONTSERRAT_16   1   /* 中号文本（正文） */
#define LV_FONT_MONTSERRAT_18   0
#define LV_FONT_MONTSERRAT_20   1   /* 大号文本（副标题） */
#define LV_FONT_MONTSERRAT_22   0
#define LV_FONT_MONTSERRAT_24   1   /* 大标题 */
#define LV_FONT_MONTSERRAT_26   0
#define LV_FONT_MONTSERRAT_28   0
#define LV_FONT_MONTSERRAT_30   0
#define LV_FONT_MONTSERRAT_32   0
#define LV_FONT_MONTSERRAT_34   0
#define LV_FONT_MONTSERRAT_36   0
#define LV_FONT_MONTSERRAT_38   0
#define LV_FONT_MONTSERRAT_40   0
#define LV_FONT_MONTSERRAT_42   0
#define LV_FONT_MONTSERRAT_44   0
#define LV_FONT_MONTSERRAT_46   0
#define LV_FONT_MONTSERRAT_48   1   /* 特大标题（温度显示等） */

/**
 * 等宽字体（数字显示更整齐）
 */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0
#define LV_FONT_SIMSUN_16_CJK            0  /* 简体中文 16px（体积大） */

/**
 * 像素完美单色字体
 */
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/**
 * 自定义字体声明（在此声明你用 lv_font_conv 生成的字体）
 * 取消注释并修改为你自己的字体变量名
 */
/* #define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_cn_16) */

/**
 * 默认字体
 */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/**
 * 启用字体压缩以减小体积
 */
#define LV_FONT_FMT_TXT_LARGE 0

/**
 * 字体子像素渲染（提升 LCD 上的文字清晰度）
 */
#define LV_FONT_SUBPX 0

/**
 * 启用字距调整
 */
#define LV_USE_FONT_SUBPX 0
#if LV_USE_FONT_SUBPX
    #define LV_FONT_SUBPX_BGR 0   /* 0: RGB, 1: BGR 子像素排列 */
#endif

/**
 * 启用字体占位符（调试时显示缺失字符的方块）
 */
#define LV_USE_FONT_PLACEHOLDER 1

/*============================
 * 文本设置
 *==========================*/

/**
 * 文本编码
 *   LV_TXT_ENC_UTF8    - UTF-8（推荐）
 *   LV_TXT_ENC_ASCII   - 仅 ASCII
 */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/**
 * 文本换行字符
 */
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/**
 * 文本换行算法
 *   0 = 按字符换行（中文适用）
 *   1 = 按单词换行（英文适用）
 */
#define LV_TXT_LINE_BREAK_LONG_LEN 0

/**
 * 长文本换行时的最小/最大字符数
 */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/**
 * 颜色标记支持（用 #RRGGBB 格式在文本中插入颜色）
 */
#define LV_TXT_COLOR_CMD "#"

/**
 * 文本选择支持
 */
#define LV_USE_TEXTAREA_SELECTION 1

/*============================
 * 控件 (Widgets)
 *==========================*/

/**
 * 基础控件（通常都需要开启）
 */
#define LV_USE_ARC        1   /* 弧形（温控旋钮、进度环） */
#define LV_USE_BAR        1   /* 进度条 */
#define LV_USE_BTN        1   /* 按钮 */
#define LV_USE_BTNMATRIX  1   /* 按钮矩阵（键盘、模式选择） */
#define LV_USE_CANVAS     1   /* 画布 */
#define LV_USE_CHECKBOX   1   /* 复选框 */
#define LV_USE_DROPDOWN   1   /* 下拉框 */
#define LV_USE_IMG        1   /* 图片 */
#define LV_USE_LABEL      1   /* 标签（文本显示） */
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1  /* 允许文本选中 */
    #define LV_LABEL_LONG_TXT_HINT 1   /* 长文本优化（缓存布局） */
#endif
#define LV_USE_LINE       1   /* 线条 */
#define LV_USE_ROLLER     1   /* 滚轮选择器 */
#if LV_USE_ROLLER
    #define LV_ROLLER_INF_PAGES 7  /* 无限模式显示的页数 */
#endif
#define LV_USE_SLIDER     1   /* 滑块 */
#define LV_USE_SWITCH     1   /* 开关 */
#define LV_USE_TEXTAREA   1   /* 文本输入框 */
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500  /* 密码显示时间(ms) */
#endif
#define LV_USE_TABLE      1   /* 表格 */

/**
 * 高级控件（按需开启，不用可以关闭节省空间）
 */
#define LV_USE_ANIMIMG    1   /* 动画图片（GIF 替代品） */
#define LV_USE_CALENDAR   0   /* 日历 */
#define LV_USE_CHART      1   /* 图表（折线、柱状、散点） */
#define LV_USE_COLORWHEEL 0   /* 色轮 */
#define LV_USE_IMGBTN     1   /* 图片按钮 */
#define LV_USE_KEYBOARD   1   /* 虚拟键盘 */
#define LV_USE_LED        1   /* LED 指示灯 */
#define LV_USE_LIST       1   /* 列表 */
#define LV_USE_MENU       0   /* 菜单 */
#define LV_USE_METER      1   /* 仪表盘 */
#define LV_USE_MSGBOX     1   /* 消息框 */
#define LV_USE_SPAN       0   /* 富文本 */
#define LV_USE_SPINBOX    1   /* 数字微调框 */
#define LV_USE_SPINNER    1   /* 加载动画 */
#define LV_USE_TABVIEW    1   /* 标签页 */
#define LV_USE_TILEVIEW   1   /* 平铺视图（滑动页面） */
#define LV_USE_WIN        0   /* 窗口 */

/*============================
 * 主题 (Theme)
 *==========================*/

/**
 * 默认主题
 * 包含一套现代化的控件样式
 */
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    /**
     * 默认颜色模式
     *   0 = 浅色主题 (Light)
     *   1 = 深色主题 (Dark)
     */
    #define LV_THEME_DEFAULT_DARK 0

    /**
     * 默认主题的主色调和副色调
     */
    /* 在代码中初始化主题时指定，此处不需要修改 */

    /**
     * 默认字体（主题使用的）
     */
    #define LV_THEME_DEFAULT_FONT_SMALL    &lv_font_montserrat_12
    #define LV_THEME_DEFAULT_FONT_NORMAL   &lv_font_montserrat_14
    #define LV_THEME_DEFAULT_FONT_LARGE    &lv_font_montserrat_20
#endif

/**
 * Mono 主题（单色屏幕用）
 */
#define LV_USE_THEME_MONO 0

/**
 * 基础主题（最小化样式）
 */
#define LV_USE_THEME_BASIC 1

/*============================
 * 布局 (Layout)
 *==========================*/

/**
 * Flex 布局（弹性盒子）
 */
#define LV_USE_FLEX 1

/**
 * Grid 布局（网格）
 */
#define LV_USE_GRID 1

/*============================
 * 文件系统驱动
 *==========================*/

/**
 * 启用文件系统抽象层
 * 用于从 SD 卡/NAND 加载图片和字体
 */
#define LV_USE_FS_STDIO 1
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER 'A'       /* 驱动字母 A: */
    #define LV_FS_STDIO_PATH ""          /* 路径前缀 */
    #define LV_FS_STDIO_CACHE_SIZE 0     /* 缓存大小（0=不缓存） */
#endif

#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_FATFS 0

/*============================
 * 图片解码器
 *==========================*/

/**
 * BMP 图片支持
 */
#define LV_USE_IMG_BMP 1

/**
 * JPG 图片支持（需要 libjpeg 或 tjpgd）
 */
#define LV_USE_IMG_SJPG 1

/**
 * PNG 图片支持（需要 libpng）
 * 注意：i.MX6ULL 上解码 PNG 较慢，建议转为 LVGL 自定义格式
 */
#define LV_USE_PNG 0

/**
 * GIF 动画支持
 */
#define LV_USE_GIF 1

/**
 * QR 码生成
 */
#define LV_USE_QRCODE 0

/*============================
 * 动画
 *==========================*/

/**
 * 启用动画系统
 */
#define LV_USE_ANIMATION 1

/*============================
 * 其他设置
 *==========================*/

/**
 * 启用性能监控（在屏幕角落显示 FPS 和 CPU 占用）
 * 开发调试用，量产关闭
 */
#define LV_USE_PERF_MONITOR 0
#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
#endif

/**
 * 启用内存监控（在屏幕角落显示内存使用情况）
 */
#define LV_USE_MEM_MONITOR 0
#if LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif

/**
 * 启用重绘区域高亮（调试用，将重绘区域用颜色标出）
 */
#define LV_USE_REFR_DEBUG 0

/**
 * 启用用户数据字段
 */
#define LV_USE_USER_DATA 1

/**
 * GC 垃圾回收（如果使用 MicroPython 等需要）
 */
#define LV_ENABLE_GC 0

/*============================
 * 编译器设置
 *==========================*/

/**
 * 大常量存储位置
 *   0 = RAM
 *   1 = Flash（适合 MCU，i.MX6ULL 不需要）
 */
#define LV_BIG_CONST 0

/**
 * 导出变量属性（通常不需要修改）
 */
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning

/**
 * 启用属性装饰器
 */
#define LV_USE_LARGE_COORD 1   /* 支持大于 2048 的坐标值 */

/*============================
 * 第三方库
 *==========================*/

#define LV_USE_FS_MEMFS 0
#define LV_USE_LODEPNG 0
#define LV_USE_BMP 1
#define LV_USE_RLE 1
#define LV_USE_SJPG 1
#define LV_USE_LIBPNG 0

#endif /* LV_CONF_H */
