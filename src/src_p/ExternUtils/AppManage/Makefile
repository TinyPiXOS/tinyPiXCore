export CC = gcc
export G++ = g++
#当前目录下所有的.c源文件
SRC_C=$(wildcard *.c)
#整个文件所有的.o文件
OBJ_ALL=$(shell find ./ -name "*.o")
#源码路径
SRC=./manage ./dopack ./cppapi
USER=./app
#头文件路径
export INC=-I./dopack -I./manage -I./cppapi -I./inc
#export G_CFLAGS = `pkg-config --cflags gstreamer-1.0`	#
#引用的库
export G_LIBS = -ltparchive -ljson-c -lssl -lcrypto#
#伪目标：需要依赖的目标
all:$(SRC) $(USER) clean
#
.PHONY:$(SRC) $(USER)
$(SRC):
	$(MAKE) -C $@
$(USER):
	$(MAKE) -C $@
#清除
clean:
	rm $(OBJ_ALL)
#	rm *.o