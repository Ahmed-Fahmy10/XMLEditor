// SampleView.cpp : implementation of the CSampleView class
//

#include "stdafx.h"
#include "XMLEditor.h"

#include "SampleDoc.h"
#include "SampleView.h"

#include "json/xml2json.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampleView


IMPLEMENT_DYNCREATE(CSampleView, CCrystalEditView)

BEGIN_MESSAGE_MAP(CSampleView, CCrystalEditView)
	//{{AFX_MSG_MAP(CSampleView)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCrystalEditView::OnFilePrintPreview)
	ON_WM_CLOSE()
	ON_COMMAND(ID_EDIT_FIXUP, CSampleView::OnEditFixup)
	ON_COMMAND(ID_EDIT_FORMAT, CSampleView::OnEditMinify)
	ON_COMMAND(ID_EDIT_JSON, CSampleView::OnEditJson)
	ON_UPDATE_COMMAND_UI(ID_EDIT_JSON, CSampleView::OnUpdateEditJson)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIXUP, CSampleView::OnUpdateEditFixup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FORMAT, CSampleView::OnUpdateEditMinify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleView construction/destruction

CSampleView::CSampleView()
{
	// TODO: add construction code here

}

CSampleView::~CSampleView()
{

}

BOOL CSampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CCrystalEditView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CSampleView diagnostics

#ifdef _DEBUG
void CSampleView::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void CSampleView::Dump(CDumpContext& dc) const
{
	CCrystalEditView::Dump(dc);
}

CSampleDoc* CSampleView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampleDoc)));
	return (CSampleDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSampleView message handlers

CCrystalTextBuffer *CSampleView::LocateTextBuffer()
{
	return &GetDocument()->m_xTextBuffer;
}

void CSampleView::OnInitialUpdate() 
{
	CCrystalEditView::OnInitialUpdate();

	CSampleDoc* pDoc = GetDocument();
	SetFont(pDoc->m_lf);
	
}

void CSampleView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	AfxMessageBox("Build your own context menu!");
}


void CSampleView::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CCrystalEditView::OnClose();
}


void CSampleView::OnEditFixup()
{
	CSampleDoc* pDoc = GetDocument();
	pDoc->LoadXMLDom();
	CString strMin = pDoc->m_xmlDoc.GetXML();
	int nLines = pDoc->m_xTextBuffer.GetLineCount() - 1;
	pDoc->m_xTextBuffer.DeleteText(this, 0, 0, nLines, pDoc->m_xTextBuffer.GetLineLength(nLines));
	int nLastLine = 0, nLastLength = 0;
	pDoc->m_xTextBuffer.InsertText(this, 0, 0, strMin, nLastLine, nLastLength);
}


void CSampleView::OnEditMinify()
{
	CSampleDoc* pDoc=GetDocument();
	pDoc->LoadXMLDom();
	CString strMin= pDoc->m_xmlDoc.GetXMLMinify();
	int nLines = pDoc->m_xTextBuffer.GetLineCount()-1;
	pDoc->m_xTextBuffer.DeleteText(this, 0, 0, nLines, pDoc->m_xTextBuffer.GetLineLength(nLines));
	int nLastLine = 0, nLastLength = 0;
	pDoc->m_xTextBuffer.InsertText(this, 0, 0, strMin, nLastLine, nLastLength);
}


void CSampleView::OnEditJson()
{
	CFile f;

	char strFilter[] = { "JSON Files (*.json)|*.json|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(FALSE, ".json", NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		f.Open(FileDlg.GetFileName(), CFile::modeCreate | CFile::modeWrite);

		CSampleDoc* pDoc = GetDocument();
		pDoc->LoadXMLDom();
		CString strMin = pDoc->m_xmlDoc.GetXMLMinify();

		std::string xml_str(strMin), json_str;

		json_str = json::xml2json(xml_str.c_str());

		f.Write(json_str.data(), json_str.length());
	}
	else
		return;

	f.Close();

	
}


void CSampleView::OnUpdateEditJson(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()-> m_xmlDoc.GetXML() != _T(""));
}


void CSampleView::OnUpdateEditFixup(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()->m_xmlDoc.GetXML() != _T(""));
}


void CSampleView::OnUpdateEditMinify(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetDocument()->m_xmlDoc.GetXML() != _T(""));
}
