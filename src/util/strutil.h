#ifndef STRUTILS
#define STRUTILS
#include <string>

using std::string;

class StringSpilter{
	public:
		StringSpilter(string s);
		void split(string token);
		string get(int index);
	private:
		string s;
		string *strResult;
};
#endif
