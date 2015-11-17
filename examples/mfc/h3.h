// h3.h : main header file for the H3 application
//

#if !defined(AFX_H3_H__24E584F9_0D87_11D3_90C4_00A0C996066F__INCLUDED_)
#define AFX_H3_H__24E584F9_0D87_11D3_90C4_00A0C996066F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CH3App:
// See h3.cpp for the implementation of this class
//

class CH3App : public CWinApp
{
public:
	CH3App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CH3App)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CH3App) 
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnViewSphere();
	afx_msg void OnViewToggleboundingsphere();
	afx_msg void OnNavigateLocate();
	afx_msg void OnSearch();
	afx_msg void OnWindowNew();
	afx_msg void OnViewMorelabels();
	afx_msg void OnViewFewerlabels();
	afx_msg void OnViewHeatmaptoggle();
	//bool searchClick;
	afx_msg void OnViewNode();
	afx_msg void OnNavigateCroptosubtree();
	afx_msg void OnViewViewindrawgram();
	afx_msg void OnViewThickenline();
	afx_msg void OnNavigateRecolor();
	afx_msg void OnBnClickedOk();
	afx_msg void OnExporttopdfExportwithdefaultsettings();
	afx_msg void OnMenu32790();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnNavigateReverttoloadedfile();
	afx_msg void OnFileResetgraph();
	afx_msg void OnWebGooglesearchcurrentnode();
	afx_msg void OnWebSetcustomsearchparameters();
	afx_msg void OnWebCustomsearchcurrentnode();
	afx_msg void OnMenu32798();
	afx_msg void OnWebTaedsearchcurrentnode32800();
	afx_msg void OnBnClickedButton1();
	afx_msg void On2dviewerToggleinteractive();
//	afx_msg void OnFileSaveas();
	afx_msg void OnFileSave32806();
	afx_msg void On2dviewerChoosestaticoptions();
	afx_msg void OnNavigateCenter();
	afx_msg void OnNavigateTreatnumeric();
	afx_msg void OnViewDensegraphtoggleandredraw();
	afx_msg void OnBnClickedButton5();
	//afx_msg void OnBnClickedTagleft();
	//afx_msg void OnBnClickedTagRight();
	//afx_msg void OnBnClickedShowPath();
	afx_msg void OnNavigateTagleft();
	afx_msg void OnNavigateTagright();
	afx_msg void OnNavigateShowpath();
	afx_msg void OnNavigateHidepath();
	afx_msg void OnViewGhostboxes();
	afx_msg void OnViewNoghostboxes();
	afx_msg void OnViewCyclelabelelements();
	afx_msg void OnViewToggleelements();
	afx_msg void OnNcbiSearchCurrentNode();
	afx_msg void OnShapesDensechordateview();
	afx_msg void OnLabelsTogglelandmarknode();
	afx_msg void OnLabelsClear();
	afx_msg void OnWebNcbigilookup();
	afx_msg void OnLabelsLandmarkhighdnds();
	afx_msg void OnLabelsLandmarkcurrentpath();
	afx_msg void OnColoringTogglelinedraw();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_H3_H__24E584F9_0D87_11D3_90C4_00A0C996066F__INCLUDED_)
