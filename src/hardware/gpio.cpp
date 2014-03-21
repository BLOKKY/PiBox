//버튼/LCD 백라이트 관리
#include "gpio.h"
#include <wiringPi.h>
#include <cmath>
#include <cstdio>
#include <iostream>

using std::cout;
using std::endl;

#define BACKLIGHT	1	//GPIO 18(PWM)
#define	BUTTON1		3	//GPIO 22
#define	BUTTON2		2	//GPIO 27

//주어진 입력범위와 출력범위로 특정 값을 출력범위에 맞게 계산
//숫자와 최대값으로 백분율 구하는 식(혹은 백분율과 최대값으로 숫자를 구하는 식) 과 비슷함
#define map(in_max, out_max, value)	(value/ in_max) * out_max

ButtonReader ioinit(){
	wiringPiSetup();
	pinMode(BACKLIGHT, PWM_OUTPUT);
	pinMode(BUTTON1, INPUT);
	pinMode(BUTTON2, INPUT);
	return ButtonReader();	//버튼 읽는 쓰레드 반환
}

int brightness = 255;
bool turnedOn = true;	//백라이트가 켜져있는지

//백라이트 끄기
void backlight_off(){
	if(!turnedOn) return;	//백라이트가 이미 꺼졌다면 중단
	turnedOn = false;
	//천천히 백라이트를 끔
	int minus = 1;
	for(int i = brightness; i >= 0; i-=minus){
		pwmWrite(BACKLIGHT, i);
		delay(10);
		minus++;
	}
	pwmWrite(BACKLIGHT, 0);
}

//백라이트 켜기
void backlight_on(){
	if(turnedOn) return;	//백라이트가 이미 켜졌다면 중단
	turnedOn = true;
	int value;	//밝기 설정값(0 ~ 255)
	FILE* fp = fopen("brightness.pbdat", "r");	//밝기 설정 파일을 읽기 모드로 연다
	if(fp == NULL){	//열 수 없으면
		fp = fopen("brightness.pbdat", "w");	//밝기 설정 파일을 쓰기 모드로 연다
		if(fp) fputc(255, fp);	//파일이 열렸다면 파일에 씀.
		else{
			cout << "Warning: failed to open backlight config as writing mode." << endl; 
		}
		value = 255;		//밝기 설정값을 255로 설정
		if(fp) fclose(fp);	//파일이 열렸다면 쓰기 모드로 열린 파일을 닫는다.
	}else{	//열렸으면
		value = fgetc(fp);	//설정값을 읽음
		if(value == -1){
			cout << "Warning: failed to read backlight config!" << endl; 
			value = 255;	//읽을 수 없다면 255로 설정
		}
		fclose(fp);	//파일을 닫는다.
	}
	brightness = map(255, 1023, value);
	//천천히 백라이트를 켬
	int plus = 1;
	for(int i = 0; i <= brightness; i+=plus){
		pwmWrite(BACKLIGHT, i);
		delay(10);
		plus++;
	}
	pwmWrite(BACKLIGHT, brightness);
}

ButtonReader::ButtonReader(){
	btn1 = false;
	btn2 = false;
}

//버튼 값을 읽는 쓰레드 함수
int ButtonReader::run(){
	while(1){
		if(digitalRead(BUTTON1) == LOW) btnBack = true;
		else{
			btn1 = false;
		}
		if(digitalRead(BUTTON2) == LOW) 
	}
	return 0;
}
