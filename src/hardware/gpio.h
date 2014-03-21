#ifndef GPIO_H
#define GPIO_H
#include "../util/thread.h"
class ButtonReader : public Thread{
	public:
		ButtonReader();
		int run();
		bool btn1, btn2;	//각 버튼이 눌렸는지 저장
};
ButtonReader ioinit();
void backlight_off();
void backlight_on();

#endif
