#pragma once
#ifndef _UICOMBOBOXEX_H_
#define _UICOMBOBOXEX_H_

#include "../StdAfx.h"

namespace DuiLib
{
	/*********编辑框************/
	class CComboEditWnd;
	/***********下拉列表***************/
	class CComboDownWnd;
	class UILIB_API CComboBoxExUI : public CContainerUI, public IListOwnerUI
	{
		DECLARE_DUICONTROL(CComboBoxExUI)
		friend class CComboEditWnd;
		friend class CComboDownWnd;
	public:
		CComboBoxExUI();
		//条目发生改变信号
		//sigslot::signal2<int,CComboBoxExUI *> SignalItemChange;

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoInit();
		UINT GetControlFlags() const;

		CDuiString GetText() const;
		void SetEnabled(bool bEnable = true);

		CDuiString GetDropBoxAttributeList();
		void SetDropBoxAttributeList(LPCTSTR pstrList);
		SIZE GetDropBoxSize() const;
		void SetDropBoxSize(SIZE szDropBox);

		int GetCurSel() const;  
		bool SelectItem(int iIndex, bool bTakeFocus = false);

		bool SetItemIndex(CControlUI* pControl, int iIndex);
		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();

		bool Activate();

		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		LPCTSTR GetNormalImage() const;
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage() const;
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage() const;
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage() const;
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage() const;
		void SetDisabledImage(LPCTSTR pStrImage);

		TListInfoUI* GetListInfo();
		void SetItemFont(int index);
		void SetItemTextStyle(UINT uStyle);
		RECT GetItemTextPadding() const;
		void SetItemTextPadding(RECT rc);
		DWORD GetItemTextColor() const;
		void SetItemTextColor(DWORD dwTextColor);
		DWORD GetItemBkColor() const;
		void SetItemBkColor(DWORD dwBkColor);
		LPCTSTR GetItemBkImage() const;
		void SetItemBkImage(LPCTSTR pStrImage);
		bool IsAlternateBk() const;
		void SetAlternateBk(bool bAlternateBk);
		DWORD GetSelectedItemTextColor() const;
		void SetSelectedItemTextColor(DWORD dwTextColor);
		DWORD GetSelectedItemBkColor() const;
		void SetSelectedItemBkColor(DWORD dwBkColor);
		LPCTSTR GetSelectedItemImage() const;
		void SetSelectedItemImage(LPCTSTR pStrImage);
		DWORD GetHotItemTextColor() const;
		void SetHotItemTextColor(DWORD dwTextColor);
		DWORD GetHotItemBkColor() const;
		void SetHotItemBkColor(DWORD dwBkColor);
		LPCTSTR GetHotItemImage() const;
		void SetHotItemImage(LPCTSTR pStrImage);
		DWORD GetDisabledItemTextColor() const;
		void SetDisabledItemTextColor(DWORD dwTextColor);
		DWORD GetDisabledItemBkColor() const;
		void SetDisabledItemBkColor(DWORD dwBkColor);
		LPCTSTR GetDisabledItemImage() const;
		void SetDisabledItemImage(LPCTSTR pStrImage);
		DWORD GetItemLineColor() const;
		void SetItemLineColor(DWORD dwLineColor);
		bool IsItemShowHtml();
		void SetItemShowHtml(bool bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoPaint(HDC hDC, const RECT& rcPaint);
		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

		UINT GetListType() { return 0; };
		bool SelectMultiItem(int iIndex, bool bTakeFocus = false) { return false; };
		bool UnSelectItem(int iIndex, bool bOthers = false) { return false; };
	protected:
		CComboEditWnd *p_EditWnd;
		CComboDownWnd* m_pWindow;
		int m_iCurSel;
		RECT m_rcTextPadding;
		CDuiString m_sDropBoxAttributes;
		SIZE m_szDropBox;
		UINT m_uButtonState;

		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sPushedImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;

		TListInfoUI m_ListInfo;
	};
}

#endif // __UICOMBOBOX_H__