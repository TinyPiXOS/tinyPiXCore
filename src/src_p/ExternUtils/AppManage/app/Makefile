#当前目录下所有的.c源文件
SRC_D_C=dopack.c
SRC_M_C=manage.cpp
SRC_P_C=creatpack.cpp
#
OBJ_D=dopack.o
OBJ_M=manage.o
OBJ_TPM=tpmanage.o
OBJ_P=creatpack.o
OBJ_U=unpack.o
#整个文件所有的.o文件
OBJ_ALL=$(shell find ../dopack ../manage -name "*.o")
OBJ_MANAGE=$(shell find ../cppapi/appmanage.o ../manage -name "*.o")
OBJ_DOPACK=$(shell find ../dopack -name "*.o")
OBJ_CREATPACK=$(shell find ../cppapi ../dopack ../manage -name "*.o")
#源码路径
#SRC=../src
#头文件路径
INC=-I../dopack -I../manage -I../cppapi -I../inc
#引用的库
LIB = -lpthread
#伪目标：需要依赖的目标
.PHONY:AppDopack AppManager	AppCreatPack tpManage	#即使该目标存在也重新生成
all:AppDopack AppManager AppCreatPack tpManage	#

AppManager:
	$(CC) -c manage.c $(INC)
	$(CC) -o AppManager $(OBJ_ALL) $(OBJ_M) $(LIB) $(INC) $(G_LIBS) $(G_CFLAGS)	
AppCreatPack:
	$(G++) -c creatpack.cpp $(INC)
	$(G++) -o AppCreatPack $(OBJ_CREATPACK) $(OBJ_P) $(LIB) $(INC) $(G_LIBS) $(G_CFLAGS)
tpManage:
	$(G++) -c tpmanage.cpp $(INC)
	$(G++) -o tpManage $(OBJ_MANAGE) $(OBJ_TPM) $(LIB) $(INC) $(G_LIBS) $(G_CFLAGS)	

