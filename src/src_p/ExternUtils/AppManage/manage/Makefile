#当前目录下所有的.c源文件
SRC_C=$(wildcard *.c)
#当前目录下应有的所有的.o目标文件
OBJ_THIS=$(SRC_C:.c=.o)

G_LIBS = -ltparchive -ljson-c -lssl -lcrypto -lpthread

#整个文件所有的.o文件
OBJ_ALL=$(shell find ./ -name "*.o")
OBJ=manage.o

#伪目标：需要依赖的目标
all:$(OBJ_THIS)
$(OBJ_THIS):%.o:%.c
	$(CC) -c $< -o $@ $(INC) $(LIB_THREAD) $(G_LIBS) $(G_CFLAGS)
