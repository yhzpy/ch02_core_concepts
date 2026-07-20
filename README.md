# LVGL 核心概念演示 — 第二章配套项目

## 项目概述

本项目通过 4 个交互式演示页面，完整展示 LVGL 的核心概念：

| 页面 | 演示内容 |
|------|----------|
| **ObjTree** | 对象树、父子关系、遍历、flags（HIDDEN）、lv_obj_clean |
| **Events** | 事件回调、多事件码、事件冒泡、user_data、stop_bubbling |
| **Styles** | 全局/局部样式、Part（MAIN/INDICATOR/KNOB）、State、过渡动画 |
| **Lifecycle** | 创建/销毁、lv_obj_del_async、lv_timer、内存监控 |

## 文件结构

```
core_demo/
├── lv_conf.h           ← LVGL 配置（与第1章相同）
├── Makefile            ← 构建脚本
├── main.c              ← 入口 + 导航框架
├── README.md
└── src/
    ├── display.h/c     ← Framebuffer 驱动（复用第1章）
    ├── input.h/c       ← 触摸驱动（复用第1章）
    ├── ui.h            ← UI 接口
    ├── ui_obj_tree.c   ← 演示1：对象树
    ├── ui_events.c     ← 演示2：事件系统
    ├── ui_styles.c     ← 演示3：样式系统
    └── ui_lifecycle.c  ← 演示4：生命周期
```

## 编译运行

### 前置条件

确保 LVGL 源码在正确位置（默认引用第1章的 lvgl 目录）：
```
ch01_environment/lvgl/    ← LVGL 源码
ch02_core_concepts/core_demo/  ← 本项目
```

如果路径不同：
```bash
make LVGL_DIR=/path/to/lvgl
```

### 交叉编译（i.MX6ULL）

```bash
make
scp core_demo root@<板子IP>:/opt/
ssh root@<板子IP> "/opt/core_demo"
```

### PC 模拟器

```bash
make sim
./core_demo_sim
```

## 操作说明

启动后底部有 4 个导航按钮，点击切换演示页面。

### ObjTree 页面
- **Add Child** — 在蓝色容器中创建子对象
- **Traverse** — 在日志区打印对象树结构
- **Clean All** — 清空容器（lv_obj_clean）
- **Toggle HIDDEN** — 隐藏/显示容器

### Events 页面
- 点击/长按 "Press / Hold Me" 观察不同事件码
- 点击 Child A/B 观察事件冒泡（B 会阻止冒泡）
- 点击 #1/#2/#3 观察 user_data 传递

### Styles 页面
- 按住蓝色按钮观察 PRESSED 状态 + 过渡动画
- 拖动滑块观察 INDICATOR 和 KNOB 部件样式
- 点击 "Toggle Dark" 观察局部样式覆盖全局样式
- 切换开关观察 CHECKED 状态

### Lifecycle 页面
- **Create** — 创建对象（点击对象可异步删除自身）
- **Del First** — 立即删除第一个对象
- **Clean All** — 清空所有
- **Pause Timer** — 暂停/恢复定时器
- 右上角显示内存使用和定时器计数

## 清理

```bash
make clean
```
