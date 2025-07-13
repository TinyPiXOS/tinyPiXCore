#include <tpUtils.h>
#include <iostream>
#include <cstring>
#include "tpClipboard.h"

int32_t main(int32_t argc, char *argv[])
{
	tpClipboard *clipboard = tpClipboard::Inst();
	
	if(clipboard){
		bool ret = clipboard->hasRichText();
		
		if(ret){
			puts("has rich text");
		}
		else{
			puts("has no rich text");
		}
		
		ret = clipboard->hasPlainText();
		
		if(ret){
			puts("has plain text");
			puts(clipboard->text());
		}
		else{
			puts("has no plain text");
		}
		
		ret = clipboard->hasHtmlText();
		
		if(ret){
			puts("has html text");
		}
		else{
			puts("has no html text");
		}
		
		ret = clipboard->hasUriText();
		
		if(ret){
			puts("has uri text");
		}
		else{
			puts("has no uri text");
		}	
		
		getchar();
		
		ret = clipboard->hasImage();
		
		if(ret){
			puts("has image");
		}
		else{
			puts("has no image");
		}
		
		tpImage *image = clipboard->image();
		
		if(image){
			image->save("test.bmp", tpSurface::SAVE_BMP_FMT);
			delete image;
		}
		
		clipboard->clear();
		
		delete clipboard;
	}
	return 0;
}
