#当前目录下所有的.c源文件
SRC_C=$(wildcard *.c)
#当前目录下应有的所有的.o目标文件
OBJ_THIS=$(SRC_C:.c=.o)
#依赖的库
G_LIBS = -ltparchive
#整个文件所有的.o文件
OBJ_ALL=$(shell find ./ -name "*.o")
OBJ=pixpkg.o

#伪目标：需要依赖的目标
all:$(OBJ_THIS)
$(OBJ_THIS):%.o:%.c
	$(CC) -c $< -o $@ $(G_LIBS) $(G_CFLAGS) -I../manage
