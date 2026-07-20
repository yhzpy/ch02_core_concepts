# =============================================================================
# LVGL 核心概念演示 — 构建脚本
# 用法：make / make sim / make clean / make deploy / make run
# =============================================================================

CROSS_CC   = arm-linux-gnueabihf-gcc
SIM_CC     = gcc
TARGET     = core_demo

BOARD_IP   ?= 192.168.1.100
BOARD_USER ?= root
BOARD_DIR  ?= /opt/lvgl

LVGL_DIR   ?= ../../ch01_environment/lvgl

# ---- 源文件 ----
APP_SRCS = main.c \
           src/display.c \
           src/input.c \
           src/ui_obj_tree.c \
           src/ui_events.c \
           src/ui_styles.c \
           src/ui_lifecycle.c

LVGL_SRCS = $(shell find $(LVGL_DIR)/src -name "*.c" 2>/dev/null)
ALL_SRCS  = $(APP_SRCS) $(LVGL_SRCS)
ALL_OBJS  = $(ALL_SRCS:.c=.o)

INCLUDES = -I. -I$(LVGL_DIR)/..
COMMON_CFLAGS = -Wall -Wextra -Wno-unused-parameter $(INCLUDES) -DLV_CONF_INCLUDE_SIMPLE

# 交叉编译
CFLAGS  = -O2 $(COMMON_CFLAGS)
LDFLAGS = -lm -lpthread

# 模拟器
SIM_CFLAGS  = -O0 -g -DDEBUG -DSIMULATOR $(COMMON_CFLAGS) \
              $(shell sdl2-config --cflags 2>/dev/null)
SIM_LDFLAGS = -lm -lpthread $(shell sdl2-config --libs 2>/dev/null)

.PHONY: all sim clean deploy run help

all: $(TARGET)

sim: CC = $(SIM_CC)
sim: CFLAGS = $(SIM_CFLAGS)
sim: LDFLAGS = $(SIM_LDFLAGS)
sim: $(TARGET)_sim
	@echo "Run: ./$(TARGET)_sim"

$(TARGET): $(ALL_OBJS)
	@$(CROSS_CC) $(ALL_OBJS) -o $@ $(LDFLAGS)
	@echo "Build OK: $@"

$(TARGET)_sim: $(ALL_OBJS)
	@$(SIM_CC) $(ALL_OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

deploy: $(TARGET)
	@ssh $(BOARD_USER)@$(BOARD_IP) "mkdir -p $(BOARD_DIR)"
	@scp $(TARGET) $(BOARD_USER)@$(BOARD_IP):$(BOARD_DIR)/

run: deploy
	@ssh $(BOARD_USER)@$(BOARD_IP) "$(BOARD_DIR)/$(TARGET)"

clean:
	@find . -name "*.o" -delete
	@rm -f $(TARGET) $(TARGET)_sim

help:
	@echo "make        Cross-compile for i.MX6ULL"
	@echo "make sim    Compile PC simulator (SDL2)"
	@echo "make deploy SCP to board"
	@echo "make run    Deploy + run"
	@echo "make clean  Remove artifacts"
	@echo ""
	@echo "LVGL_DIR=$(LVGL_DIR)"
