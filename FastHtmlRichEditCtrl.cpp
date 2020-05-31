/****************************************************************************\
*            
*     FILE:     FastHtmlRichEditCtrl.cpp
*
*     PURPOSE:  Fast Syntax Highlighting for HTML Code
*
*     COMMENTS: This file contains the main window and instance handing
*               for this project.
*
*     AUTHOR:	Ziv Gilad, based on Derek Lakin’s CHtmlRichEditCtrlSSL
*
*
* History:
*                March '2006 - Created
*
\****************************************************************************/

#include "pch.h"
#include "framework.h"
#include "FastHtmlRichEditCtrl.h"

#define KEY_TAG_START	'<'
#define KEY_TAG_END		'>'
#define KEY_DBL_QUOTE	'"'
#define COMMENT_START	_T("<!--")
#define COMMENT_END		_T("-->")

#define TIMER_BACKGROUNDCOLORING	100

/////////////////////////////////////////////////////////////////////////////
// CFastHtmlRichEditCtrl
/////////////////////////////////////////////////////////////////////////////

// Callback procedure that reads a files contents into rich edit control.
static DWORD CALLBACK StreamInCallback(DWORD dwCookie, LPBYTE pbBuff, 
									   LONG cb, LONG *pcb)
{
   CFile* pFile = (CFile*)dwCookie;
   *pcb = pFile->Read(pbBuff, cb);
   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

// Default constructor
CFastHtmlRichEditCtrl::CFastHtmlRichEditCtrl()
{
	// Common values
	UINT	uiSize			= sizeof(CHARFORMAT);
	DWORD	dwMask			= CFM_COLOR | CFM_FACE | CFM_SIZE; /* | CFM_CHARSET;*/
	LONG	lHeight			= 160;	// 8 point => 160 * (1/20)

	// Initialise the Tags CHARFORMAT
	m_cfTags.cbSize			= uiSize;
	m_cfTags.dwMask			= dwMask;
	m_cfTags.dwEffects		= 0;
	m_cfTags.yHeight		= lHeight;
	m_cfTags.crTextColor	= RGB(128, 0, 0);
	m_cfTags.bCharSet		= ANSI_CHARSET;
	strcpy_s(m_cfTags.szFaceName,	_T("Courier New"));

	// Initialise the Text CHARFORMAT
	m_cfText.cbSize			= uiSize;
	m_cfText.dwMask			= dwMask;
	m_cfText.dwEffects		= 0;
	m_cfText.yHeight		= lHeight;
	m_cfText.crTextColor	= RGB(0, 0, 0);
	m_cfText.bCharSet		= ANSI_CHARSET;
	strcpy_s(m_cfText.szFaceName,	_T("Courier New"));

	// Initialise the Quoted Text CHARFORMAT
	m_cfQuoted.cbSize		= uiSize;
	m_cfQuoted.dwMask		= dwMask;
	m_cfQuoted.dwEffects	= 0;
	m_cfQuoted.yHeight		= lHeight;
	m_cfQuoted.crTextColor	= RGB(0, 128, 128);
	m_cfQuoted.bCharSet		= ANSI_CHARSET;
	strcpy_s(m_cfQuoted.szFaceName, _T("Courier New"));

	// Initialise the Comment CHARFORMAT
	m_cfComment.cbSize		= uiSize;
	m_cfComment.dwMask		= dwMask;
	m_cfComment.dwEffects	= 0;
	m_cfComment.yHeight		= lHeight;
	m_cfComment.crTextColor	= RGB(0, 128, 0);
	m_cfComment.bCharSet		= ANSI_CHARSET;
	strcpy_s(m_cfComment.szFaceName, _T("Courier New"));

	// For tracking the color state at the end of every line: 
	m_nLineCount = 0;
	m_pLinesEndState = NULL;

	m_bOnEnVscrollDisabled = false;	// Disable OnEnVscroll coloring during ColorVisibleLines and OnKeyDown
	m_nOnChangeCharPosition = -1;	// OnKeyDown defers coloring to OnChange when overriding selected text

	// Background coloring timer:
	m_uiBckgdTimerInterval = 100;
	m_nBckgdTimerNumOfLines = 10;
	m_bBckgdTimerActivated = false;
}

// Default destructor
CFastHtmlRichEditCtrl::~CFastHtmlRichEditCtrl()
{
	// Free the allocated vector:
	if (m_pLinesEndState)
		{free(m_pLinesEndState);	m_pLinesEndState = NULL;}
}

/////////////////////////////////////////////////////////////////////////////
// Character format functions
/////////////////////////////////////////////////////////////////////////////

// Sets the character format to be used for Tags
void CFastHtmlRichEditCtrl::SetTagCharFormat(int nFontHeight, 
											COLORREF clrFontColour, 
											CString strFontFace, bool bParse)
{
	m_cfTags.yHeight		= 20 * nFontHeight;
	m_cfTags.crTextColor	= clrFontColour;
	strcpy_s(m_cfTags.szFaceName, strFontFace);

	if(true == bParse)	ParseAllLines();
}

// Sets  the character format to be used for Tags
void CFastHtmlRichEditCtrl::SetTagCharFormat(CHARFORMAT& cfTags, bool bParse)
{
	m_cfTags = cfTags;
	if(true == bParse)	ParseAllLines();
}

// Sets the character format to be used for Quoted text
void CFastHtmlRichEditCtrl::SetQuoteCharFormat(int nFontHeight, 
											  COLORREF clrFontColour, 
											  CString strFontFace, bool bParse)
{
	m_cfQuoted.yHeight		= 20 * nFontHeight;
	m_cfQuoted.crTextColor	= clrFontColour;
	strcpy_s(m_cfQuoted.szFaceName, strFontFace);

	if(true == bParse)	ParseAllLines();
}

// Sets  the character format to be used for Quoted text
void CFastHtmlRichEditCtrl::SetQuoteCharFormat(CHARFORMAT& cfQuoted, bool bParse)
{
	m_cfQuoted = cfQuoted;
	if(true == bParse)	ParseAllLines();
}

// Sets the character format to be used for Comments
void CFastHtmlRichEditCtrl::SetCommentCharFormat(int nFontHeight,
												COLORREF clrFontColour, 
												CString strFontFace, 
												bool bParse)
{
	m_cfComment.yHeight		= 20 * nFontHeight;
	m_cfComment.crTextColor	= clrFontColour;
	strcpy_s(m_cfComment.szFaceName, strFontFace);

	if(true == bParse)	ParseAllLines();
}

// Sets  the character format to be used for Comments
void CFastHtmlRichEditCtrl::SetCommentCharFormat(CHARFORMAT& cfComments, 
												bool bParse)
{
	m_cfComment = cfComments;
	if(true == bParse)	ParseAllLines();
}

// Sets the character format to be used for Normal Text
void CFastHtmlRichEditCtrl::SetTextCharFormat(int nFontHeight, 
											 COLORREF clrFontColour, 
											 CString strFontFace, bool bParse)
{
	m_cfText.yHeight		= 20 * nFontHeight;
	m_cfText.crTextColor	= clrFontColour;
	strcpy_s(m_cfText.szFaceName, strFontFace);

	if(true == bParse)	ParseAllLines();
}

// Sets  the character format to be used for Normal Text
void CFastHtmlRichEditCtrl::SetTextCharFormat(CHARFORMAT& cfText, bool bParse)
{
	m_cfText = cfText;
	if(true == bParse)	ParseAllLines();
}

/////////////////////////////////////////////////////////////////////////////
// Parsing functions
/////////////////////////////////////////////////////////////////////////////

// Parses all lines in the control, coloring each line accordingly.
void CFastHtmlRichEditCtrl::ParseAllLines()
{
	// Get control's text (send WM_GETTEXT  message):
	CString strCtrlText;
	GetWindowText(strCtrlText);
	
	// Allocate the vector for holding the color states of all lines: 
	m_nLineCount = GetLineCount();
	free(m_pLinesEndState);
	m_pLinesEndState = (BYTE*)malloc(m_nLineCount * sizeof(BYTE));	ASSERT(m_pLinesEndState != NULL);
	ZeroMemory(m_pLinesEndState, m_nLineCount * sizeof(BYTE));
	
	// Go over HTML line by line and calculate the color state (Comment/Quoted/Tag/Normal)
	// of the last char.
	// NOTE: This calulation is quite fast as we do it in one pass over HTML without any coloring.
	const TCHAR* pCtrlText = (LPCTSTR)strCtrlText;
	ParseLines(pCtrlText, -1 , false, 0);
	
	// Disable redraw to prevent flickering
	SetRedraw(FALSE);

	// Store the current selection and the first visible line
	CHARRANGE crCurrent;
	GetSel(crCurrent);

	// Color the all the text as Text initially
	SetSel(0, -1);
	SetWordCharFormat(m_cfText);

	// Get the control's visible range
	int nFirstLine = GetFirstVisibleLine();	// Send the control EM_GETFIRSTVISIBLELINE message
	int nLastLine = GetLastVisibleLine();	// No such message as EM_GETLASTVISIBLELINE - use our own algorithm
	for(int i = nFirstLine; i <= nLastLine; i++)
	{
		CString strLine;
		int nLineLength = GetLineHelper(i, strLine);

		if(nLineLength > 0)
		{
			ParseLines((LPCTSTR)strLine, -1, true, i);	// Color the line (only one line)
			m_pLinesEndState[i] |= LINE_COLORED;
		}
	}

	// Restore the original selection
	SetSel(crCurrent);

	// Restore the original view position
	LineScroll(-nFirstLine, 0);

	SetRedraw(TRUE);
	Invalidate(FALSE);

	// Activate the background coloring timer
	StartColoringTimer();
}

/////////////////////////////////////////////////////////////////////////////
// Miscellaneous functions
/////////////////////////////////////////////////////////////////////////////

// Loads the contents of the specified file into the control.
// Replaces the existing contents. To parse lines ParseAllLines must be called
void CFastHtmlRichEditCtrl::LoadFile(CString& strPath)
{
	if(strPath.GetLength() > 0)
	{
		CFile file(strPath, CFile::modeRead);
		EDITSTREAM es;

		es.dwCookie = (DWORD)&file;
		es.pfnCallback = StreamInCallback;
		StreamIn(SF_TEXT, es);
	}
}

void CFastHtmlRichEditCtrl::SetBckgdColorTimer(UINT uiInterval /*= 1000*/, int nNumOfLines /*= 10*/)
{
	if (uiInterval == 0 || nNumOfLines <= 0 && m_bBckgdTimerActivated)
	{// Disable current background coloring timer:
		BOOL bRes = KillTimer(TIMER_BACKGROUNDCOLORING);	ASSERT(bRes);
		m_bBckgdTimerActivated = false;
	}
	
	m_uiBckgdTimerInterval = uiInterval;
	m_nBckgdTimerNumOfLines = nNumOfLines;
}

/////////////////////////////////////////////////////////////////////////////
// Overrides
/////////////////////////////////////////////////////////////////////////////

// PreSubclassWindow override. Ensures that the control is registered to 
// receive ENM_CHANGE notifications.
// Also register to receive ENM_SCROLL notifications when a keyboard event causes a change 
// in the view area of the edit control, for example, pressing HOME, END, PAGE UP, PAGE DOWN, UP ARROW, or DOWN ARROW.
void CFastHtmlRichEditCtrl::PreSubclassWindow() 
{
	CRichEditCtrl::PreSubclassWindow();

	// Set the event mask to include ENM_CHANGE
	long lMask = GetEventMask();
	lMask |= ENM_CHANGE | ENM_SCROLL;
	SetEventMask(lMask);

	// Set the default character format to be m_cfText
	SetDefaultCharFormat(m_cfText);

	// The CRichEditCtrl always starts with LineCount == 1, so you can start typing text immediately
	// NOTE: When CRichEditCtrl used from resource, OnCreate isn't called and GetLineCount returns 1.
	// When used as a member and Create() is explicitly called, GetLineCount here return 0 and in OnCreate returns 1.
	m_nLineCount = 1;
	m_pLinesEndState = (BYTE*)malloc(sizeof(BYTE));	ASSERT(m_pLinesEndState != NULL);
	m_pLinesEndState[0] = epsInNormalText;	// Until changed by <, " and so, we're in state "text"
}

/////////////////////////////////////////////////////////////////////////////
// CFastHtmlRichEditCtrl message handlers
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CFastHtmlRichEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CFastHtmlRichEditCtrl)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_CONTROL_REFLECT(EN_VSCROLL, OnEnVscroll)
	ON_WM_HSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

