
// CCarSurface.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCCarSurfaceApp: 
// �йش����ʵ�֣������ CCarSurface.cpp
//

class CCCarSurfaceApp : public CWinApp
{
public:
	CCCarSurfaceApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCCarSurfaceApp theApp;