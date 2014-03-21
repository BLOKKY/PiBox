#include "strutil.h"
#include <iostream>

using std::string;
using std::cout;
using std::endl;

//문자열을 쪼개는 클래스

//생성자
//s: 쪼갤 문자열
StringSpilter::StringSpilter(string s){
	this->s = s;
}

//쪼개는 함수
//token: 구분자
void StringSpilter::split(string token){
	int cutAt;	//자를 위치
	int index = 0;	//인덱스
	strResult = new string[100];
	while((cutAt = s.find_first_of(token)) != s.npos){	//문자열의 끝이 아닐동안 찾음
		cout << "TO FIND: " << s << endl;
		if(cutAt > 0){	//문자열을 찾았다면
			cout << "FOUND: " << s.substr(0, cutAt) << endl;
			strResult[index] = s.substr(0, cutAt);	//잘라서 배열에 추가
			index++;
		}
		s = s.substr(cutAt+1);	//다음부터는 현재 찾은 문자열은 빼고 찾음
		cout << "NEXT: " << s << endl;
	}
	//문자열이 남았다면 나머지를 배열에 추가
	if(s.length() > 0) strResult[index] = s.substr(0, cutAt);
}

string StringSpilter::get(int index){
	return strResult[index];
}
