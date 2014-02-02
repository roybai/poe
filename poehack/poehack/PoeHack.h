// PoeHack.h : main header file for the PoeHack DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPoeHackApp
// See PoeHack.cpp for the implementation of this class
//

class CPoeHackApp : public CWinApp
{
public:
	CPoeHackApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