/**
 * WM_CHAR handler traps "interesting chars" (chars that change the color state, for example ‘<’ or ‘”)
 * and updates the colors accordingly.
 * Sometimes we invalidate up to 3 preceding chars to take care of 
 * comments, as pressing the ‘-‘ char might complete a comment start combination. 
 * NOTE that OnChar is too late for handling VK_BACK and VK_DELETE,
 * as chars have already been deleted at this stage
 */
void CFastHtmlRichEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE(_T("OnChar\n"));

	// Get current caret position. Not sure if GetSel the best choice???
	// NOTE: We call GetSel before base class to get the correct caret position
	long lStart = 0, lEnd	= 0;
	GetSel(lStart, lEnd);
	int nCharPosition = lStart;

	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);

	// After CRichEditCtrl has added the char, we can get the following:
	long lCharStart	= LineIndex();	// Retrieves the character index of the current line (the line that contains the caret)
	long lCharFromLineStart = nCharPosition - lCharStart;
	int nLineIndex = LineFromChar(lCharStart);
	
	// DEBUG:
	int nLineIndex1 = LineFromChar(-1);
	CString strLine;
	int nLineLength = GetLineHelper(nLineIndex, strLine);

	// Is it a special char? If not, basically the RichEditCtrl maintains himself the current WordCharFormat.
	// However this may change when pressing a key breaks a comment combination or after the insertion point
	// was moved just passed an ending tag or ending double quotes. This will be handled
	// by the 'default' clause
	switch(nChar)
	{
	case KEY_TAG_START :
		{// Invalide the colors of this and next lines as we might move to epsInTag
			InvalidateColorStates(nCharPosition);
			break;
		}
	case KEY_TAG_END :
		{// Invalide the colors of this and next lines as we might move to InNormalText (or stay in DblQuotes\Comment)
		// Recalculate the color state of this line because after '>' we might move to InNormalText or stay in DblQuotes\Comment.
		 // However the color of this '>' stays the same as the previous char (Tag\DblQuotes\Comment\NormalText)	
			InvalidateColorStates(nCharPosition);
			break;
		}
	case KEY_DBL_QUOTE:
		{// Recalculate the color state of this line because after '"' we might move to DblQuotes\Tag or stay in Comment\NormalText.
		 // The color of this '" changes if me move to DblQuotes and affects the following lines as well, so invalide the colors of this and next lines
			InvalidateColorStates(nCharPosition);
			if ((m_pLinesEndState[nLineIndex] &~ LINE_COLORED) == epsInTag)	// These were ending double quotes so now we're in Tag
				{BOOL bRes = SetWordCharFormat(m_cfTags);	ASSERT(bRes);}
			break;
		}
	case '-' :
		{// Trap the Comment start ("<!--") and Comment end combinations, so we can color correctly in these case:
			// Check if we move to epsInComment:
			int nCommentStart = FindCommentStartHelper(nCharPosition);
			if (nCommentStart != -1)
				InvalidateColorStates(nCommentStart);
			else
			{// Check if we move to epsInNormalText
				CString strTmp;
				int nChars = GetTextRange(nCharPosition - 1, nCharPosition + 2, strTmp);	// Search for "-->" and this char is the middle '-'
				if (strTmp != _T("-->"))
					{nChars = GetTextRange(nCharPosition, nCharPosition + 3, strTmp);}	// Search for "-->" and this char is the leftmost '-'
				if (strTmp == _T("-->"))	// Search for "-->" and this char is the leftmost '-'
					{InvalidateColorStates(nCharPosition);}
			}
			break;
		}
	case '!' :	// Trap the Comment start ("<!--"), so we can color correctly when state moves to epsInComment:
		{
			int nCommentStart = FindCommentStartHelper(nCharPosition);
			if (nCommentStart != -1)	InvalidateColorStates(nCommentStart);
			break;
		}
	case VK_RETURN :
		{
			UpdateLinesArraySize();	// Since a new line has just been added

			// After VK_RETURN we must explicitly set the correct color:
			SetFirstLineCharColor(nLineIndex);
			break;
		}
	default:
		{
			if (nChar != VK_BACK)	// VK_BACK was already handled by OnKeyDown
			{// Pressing a key might break comment combination.
			 // Pressing a key after the insertion point might result in a wrong color when we're
	         // just passed an ending tag or ending double quotes.
				int nCommentStart = FindCommentStartHelper(nCharPosition);
				if (nCommentStart != -1)	// Just broke comment start combination
					InvalidateColorStates(nCommentStart);
				else
				{
					CString test;
					GetWindowText(test);
					
					int nCommentEnd = FindCommentEndHelper(nCharPosition);	// +1 to pass the currently added char 
					if (nCommentEnd != -1)	// Just broke comment end combination
						InvalidateColorStates(nCommentStart);
					else
					{
						CString prevChar;
						int nChars = GetTextRange(nCharPosition - 1 , nCharPosition, prevChar);
						if (prevChar == _T('>') || prevChar == _T('\"'))	// We just past ending tag/ending quotes)
							InvalidateColorStates(nCharPosition - 1);
					}
				}
			}
		}
	}
	
	return;
}

