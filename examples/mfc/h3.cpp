// h3.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "h3.h"

#include "MainFrm.h"
#include "h3Doc.h"
#include "H3View.h"
#include <iostream>
//#include <string>
#include "main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CH3App

BEGIN_MESSAGE_MAP(CH3App, CWinApp)
	//{{AFX_MSG_MAP(CH3App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVEAS, CWinApp::OnFileOpen)//DoPromptFileName(CString &fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate *pTemplate));
	//ON_COMMAND(ID_FILE_SAVEAS, CWinApp::DoPromptFileName(CString &fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate *pTemplate));
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_VIEW_SPHERE, &CH3App::OnViewSphere)
	ON_COMMAND(ID_VIEW_TOGGLEBOUNDINGSPHERE, &CH3App::OnViewToggleboundingsphere)
	//ON_COMMAND(ID_NAVIGATE_LOCATE, &CH3App::OnNavigateLocate) 
	//ON_COMMAND(IDD_SEARCH, &CH3App::OnSearch)
	ON_COMMAND(IDD_SEARCH, OnSearch)
	ON_COMMAND(ID_WINDOW_NEW, &CH3App::OnWindowNew)
	ON_COMMAND(ID_VIEW_MORELABELS, &CH3App::OnViewMorelabels)
	ON_COMMAND(ID_VIEW_FEWERLABELS, &CH3App::OnViewFewerlabels)
	ON_COMMAND(ID_VIEW_HEATMAPTOGGLE, &CH3App::OnViewHeatmaptoggle)
//	ON_BN_CLICKED(IDC_BUTTON1, &CH3App::OnBnClickedButton1)
ON_COMMAND(ID_VIEW_NODE, &CH3App::OnViewNode)
ON_COMMAND(ID_NAVIGATE_CROPTOSUBTREE, &CH3App::OnNavigateCroptosubtree)
ON_COMMAND(ID_VIEW_VIEWINDRAWGRAM, &CH3App::OnViewViewindrawgram)
ON_COMMAND(ID_VIEW_THICKENLINE, &CH3App::OnViewThickenline)
ON_COMMAND(ID_NAVIGATE_RECOLOR, &CH3App::OnNavigateRecolor)

