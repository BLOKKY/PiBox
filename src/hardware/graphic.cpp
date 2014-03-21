#include <cstdio>
#include <cstdarg>
#include <gx.h>
#include <iostream>
#include <errno.h>
#include "graphic.h"
#include "gpio.h"

#define	FB	"/dev/fb1"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

dc_t *scr, *buf;
font_t *font12, *font24, *font36;

bool gbegin(){
	if(GX_SUCCESS != gx_open(FB)){
		cerr << "Failed to open framebuffer " << FB << endl;
		return false;
	}
	if(NULL == (scr = gx_get_screen_dc())){
		cerr << "Failed to get device context" << endl;
		gx_close();
		return false;
	}
	if(NULL == (buf = gx_get_compatible_dc(scr))){
		cerr << "Failed to get compatible device context" << endl;
		gx_release_dc(scr);
		gx_close();
		return false;
	}
	if(NULL == (font12 = gx_open_font("font12.bdf"))){
		cerr << "Failed to oepn font font12.bdf" << endl;
		gend();
		return false;
	}
	if(NULL == (font24 = gx_open_font("font24.bdf"))){
		cerr << "Failed to oepn font font24.bdf" << endl;
		gend();
		return false;
	}
	if(NULL == (font36 = gx_open_font("font36.bdf"))){
		cerr << "Failed to oepn font font36.bdf" << endl;
		gend();
		return false;
	}
	return true;
}

int gwidth(){
	return buf->width;
}

int gheight(){
	return buf->height;
}

void gend(){
	gx_release_dc(buf);
	gx_release_dc(scr);
	gx_close();
}

void gupdate(){
	gx_bitblt(scr, 0, 0, buf, 0, 0, buf->width-1, buf->height-1);
	backlight_on();
}

//화면을 지움
void gclear(int r, int g, int b){
	gx_clear(buf, gx_color(r, g, b, 255));
}

//화면을 지우되 그라데이션을 사용
void gclear(int startR, int startG, int startB, int endR, int endG, int endB){
	int r = startR, g = startG, b = startB;
	gx_clear(buf, gx_color(r, g, b, 255));
	for(int x = buf->width-128; x < buf->width-1 ;x++){
		color_t c = gx_color(r, g ,b, 255);
		gx_vline(buf, x, 0, buf->height-1, c);
		if(r < endR) r+=2;
		if(g < endG) g+=2;
		if(b < endB) b+=2;
		if(r > endR) r-=2;
		if(g > endG) g-=2;
		if(b > endB) b-=2;
	}
}

void gcol_text(int r, int g, int b){
	buf->font_color = gx_color(r, g, b, 255);
}

void gcol_pen(int r, int g, int b){
	gx_pen_color(buf, gx_color(r, g, b, 255));
}

void gcol_brush(int r, int g, int b){
	gx_brush_color(buf, gx_color(r, g, b, 255));
}

void gtext_size(int size){
	switch(size){
		case 1:{
			buf->font = font12;
			break;
		}case 2:{
			buf->font = font24;
			break;
		}case 3:{
			buf->font = font36;
			break;
		}
	}
}

void grect(int sx, int sy, int ex, int ey){	//sx: 시작 x, sy: 시작 y, ex: 끝 x, ey: 끝 y
	gx_rectangle(buf, sx, sy, ex, ey);
}

//십자가 그림
void gplus(int x, int y, int r, int g, int b){
	color_t c = gx_color(r, g, b, 255);
	//가로선
	gx_hline(buf, x-5, x+5, y, c);
	//세로선
	gx_vline(buf, x, y-5, y+5, c);
}

//글자 쓰기
void gtext(int x, int y, string format, ...){
	char str[100];
	va_list args;
	va_start(args, format);
	vsprintf(str, format.c_str(), args);
	va_end(args);
	gx_text_out(buf, x, y, str);
}

bool gfillbmp(string filename){
	dc_t *b;
	if(!(b = gx_bmp_open(const_cast<char*>(filename.c_str())))){
		cerr << "Failed to open " << filename << endl;
		return false;
	}
	gx_bitblt(buf, 0, 0, (dc_t *)b, 0, 0, b->width-1, b->height-1);
	gupdate();
	return true;
}