// WM_GETDLGCODE mesasge handler. Ensures that the edit control
// handles the TAB key.
UINT CFastHtmlRichEditCtrl::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS;
}

/**
 * OnChange is used to handle the cases of overriding selected text.
 * In these cases OnChange is called after OnKeyDown, thus OnKeyDown can't
 * see the updated text and defers the coloring to OnChange.
 */
void CFastHtmlRichEditCtrl::OnChange() 
{
	TRACE(_T("OnChange\n"));

	if (m_nOnChangeCharPosition != -1)
	{// OnKeyDown defers the call to OnChange when overriding selected text
		UpdateLinesArraySize();
		InvalidateColorStates(m_nOnChangeCharPosition);
		m_nOnChangeCharPosition = -1;
	}
}

// WM_CREATE message handler. Ensures that the control is registered to
// receive ENM_CHANGE messages.
// NOTE that no need for initialization here since alrady done in PreSubclassWindow
int CFastHtmlRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set the default character format to be m_cfText
	SetDefaultCharFormat(m_cfText);

	return 0;
}

/**
 * This helper was found somewhere on the net
 */
BOOL CFastHtmlRichEditCtrl::IsWindowCompletelyObscured()
{
	RECT clip, winrect; 

	// Get Clipping box for window area (client not enough since I want to include the scrolls) 
	HDC hdc = ::GetWindowDC(m_hWnd);
	int cliptype = GetClipBox(hdc, &clip);	// In logical units
	::ReleaseDC(m_hWnd, hdc);

	// Check clipbox type
	if (cliptype == NULLREGION)
		{return TRUE;}	// Completely covered
	else if (cliptype == COMPLEXREGION)
		{return FALSE;}	// Partially covered
	else if (cliptype == SIMPLEREGION)
	{
		GetWindowRect(&winrect);
		// Normalize coordinates:
		winrect.bottom -= winrect.top;
		winrect.top = 0;
		winrect.right -= winrect.left;
		winrect.left = 0;
        if(EqualRect(&clip,&winrect))
			{return FALSE;}	//completely exposed 
		else
			{return TRUE;}	//completely covered
	}
	return FALSE;
}

