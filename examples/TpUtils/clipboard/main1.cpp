#include <TpUtils.h>
#include <iostream>
#include <cstring>
#include "TpClipboard.h"

int32_t main(int32_t argc, char *argv[])
{
	TpClipboard *clipboard = TpClipboard::Inst();
	
	if(clipboard){
		clipboard->setPlainText("I am a good boy!");
		bool ret = clipboard->hasPlainText();
		
		if(ret){
			puts("has plain text");
		}
		else{
			puts("has no plain text");
		}
		
		getchar();
		
		TpImage *image = new TpImage();
		
		if(image){
			ret = image->fromFile("1920x1080.bmp", true);
			
			if(ret){
				clipboard->setImage(image);
			}
			
			delete image;
		}
		
		delete clipboard;
	}
	
	return 0;
}
