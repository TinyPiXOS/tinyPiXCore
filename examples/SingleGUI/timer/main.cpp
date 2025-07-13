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
#include "tpFont.h"
#include "tpTimer.h"
#include <math.h>
#include <cairo.h>
#include <ctime>

class clockPicture : public tpPicture
{
public slots:
	clockPicture(tpObject *parent) : tpPicture(parent)
	{
	}
	void doTimer()
	{
		this->update();
	};
};

static inline float distace(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

static inline void drawPointer(tpCanvas *canvas, tpFont *font, double cx, double cy, double rad, int32_t margin)
{
	std::time_t now = std::time(nullptr);
	std::tm *local_time = std::localtime(&now);

	int32_t year = local_time->tm_year + 1900;
	int32_t month = local_time->tm_mon + 1;
	int32_t day = local_time->tm_mday;
	int32_t hour = local_time->tm_hour;
	int32_t minute = local_time->tm_min;
	int32_t second = local_time->tm_sec;

	char clock[128];
	sprintf(clock, "%04d年%02d月%02d日", year, month, day);
	font->setText(clock);
	ItpSize size = font->pixelSize();
	font->setFontForeColor(tpColors::Firebrick);
	int32_t cCx = cx - size.w / 2, cCy = cy + rad / 4;

	canvas->renderText(*font, cCx, cCy);

	int32_t sec_angle = 0;
	if (second <= 15)
	{
		sec_angle = second * 6 + 270;
	}
	else
	{
		sec_angle = (second - 15) * 6;
	}

	int32_t sec_x = cx + (rad - margin * 12) * cos(sec_angle * M_PI / 180);
	int32_t sec_y = cy + (rad - margin * 12) * sin(sec_angle * M_PI / 180);

	canvas->antiAliasingLine(cx, cy, sec_x, sec_y, _RGB(255, 0, 0));
	canvas->antiAliasCircle(cx, cy, 100, _RGBA(128, 128, 128, 128));

	int32_t min_angle = 0;

	if (minute <= 15)
	{
		min_angle = minute * 6 + 270;
	}
	else
	{
		min_angle = (minute - 15) * 6;
	}
	int32_t min_x = cx + (rad - margin * 18) * cos(min_angle * M_PI / 180);
	int32_t min_y = cy + (rad - margin * 18) * sin(min_angle * M_PI / 180);

	canvas->antiAliasingLine(cx, cy, min_x, min_y, _RGB(0, 255, 0));

	int32_t hour_angle = 0;
	if (hour <= 3)
	{
		hour_angle = hour * 30 + 270;
	}
	else
	{
		hour_angle = (hour - 3) * 30;
	}

	int32_t hour_x = cx + (rad - margin * 24) * cos(hour_angle * M_PI / 180);
	int32_t hour_y = cy + (rad - margin * 24) * sin(hour_angle * M_PI / 180);

	canvas->antiAliasingLine(cx, cy, hour_x, hour_y, _RGB(0, 0, 255));
	canvas->filledCircle(cx, cy, 4, _RGB(0, 0, 0));
	canvas->antiAliasCircle(cx, cy, 4, _RGB(0, 0, 0));
}

// also you can use cairo
static inline bool reDraw(tpPicture *picture, tpSurface *surface, tpCanvas *canvas, int32_t width, int32_t height)
{
	static int32_t count = 0;
	static cairo_surface_t *cairo_clock = nullptr;
	static tpFont font;
	static ItpSize size;

	int32_t margin = 5;
	int32_t minute_len = 10;
	double rad = TP_MIN(width, height) / 2.0;
	double cx = width / 2.0, cy = height / 2.0;

	if (count++ == 0)
	{
		cairo_surface_t *cairo_frame = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

		if (cairo_frame == nullptr)
		{
			return false;
		}

		cairo_t *cairo_frame_cr = cairo_create(cairo_frame);

		if (cairo_frame_cr == nullptr)
		{
			cairo_surface_destroy(cairo_frame);
			return false;
		}

		cairo_surface_t *cairo_image = tpCanvas::convertFromSurfaceToCairo(surface);

		if (cairo_image == nullptr)
		{
			cairo_destroy(cairo_frame_cr);
			cairo_surface_destroy(cairo_frame);
			return false;
		}

		cairo_t *cairo_image_cr = cairo_create(cairo_image);

		if (cairo_image_cr == nullptr)
		{
			cairo_destroy(cairo_frame_cr);
			cairo_surface_destroy(cairo_frame);
			cairo_surface_destroy(cairo_image);
			return false;
		}

		cairo_set_source_surface(cairo_frame_cr, cairo_image, 0, 0);

		for (int32_t i = 0; i < 60; i++)
		{
			double x1 = cx + (rad - margin) * cos(i * 6.0 * M_PI / 180.0);
			double y1 = cy + (rad - margin) * sin(i * 6.0 * M_PI / 180.0);
			double x2 = cx + (rad - minute_len) * cos(i * 6.0 * M_PI / 180.0);
			double y2 = cy + (rad - minute_len) * sin(i * 6.0 * M_PI / 180.0);

			cairo_move_to(cairo_image_cr, x1, y1);
			cairo_line_to(cairo_image_cr, x2, y2);
		}

		tpCanvas *tmpCanvas = new tpCanvas(surface);

		font.setFontStyle(TINY_FONT_BOLD);

		int32_t hour_len = 20;
		for (int32_t i = 0; i < 12; i++)
		{
			double x1 = cx + (rad - margin) * cos(i * 30.0 * M_PI / 180.0);
			double y1 = cy + (rad - margin) * sin(i * 30.0 * M_PI / 180.0);
			double x2 = cx + (rad - hour_len) * cos(i * 30.0 * M_PI / 180.0);
			double y2 = cy + (rad - hour_len) * sin(i * 30.0 * M_PI / 180.0);

			cairo_move_to(cairo_image_cr, x1, y1);
			cairo_line_to(cairo_image_cr, x2, y2);

			if (i == 0)
			{
				font.setText(tpHelper::intToString(i + 3).c_str());
				size = font.pixelSize();
				tmpCanvas->renderText(font, (int32_t)x2 - size.w - size.h / 4, (int32_t)y2 - size.h / 2);
			}
			else if (i == 3)
			{
				font.setText(tpHelper::intToString(i + 3).c_str());
				size = font.pixelSize();
				tmpCanvas->renderText(font, (int32_t)x2 - size.w / 2, (int32_t)y2 - size.h);
			}
			else if (i == 6)
			{
				font.setText(tpHelper::intToString(i + 3).c_str());
				size = font.pixelSize();
				tmpCanvas->renderText(font, (int32_t)x2 + size.h / 4, (int32_t)y2 - size.h / 2);
			}
			else if (i == 9)
			{
				font.setText(tpHelper::intToString(i + 3).c_str());
				size = font.pixelSize();
				tmpCanvas->renderText(font, (int32_t)x2 - size.w / 2, (int32_t)y2);
			}
		}

		cairo_stroke(cairo_image_cr);
		cairo_set_line_width(cairo_frame_cr, 2);
		cairo_arc(cairo_frame_cr, cx, cy, rad, 0, 2 * M_PI);
		cairo_clip(cairo_frame_cr);

		cairo_paint(cairo_frame_cr);
		cairo_stroke(cairo_frame_cr);

		tpSurface *res = tpCanvas::convertFromCairoToSurface(cairo_frame);

		if (res)
		{
			canvas->paintSurface(*res, nullptr, nullptr, false);
			delete res;
		}

		cairo_surface_write_to_png(cairo_frame, "./clock.png");

		cairo_surface_destroy(cairo_image);
		cairo_surface_destroy(cairo_frame);
		cairo_destroy(cairo_image_cr);
		cairo_destroy(cairo_frame_cr);

		drawPointer(canvas, &font, cx, cy, rad, margin);

		delete tmpCanvas;
	}
	else
	{
		if (cairo_clock == nullptr)
		{
			cairo_clock = cairo_image_surface_create_from_png("./clock.png");
		}

		tpSurface *res = tpCanvas::convertFromCairoToSurface(cairo_clock);

		if (res)
		{
			canvas->paintSurface(*res, nullptr, nullptr, false);
			delete res;
		}

		drawPointer(canvas, &font, cx, cy, rad, margin);
	}

	return false;
}

int32_t main(int32_t argc, char *argv[])
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();

	tpShared<tpSurface> surface = tpMakeShared<tpSurface>();
	surface->fromFile("exam1.bmp");
	vScreen->setBackGroundImage(surface);

	vScreen->setVisible(true); // vScreen setvisible will be update display
	app.bindVScreen(vScreen);
	vScreen->update();

	int32_t width = 499, height = 312;
	int32_t cx = (vScreen->width() - width) / 2, cy = (vScreen->height() - height) / 2;

	clockPicture *picture3 = new clockPicture(vScreen);
	picture3->setRect(cx, cy, width, height);
	picture3->pictureFromFile("./exam2.bmp");
	picture3->setVisible(true);
	picture3->setSelfBlitFunction(reDraw);			// set blit function
	picture3->setBlitType(tpPicture::TP_PIC_SELF); // call redraw
	picture3->update();

	tpTimer *timer = new tpTimer(1000);
	connect(timer, SIGNALS(tpTimer, timeout), picture3, SLOTS(clockPicture, doTimer));
	timer->start();

	return app.run();
}
