
// BMA_Stabilization.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CBMA_StabilizationApp:
// �� Ŭ������ ������ ���ؼ��� BMA_Stabilization.cpp�� �����Ͻʽÿ�.
//

class CBMA_StabilizationApp : public CWinApp
{
public:
	CBMA_StabilizationApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CBMA_StabilizationApp theApp;