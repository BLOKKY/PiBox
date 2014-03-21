#include "hardware/graphic.h"
#include "hardware/touch.h"
#include "hardware/gpio.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <time.h>
#include <cstring>

using std::cout;
using std::cerr;
using std::endl;

TouchReader t;

const char* ver = "A8";

void printHelp();
void printVer();
string getTime();
string getDate();

int main(int argc, char* argv[]){
	ButtonReader b;
	b = ioinit();
	backlight_off();
	if(!gbegin()) exit(1);
	t.start();	//터치 감지 쓰레드 시작
	b.start();	//버튼 감지 쓰레드 시작
	//인자 확인
	opterr = 0;
	int option;
	bool run_calibrate = false;
	while( -1 !=(option = getopt( argc, argv, "chv"))){
		switch(option){
			case 'c':{	//켈리브레이션 실행
				run_calibrate = true;	//실행하도록 마크
				break;
			}case 'h':{
				printHelp();
			}case 'v':{
				printVer();
			}case '?':{	//알 수 없는 옵션
				cerr << "Unknown option: " << optopt;
				cout << "run pb -h to see help" << endl;
			}
		}
	}
	//보정값 설정
	if(run_calibrate){
		TouchCalibrator tc(t);	//보정 실행
		t.setCalibration(tc.A, tc.B, tc.C, tc.D, tc.E, tc.F);
	}else{
		TouchCalibrator tc;	//기존 보정값 가져옴
		t.setCalibration(tc.A, tc.B, tc.C, tc.D, tc.E, tc.F);
	}
	while(1){
		gclear(255, 0, 0, 0, 0, 0);
		gtext_size(3);
		gtext(0, 50, "PiBox");
		gtext_size(2);
		gtext(0, 100, "TOOLS");
		gtext(0, 140, "PLAY");
		gtext(0, 180, "SYSTEM");
		gupdate();
		while(!t.touched){
			bool quit = false;
			if(b.btnBack == true){
				while(b.btnBack == true) usleep(1000);
				backlight_off();
				//시스템 종료 화면 보이기
				while(1){	//뒤로가기 버튼이 눌리면 홈으로 나가기
					gclear(0, 0, 0);
					gtext_size(3);
					gtext(0, 50, "Shutdown");
					gtext_size(2);
					gtext(0, 100, "Halt");
					gtext(0, 140, "Reboot");
					gupdate();
					while(!t.touched){
						if(b.btnBack == true){
							while(b.btnBack == true) usleep(1000);
							quit = true;
							break;
						}
						usleep(1000);
					}
					if(quit)
						break;
					int x = t.x;
					int y = t.y;
					if(x < 160){
						if(60 < y && y < 100){	//Halt 선택됨
							backlight_off();
							system("halt");
						}else if(100 < y && y < 140){	//Reboot 선택됨
							backlight_off();
							system("reboot");
						}
					}
				}
				backlight_off();
			}
			if(quit) break;
			usleep(1000);
		}
		int x = t.x;
		int y = t.y;
		while(t.touched) usleep(1000);
		if(x < 160){
			if(60 < y && y < 100){	//TOOLS 선택됨
				backlight_off();
				while(1){
					gclear(0, 0, 0);
					gtext_size(3);
					gtext(0, 50, "Tools");
					gtext_size(2);
					gtext(0, 100, "CLOCK");
					gtext(0, 140, "NO");
					gtext(0, 180, "NO");
					gupdate();
					bool quit = false;
					while(!t.touched){
						if(b.btnBack == true){
							while(b.btnBack == true) usleep(1000);;
							quit = true;
							break;
						}
						usleep(1000);
					}
					if(quit)
						break;
					int x = t.x;
					int y = t.y;
					while(t.touched) usleep(1000);
					if(x < 160){
						if(60 < y && y < 100){	//CLOCK 선택됨
							backlight_off();
							while(b.btnBack == false){	//뒤로가기 버튼이 눌리면 종료
								gclear(128, 128, 128);
								gtext_size(3);
								gtext(0, 50, "Clock");
								gtext_size(2);
								gtext(0, 100, getTime());
								gtext_size(1);
								gtext(0, 	125, getDate());
								gupdate();
								usleep(1000);
							}
							while(b.btnBack == true) usleep(1000);
							backlight_off();
							continue;
						}else if(100 < y && y < 140){	//NO 선택됨
						}else if(140 < y && y < 180){	//NO 선택됨
						}
					}
				}
				backlight_off();
				continue;
			}else if(100 < y && y < 140){	//PLAY 선택됨

			}else if(140 < y && y < 180){	//SYSTEM 선택됨

			}
		}
		//while(t.touched) usleep(1000);
	};
}

void printHelp(){
	cout << "Help for starting PiBox:" << endl;
	cout << "Usage: pb [-chv]" << endl;
	cout << "c: Run touch calibration. You need to run this at first time." << endl;
	cout << "h: Print this help." << endl;
	cout << "v: Print PiBox's version" << endl;
	cout << "Warning: You cannot use h with v" << endl;
	exit(0);
}

void printVer(){
	cout << "PiBox version: " << ver << endl;
	cout << "Build: IS" << endl;
	exit(0);
}

string getTime(){
	tm *now;
	time_t current_time;
	time(&current_time);
	now = localtime(&current_time);
	char temp[100];
	strftime(temp, 1000, "%H:%M", now);
	string s(temp);
	return s;
}

string getDate(){
	tm *now;
	time_t current_time;
	time(&current_time);
	now = localtime(&current_time);
	char temp[100];
	strftime(temp, 1000, "%Y/%m/%d(%a)", now);
	string s(temp);
	return s;
}