ON_COMMAND(ID_EXPORTTOPDF_EXPORTWITHDEFAULTSETTINGS, &CH3App::OnExporttopdfExportwithdefaultsettings)
ON_COMMAND(ID_Menu32790, &CH3App::OnMenu32790)
//ON_BN_CLICKED(IDC_BUTTON4, &CH3App::OnBnClickedButton4)
ON_COMMAND(ID_NAVIGATE_REVERTTOLOADEDFILE, &CH3App::OnNavigateReverttoloadedfile)
ON_COMMAND(ID_FILE_RESETGRAPH, &CH3App::OnFileResetgraph)
ON_COMMAND(ID_WEB_GOOGLESEARCHCURRENTNODE, &CH3App::OnWebGooglesearchcurrentnode)
ON_COMMAND(ID_WEB_SETCUSTOMSEARCHPARAMETERS, &CH3App::OnWebSetcustomsearchparameters)
ON_COMMAND(ID_WEB_CUSTOMSEARCHCURRENTNODE, &CH3App::OnWebCustomsearchcurrentnode)
ON_COMMAND(ID_Menu32798, &CH3App::OnMenu32798)
ON_COMMAND(ID_WEB_TAEDSEARCHCURRENTNODE32800, &CH3App::OnWebTaedsearchcurrentnode32800)
//ON_BN_CLICKED(IDC_BUTTON1, &CH3App::OnBnClickedButton1)
ON_COMMAND(ID_2DVIEWER_TOGGLEINTERACTIVE, &CH3App::On2dviewerToggleinteractive)
//ON_COMMAND(ID_FILE_SAVEAS, &CH3App::OnFileSaveas)
ON_COMMAND(ID_FILE_SAVE32806, &CH3App::OnFileSave32806)
ON_COMMAND(ID_2DVIEWER_CHOOSESTATICOPTIONS, &CH3App::On2dviewerChoosestaticoptions)
ON_COMMAND(ID_NAVIGATE_CENTER, &CH3App::OnNavigateCenter)
ON_COMMAND(ID_NAVIGATE_TREATNUMERIC, &CH3App::OnNavigateTreatnumeric)
ON_COMMAND(ID_VIEW_DENSEGRAPHTOGGLEANDREDRAW, &CH3App::OnViewDensegraphtoggleandredraw)
ON_BN_CLICKED(IDC_BUTTON5, &CH3App::OnBnClickedButton5)
//ON_BN_CLICKED(IDC_TAGLEFT, &CAboutDlg2::OnBnClickedTagleft)
//ON_BN_CLICKED(IDC_TAG_RIGHT, &CAboutDlg2::OnBnClickedTagRight)
//ON_BN_CLICKED(IDC_SHOW_PATH, &CH3App::OnBnClickedShowPath)
//ON_BN_CLICKED(IDC_SHOW_PATH, &CAboutDlg2::OnBnClickedShowPath)
ON_COMMAND(ID_NAVIGATE_TAGLEFT, &CH3App::OnNavigateTagleft)
ON_COMMAND(ID_NAVIGATE_TAGRIGHT, &CH3App::OnNavigateTagright)
ON_COMMAND(ID_NAVIGATE_SHOWPATH, &CH3App::OnNavigateShowpath)
ON_COMMAND(ID_NAVIGATE_HIDEPATH, &CH3App::OnNavigateHidepath)
ON_COMMAND(ID_VIEW_GHOSTBOXES, &CH3App::OnViewGhostboxes)
ON_COMMAND(ID_VIEW_NOGHOSTBOXES, &CH3App::OnViewNoghostboxes)
ON_COMMAND(ID_VIEW_CYCLELABELELEMENTS, &CH3App::OnViewCyclelabelelements)
ON_COMMAND(ID_VIEW_TOGGLEELEMENTS, &CH3App::OnViewToggleelements)
ON_COMMAND(ID_NCBI_SEARCH_CURRENT_NODE, &CH3App::OnNcbiSearchCurrentNode)
ON_COMMAND(ID_SHAPES_DENSECHORDATEVIEW, &CH3App::OnShapesDensechordateview)
ON_COMMAND(ID_LABELS_TOGGLELANDMARKNODE, &CH3App::OnLabelsTogglelandmarknode)
ON_COMMAND(ID_LABELS_CLEAR, &CH3App::OnLabelsClear)
ON_COMMAND(ID_WEB_NCBIGILOOKUP, &CH3App::OnWebNcbigilookup)
ON_COMMAND(ID_LABELS_LANDMARKHIGHDNDS, &CH3App::OnLabelsLandmarkhighdnds)
ON_COMMAND(ID_LABELS_LANDMARKCURRENTPATH, &CH3App::OnLabelsLandmarkcurrentpath)
ON_COMMAND(ID_COLORING_TOGGLELINEDRAW, &CH3App::OnColoringTogglelinedraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CH3App construction

CH3App::CH3App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CH3App object

CH3App theApp;

/////////////////////////////////////////////////////////////////////////////
// CH3App initialization

BOOL CH3App::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CH3Doc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CH3View));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow(); 
	

	return TRUE;
}

class CSetStatusLoadFile : public CDialog
{
public:
	CSetStatusLoadFile();
	enum {DRAWGRAM=IDD_STATUS_LOADFILE};
protected:
	virtual void DoDataExchange(CDataExchange * pDX);
	DECLARE_MESSAGE_MAP()
public:
//	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButton1();
};
CSetStatusLoadFile::CSetStatusLoadFile() : CDialog(IDD_STATUS_LOADFILE)
{
	int x=0;
	x=x+1;
//code goes here
}
void CSetStatusLoadFile::DoDataExchange(CDataExchange * pDX){
}
void CSetStatusLoadFile::OnBnClickedButton1(){
}
BEGIN_MESSAGE_MAP(CSetStatusLoadFile,CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CSetStatusLoadFile::OnBnClickedButton1)
END_MESSAGE_MAP()


class CSetDrawgram : public CDialog
{
public:
	CSetDrawgram();
	enum {DRAWGRAM=IDD_DRAWGRAM};
protected:
	virtual void DoDataExchange(CDataExchange * pDX);
	DECLARE_MESSAGE_MAP()
public:
//	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButton1();
};
CSetDrawgram::CSetDrawgram() : CDialog(IDD_DRAWGRAM)
{
//code goes here
}



