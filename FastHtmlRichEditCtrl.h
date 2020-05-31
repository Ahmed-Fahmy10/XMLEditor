#if !defined(AFX_FASTHTMLRICHEDITCTRL_H__46148F2F_5109_11D6_A00C_0004768C7143__INCLUDED_)
#define AFX_FASTHTMLRICHEDITCTRL_H__46148F2F_5109_11D6_A00C_0004768C7143__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcmn.h>

enum FastHtmlColorState
{
	epsUnknown		= 0x00,
	epsInTag		= 0x01,
	epsInDblQuotes	= 0x02,
	epsInComment	= 0x03,
	epsInNormalText	= 0x04,
	epsError		= 0x10
};
#define LINE_COLORED	0x80	// This bit indicates that a line is already colored

/////////////////////////////////////////////////////////////////////////////
// CFastHtmlRichEditCtrl window

class CFastHtmlRichEditCtrl : public CRichEditCtrl
{
// Construction/Destruction
public:
	// Default constructor
	CFastHtmlRichEditCtrl();
	// Default destructor
	virtual ~CFastHtmlRichEditCtrl();

public:
// Character format functions
	// Sets the character format to be used for Tags
	void SetTagCharFormat(int nFontHeight = 8, 
		COLORREF clrFontColour = RGB(128, 0, 0), 
		CString strFontFace = _T("Courier New"),
		bool bParse = true);
	// Sets  the character format to be used for Tags
	void SetTagCharFormat(CHARFORMAT& cfTags, bool bParse = true);
	// Sets the character format to be used for Quoted text
	void SetQuoteCharFormat(int nFontHeight = 8, 
		COLORREF clrFontColour = RGB(0, 128, 128), 
		CString strFontFace = _T("Courier New"),
		bool bParse = true);
	// Sets  the character format to be used for Quoted text
	void SetQuoteCharFormat(CHARFORMAT& cfQuoted, bool bParse = true);
	// Sets the character format to be used for Comments
	void SetCommentCharFormat(int nFontHeight = 8, 
		COLORREF clrFontColour = RGB(0, 128, 0), 
		CString strFontFace = _T("Courier New"),
		bool bParse = true);
	// Sets  the character format to be used for Comments
	void SetCommentCharFormat(CHARFORMAT& cfComments, bool bParse = true);
	// Sets the character format to be used for Normal Text
	void SetTextCharFormat(int nFontHeight = 8, 
		COLORREF clrFontColour = RGB(0, 0, 0), 
		CString strFontFace = _T("Courier New"),
		bool bParse = true);
	// Sets  the character format to be used for Normal Text
	void SetTextCharFormat(CHARFORMAT& cfText, bool bParse = true);

// Parsing functions
	// Parses all lines in the control, colouring each line accordingly.
	void ParseAllLines();
	
// Miscellaneous functions
	// Loads the contents of the specified file into the control.
	// Replaces the existing contents and parses all lines.
	void LoadFile(CString& strPath);
	
	// Enables/disables the background coloring timer. If enabled, event
	// is raised every uiInterval millis and nNumOfLines uncolored lines are colored.
	void SetBckgdColorTimer(UINT uiInterval = 1000, int nNumOfLines = 10);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastHtmlRichEditCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CFastHtmlRichEditCtrl)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CHARFORMAT	m_cfTags;
	CHARFORMAT	m_cfText;
	CHARFORMAT	m_cfQuoted;
	CHARFORMAT	m_cfComment;

	// To handle multiline comments correctly, I must track the color state of the end of every line  
	int		m_nLineCount;		// Number of lines in RichEditCtr
	BYTE*	m_pLinesEndState;	// Array of size m_nLineCount for holding the color state of each line's end char   

	bool	m_bOnEnVscrollDisabled;
	int		m_nOnChangeCharPosition;	// OnKeyDown signals OnChange to InvalidateColorStates

	UINT	m_uiBckgdTimerInterval;		// How many millis between each iteration
	int		m_nBckgdTimerNumOfLines;	// How many lines to color in each iteration
	bool	m_bBckgdTimerActivated;

private:

// Helper functions
	int GetLastVisibleLine();
	FastHtmlColorState ParseLines(LPCTSTR pLines, int nCharPosition, bool bColor, int nCurrentLine = -1);
	void ColorVisibleLines(int nCharPosition = -1);
	void InvalidateColorStates(int nLineIndex);
	void UpdateLinesArraySize();

	int ColorRangeHelper(int nColorStart, int nColorEnd, CHARFORMAT charFormat, int nColorFromChar = -1);
	int GetLineHelper(int nLineIndex, CString& strLine, int nLineLength = -1);
	void TrimRightCrLfHelper(CString& strText, int nLength = -1);
	int FindCommentStartHelper(int nCharPosition);
	int FindCommentEndHelper(int nCharPosition);
	void SetFirstLineCharColor(int nLineIndex);

	void StartColoringTimer();
	void StopColoringTimer();	// CURRENTLY NOT USED
	BOOL IsWindowCompletelyObscured();

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnVscroll();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTHTMLRICHEDITCTRL_H__46148F2F_5109_11D6_A00C_0004768C7143__INCLUDED_)
