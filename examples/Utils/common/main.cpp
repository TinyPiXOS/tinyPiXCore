#include <tpUtils.h>
#include <iostream>
#include <cstring>
#include "tpCache.h"
#include "tpShm.h"
#include "tpDrop.h"
#include "tpRect.h"
#include "tpSurface.h"
#include "tpColors.h"
#include "tpCanvas.h"
#include "tpFont.h"
#include "tpMD5.h"
#include "tpUUID.h"

static inline int32_t cal_stride(int32_t width, int32_t depth)
{
	int32_t bpp = depth/8;
	int32_t stride = width*bpp;

	switch (depth) {
		case 4:
			stride = (stride + 1)/2;
			break;
		default:
			break;
	}

	return ((stride + 3) & ~3);
}

int32_t main(int32_t argc, char *argv[])
{
	tpFont *font = new tpFont();
	
	font->setFontBackColor(_RGBA(0, 0, 0, 255));
	font->setFontForeColor(_RGBA(255, 0, 0, 255));
	font->setFontDPI(96);
	font->setFontSize(30);
	font->setText("我爱你中国");
	font->setFontStyle(TINY_FONT_UNDERLINE | TINY_FONT_STROKELINE | TINY_FONT_TOPLINE);
	tpFontFamily *family = font->getSysFamilyFont();

	tpSurface *s1 = new tpSurface();
	
	bool ret = s1->fromFile("./1920x1080.bmp", true);

	if(ret){
		tpCanvas *canvas = new tpCanvas(s1);
		
		if(canvas){
			canvas->pixel(1, 1, tpColors::Yellow1);
			canvas->box(20, 20, 50, 50, tpColors::Green1);
			canvas->box(100, 100, 250, 250, _RGBA(255, 255, 255, 128));
			canvas->box(125, 125, 300, 300, _RGBA(255, 0, 0, 64));
			canvas->rectangle(120, 120, 350, 350, tpColors::Green1);
			canvas->thickLine(120, 120, 250, 250, 5, tpColors::Green1);
			canvas->filledPie(240, 320, 200, 45, 135, _RGBA(0, 0, 255, 128));
			canvas->renderText(*font, 200, 200);
			canvas->renderText(*font, 200, 200, "我爱你中国");
			canvas->renderText(*font, 10, 10, "Very good");

			delete canvas;
		}
		
		tpSurface *s2 = s1->rotoZoomXY(0.5, 0.5, 45);
		s2->setAlpha(128);
			
		tpRect s(0, 0, s2->width(), s2->height()), d(0, 0, s2->width(), s2->height());
		
		s2->directBlitT(*s1, s, d);
		
		s1->save("test.png", tpSurface::SAVE_PNG_FMT);
		
		delete s2;
	}

	tpMD5 *md5 = new tpMD5();

	if(md5){
		const char *p = md5->create("连接在计算机网络上的主机在数字信道上传送数据的速率", tpMD5::GEN_512_BYTES);

		if(p){
			puts(p);
			printf("length=%d\n", strlen(p));
		}
		
		delete md5;
	}
	
	tpUUID *uuid = new tpUUID();
	
	if(uuid){
		const char *uuidString = uuid->create();
		puts(uuidString);
		delete uuid;
	}
	
	delete s1;
	delete font;
	
	return 0;
}