class CSetWebDlg : public CDialog
{
public:
	CSetWebDlg();
//	OnWebSetcustomsearchparameters();
	enum {WEB=IDD_CUSTOM_WEB};
	afx_msg void OnBnClickedButton1();
protected:
	CString	Prefix;
	CString	Suffix;
	CWnd Web_T1;
	CWnd Web_T2;
	CString webPrefix;
    CString webPostfix;
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual void getDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual void putDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
};//CSetWebDlg
BEGIN_MESSAGE_MAP(CSetWebDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
	//ON_COMMAND(ID_NAVIGATE_LOCATE, &CAboutDlg::OnNavigateLocate)
	//ON_BN_CLICKED(IDOK, &CSetWebDlg::OnOK)
	
	ON_BN_CLICKED(IDOK, &CSetWebDlg::OnBnClickedOk)
	
	ON_BN_CLICKED(IDC_BUTTON1, &CSetWebDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON14, &CSetWebDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON2, &CSetWebDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSetWebDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CSetWebDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CSetWebDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CSetWebDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CSetWebDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CSetWebDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CSetWebDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CSetWebDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CSetWebDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CSetWebDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CSetWebDlg::OnBnClickedButton13)
END_MESSAGE_MAP()

CSetWebDlg::CSetWebDlg() : CDialog(IDD_CUSTOM_WEB)
{
//code goes here
}


class CPdfDlg : public CDialog
{
public:
	CPdfDlg();
	enum {PDF=IDD_PDF1};
protected:
	CString	StatusBox;
	CString	savePdfName;
	int Pdf_X;
	int Pdf_Y;
	int Pdf_S;
	CWnd Pdf_T1;
	CWnd Pdf_T2;
	CWnd Pdf_T3;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual void getDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual void putDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton5();
	
};
BEGIN_MESSAGE_MAP(CPdfDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
	//ON_COMMAND(ID_NAVIGATE_LOCATE, &CAboutDlg::OnNavigateLocate)
	ON_BN_CLICKED(IDC_BUTTON5, &CPdfDlg::OnBnClickedButton5)
	
END_MESSAGE_MAP()
CPdfDlg::CPdfDlg() : CDialog(IDD_SEARCH)
{
Pdf_X=1000;Pdf_Y=1000;Pdf_S=0;savePdfName="TreeThrasher.pdf";
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	//enum { IDD = IDD_ABOUTBOX, ID2 = IDD_SEARCH };
	enum { IDD = IDD_ABOUTBOX,ID2 = IDD_PDF1 };
	CString	m_Copyright;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//public:
	//afx_msg void OnNavigateLocate();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_Copyright = _T("\
   \nCopyright 2011, University of Wyoming. -- ALL RIGHTS RESERVED \
   \nTreeThrasher by Stormy Knight is licensed under the following licences\
   \n\tSilicon Graphics (below)\
   \n\tGL2PS            (below)\n\
   \n\tNote: Requirements for both licences must be met.\
   \n\tNote: contact stormyk@gmail.com for technical questions about TreeThrasher\n\
   \tAll Source files are licensed under the Silicon Graphics license\n\tunless specified otherwise.\
   \n\n\
Copyright 1998-1999, Silicon Graphics, Inc. -- ALL RIGHTS RESERVED \
\
Permission is granted to copy, modify, use and distribute this software and \
accompanying documentation free of charge provided (i) you include the \
entirety of this reservation of rights notice in all such copies, (ii) you \
comply with any additional or different obligations and/or use restrictions \
specified by any third party owner or supplier of the software and \
accompanying documentation in other notices that may be included with the \
software, (iii) you do not charge any fee for the use or redistribution of \
the software or accompanying documentation, or modified versions \
thereof.  \
 \
Contact sitemgr@sgi.com for information on licensing this software for \
commercial use. Contact munzner@cs.stanford.edu for technical \
questions. \n\n\n\
\t\t\t     GL2PS LICENSE\n\
\t\t\t Version 2, November 2003\n\
\t\t Copyright (C) 2003, Christophe Geuzaine\n\n\
Permission to use, copy, and distribute this software and its\
documentation for any purpose with or without fee is hereby granted,\
provided that the copyright notice appear in all copies and that both\
that copyright notice and this permission notice appear in supporting\
documentation.\n\
Permission to modify and distribute modified versions of this software\
is granted, provided that:\n\
1) the modifications are licensed under the same terms as this \
software;\n\
2) you make available the source code of any modifications that you \
distribute, either on the same media as you distribute any executable \
or other form of this software, or via a mechanism generally accepted \
in the software development community for the electronic transfer of \
data.\n\
This software is provided \"as is\" without express or implied warranty.\
");
	//}}AFX_DATA_INIT
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_COPYRIGHT, m_Copyright);
	//}}AFX_DATA_MAP
}








BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
	//ON_COMMAND(ID_NAVIGATE_LOCATE, &CAboutDlg::OnNavigateLocate)
END_MESSAGE_MAP()

// App command to run the dialog
void CH3App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class CAboutDlg2 : public CDialog
{
public:
	CAboutDlg2();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	//enum { IDD = IDD_ABOUTBOX, ID2 = IDD_SEARCH };
	enum { IDD = IDD_ABOUTBOX,ID2 = IDD_SEARCH };
	CString	m_Copyright;
	CString	locateName;
	CEdit  edLocName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	bool searchClick;
	DECLARE_MESSAGE_MAP()
//public:
	//afx_msg void OnNavigateLocate();
public:
	afx_msg void OnBnClickedButton1();
	
	//afx_msg void OnExporttopdfExportwithcustomsettings();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnBnClickedTagleft();
//	afx_msg void OnBnClickedTagRight();
	afx_msg void OnBnClickedTagleft();
	afx_msg void OnBnClickedTagRight();
	afx_msg void OnBnClickedShowPath();
};

CAboutDlg2::CAboutDlg2() : CDialog(CAboutDlg::ID2)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_Copyright = _T("\
TreeThrasher by Stormy Knight\n\
Copyright 2011, University of Wyoming.\nALL RIGHTS RESERVED \
");
	//}}AFX_DATA_INIT
	//edLocName.Create(WS_CHILD | WS_VISIBLE| WS_BORDER, CRect(CPoint (5,5), CSize(60,25) ),this,IDC_EDIT1);
}


void CAboutDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_COPYRIGHT, m_Copyright);
	locateName="Enter Node Name Here";
    //UpdateData(false);
	DDX_Text(pDX, IDC_EDIT1, locateName);
	//UpdateData(true);
	//locateName="NODE_33";
	//}}AFX_DATA_MAP
}

void CPdfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
//	DDX_Text(pDX, IDC_COPYRIGHT, m_Copyright);
	//locateName="BONK";
    //UpdateData(false);
	//DDX_Text(pDX, IDC_EDIT1, locateName);
	//UpdateData();
	//}}AFX_DATA_MAP
	//savePdfName=("TYPE IN FILENAME HERE");
	//Pdf_X=0;Pdf_Y=0;
	
	DDX_Text(pDX, IDC_EDIT1, savePdfName);
      DDX_Text(pDX, IDC_EDIT3, Pdf_X);
	DDX_Text(pDX, IDC_EDIT4, Pdf_Y);
	DDX_Text(pDX, IDC_EDIT5, Pdf_S);
	DDV_MinMaxInt(pDX,Pdf_X,500,5000);
	DDV_MinMaxInt(pDX,Pdf_Y,500,5000);
	DDV_MinMaxInt(pDX,Pdf_S,0,120);
	//DDX_Control(pDX,IDC_RADIO1,Pdf_T1);
	

	//DDX_Text(pDX, IDC_STATIC11, CString("Status-> Writing"));	
	std::string sbTemp;
	char sbTemp_cstr[512];
	sprintf(sbTemp_cstr,"Awaiting Input");//Status->writing %s\n Resolution %d X %d",savePdfName,Pdf_X,Pdf_Y);
	if ((Pdf_X>499)&&(Pdf_Y>499)&&(Pdf_X<5001)&&(Pdf_Y<50001)) {
		sprintf(sbTemp_cstr,"Status->writing %s\n Resolution %d X %d \n Static Label Size \n Render Seconds: %d",savePdfName,Pdf_X,Pdf_Y,Pdf_S);
	}

	sbTemp.assign(sbTemp_cstr);

	
	StatusBox=CString(sbTemp_cstr);
	DDX_Text(pDX, IDC_STATIC11, StatusBox);	

	char buffer[512];
	sprintf(buffer,"%s",savePdfName.GetString());
	CDialog::DoDataExchange(pDX);
	std::string tempFileName;
	tempFileName.assign(buffer);
	if ((tempFileName.size()<1)&& true) {tempFileName.assign("TreeThrasher.pdf");}
	pdfPlot(Pdf_X,Pdf_Y,tempFileName,1);
	
	
}




void CSetWebDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	

	DDX_Text(pDX, IDC_EDIT1, webPrefix);
    DDX_Text(pDX, IDC_EDIT2, webPostfix);
	std::string sbTemp;
	char sbTemp_cstr[512];
	//sprintf(sbTemp_cstr,"Awaiting input");//Status->writing %s\n Resolution %d X %d",savePdfName,Pdf_X,Pdf_Y);
	
	sbTemp.assign(sbTemp_cstr);
	//StatusBox=CString(sbTemp_cstr);
	//DDX_Text(pDX, IDC_STATIC11, StatusBox);	
	
	
}
void CSetDrawgram::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	

//	DDX_Text(pDX, IDC_EDIT1, webPrefix);
//    DDX_Text(pDX, IDC_EDIT2, webPostfix);
//	std::string sbTemp;
//	char sbTemp_cstr[512];
	//sprintf(sbTemp_cstr,"Awaiting input");//Status->writing %s\n Resolution %d X %d",savePdfName,Pdf_X,Pdf_Y);
	
//	sbTemp.assign(sbTemp_cstr);
	//StatusBox=CString(sbTemp_cstr);
	//DDX_Text(pDX, IDC_STATIC11, StatusBox);	
	
	
}










BEGIN_MESSAGE_MAP(CAboutDlg2, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
	//ON_COMMAND(ID_NAVIGATE_LOCATE, &CAboutDlg::OnNavigateLocate)
	ON_BN_CLICKED(IDC_BUTTON1, &CAboutDlg2::OnBnClickedButton1)


	ON_BN_CLICKED(IDC_BUTTON2, &CAboutDlg2::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CAboutDlg2::OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, &CAboutDlg2::OnBnClickedOk)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CAboutDlg2::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_TAGLEFT, &CAboutDlg2::OnBnClickedTagleft)
	ON_BN_CLICKED(IDC_TAG_RIGHT, &CAboutDlg2::OnBnClickedTagRight)
	ON_BN_CLICKED(IDC_SHOW_PATH, &CAboutDlg2::OnBnClickedShowPath)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CH3App message handlers


BOOL CH3App::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CWinApp::OnIdle(lCount);
	return onidle();
}


void CH3App::OnViewSphere()
{
	// TODO: Add your command handler code here
	keyboard('K');
}

void CH3App::OnViewToggleboundingsphere()
{
	keyboard('s');
	// TODO: Add your command handler code here
}

//void CH3App::OnNavigateLocate()
//{
	//Display help box sham code
//	CAboutDlg aboutDlg;
//	aboutDlg.DoModal();
	// TODO: Add your command handler code here
//}


void CH3App::OnSearch()
{
	CAboutDlg2 searchDlg;
	//archDlg.
	
	searchDlg.DoModal();
	
	//keyboard('W');
	
	
	//std::cout << junk101<<std::endl;
	
	//std::cout <<"SEARCH"<<std::endl;
	// TODO: Add your command handler code here
}

void CH3App::OnMenu32790()  //PDF CUSTOM
{
	CPdfDlg searchDlg;	
	searchDlg.DoModal();
	
	// TODO: Add your command handler code here
}

void CH3App::OnWindowNew()
{
	// TODO: Add your command handler code here
	keyboard('s');
}

void CH3App::OnViewMorelabels()
{
	// TODO: Add your command handler code here
	keyboard('b');
}

void CH3App::OnViewFewerlabels()
{
	// TODO: Add your command handler code here
	keyboard ('B');
}

void CH3App::OnViewHeatmaptoggle()
{
	// TODO: Add your command handler code here
	keyboard ('T');
}


