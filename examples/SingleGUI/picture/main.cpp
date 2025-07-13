#include "tpApp.h"
#include "tpFixScreen.h"
#include "tpDialog.h"
#include "tpColors.h"
#include "tpPanel.h"
#include "tpSurface.h"
#include "tpButton.h"
#include "tpGridLayout.h"
#include "tpHelper.h"
#include "tpPicture.h"
#include "tpCanvas.h"

//also you can use cairo 
static inline bool reDraw(tpPicture *picture, tpSurface *surface, tpCanvas *canvas,  int32_t width, int32_t height)
{
	tpSurface *tmpSurface = picture->zoomRotateXY(1, 1, 45);
	
	if(tmpSurface){
		canvas->paintSurface(*tmpSurface, nullptr, nullptr);
		delete tmpSurface;
		return true;
	}
	
	return false;
}

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true);//vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	vScreen->update();
	
	tpDialog *floatScreen = new tpDialog();
	floatScreen->setBackGroundColor(tpColors::Red);//or use tpColors
	floatScreen->setRect(100, 100, 300, 300);
	floatScreen->setAlpha(128);
	floatScreen->setVisible(true);
	floatScreen->setBeMoved(true);
	floatScreen->update();
	
	tpPicture *picture1 = new tpPicture(vScreen);
	picture1->setRect(100, 100, 200, 300);
	picture1->pictureFromFile("./exam2.bmp");
	picture1->setBlitType(tpPicture::TP_PIC_ROUND);
	picture1->setVisible(true);
	picture1->update();
	
	tpPicture *picture2 = new tpPicture(floatScreen);
	picture2->setRect(0, 0, 300, 300);
	picture2->pictureFromFile("./exam2.bmp");
	picture2->setVisible(true);
	picture2->update();
	
	tpPicture *picture3 = new tpPicture(vScreen);
	picture3->setRect(100, 250, 400, 500);
	picture3->pictureFromFile("./exam2.bmp");
	picture3->setVisible(true);
	picture3->setSelfBlitFunction(reDraw);//set blit function
	picture3->setBlitType(tpPicture::TP_PIC_SELF);//call redraw
	picture3->update();
	
	return app.run();
}
