
#include"stdafx.h"

#include"IniFile.h"
#include <iostream>
using namespace std;
int __cdecl _tmain1(int argc, _TCHAR* argv[])
{
	std::string retVal, defaultVal= "fku";
	bool b = CIniFile::GetKey("config.ini","system","plugins", retVal,defaultVal);

	cout << b << endl;

	cout << retVal << endl;

	cout << defaultVal << endl;
	return 1;

}
