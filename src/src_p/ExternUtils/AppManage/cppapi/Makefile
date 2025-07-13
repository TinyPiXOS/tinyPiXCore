#当前目录下所有的.c源文件
SRC_C=$(wildcard *.cpp)
#当前目录下应有的所有的.o目标文件
OBJ_THIS=$(SRC_C:.cpp=.o)
#整个文件所有的.o文件
OBJ_ALL=$(shell find ./ -name "*.o")
INC=-I../dopack -I../manage -I../inc
#伪目标：需要依赖的目标
all:$(OBJ_THIS)
$(OBJ_THIS):%.o:%.cpp
	$(G++) -c $< -o $@ $(INC)