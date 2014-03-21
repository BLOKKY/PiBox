#ifndef TOUCH_H
#define TOUCH_H
#include "../util/thread.h"

class TouchReader : public Thread{
	public:
		TouchReader();
		~TouchReader();
		int run();
		void setCalibration(float A, float B, float C, float D, float E, float F);
		int x, y;
		float rawX, rawY;
		bool touched;
	private:
		bool readX, readY;
		int fd;
		float A, B, C, D, E, F;	//터치 보정값
};

class TouchCalibrator{
	public:
		TouchCalibrator();
		TouchCalibrator(TouchReader &t);
		~TouchCalibrator();
		float A, B, C, D, E, F;
};

#endif
