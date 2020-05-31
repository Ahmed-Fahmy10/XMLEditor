#ifndef __CCrystalEditView_INL_INCLUDED
#define __CCrystalEditView_INL_INCLUDED

#include "CCrystalEditView.h"

CE_INLINE BOOL CCrystalEditView::GetOverwriteMode() const
{
	return m_bOvrMode;
}

CE_INLINE void CCrystalEditView::SetOverwriteMode(BOOL bOvrMode /*= TRUE*/)
{
	m_bOvrMode = bOvrMode;
}

CE_INLINE BOOL CCrystalEditView::GetDisableBSAtSOL() const
{
	return m_bDisableBSAtSOL;
}

CE_INLINE BOOL CCrystalEditView::GetAutoIndent() const
{
	return m_bAutoIndent;
}

CE_INLINE void CCrystalEditView::SetAutoIndent(BOOL bAutoIndent)
{
	m_bAutoIndent = bAutoIndent;
}

#endif
