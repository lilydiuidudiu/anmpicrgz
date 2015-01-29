#include "stdafx.h"
#include "InitialFile.h"
using namespace std;

WCHAR* InitialFile::CharToWChar(const char* myChar)
{
 int nLen = MultiByteToWideChar(CP_ACP,0,myChar,-1,NULL,0);
 WCHAR *myWChar = new WCHAR[nLen];
 MultiByteToWideChar(CP_ACP,0,myChar,-1,myWChar,nLen);
 myWChar[nLen-1] = '\0';
 return myWChar;
}

char* InitialFile::WCharToChar(WCHAR* myWChar)
{
 int nLen = WideCharToMultiByte(CP_ACP,0,myWChar,-1,NULL,0,NULL,NULL);
 char* myChar = new char[nLen];
 WideCharToMultiByte(CP_ACP,0,myWChar,-1,myChar,nLen,NULL,NULL);
 myChar[nLen-1]='\0';
 return myChar;
}

int InitialFile::searchFileInDirectroy( const string& dir, vector<string>& outList )
{
		int Acount;
        WIN32_FIND_DATA findData;
        HANDLE hHandle;
        string filePathName;
        string fullPathName;
        filePathName = dir;
        filePathName += "\\*.JPG";
		Acount=0;
        hHandle = FindFirstFile( CharToWChar(filePathName.c_str()), &findData );
        if( INVALID_HANDLE_VALUE == hHandle )
        {
                //cout<<"Error"<<endl;
                return -1;
        }

        do
        {
                if( strcmp(".", WCharToChar(findData.cFileName)) == 0 || strcmp("..", WCharToChar(findData.cFileName)) == 0 )
                {
                        continue;
                }

                fullPathName = dir;
                fullPathName += "\\";
                fullPathName += WCharToChar(findData.cFileName);

                if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                        searchFileInDirectroy( fullPathName, outList );
                }
                else
                {
                        outList.push_back(fullPathName);
						Acount++;
                }

        }
		while( FindNextFile( hHandle, &findData ) );
        FindClose( hHandle );
	return Acount;
}



int InitialFile::searchFolderInDirectroy(const string& drt,vector<string>& outList)
{
	 char szFind[MAX_PATH];
	 //char szFile[MAX_PATH];
	 int Acount=0;

	 WIN32_FIND_DATA FindFileData;

	 const char* lpPath=drt.c_str();
	 strcpy_s(szFind,lpPath);
	 strcat_s(szFind,"\\*.*");
	 //strcat(szFind,secName);//¹ýÂËµÄÃû×Ö

	 HANDLE hFind=::FindFirstFile( CharToWChar(szFind),&FindFileData);

	 if(INVALID_HANDLE_VALUE == hFind)    return -1;
	 while(TRUE)
	 {
		  if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		  {
			   if(FindFileData.cFileName[0]!='.')
			   {
				 //cout << FindFileData.cFileName<<endl;
				 outList.push_back(WCharToChar(FindFileData.cFileName));
				 Acount++;
			   }
		  }
		  if(!FindNextFile(hFind,&FindFileData))    break;
	 }

	 FindClose(hFind);
	 return Acount;
}
vector<string> InitialFile::ThousandOnly(vector<string>* allList,int* num)
{
	vector<string>::iterator copy;
	vector<string>::iterator end;
	end=(*allList).end();
	vector<string> pathlist;
	int count=0;
	if (*num>110)
	{
		for(copy=(*allList).begin();count<10;++count)
		{
			pathlist.push_back(*copy);
			copy=(*allList).erase(copy);
		}
		*num=*num-100;
	}
	else
	{
		for(copy=(*allList).begin();copy!=end;++copy)
		{
			pathlist.push_back(*copy);
		}
		(*allList).swap(vector<string>());
		*num=0;
	}
	return pathlist;
}