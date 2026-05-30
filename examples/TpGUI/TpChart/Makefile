CROSS_COMPILE ?=
G++ = $(CROSS_COMPILE)g++

LIB_PATH ?= -L/usr/lib -L/usr/lib/TinyPiX
RPATH ?= -Wl,-rpath,/usr/lib/TinyPiX

INC_PATH ?= -I. \
      -I../../../src/include \
      -I../../../src/include/TpCore \
      -I../../../src/include/TpUtils \
      -I../../../src/include/TpGUI \
      -I../../../src/include/TpGUI/GUI \
      -I../../../src/include/TpGUI/Widgets \
      -I/usr/include \
      -I/usr/include/SDL2 \
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
      -I/usr/include/TinyPiX/TpGUI/app \
      -I/usr/include/TinyPiX/TpWM

LIB = \
      -lTpGUI -lTpUtils -lTpCore \
      -lwebp \
      -lturbojpeg \
      -lpng \
      -fopenmp -lgomp \
      -lrime -lyaml-cpp -lmarisa -lopencc -lleveldb -lglog \
      -lSDL2 -lgobject-2.0 -lglib-2.0 \
      -lgbm -lGLESv2 -lEGL \
      -ldl -lpthread

LOCAL_SOURCE = \
      ../../../src/src/TpGUI/Widgets/TpChart.cpp \
      ../../../src/src/TpGUI/Widgets/TpSeries.cpp \
      ../../../src/src/TpGUI/Widgets/TpRenderUtils.cpp

CFLAGS = #-Wall -O2 -g

.PHONY: all clean TpChart TpChartAdvanced

all: TpChart TpChartAdvanced

TpChart:
	$(G++) $(CFLAGS) -c main.cpp $(INC_PATH)
	$(G++) $(CFLAGS) -c MainWindowService.cpp $(INC_PATH)
	$(G++) $(CFLAGS) -o TpChart main.o MainWindowService.o $(LIB) $(LIB_PATH) $(RPATH)

TpChartAdvanced:
	$(G++) $(CFLAGS) -c main_advanced.cpp $(INC_PATH)
	$(G++) $(CFLAGS) -c MainWindowServiceAdvanced.cpp $(INC_PATH)
	$(G++) $(CFLAGS) -c $(LOCAL_SOURCE) $(INC_PATH)
	$(G++) $(CFLAGS) -o TpChartAdvanced main_advanced.o MainWindowServiceAdvanced.o TpChart.o TpSeries.o TpRenderUtils.o $(LIB) $(LIB_PATH) $(RPATH)

clean:
	rm -f *.o TpChart TpChartAdvanced
