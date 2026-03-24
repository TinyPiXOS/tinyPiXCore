# TpChart 示例程序 Makefile
# 支持本地编译和交叉编译

# 交叉编译说明：
# make CROSS_COMPILE=aarch64-linux-gnu- LIB_PATH="-L/home/pix/pix_depend/tinyPiXNix_2.0/sysroot/arm_64/lib -L/home/pix/pix_dev/TinyPiXOS/install/arm_64/lib"
# 如果遇到问题可以加上 -Wl,-rpath-link，示例如下：
# make CROSS_COMPILE=aarch64-linux-gnu- LIB_PATH="-L/home/pix/pix_depend/tinyPiXNix_2.0/sysroot/arm_64/lib -L/home/pix/pix_dev/TinyPiXOS/install/arm_64/lib -Wl,-rpath-link=/home/pix/pix_depend/tinyPiXNix_2.0/sysroot/arm_64/lib -Wl,-rpath-link=/home/pix/pix_dev/TinyPiXOS/install/arm_64/lib"

CROSS_COMPILE ?=
G++ = $(CROSS_COMPILE)g++

LIB_PATH ?= -L/usr/lib -L/usr/lib/TinyPiX

INC_PATH ?= -I. \
      -I/usr/include \
      -I/usr/include/glib-2.0 \
      -I/usr/include/lib/x86_64-linux-gnu/glib-2.0/include \
      -I/usr/include/TinyPiX \
      -I/usr/include/TinyPiX/TpCore \
      -I/usr/include/TinyPiX/TpUtils \
      -I/usr/include/TinyPiX/TpGUI \
      -I/usr/include/TinyPiX/TpGUI/GUI \
      -I/usr/include/TinyPiX/TpGUI/Core \
      -I/usr/include/TinyPiX/TpGUI/Screen \
      -I/usr/include/TinyPiX/TpGUI/Widgets \
      -I/usr/include/TinyPiX/TpWM

# 链接的库
LIB = -lTpUtils -lTpCore -lTpGUI -lpthread -ldl

CFLAGS = #-Wall -O2 -g

# 伪目标：即使该目标存在也重新生成
.PHONY: all clean TpChart

# 默认目标
all: TpChart clean

TpChart:
	$(G++) $(CFLAGS) -c main.cpp $(INC_PATH)
	$(G++) $(CFLAGS) -c MainWindowService.cpp $(INC_PATH)
	$(G++) $(CFLAGS) -o TpChart main.o MainWindowService.o $(LIB) $(LIB_PATH)

clean:
	rm -f *.o