/**
 * WM_TIMER message handler.
 * This is the background coloring timer. It runs as long as there are uncolored lines
 * After investigating, found that in order to prevent flickers:
 * 1. RichEditCtrl must have the focus to prevent vertical scrollbar flickers (Otherwise the vertical scrollbar thumb jumps and flickers)
 * 2. Caret must be positioned at the beginning of line with no selection. Otherwise the horizontal scrollbar shakes! (works only for class RichEdit20W, not RICHEDIT)
 * NOTE that when control is completely obscured - there're no restrictions!
 */
void CFastHtmlRichEditCtrl::OnTimer(UINT nIDEvent) 
{
	CRichEditCtrl::OnTimer(nIDEvent);

	if(nIDEvent == TIMER_BACKGROUNDCOLORING)
	{// Color next m_nBckgdTimerNumOfLines uncolored lines:
		TRACE(_T("OnTimer\n"));
		bool bColorEnabled = false;

		// To prevent flickering:
		CHARRANGE crCurrent;
		GetSel(crCurrent);
		int nCharPosition = crCurrent.cpMin;
		if ((GetFocus() == this) && 
			(crCurrent.cpMin == crCurrent.cpMax) &&
			(nCharPosition == LineIndex(LineFromChar(nCharPosition))) )
		{// 1. Control is in focus, caret is at beginning of line and no selection is made
			bColorEnabled = true;
		}
		else if (IsWindowCompletelyObscured())
		{// 2. Control is not in focus and its window completely obscured
			bColorEnabled = true;
		}
		if (!bColorEnabled)	return;	// To prevent flickering give up background coloring at this stage
		
		int nFirstVisibleLine = GetFirstVisibleLine();
		
		// Prepare for paint (disable redraw to prevent flickering):
		SetRedraw(FALSE);

		CString strLine;
		int nColored = 0;

		m_bOnEnVscrollDisabled = true;	// Clicking the last visible line would cause OnEnVscroll when line only partially visible	
		for (int i = 0; i < m_nLineCount && nColored < m_nBckgdTimerNumOfLines; i++)
		{
			if (!(m_pLinesEndState[i] & LINE_COLORED))
			{
				// Colour the all the text as Text initially, use nCharPosition for optimization:
				int nLineLength = GetLineHelper(i, strLine);
				long lCharStart	= LineIndex(i);

				SetSel(lCharStart, lCharStart + nLineLength);
				SetSelectionCharFormat(m_cfText);

				ParseLines((LPCTSTR)strLine, -1 , true, i);
				m_pLinesEndState[i] |= LINE_COLORED;

				nColored++;
			}
		}
		if (nColored < m_nBckgdTimerNumOfLines)
		{// All lines are colored? The background coloring timer can be turned off:
			BOOL bRes = KillTimer(TIMER_BACKGROUNDCOLORING);	ASSERT(bRes);
			m_bBckgdTimerActivated = false;
		}

		// Restore after painting:
		SetSel(crCurrent);
		int nCurrentFirstVisibleLine = GetFirstVisibleLine();	
		if (nCurrentFirstVisibleLine != nFirstVisibleLine)
		{// Coloring might scroll the control, so restore original visible line.
		 // OnEnVscroll is disabled at this stage because otherwise we get into endless recurssion
			LineScroll(nFirstVisibleLine - nCurrentFirstVisibleLine, 0);
		}
		m_bOnEnVscrollDisabled = false;

		SetRedraw(TRUE);
		Invalidate(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////////////////
int CFastHtmlRichEditCtrl::GetLastVisibleLine()
{
	// The EM_GETRECT message retrieves the formatting rectangle of an edit control:
	RECT rfFormattingRect = {0};
	GetRect(&rfFormattingRect);
	rfFormattingRect.left++;
	rfFormattingRect.bottom -= 2;

	// The EM_CHARFROMPOS message retrieves information about the character
    // closest to a specified point in the client area of an edit control
	int nCharIndex =  CharFromPos(CPoint(rfFormattingRect.left, rfFormattingRect.bottom));

	//The EM_EXLINEFROMCHAR message determines which
    //line contains the specified character in a rich edit control
	 return LineFromChar(nCharIndex);
}

/**
 * Changing/Adding text to a line invalidates the color state of this line and
 * the ones who follow. For example if a comment start ("<!--") is added, following
 * lines should be colored as Comment until comment end ("<--") is reached.
 * Note that previous lines are unaffected
 */
void CFastHtmlRichEditCtrl::InvalidateColorStates(int nCharPosition)
{
	// Invalidate all lines starting from nLineIndex:
	int nLineIndex = LineFromChar(nCharPosition);
	long lCharStart	= LineIndex(nLineIndex);
	
	CString strCtrlText;
	int nChars = GetTextRange(lCharStart, GetTextLength(), strCtrlText);
	ParseLines((LPCTSTR)strCtrlText, -1, false, nLineIndex);
	
	// Color visible lines between [nLineIndex..m_nLineCount) that are not colored already, one line at a time:	
	ColorVisibleLines(nCharPosition);

	// Activate the background coloring timer
	StartColoringTimer();
}

/**
 * Color chars within the visible line range, that are not colored already.
 * Coloring starts from char nCharPosition, assuming previous chars are already colored.
 * If nCharPosition is omitted - all lines within visible range are colored if not already.
 */
void CFastHtmlRichEditCtrl::ColorVisibleLines(int nCharPosition /*= -1*/)
{
	// Color visible lines between [nLineIndex..m_nLineCount) that are not colored already, one line at a time:	
	CString strLine;
	int nFirstLine = GetFirstVisibleLine();	// Send the control EM_GETFIRSTVISIBLELINE message
	int nLastLine = GetLastVisibleLine();	// No such message as EM_GETLASTVISIBLELINE - use our own algorithm
	int nOrigFirstVisibleLine = nFirstLine;
	long nCharPositionLine = -1;
	if (nCharPosition != -1)
	{// Adjust nFirstLine to the line containing nCharPosition
		nCharPositionLine	= LineFromChar(nCharPosition);
		if (nFirstLine < nCharPositionLine)	nFirstLine = nCharPositionLine;
	}

	// Prepare for paint (disable redraw to prevent flickering):
	CHARRANGE crCurrent;
	SetRedraw(FALSE);
	GetSel(crCurrent);
	
	m_bOnEnVscrollDisabled = true;	// Clicking the last visible line would cause OnEnVscroll when line only partially visible
	for (int i = nFirstLine; i <= nLastLine; i++)
	{
		if (!(m_pLinesEndState[i] & LINE_COLORED))
		{
			int nFirstLineDEBUG2 = GetFirstVisibleLine();

			// Colour the all the text as Text initially, use nCharPosition for optimization:
			int nLineLength = GetLineHelper(i, strLine);
			long lCharStart	= LineIndex(i);
			if (i == nCharPositionLine)
			{
				nLineLength -= (nCharPosition - lCharStart);
				lCharStart = nCharPosition;
			}
			SetSel(lCharStart, lCharStart + nLineLength);
			SetWordCharFormat(m_cfText);

			ParseLines((LPCTSTR)strLine, (i == nCharPositionLine) ? nCharPosition : -1 , true, i);
			m_pLinesEndState[i] |= LINE_COLORED;
		}
	}

	// Restore after painting:
	SetSel(crCurrent);
	int nCurrentFirstVisibleLine = GetFirstVisibleLine();	
	if (nCurrentFirstVisibleLine != nOrigFirstVisibleLine)
	{// Coloring might scroll the control, so restore original visible line.
	 // OnEnVscroll is disabled at this stage because otherwise we get into endless recurssion
		LineScroll(nOrigFirstVisibleLine - nCurrentFirstVisibleLine, 0);
	}
	m_bOnEnVscrollDisabled = false;
	SetRedraw(TRUE);
	Invalidate(FALSE);
}

/**
 * ParseLines has two modes, depending on bColor parameter:
 * 1. Calculate color state of ending char for all lines without coloring.
 * 2. Calculate and color.
 * The ending chars color states are used for handling multiline tags/quotes/text/comments correctly.
 * nCharPosition states the chars from which coloring is required
 *	(can be in the middle of a line, but still the calculation starts from the beginning)
 * NOTE:
 *		New lines are automatically recognized.
 *		Function goes over xml in one pass, i.e. quite fast when no coloring involved.
 * The no-coloring mode is used for new xml inputs and several lines of xml simultaneously,
 * The coloring mode is uses one line at a time, for coloring visible lines only
 */
FastHtmlColorState CFastHtmlRichEditCtrl::ParseLines(LPCTSTR pLines, int nCharPosition, bool bColor, int nCurrentLine /*= -1 */)
{
	if (nCurrentLine == -1)	nCurrentLine = LineFromChar(nCharPosition);

	// Get color state of beginning char (same as previous line's ending char).
	// For first line we use InNormalText, as chars preceding  '<' are considered as normal text.
	FastHtmlColorState currentState = (nCurrentLine == 0) ? epsInNormalText : (FastHtmlColorState)(m_pLinesEndState[nCurrentLine - 1] & (~LINE_COLORED));
	
	// Take care of empty ("") strings
	// When caret is at the end position and user presses the enter, we get a new empty line (OnChar).
	// OnTimer also calls ParseLines with an empty string in the background
	if (*pLines == NULL)
	{
		m_pLinesEndState[nCurrentLine] = currentState;
		if (bColor)
		{// Apply color state of previous line's ending char:
			if (currentState == epsInTag)
				{BOOL bRes = SetWordCharFormat(m_cfTags);	ASSERT(bRes);}
			else if (currentState == epsInDblQuotes)
				{BOOL bRes = SetWordCharFormat(m_cfQuoted);	ASSERT(bRes);}
			else if (currentState == epsInComment)
				{BOOL bRes = SetWordCharFormat(m_cfComment);	ASSERT(bRes);}
			else
				{BOOL bRes = SetWordCharFormat(m_cfText);	ASSERT(bRes);}
			m_pLinesEndState[nCurrentLine] |= LINE_COLORED;
		}
		return currentState;
	}

	TCHAR* pCurChar = (TCHAR*)pLines;
	long lCharStart	= LineIndex(nCurrentLine);
	
	int nColorStart = -1;

	// loop while not whole line has been coloured:
	while (*pCurChar != NULL)
	{
		if (*pCurChar == _T('\r') || *pCurChar == _T('\n'))
		{// EOL is reached? Set the ending-char color state:
			if (*pCurChar == _T('\r') && *(pCurChar+1) == _T('\n'))	pCurChar++;	// Take care of \r\n pattern
			if ((m_pLinesEndState[nCurrentLine] &~ LINE_COLORED) == currentState)
				break;	// If ending-char color state of this line hasn't changed - no point of recalculating next lines 
			m_pLinesEndState[nCurrentLine++] = currentState;
		}
		else if (currentState == epsInComment)
		{// Inside Comment all chars are acceptable. The state is only changed by the "-->" combination:
			if ((*pCurChar == KEY_TAG_END) && (*(pCurChar - 1) == _T('-')) && (*(pCurChar - 2) == _T('-')))
			{
				if (bColor)	// Colourise the Comment (If no Comment in this line - we found a Comment end of a previous line):
					nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - pLines + 1, m_cfComment, nCharPosition);
				currentState = epsInNormalText;	// After leaving InComment state, all chars till '<' are considered normal text chars
			}	
		}
		else if (*pCurChar == KEY_TAG_START)
		{// '<' can start a Tag or a Comment block:
			if (currentState == epsInTag || currentState == epsInDblQuotes)
			{// IE doesn't allow '<' to be inside quotes, thus "d<d" is illegal. However I'll handle it as moving to epsInNormalText state:
				currentState = epsInNormalText;
			}
			else if (*(pCurChar + 1) == _T('!') && *(pCurChar + 2) == _T('-') && *(pCurChar + 3) == _T('-'))
			{// If we reach "<!--" we're staring a Comment
				currentState = epsInComment;
			}
			else
				{currentState = epsInTag;}
			nColorStart = pCurChar - pLines + lCharStart;
		}
		else if (*pCurChar == _T(KEY_DBL_QUOTE))
		{// Double quotes ('"') can be starting or ending quotes.
		 // However quotes are applicable in Tag only (in InNormalText we'll treat them as regular chars)
			if (currentState == epsInDblQuotes)
			{// These are ending quotes
				if (bColor)	// Colourise the string (If no staring quotes found in this line, we're ending a Quoted Text of a previous line):
					nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - pLines + 1, m_cfQuoted, nCharPosition);
				currentState = epsInTag;	// Assumption: Before we entered the InDblQuotes state we were in a Tag:
			}
			else if (currentState == epsInTag)	// Starting quotes
			{// These are beginning quotes:
				if (bColor)	// Colourise the Tag before the starting quotes (If no staring Tag found in this line, we're ending a Tag of a previous line)
					nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - pLines, m_cfTags, nCharPosition);
				currentState = epsInDblQuotes;
			}
		}
		else if (*pCurChar == _T(KEY_TAG_END))
		{// Ending tag ('>'):
			if (currentState != epsInNormalText)	// '>' in normal text has no meaning, for example >"va>lue" as text is valid
			{
				if (currentState == epsInTag)
				{
					if (bColor)	// Colourise the Tag:
						nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - pLines + 1, m_cfTags, nCharPosition);
					currentState = epsInNormalText;	// After leaving Tag state, all chars till '<' are considered normal text chars
				}
				else	// If '>' is part of a string, for example ("d>d"), leave the InDblQuotes state
					ASSERT(currentState == epsInDblQuotes);	//It can also be part of a string, for example ("d>d")
			}
		}
		pCurChar++;
	}
	
	if (bColor && (pCurChar - 1 - pLines) >= (nColorStart - lCharStart))	// The = is because nColorStart position should be colored as well
	{// Then there are uncolored chars left till end of line. These are part of a multiline Tag/DblQuotes/Comment/NormalText
		if (nColorStart == -1)	nColorStart = lCharStart;	// Haven't found any interesting keys - color whole line according to previous state
		SetSel(nColorStart, lCharStart + pCurChar - pLines);
		if (currentState == epsInTag)		SetWordCharFormat(m_cfTags);
		if (currentState == epsInDblQuotes)	SetWordCharFormat(m_cfQuoted);
		if (currentState == epsInComment)	SetWordCharFormat(m_cfComment);
	}

	if (nCurrentLine < m_nLineCount)
	{// Set color state of last line:	 
		m_pLinesEndState[nCurrentLine] = currentState;
	}
	else
	{// NOTE: When called by UpdateLinesArraySize or InvalidateColorStates, ParseLines
	 // is called with a range of lines, starting with current line till the end.
	 // UpdateLinesArraySize or InvalidateColorStates do not remove trailing \n because
	 // this \n might belong to previous line if whole line was just \n.
	 // Thefore if we have a trailing \n, it is alrady handled by the if (*pCurChar == _T('\n')
	 // and nCurrentLine now equals m_nLineCount
		ASSERT(nCurrentLine == m_nLineCount);
	}
	return currentState;
}

