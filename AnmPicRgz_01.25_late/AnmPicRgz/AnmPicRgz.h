
// AnmPicRgz.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAnmPicRgzApp:
// �йش����ʵ�֣������ AnmPicRgz.cpp
//

class CAnmPicRgzApp : public CWinApp
{
public:
	CAnmPicRgzApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAnmPicRgzApp theApp;