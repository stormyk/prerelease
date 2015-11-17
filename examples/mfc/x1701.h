#pragma once


// x1701 dialog

class x1701 : public CDialog
{
	DECLARE_DYNAMIC(x1701)

public:
	x1701(CWnd* pParent = NULL);   // standard constructor
	virtual ~x1701();

// Dialog Data
	enum { IDD = IDD_FORMVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
