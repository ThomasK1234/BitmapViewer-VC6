// BitmapViewer.h : main header file for the BITMAPVIEWER application
//

#if !defined(AFX_BITMAPVIEWER_H__12345678_1234_1234_1234_123456789012__INCLUDED_)
#define AFX_BITMAPVIEWER_H__12345678_1234_1234_1234_123456789012__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerApp:
// See BitmapViewer.cpp for the implementation of this class
//

class CBitmapViewerApp : public CWinApp
{
public:
	CBitmapViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBitmapViewerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in THESE BLOCKS of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPVIEWER_H__12345678_1234_1234_1234_123456789012__INCLUDED_)
