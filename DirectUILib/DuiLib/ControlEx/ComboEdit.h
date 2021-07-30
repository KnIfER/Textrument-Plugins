#ifndef __UICOMBOEDIT_H__
#define __UICOMBOEDIT_H__
#pragma once

#include "../StdAfx.h"

using namespace DuiLib;

namespace DuiLib
{
	class CComboBodyUI;
	class CComboEditWnd;
	class UILIB_API CComboEditUI : public CComboUI
	{
		DECLARE_DUICONTROL(CComboEditUI)
	public:
		CComboEditUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CDuiString GetText() const;


		void DoEvent(TEventUI& event);
		int  GetFont();
		void SetFont(int iFont);
		UINT GetTextStyle();
		void SetTextStyle(UINT iTextStyle);
		int  GetMaxChar();
		void SetMaxChar(int iMaxChar);
		DWORD  GetNativeEditBkColor();
		void SetNativeEditBkColor(DWORD dwColor);
		DWORD GetTextColor();
		void SetTextColor(DWORD dwColor);
		bool Add(CControlUI* pControl);
		bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true);
		void PaintText(HDC hDC);
		int GetCurSel() const;
	public:
		CDuiString m_sText;
		CComboEditWnd* m_pEditWnd;

		int m_iMaxChar;
		int m_iFont;
		UINT m_uTextStyle;
		DWORD m_dwNativeEditBkColor;
		DWORD m_dwTextColor;
		DWORD m_dwDisabledTextColor;
	};

}// end Duilib
#endif // __UICOMBOEDIT_H__
