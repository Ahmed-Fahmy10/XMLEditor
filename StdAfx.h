// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B1B69ECB_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_)
#define AFX_STDAFX_H__B1B69ECB_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls

#include <afxtempl.h>//
#include <afxpriv.h> 
#include <afxole.h> 

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#define AFX_ZERO_INIT_OBJECT(base_class) \
memset(((base_class*)this)+1, 0, sizeof(*this) - sizeof(class base_class));

#endif // !defined(AFX_STDAFX_H__B1B69ECB_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_)
