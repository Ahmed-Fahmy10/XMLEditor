#if !defined(AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_)
#define AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "cedefs.h"
#include "CCrystalTextView.h"

/////////////////////////////////////////////////////////////////////////////
//	Forward class declarations

class CEditDropTargetImpl;


/////////////////////////////////////////////////////////////////////////////
//	CCrystalEditView view

class CRYSEDIT_CLASS_DECL CCrystalEditView : public CCrystalTextView
{
	DECLARE_DYNCREATE(CCrystalEditView)

private:
	BOOL	m_bOvrMode;
	BOOL	m_bDropPosVisible;
	CPoint	m_ptSavedCaretPos;
	CPoint	m_ptDropPos;
	BOOL	m_bSelectionPushed;
	CPoint	m_ptSavedSelStart, m_ptSavedSelEnd;
	BOOL	m_bAutoIndent;

	//	[JRT]
	BOOL m_bDisableBSAtSOL;								// Disable BS At Start Of Line

	BOOL   DeleteCurrentSelection();

protected:
	CEditDropTargetImpl *m_pDropTarget;
	virtual DROPEFFECT GetDropEffect();
	virtual void OnDropSource(DROPEFFECT de);
	void Paste();
	void Cut();
	virtual void ResetView();

// Attributes
public:
	BOOL GetAutoIndent() const; 
	void SetAutoIndent(BOOL bAutoIndent);

	//	[JRT]
	void SetDisableBSAtSOL(BOOL bDisableBSAtSOL);
	BOOL GetDisableBSAtSOL() const;

// Operations
public:
	CCrystalEditView();
	~CCrystalEditView();

	BOOL GetOverwriteMode() const;
	void SetOverwriteMode(BOOL bOvrMode = TRUE);

	void ShowDropIndicator(const CPoint &point);
	void HideDropIndicator();

	BOOL DoDropText(COleDataObject *pDataObject, const CPoint &ptClient);
	void DoDragScroll(const CPoint &point);

	virtual BOOL QueryEditable();
	virtual void UpdateView(CCrystalTextView *pSource, CUpdateContext *pContext, DWORD dwFlags, int nLineIndex = -1);

	BOOL ReplaceSelection(LPCTSTR pszNewText);

	virtual void OnEditOperation(int nAction, LPCTSTR pszText);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrystalEditView)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CCrystalEditView)
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditDeleteBack();
	afx_msg void OnEditUntab();
	afx_msg void OnEditTab();
	afx_msg void OnEditSwitchOvrmode();
	afx_msg void OnUpdateEditSwitchOvrmode(CCmdUI* pCmdUI);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEditReplace();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	//}}AFX_MSG
	afx_msg void OnUpdateIndicatorCol(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorOvr(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorRead(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#if ! (defined(CE_FROM_DLL) || defined(CE_DLL_BUILD))
#include "CCrystalEditView.inl"
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_)