void CAboutDlg2::OnBnClickedButton1()// searchBox
{
	
	CDataExchange* pDX=new CDataExchange(this,true);
	UpdateData(true);
	CAboutDlg2::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, locateName);
	char buffer[512]="BLANK_PLACEHOLDER";
	sprintf(buffer,"#%s",locateName.GetString());
	//printf("DEBUG: LOCATOR BUFFER ->%s\n",buffer);

	//gNode(buffer);
	gNextNode(buffer);// TEST SJK2011
	// TODO: Add your control notification handler code here
}

void CH3App::OnViewNode()
{
	// TODO: Add your command handler code here
	keyboard('H');
}

void CH3App::OnNavigateCroptosubtree()
{
	// TODO: Add your command handler code here
	keyboard ('g');
}

void CH3App::OnViewViewindrawgram()
{
	// TODO: Add your command handler code here
	keyboard ('O');
}

void CH3App::OnViewThickenline()
{
	// TODO: Add your command handler code here
	keyboard ('E');
}

void CH3App::OnNavigateRecolor()
{
	// TODO: Add your command handler code here
	keyboard ('R');
}


void CH3App::OnExporttopdfExportwithdefaultsettings()
{
	keyboard ('P');
	// TODO: Add your command handler code here
}



void CH3App::OnBnClickedButton4()
{
	keyboard ('P');
	// TODO: Add your control notification handler code here
}

void CPdfDlg::OnBnClickedButton5()
{
	CDataExchange* pDX=new CDataExchange(this,true);
//	CDataExchange* pDX2=NULL;
	//CDialog::DoDataExchange(pDX);
	//UpdateData(true);
	//CPdfDlg::DoDataExchange(pDX);
	
	CPdfDlg::DoDataExchange(pDX);
	UpdateData(true);
	UpdateData(false);

	//locateName.GetData();
	//char buffer[512]="NODE_13";
	//sprintf(buffer,"%s",locateName.GetString());
	//edLocName.GetWindowTextA(buffer,512);
	//printf("DEBUG: LOCATOR BUFFER ->%s\n",buffer);
	//gNode(buffer);
	// TODO: Add your control notification handler code here
}

void CH3App::OnNavigateReverttoloadedfile()
{
	keyboard('y');
	// TODO: Add your command handler code here
}

void CH3App::OnFileResetgraph()
{
	keyboard('y');
	// TODO: Add your command handler code here
}


void CAboutDlg2::OnBnClickedButton2()
{
		
	CDataExchange* pDX=new CDataExchange(this,true);
	UpdateData(true);	
	CAboutDlg2::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, locateName);
	char buffer[512]="BLANK_PLACEHOLDER";
	sprintf(buffer,"%s",locateName.GetString());
	
	//printf("DEBUG: LOCATOR BUFFER ->%s\n",buffer);

	gNode(buffer); // Goto Exact Match Only
	
	// TODO: Add your control notification handler code here
}

void CAboutDlg2::OnBnClickedButton3()
{
	CDataExchange* pDX=new CDataExchange(this,true);
	UpdateData(true);	
	CAboutDlg2::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, locateName);
	char buffer[512]="BLANK_PLACEHOLDER";
	sprintf(buffer,"%s",locateName.GetString());
	
	//printf("DEBUG: LOCATOR BUFFER ->%s\n",buffer);

	gNextNode(buffer); // Goto Exact Match Only

	// TODO: Add your control notification handler code here
}

void CAboutDlg2::OnBnClickedOk()
{
    OnBnClickedButton1();
	// TODO: Add your control notification handler code here
	OnOK();
}

void CH3App::OnWebGooglesearchcurrentnode()
{
	keyboard('W');
	// TODO: Add your command handler code here
}

void CH3App::OnWebSetcustomsearchparameters()
{
	CSetWebDlg CWebDlg;
    CWebDlg.DoModal();
//	CDataExchange* setWEBDX=new CDataExchange(this,true);
//	UpdateData(true);	
//	CWebDlg::DoDataExchange(setWebDX);
	//CWebDlg.DoModal();
	// TODO: Add your command handler code here
}

void CH3App::OnWebCustomsearchcurrentnode()
{
	keyboard ('#');
	// TODO: Add your command handler code here
}