/**
 * This helper is used by ParseLines when called with bColor = "true".
 * When recoloring is required as a reaction to user-editing, nColorFromChar states the position
 * from which recoloring is required (caret position)
 * Return value is the updated nColorStart value (which is nColorEnd)
 */
int CFastHtmlRichEditCtrl::ColorRangeHelper(int nColorStart, int nColorEnd, CHARFORMAT charFormat, int nColorFromChar/* = -1 */)
{
	if (nColorStart < nColorFromChar)	nColorStart = nColorFromChar;
	if (nColorStart < nColorEnd)
	{
		SetSel(nColorStart, nColorEnd);
		SetWordCharFormat(charFormat);
	}
	return nColorEnd;
}

// Upon OnChar or OnChange the control's line count might have been changed
// (pressing enter, pasting text, deleting text)
void CFastHtmlRichEditCtrl::UpdateLinesArraySize()
{
	int nLineCount = GetLineCount();
	// If new lines have been added to the control - expand the color state array:
	if (m_nLineCount < nLineCount)
	{// Reallocate the m_pLinesEndState buffer:
		int nPrevLineCount = m_nLineCount;
		m_pLinesEndState = (BYTE*)realloc(m_pLinesEndState, nLineCount * sizeof(BYTE));	ASSERT(m_pLinesEndState != NULL);
		ZeroMemory(m_pLinesEndState + m_nLineCount * sizeof(BYTE), (nLineCount - m_nLineCount) * sizeof(BYTE));	// Clear color state of new lines
		m_nLineCount = nLineCount;	// Update line counter to reflect new array size

		long lCharStart	= LineIndex(nPrevLineCount);	// Get first char of new line (using previous m_nLineCount)
		
		// Get range of all new lines, starting with current line:
		CString strCtrlText;
		int nChars = GetTextRange(lCharStart, GetTextLength(), strCtrlText);

		// Parse color states of all new lines (without coloring)
		ParseLines((LPCTSTR)strCtrlText, -1, false, nPrevLineCount);
	}
	else if (m_nLineCount < nLineCount)
	{// If lines have been removed - just update the m_nLineCount member. realloc isn't necessary
		m_nLineCount = nLineCount;
	}
}

