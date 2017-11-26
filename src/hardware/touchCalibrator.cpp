#include "touch.h"
#include "graphic.h"
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

using std::cout;
using std::endl;

typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

//아무 인자가 없으면 기존 결과 불러옴
TouchCalibrator::TouchCalibrator(){
	FILE* resultFile = fopen("calibrated.txt", "r");
	char temp[6][20];
	for(int i = 0; i < 6; i++)
		fgets(temp[i], 20, resultFile);
	fclose(resultFile);
	A = atof(temp[0]);
	B = atof(temp[1]);
	C = atof(temp[2]);
	D = atof(temp[3]);
	E = atof(temp[4]);
	F = atof(temp[5]);
}

//TouchReader 객체를 받으면 켈리브레이션 수행
// Thanks to: 전기박사(Member of Naver cafe "PIPC")
TouchCalibrator::TouchCalibrator(TouchReader &t){
	int LCD_Width = gwidth();
	int LCD_Height = gheight();
	gtext_size(1);
	gcol_text(0, 0, 0);
	int32_t i;
	uint32_t coordinate_X[5];
	uint32_t coordinate_Y[5];
	//Xd: 터치하는곳 X위치
	//Yd: 터치하는곳 Y 위치
	uint16_t Xd[5]={(LCD_Width / 2), 1 * (LCD_Width / 5), 1 * (LCD_Width / 5), 4 * (LCD_Width / 5), 4 * (LCD_Width / 5)};
	uint16_t Yd[5]={(LCD_Height / 2), 1 * (LCD_Height / 5), 4 * (LCD_Height / 5), 4 * (LCD_Height / 5), 1 * (LCD_Height / 5)};
	float d = 0.0, dx1 = 0.0, dx2 = 0.0, dx3 = 0.0, dy1 = 0.0, dy2 = 0.0, dy3 = 0.0;
	uint32_t X2[5];
	uint32_t Y2[5];
	uint32_t XxY[5];
	uint32_t XxXd[5];
	uint32_t YxXd[5];
	uint32_t XxYd[5];
	uint32_t YxYd[5];
	uint32_t alfa = 0, beta = 0, chi = 0, Kx = 0, Ky = 0, Lx = 0, Ly = 0;
	uint16_t epsilon = 0, fi = 0, Mx = 0, My = 0;
	
	//coordinate_X: 입력 X
	//coordinate_Y: 입력 Y

	for(int i = 0; i < 5; i++){
		gclear(255, 255, 255);
		gtext(0, 30, "Touch calibration");
		gplus(Xd[i], Yd[i], 0, 0, 0);
		gupdate();
		while(!t.touched) usleep(1000);
		uint32_t xtemp = 0, ytemp = 0;
		for(int i2 = 0; i2 < 10; i2++){
			xtemp +=t.rawX;
			ytemp +=t.rawY;
		}
		coordinate_X[i] = xtemp/10;
		coordinate_Y[i] = ytemp/10;
		while(t.touched) usleep(1000);
	}

	//X2: X^2
	//XxY: X*Y
	//XxXd: X*Xd
	//XxYd: Y*Xd
	//XxYd: X*Yd
	//YxYd: Y*Yd
	for(i=0;i<5;i++){
		X2[i] = coordinate_X[i]*coordinate_X[i];
		Y2[i] = coordinate_Y[i]*coordinate_Y[i];
		XxY[i] = coordinate_X[i]*coordinate_Y[i];
		XxXd[i] = coordinate_X[i] * Xd[i];
		YxXd[i] = coordinate_Y[i] * Xd[i];
		XxYd[i] = coordinate_X[i] * Yd[i];
		YxYd[i] = coordinate_Y[i] * Yd[i];
	}
	//alfa: X^2	값들을 더한것
	alfa = X2[0] + X2[1] + X2[2] + X2[3] + X2[4];
	//beta: Y^2 값들을 더한것
	beta = Y2[0] + Y2[1] + Y2[2] + Y2[3] + Y2[4];
	//chi: X*Y 값들을 더한것
	chi = XxY[0] + XxY[1] + XxY[2] + XxY[3] + XxY[4];
	//epsilon: 입력된 터치값(X)들을 더한것
	epsilon = coordinate_X[0] + coordinate_X[1] + coordinate_X[2] + coordinate_X[3] + coordinate_X[4];
	//fi: 입력된 터치값(Y)들을 더한것
	fi = coordinate_Y[0] + coordinate_Y[1] + coordinate_Y[2] + coordinate_Y[3] + coordinate_Y[4];
	//Kx: X*Xd 값들 합
	Kx = XxXd[0] + XxXd[1] + XxXd[2] + XxXd[3] + XxXd[4];
	//Ky: X*Yd 값들 합
	Ky = XxYd[0] + XxYd[1] + XxYd[2] + XxYd[3] + XxYd[4];
	//Lx: Y*Xd 값들 합
	Lx = YxXd[0] + YxXd[1] + YxXd[2] + YxXd[3] + YxXd[4];
	//Ly: Y*Yd 값들 합
	Ly = YxYd[0] + YxYd[1] + YxYd[2] + YxYd[3] + YxYd[4];
	//Mx: Xd 합
	Mx = Xd[0] + Xd[1] + Xd[2] + Xd[3] + Xd[4];
	//My: Yd 합
	My = Yd[0] + Yd[1] + Yd[2] + Yd[3] + Yd[4];
//d: 5 * ((X^2합 * Y^2합) - ((X*Y합)^2)) + 2 * ((X*Y합)*터치X합*터치Y합) - (X^2합*터치Y합*터치Y합) - (Y^2합 * 터치X합 * 터치X합)
	d = 5 * ( ((float)alfa * beta) - ((float)chi * chi) ) + 2 * ((float)chi * epsilon * fi) - ((float)alfa * fi * fi ) - ( (float)beta * epsilon * epsilon );
	//a: dx1: X*Xd합, dy1: X*Yd합
	//b: dx1: Y*Xd합, dy1: Y*Yd합
	//c: dx1: Xd합, dy1: Yd합
	//d*1: 5 * ((a*Y^2합) - (b * (X*Y합)) + (터치Y합((b*터치X합)-(a*터치Y합))) +	(c*(((X*Y합)*터치Y합)-(Y^2합*터치X합))
	//d*2: 5 * ((b*X^2합) - (a * (X*Y합)) + (터치X합 *((a*터치Y합)-(b*터치X합))) + (c*(((X*Y합)*터치X합) - (X^2합*터치Y합)))
	//d*3: (a*(((X*Y합)*터치Y합)-(Y^2합*터치X합))) + (b*(((X*Y합)*터치X합) - (X^2합*터치Y합) + (c*((X^2합*Y^2합)-((X*Y합)^2)))
//d*1: dx1이나 dy1, d*2: dx2나 dy2, d*3: dx3나 dy3
//abc가 뭔지는 위에 적었습니다.	
	dx1 = 5 * ( ((float)Kx * beta) - ((float)Lx * chi) ) + ((float)fi * ( ((float)Lx * epsilon) - ((float)Kx * fi) )) + ((float)Mx * ( ((float)chi * fi) - ((float)beta * epsilon) ));
	dx2 = 5 * ( ((float)Lx * alfa) - ((float)Kx * chi) ) + ((float)epsilon * ( ((float)Kx * fi) - ((float)Lx * epsilon) )) + ((float)Mx * ( ((float)chi * epsilon) - ((float)alfa * fi) ));
	dx3 = ((float)Kx * ( ((float)chi * fi) - ((float)beta * epsilon) )) + ((float)Lx * ( ((float)chi * epsilon) - ((float)alfa * fi) )) + ((float)Mx * ( ((float)alfa * beta) - ((float)chi * chi) ));
	dy1 = 5 * ( ((float)Ky * beta) - ((float)Ly * chi) ) + ((float)fi * ( ((float)Ly * epsilon) - ((float)Ky * fi) )) + ((float)My * ( ((float)chi * fi) - ((float)beta * epsilon) ));
	dy2 = 5 * ( ((float)Ly * alfa) - ((float)Ky * chi) ) + ((float)epsilon * ( ((float)Ky * fi) - ((float)Ly * epsilon) )) + ((float)My * ( ((float)chi * epsilon) - ((float)alfa * fi) ));
	dy3 = ((float)Ky * ( ((float)chi * fi) - ((float)beta * epsilon) )) + ((float)Ly * ( ((float)chi * epsilon) - ((float)alfa * fi) )) + ((float)My * ( ((float)alfa * beta) - ((float)chi * chi) ));
	//d**/d
	A = dx1 / d;
	B = dx2 / d;
	C = dx3 / d;
	D = dy1 / d;
	E = dy2 / d;
	F = dy3 / d;
	//파일에 저장
	FILE* resultFile = fopen("calibrated.txt", "w");
	fprintf(resultFile, "%f\n%f\n%f\n%f\n%f\n%f", A, B, C, D, E, F);
	fclose(resultFile);
}

TouchCalibrator::~TouchCalibrator(){}