void CH3App::OnMenu32798()
{
	// TODO: Add your command handler code here
	keyboard('G');
}

void CH3App::OnWebTaedsearchcurrentnode32800()
{
	keyboard('a');
	// TODO: Add your command handler code here
}

void  CSetWebDlg::OnBnClickedButton1()// Google Image
{
	// TODO: Add your control notification handler code here
	
	char * buffer="http://google.com/search?tbm=isch&hl=en&q=";
	setPrefix(buffer);
    setPostfix("");
	OnOK();

}

void CSetWebDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButton14();
	OnOK();
}

void CSetWebDlg::OnBnClickedButton14()
{
	CDataExchange* pDX=new CDataExchange(this,true);
	UpdateData(true);
	CSetWebDlg::DoDataExchange(pDX);
	CString prefix;
	CString postfix;
	DDX_Text(pDX, IDC_EDIT1, prefix);
	DDX_Text(pDX, IDC_EDIT2, postfix);
	char buffer[512]="BLANK_PLACEHOLDER";
	sprintf(buffer,"%s",prefix.GetString());
	//printf(buffer,"DEBUG::PREFIX IN MFC>%s",prefix.GetString());
	setPrefix(buffer);// TEST SJK2011
	sprintf(buffer,"%s",postfix.GetString());
	//printf(buffer,"DEBUG::PREFIX IN MFC>%s",postfix.GetString());
	setPostfix(buffer);// TEST SJK2011

	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton2()
{

	char * buffer="http://google.com/search?q=%2Bsite:wikipedia.org ";
	setPrefix(buffer);
    setPostfix("");
	
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton3()
{
	char * buffer="http://bing.com/search?q=";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton4()
{
	char * buffer="http://google.com/search?q=%2Bsite:ncbi.nlm.nih.gov ";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton5()
{
	char * buffer="http://google.com/search?q=%2Bsite:nih.gov ";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton6()//google PDB
{
	char * buffer="http://google.com/search?q=%2B+filetype:pdb ";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}



void CSetWebDlg::OnBnClickedButton7()
{
	char * buffer="http://google.com/search?q=%2Bsite:nature.com ";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton8()
{
	char * buffer="http://google.com/search?q=%2Bsite:sciencemag.org ";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton9()
{
    char * buffer="http://google.com/scholar?q= ";
	setPrefix(buffer);
    setPostfix("");
	OnOK();

	// TODO: Add your control notification handler code here
}


void CSetWebDlg::OnBnClickedButton10()
{

	char * buffer="http://www.youtube.com/results?search_query=";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton11()
{
	char * buffer="http://uswest.ensembl.org/Multi/Search/Results?species=all;idx=;q= ";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton12()
{
	
	char * buffer="http://www.cathdb.info/cgi-bin/search.pl?search_text=";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CSetWebDlg::OnBnClickedButton13()
{
	char * buffer="http://www.cathdb.info/cgi-bin/search.pl?search_text=";
	setPrefix(buffer);
    setPostfix("");
	OnOK();
	// TODO: Add your control notification handler code here
}

void CH3App::On2dviewerToggleinteractive()
{
	keyboard('o');
	// TODO: Add your command handler code here
}

//void CH3App::OnFileSaveas()//INTERCEPTED CALL
//{
	
	// TODO: Add your command handler code here
//}

void CH3App::OnFileSave32806()
{
	//keyboard('R');
	char strFilter[] = { "NHX Files (*.nhx)|*.nhx|All Files (*.*)|*.*||" };
	
	CFileDialog  mySaveBox(FALSE,".nhx",NULL,OFN_LONGNAMES,strFilter,NULL,0,TRUE);
	if (mySaveBox.DoModal()==IDOK){
		printf("%s\n",mySaveBox.GetFileName());
		printf("%s\n",mySaveBox.GetPathName());
		//printf("%s\n",mySaveBox.GetFolderPath());
		//std::cout << mySaveBox.GetFolderPath().GetString()<<std::endl;
		//std::cout << mySaveBox.GetFileName().GetString()<<std::endl;
		
		//CString outfileName=mySaveBox.GetFileName();
		//CString outfilePath=mySaveBox.GetFolderPath();
		char fname[1024];
		sprintf(fname,"%s",mySaveBox.GetPathName());
		saveFile(fname);
//		std::cout << "Attempting to save: "<<fname<<std::endl;
		//std::cout << "Attempting to save: "<<outfileName<<std::endl;
		//std::cout << "Attempting to save: "<<outfilePath<<std::endl;
	}
	// TODO: Add your command handler code here
}
BEGIN_MESSAGE_MAP(CSetDrawgram, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CSetDrawgram::OnBnClickedButton1)
END_MESSAGE_MAP()

void CSetDrawgram::OnBnClickedButton1()
{
	// main function Drawgram_A Drawgram_B DrawGram_C
	//DrawgramSet("sc");
	// TODO: Add your control notification handler code here
}

void CH3App::On2dviewerChoosestaticoptions()
{
	CSetDrawgram CDrawgram;
    CDrawgram.DoModal();
	// TODO: Add your command handler code here
}

void CH3App::OnNavigateCenter()
{
	keyboard('c');
	// TODO: Add your command handler code here
}

void CH3App::OnNavigateTreatnumeric()
{
	keyboard('Y');
	// TODO: Add your command handler code here
}

void CAboutDlg2::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CH3App::OnViewDensegraphtoggleandredraw()
{
	keyboard('M');
	// TODO: Add your command handler code here
}

void CH3App::OnBnClickedButton5()
{
	
	// TODO: Add your control notification handler code here
}
/*
void CAboutDlg2::OnBnClickedTagleft()
//void CH3App::OnBnClickedTagleft()
{
	tagLeft();
	// TODO: Add your control notification handler code here
}

void CAboutDlg2::OnBnClickedTagRight()
//void CH3App::OnBnClickedTagRight()
{
	tagRight();
	// TODO: Add your control notification handler code here
}

void CH3App::OnBnClickedShowPath()
{
	keyboard('f');
	// TODO: Add your control notification handler code here
}
*/
void CAboutDlg2::OnBnClickedTagleft()
{
	tagLeft();
	// TODO: Add your control notification handler code here
}

void CAboutDlg2::OnBnClickedTagRight()
{
	tagRight();
	// TODO: Add your control notification handler code here
}

void CAboutDlg2::OnBnClickedShowPath()
{
	keyboard('?');
	// TODO: Add your control notification handler code here
}

void CH3App::OnNavigateTagleft()
{
	keyboard('<');
	// TODO: Add your command handler code here
}

void CH3App::OnNavigateTagright()
{
	keyboard('>');
	// TODO: Add your command handler code here
}

void CH3App::OnNavigateShowpath()
{
	keyboard('?');
	// TODO: Add your command handler code here
}

void CH3App::OnNavigateHidepath()
{
	keyboard('/');
	// TODO: Add your command handler code here
}

void CH3App::OnViewGhostboxes()
{
	keyboard('0');
	// TODO: Add your command handler code here
}

void CH3App::OnViewNoghostboxes()
{
	keyboard('*');
	// TODO: Add your command handler code here
}

void CH3App::OnViewCyclelabelelements()
{
	keyboard('L');
	// TODO: Add your command handler code here
}

void CH3App::OnViewToggleelements()
{
		keyboard('L');// deprecated
	// TODO: Add your command handler code here
}

void CH3App::OnNcbiSearchCurrentNode()
{
	keyboard('w');
	// TODO: Add your command handler code here
}

void CH3App::OnShapesDensechordateview()
{
	keyboard('D');
	// TODO: Add your command handler code here
}

void CH3App::OnLabelsTogglelandmarknode()
{
	keyboard('t');
	// TODO: Add your command handler code here
}

void CH3App::OnLabelsClear()
{
	keyboard('T');
	// TODO: Add your command handler code here
}

void CH3App::OnWebNcbigilookup()
{
	keyboard('A');
	// TODO: Add your command handler code here
}

void CH3App::OnLabelsLandmarkhighdnds()
{
	keyboard('z');
	// TODO: Add your command handler code here
}

void CH3App::OnLabelsLandmarkcurrentpath()
{
	keyboard('Z');
	// TODO: Add your command handler code here
}

void CH3App::OnColoringTogglelinedraw()
{
	keyboard('N');
	// TODO: Add your command handler code here
}