/**
 * NOTE: When I press enter on the first line add press chars on the second line, GetLine
 * always returns empty strings???
 */
int CFastHtmlRichEditCtrl::GetLineHelper(int nLineIndex, CString& strLine, int nLineLength /* = -1 */)
{
	if (nLineLength == -1)	nLineLength = LineLength(LineIndex(nLineIndex));
	
	int nChars = GetLine(nLineIndex, strLine.GetBuffer(nLineLength + 3), nLineLength);
	strLine.ReleaseBuffer(nChars);

	// The RichEditCtrl sometimes appends \r and sometimes \r\n so remove them:
	TrimRightCrLfHelper(strLine, nLineLength);

	return strLine.GetLength();
}

/**
 * The RichEditCtrl sometimes appends \r and sometimes \r\n which interfere with my calculations in ParseLines.
 * However I don't want to allocate a new string with SpanExcluding(_T("\r\n")), so I'm placing NULLs in original string: 
 */
void CFastHtmlRichEditCtrl::TrimRightCrLfHelper(CString& strText, int nLength /* = -1 */)
{
	if (nLength == -1)	nLength = strText.GetLength();

	if (nLength >= 1 && (strText[nLength - 1] == _T('\r') || strText[nLength - 1] == _T('\n')))
		strText.SetAt(nLength - 1, NULL);
	if (nLength >= 2 && strText[nLength - 2] == _T('\r'))
		strText.SetAt(nLength - 2, NULL);
}

