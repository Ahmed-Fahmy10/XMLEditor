#if !defined(AFX_CFINDTEXTDLG_H__F59009E3_7B01_11D2_8C4F_0080ADB86836__INCLUDED_)
#define AFX_CFINDTEXTDLG_H__F59009E3_7B01_11D2_8C4F_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "cedefs.h"
#include "editcmd.h"

class CCrystalTextView;


/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg dialog

class CFindTextDlg : public CDialog
{
private:
	CCrystalTextView *m_pBuddy;

// Construction
public:
	CFindTextDlg(CCrystalTextView *pBuddy);

	CPoint m_ptCurrentPos;
// Dialog Data
	//{{AFX_DATA(CFindTextDlg)
	enum { IDD = IDD_EDIT_FIND };
	int		m_nDirection;
	BOOL	m_bMatchCase;
	CString	m_sText;
	BOOL	m_bWholeWord;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTextDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindTextDlg)
	virtual void OnOK();
	afx_msg void OnChangeEditText();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFINDTEXTDLG_H__F59009E3_7B01_11D2_8C4F_0080ADB86836__INCLUDED_)
