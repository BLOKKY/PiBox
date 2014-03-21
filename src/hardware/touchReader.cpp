#include "touch.h"
#include <linux/input.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

using std::cout;
using std::endl;

TouchReader::TouchReader(){
	fd = open("/dev/input/event0", O_RDONLY);
	x = 0;
	y = 0;
	readX = false;
	readY = false;
}

TouchReader::~TouchReader(){
	close(fd);
}

int TouchReader::run(){
	fd_set fdmgr;
	while(true){
		FD_ZERO(&fdmgr);
		FD_SET(fd, &fdmgr);
		timeval timeout;
		timeout.tv_sec=0;
		timeout.tv_usec=100000;
		int v = select(fd + 1, &fdmgr, NULL, NULL, &timeout);
		switch(v){
			case -1:{	//오류 발생
				return 0;	//터치 입력 종료
			}case 0:{	//읽을 것 없음
				touched = false;	//터치되지 않음
				break;
			}default:{	//읽을 것 있음
				input_event e;
				read(fd, &e, sizeof(e));
				if(e.type == 3){	//타입이 3이고
					if(e.code == 0 && !readX){	//기존에 X를 읽지 않았고 X 값이 날아왔다면
						rawX = e.value;		//값 설정
						readX = true;	//X 읽음
					}else if(e.code == 1 && !readY) { //기존에 Y를 읽지 않았고 Y 값이 날아왔다면
						rawY = e.value;		//값 설정
						readY = true;	//Y 읽음
					}
				}
				if(readX&&readY){	//X와 Y를 모두 읽었다면
					x = A * rawX + B * rawY + C;
					y = D * rawX + E * rawY + F;
					readX = false;
					readY = false;
					touched = true;	//터치됨
				}
			}
		}
	}
}

//보정값 설정
void TouchReader::setCalibration(float A, float B, float C, float D, float E, float F){
	this->A = A;
	this->B = B;
	this->C = C;
	this->D = D;
	this->E = E;
	this->F = F;
}