/**
 * Pressing '-' or '!' might bring us to a comment start.
 * Pressing a key inside "<!--" breaks a comment start.
 * Deleting/Pasting inside "<!--" breaks a comment start.
 * Return value: The invalidat position (up to 3 chars before nCharPosition) or -1 of none found
 */
int CFastHtmlRichEditCtrl::FindCommentStartHelper(int nCharPosition)
{
	CString str1backwards, str2backwards, str3backwards;
	if (nCharPosition > 0)	{int nChars = GetTextRange(nCharPosition - 1 , nCharPosition, str1backwards);}
	if (nCharPosition > 1)	{int nChars = GetTextRange(nCharPosition - 2 , nCharPosition, str2backwards);}
	if (nCharPosition > 2)	{int nChars = GetTextRange(nCharPosition - 3 , nCharPosition, str3backwards);}
	
	if (str1backwards == _T('<'))			return  nCharPosition - 1;
	else if (str2backwards == _T("<!"))		return  nCharPosition - 2;
	else if (str3backwards == _T("<!-"))	return nCharPosition - 3;
	else									return -1;
}

int CFastHtmlRichEditCtrl::FindCommentEndHelper(int nCharPosition)
{
	CString str1forward, str2forward;
    int nChars = GetTextRange(nCharPosition + 1, nCharPosition + 2, str1forward);	// Search for "-->" and this char is the middle '-'
	nChars = GetTextRange(nCharPosition + 1, nCharPosition + 3, str2forward);
	if (str1forward == _T('>'))				return  nCharPosition + 2;
	else if (str2forward == _T("->"))		return  nCharPosition + 3;
	else									return -1;
}

/**
 * When user presses VK_RETURN or sets caret at begining of line, we must explicitly
 * set the correct color. For example if user writes <a>, then moves caret to line start
 * and presses a char, this char gets the color of the Tag instead of the InNormalText
 */
void CFastHtmlRichEditCtrl::SetFirstLineCharColor(int nLineIndex)
{
	// First char of this new line gets the color state of previous line's ending char:
	FastHtmlColorState prevState = (FastHtmlColorState)(m_pLinesEndState[nLineIndex - 1] &~ LINE_COLORED);
	if (prevState == epsInTag)
		{BOOL bRes = SetWordCharFormat(m_cfTags);	ASSERT(bRes);}
	else if (prevState == epsInDblQuotes)
		{BOOL bRes = SetWordCharFormat(m_cfQuoted);	ASSERT(bRes);}
	else if (prevState == epsInComment)
		{BOOL bRes = SetWordCharFormat(m_cfComment);	ASSERT(bRes);}
	else
		{BOOL bRes = SetWordCharFormat(m_cfText);	ASSERT(bRes);}
}

/**
 * For back I'm getting OnKeyDown, OnChange, OnChar.
 * For Delete I'm getting OnKeyDown, OnChange.
 * If a text is selected, I'm getting OnKeyDown, OnChar and OnChange.
 *		In this case when base class's OnKeyDown is done, GetWindowText doesn't
 *		return the updated text, so I'm defering the action to OnChange
 */
void CFastHtmlRichEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TRACE(_T("OnKeyDown\n"));

	// TODO: Add your message handler code here and/or call default
	long lStart = 0, lEnd = 0;
	GetSel(lStart, lEnd);
	int nCharPosition = lStart;

	// If caret at begining of line, we must explicitly set the correct color:
	long lCharStart	= LineIndex();	// Retrieves the character index of the current line (the line that contains the caret)
	if (lCharStart == lStart)
	{
		int nLineIndex = LineFromChar(lCharStart);
		SetFirstLineCharColor(nLineIndex);
	}

	bool bDeferedToOnChange = false;	// When selection is made, we have to defer the action to OnChange
	
	CString strDeleted;	// For trapping the deleted char/chars (GetTextRange must be called before base class's OnKeyDown)

	// Pressing ctrl+v invalidates all chars from nCharPosition and might also scroll the control.
	// OnKeyDown's base class calls OnEnVScroll and finally OnChar. Therefore we can query for updated
	// line count only after call to base class's OnKeyDown is made (GetWindowText after OnKeyDown returns updated text)
	bool bIsPasting = false;
	if (nChar == _T('V') && GetKeyState(VK_CONTROL) & 0x8000)
	{//check state of left and right CTRL keys. If high order bit is 1 - indicates the key is down
		bIsPasting = true;
		m_bOnEnVscrollDisabled = true;	// Disable OnEnVScroll because we don't have updated line count yet
	}
	else
	{// Not pasted text - check for deleted chars
		if (lStart != lEnd)
		{// A selection is made - pressing any "regular" char would delete the selection.
		 // However we cannot detect the changes here, so we must defer the action to OnChange
			int nChars = GetTextRange(lStart, (lEnd == lStart) ? (lEnd + 1) : lEnd, strDeleted);
			bDeferedToOnChange = true;
		}
		else
		{// The RichEditCtrl seems to add a "regular" char in his OnChar handler.
		 // However VK_BACK and VK_DELETE seem to be handled here (GetWindowText after OnKeyDown returns updated text)
			if (nChar == VK_DELETE)
			{// Trap the about-to-be-deleted char:
				int nChars = GetTextRange(lStart, lEnd + 1, strDeleted);	// Delete without selection is the same as delete with selecting the next char
			}
			else if (nChar == VK_BACK)
			{// The char just-before-caret-possition will be deleted:
				int nChars = GetTextRange(lStart - 1 , lStart, strDeleted);
				nCharPosition--;	// Because after pressing "back" the caret moves to the left
			}
		}
	}

	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	if (bIsPasting)
	{// At this point the chars have already been pasted:
		m_bOnEnVscrollDisabled = false;
		UpdateLinesArraySize();
		
		// Take care of breaking/completing "<!--" combination:
		int nCommentStart = FindCommentStartHelper(nCharPosition);
		if (nCommentStart != -1)
			InvalidateColorStates(nCommentStart);
	}
	else if (strDeleted.GetLength())
	{// At this point the chars have already been deleted:
		UpdateLinesArraySize();
		if (strDeleted.FindOneOf(_T("<>\"")) != -1)
		{
			bDeferedToOnChange ? (m_nOnChangeCharPosition = nCharPosition) : InvalidateColorStates(nCharPosition);
		}
		else
		{// Take care of breaking/completing "<!--" "-->" combinations:
			int nCommentStart = FindCommentStartHelper(nCharPosition);
			if (nCommentStart != -1)
				InvalidateColorStates(nCommentStart);
			else
			{
				int nCommentEnd = FindCommentEndHelper(nCharPosition - 1);	// For VK_DELETE and VK_BACK nCharPosition - 1 should be used
				if (nCommentEnd != -1)	// Just broke/completed comment end combination
					InvalidateColorStates(nCommentStart);
			}
		}
	}
}

void CFastHtmlRichEditCtrl::OnSize(UINT nType, int cx, int cy)
{
	TRACE(_T("OnSize \n"));
	CRichEditCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_nLineCount > 1)
	{// m_nLineCount is initialized to 1 in PreSubclassWindow.
	 // However LoadFile->StreamIn also gets here before m_nLineCount had a chance to be updated,
	 // so this if is just a workarround for that
		ColorVisibleLines();
	}
}

/**
 * OnEnVscroll handles all cases of change in the view area of the edit control, with one exception -
 * When clicking the scroll bar mouse itself, EN_VSCROLL isn't send.
 * This handler handles this case.
 */
void CFastHtmlRichEditCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{	
	// TODO: Add your message handler code here and/or call default
	TRACE(_T("OnVScroll nSBCode = %d\n"), nSBCode);
	CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

	// The SB_THUMBTRACK request code occurs as the user drags the scroll box. 
	if (nSBCode == SB_THUMBTRACK /*SB_THUMBPOSITION*/)	// Drag scroll box to specified position
	{
		SetFocus();	// This is important!!! If the user drags/releases the scroll thumb without focus - the thub jumps and flickers!
		ColorVisibleLines();
	}
}

/**
 * OnEnVscroll is received when a keyboard event causes a change in the view area of the edit control,
 * for example, pressing HOME, END, PAGE UP, PAGE DOWN, UP ARROW, or DOWN ARROW.
 * However EN_VSCROLL isn't sent when when clicking/dragging the scroll bar mouse itself, which will be handled by OnVScroll.
 * NOTE: Wh might also get here upon ctrl+v when pasted text scrolls the contents.
 */
void CFastHtmlRichEditCtrl::OnEnVscroll()
{
	TRACE(_T("OnEnVScroll\n"));
	// TODO: Add your control notification handler code here

	if (!m_bOnEnVscrollDisabled)
		ColorVisibleLines();
}

// Activate the background coloring timer if CRichEditCtrl has the focus.
// Otherwise we'll just wait till WM_SETFOCUS
void CFastHtmlRichEditCtrl::StartColoringTimer()
{
	if (m_uiBckgdTimerInterval == 0 || m_nBckgdTimerNumOfLines <= 0)
		return;	// Timer was disabled by a call to SetBckgdColorTimer
	
	m_bBckgdTimerActivated = true;
	SetTimer(TIMER_BACKGROUNDCOLORING, m_uiBckgdTimerInterval, NULL);
}

// CURRENTLY NOT USED
void CFastHtmlRichEditCtrl::StopColoringTimer()
{
	if (m_bBckgdTimerActivated)
	{// Disable current background coloring timer:
		BOOL bRes = KillTimer(TIMER_BACKGROUNDCOLORING);	ASSERT(bRes);
		m_bBckgdTimerActivated = false;
	}
}