#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
using namespace std;

#define PicNumInGroup 1000

class InitialFile
{
public:
	WCHAR* CharToWChar(const char* myChar);
	char* WCharToChar(WCHAR* myWChar);
	int searchFileInDirectroy( const string& dir, vector<string>& outList );
	int searchFolderInDirectroy(const string& drt,vector<string>& outList);
	vector<string> ThousandOnly(vector<string>* allList,int* num);
};