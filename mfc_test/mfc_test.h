
// mfc_test.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cmfc_testApp:
// See mfc_test.cpp for the implementation of this class
//

class Cmfc_testApp : public CWinApp
{
public:
	Cmfc_testApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cmfc_testApp theApp;