#include "StdAfx.h"

namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_QKCONTROL(CListUI)

	CListUI::CListUI()
		: m_pCallback(NULL)
		, m_bScrollSelect(false)
		, m_iCurSel(-1)
		, m_iExpandedItem(-1)
		, m_bMultiSel(false)
		, m_iFirstSel(-1)
		, m_bFixedScrollbar(false)
	{
		m_pList = new CListBodyUI(this);
		m_pHeader = new CListHeaderUI;
		Add(m_pHeader);
		Add(m_pList);

		// 列表配置
		m_ListInfo.nColumns = 0;
		m_ListInfo.nFont = -1;
		m_ListInfo.uTextStyle = DT_VCENTER | DT_SINGLELINE;
		m_ListInfo.dwTextColor = 0xFF000000;
		m_ListInfo.dwBkColor = 0;
		m_ListInfo.bAlternateBk = false;
		m_ListInfo.dwSelectedTextColor = 0xFF000000;
		m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
		m_ListInfo.dwHotTextColor = 0xFF000000;
		m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
		m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
		m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
		m_ListInfo.dwLineColor = 0;
		m_ListInfo.bShowRowLine = false;
		m_ListInfo.bShowColumnLine = false;
		m_ListInfo.bShowHtml = false;
		m_ListInfo.bMultiExpandable = false;
		m_ListInfo.bRSelected = false;
		::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
		//::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));

	}

	LPCTSTR CListUI::GetClass() const
	{
		return _T("ListUI");
	}

	UINT CListUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP;
	}

	LPVOID CListUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_LIST) == 0) return static_cast<CListUI*>(this);
		if (_tcsicmp(pstrName, _T("IList")) == 0) return static_cast<IListUI*>(this);
		if (_tcsicmp(pstrName, _T("IListOwner")) == 0) return static_cast<IListOwnerUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	CControlUI* CListUI::GetItemAt(int iIndex) const
	{
		return m_pList->GetItemAt(iIndex);
	}

	int CListUI::GetItemIndex(CControlUI* pControl) const
	{
		if (pControl->GetInterface(_T("ListHeader")) != NULL) return CVerticalLayoutUI::GetItemIndex(pControl);
		// We also need to recognize header sub-items
		if (_tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL) return m_pHeader->GetItemIndex(pControl);

		return m_pList->GetItemIndex(pControl);
	}

	bool CListUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		if (pControl->GetInterface(_T("ListHeader")) != NULL) return CVerticalLayoutUI::SetItemIndex(pControl, iIndex);
		// We also need to recognize header sub-items
		if (_tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL) return m_pHeader->SetItemIndex(pControl, iIndex);

		int iOrginIndex = m_pList->GetItemIndex(pControl);
		if (iOrginIndex == -1) return false;
		if (iOrginIndex == iIndex) return true;

		IListItemUI* pSelectedListItem = NULL;
		if (m_iCurSel >= 0) pSelectedListItem =
			static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(_T("ListItem")));
		if (!m_pList->SetItemIndex(pControl, iIndex)) return false;
		int iMinIndex = min(iOrginIndex, iIndex);
		int iMaxIndex = max(iOrginIndex, iIndex);
		for (int i = iMinIndex; i < iMaxIndex + 1; ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if (pListItem != NULL) {
				pListItem->SetIndex(i);
			}
		}
		if (m_iCurSel >= 0 && pSelectedListItem != NULL) m_iCurSel = pSelectedListItem->GetIndex();
		return true;
	}

	int CListUI::GetCount() const
	{
		return m_pList->GetCount();
	}

	bool CListUI::Add(CControlUI* pControl)
	{
		// Override the Add() method so we can add items specifically to
		// the intended widgets. Headers are assumed to be
		// answer the correct interface so we can add multiple list headers.
		if (pControl->GetInterface(_T("ListHeader")) != NULL) {
			if (m_pHeader != pControl && m_pHeader->GetCount() == 0) {
				CVerticalLayoutUI::Remove(m_pHeader);
				m_pHeader = static_cast<CListHeaderUI*>(pControl);
			}
			m_ListInfo.nColumns = m_pHeader->GetCount();
			m_ListInfo.headerView = m_pHeader;
			return CVerticalLayoutUI::AddAt(pControl, 0);
		}
		// We also need to recognize header sub-items
		if (_tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL) {
			bool ret = m_pHeader->Add(pControl);
			m_ListInfo.nColumns = m_pHeader->GetCount();
			return ret;
		}
		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if (pListItem != NULL) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(GetCount());
			return m_pList->Add(pControl);
		}
		return CVerticalLayoutUI::Add(pControl);
	}

	bool CListUI::AddAt(CControlUI* pControl, int iIndex)
	{
		// Override the AddAt() method so we can add items specifically to
		// the intended widgets. Headers and are assumed to be
		// answer the correct interface so we can add multiple list headers.
		if (pControl->GetInterface(_T("ListHeader")) != NULL) {
			if (m_pHeader != pControl && m_pHeader->GetCount() == 0) {
				CVerticalLayoutUI::Remove(m_pHeader);
				m_pHeader = static_cast<CListHeaderUI*>(pControl);
			}
			m_ListInfo.nColumns = m_pHeader->GetCount();
			return CVerticalLayoutUI::AddAt(pControl, 0);
		}
		// We also need to recognize header sub-items
		if (_tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL) {
			bool ret = m_pHeader->AddAt(pControl, iIndex);
			m_ListInfo.nColumns = m_pHeader->GetCount();
			return ret;
		}
		if (!m_pList->AddAt(pControl, iIndex)) return false;

		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if (pListItem != NULL) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
		}

		for (int i = iIndex + 1; i < m_pList->GetCount(); ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if (pListItem != NULL) {
				pListItem->SetIndex(i);
			}
		}
		if (m_iCurSel >= iIndex) m_iCurSel += 1;
		return true;
	}

	bool CListUI::Remove(CControlUI* pControl)
	{
		if (pControl->GetInterface(_T("ListHeader")) != NULL) return CVerticalLayoutUI::Remove(pControl);
		// We also need to recognize header sub-items
		if (_tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL) return m_pHeader->Remove(pControl);

		int iIndex = m_pList->GetItemIndex(pControl);
		if (iIndex == -1) return false;

		if (!m_pList->RemoveAt(iIndex)) return false;

		for (int i = iIndex; i < m_pList->GetCount(); ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if (pListItem != NULL) {
				pListItem->SetIndex(i);
			}
		}

		if (iIndex == m_iCurSel && m_iCurSel >= 0) {
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if (iIndex < m_iCurSel) m_iCurSel -= 1;
		return true;
	}

	bool CListUI::RemoveAt(int iIndex)
	{
		if (!m_pList->RemoveAt(iIndex)) return false;

		for (int i = iIndex; i < m_pList->GetCount(); ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if (pListItem != NULL) pListItem->SetIndex(i);
		}

		if (iIndex == m_iCurSel && m_iCurSel >= 0) {
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if (iIndex < m_iCurSel) m_iCurSel -= 1;
		return true;
	}

	void CListUI::RemoveAll()
	{
		m_iCurSel = -1;
		m_iExpandedItem = -1;
		m_aSelItems.Empty();
		m_pList->RemoveAll();
	}

	void CListUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CVerticalLayoutUI::SetPos(rc, bNeedInvalidate);

		if (m_pHeader == NULL) return;
		// Determine general list information and the size of header columns
		m_ListInfo.nColumns = m_pHeader->GetCount();
		// The header/columns may or may not be visible at runtime. In either case
		// we should determine the correct dimensions...

		if (!m_pHeader->IsVisible()) {
			//for (int it = 0; it < m_pHeader->GetCount(); it++) {
			//	static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
			//}
			m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), bNeedInvalidate);
		}

		//for (int i = 0; i < m_ListInfo.nColumns; i++) {
		//	CControlUI* pControl = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
		//	if (!pControl->IsVisible()) continue;
		//	if (pControl->IsFloat()) continue;
		//	RECT rcPos = pControl->GetPos();
		//	m_ListInfo.rcColumn[i] = pControl->GetPos();
		//}

		if (!m_pHeader->IsVisible()) {
			//for (int it = 0; it < m_pHeader->GetCount(); it++) {
			//	static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
			//}
		}
		m_pList->SetPos(m_pList->GetPos(), bNeedInvalidate);
	}

	void CListUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CVerticalLayoutUI::Move(szOffset, bNeedInvalidate);
		if (!m_pHeader->IsVisible()) m_pHeader->Move(szOffset, false);
	}

	int CListUI::GetMinSelItemIndex()
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;
		int min = (int)m_aSelItems.GetAt(0);
		int index;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			index = (int)m_aSelItems.GetAt(i);
			if (min > index)
				min = index;
		}
		return min;
	}

	int CListUI::GetMaxSelItemIndex()
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;
		int max = (int)m_aSelItems.GetAt(0);
		int index;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			index = (int)m_aSelItems.GetAt(i);
			if (max < index)
				max = index;
		}
		return max;
	}

	void CListUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (_parent != NULL) _parent->DoEvent(event);
			else CVerticalLayoutUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused_YES;
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused_NO;
			return;
		}

		switch (event.Type) {
		case UIEVENT_KEYDOWN:
			switch (event.chKey) {
			case VK_UP:
				{
					if (m_aSelItems.GetSize() > 0) {
						int index = GetMinSelItemIndex() - 1;
						UnSelectAllItems();
						index > 0 ? SelectItem(index, true) : SelectItem(0, true);
					}
				}
				return;
			case VK_DOWN:
				{
					if (m_aSelItems.GetSize() > 0) {
						int index = GetMaxSelItemIndex() + 1;
						UnSelectAllItems();
						index + 1 > m_pList->GetCount() ? SelectItem(GetCount() - 1, true) : SelectItem(index, true);
					}
				}
				return;
			case VK_PRIOR:
				PageUp();
				return;
			case VK_NEXT:
				PageDown();
				return;
			case VK_HOME:
				SelectItem(FindSelectable(0, false), true);
				return;
			case VK_END:
				SelectItem(FindSelectable(GetCount() - 1, true), true);
				return;
			case VK_RETURN:
				if (m_iCurSel != -1) GetItemAt(m_iCurSel)->Activate();
				return;
			case 0x41:// Ctrl+A 
				{
					//全选
					if (IsMultiSelect() && (GetKeyState(VK_CONTROL) & 0x8000)) {
						SelectAllItems();
					}
					return;
				}
			}
			break;
		case UIEVENT_SCROLLWHEEL:
			{
				if ((short)HIWORD(event.wParam)<0)
				{
					if (m_bScrollSelect && !IsMultiSelect()) SelectItem(FindSelectable(m_iCurSel + 1, true), true);
					else LineDown();
				}
				else
				{
					if (m_bScrollSelect && !IsMultiSelect()) SelectItem(FindSelectable(m_iCurSel - 1, false), true);
					else LineUp();
				}
			}
			break;
		}
		CVerticalLayoutUI::DoEvent(event);
	}

	bool CListUI::IsFixedScrollbar()
	{
		return m_bFixedScrollbar;
	}

	void CListUI::SetFixedScrollbar(bool bFixed)
	{
		m_bFixedScrollbar = bFixed;
		Invalidate();
	}

	CListHeaderUI* CListUI::GetHeader() const
	{
		return m_pHeader;
	}

	CContainerUI* CListUI::GetList() const
	{
		return m_pList;
	}

	bool CListUI::GetScrollSelect()
	{
		return m_bScrollSelect;
	}

	void CListUI::SetScrollSelect(bool bScrollSelect)
	{
		m_bScrollSelect = bScrollSelect;
	}

	int CListUI::GetCurSelActivate() const
	{
		return m_iCurSelActivate;
	}

	bool CListUI::SelectItemActivate(int iIndex)
	{
		if (!SelectItem(iIndex, true)) {
			return false;
		}

		m_iCurSelActivate = iIndex;
		return true;
	}

	int CListUI::GetCurSel() const
	{
		if (m_aSelItems.GetSize() <= 0) {
			return -1;
		}
		else {
			return (int)m_aSelItems.GetAt(0);
		}

		return -1;
	}

	bool CListUI::SelectItem(int iIndex, bool bTakeFocus)
	{
		return true;
	}

	bool CListUI::SelectMultiItem(int iIndex, bool bTakeFocus)
	{
		return true;
	}

	void CListUI::SetMultiSelect(bool bMultiSel)
	{
		m_bMultiSel = bMultiSel;
		if (!bMultiSel) UnSelectAllItems();
	}

	bool CListUI::IsMultiSelect() const
	{
		return m_bMultiSel;
	}

	bool CListUI::UnSelectItem(int iIndex, bool bOthers)
	{
		return true;
	}

	void CListUI::SelectAllItems()
	{
	}

	void CListUI::UnSelectAllItems()
	{
	}

	int CListUI::GetSelectItemCount() const
	{
		return m_aSelItems.GetSize();
	}

	int CListUI::GetNextSelItem(int nItem) const
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;

		if (nItem < 0) {
			return (int)m_aSelItems.GetAt(0);
		}
		int aIndex = m_aSelItems.Find((LPVOID)nItem);
		if (aIndex < 0) return -1;
		if (aIndex + 1 > m_aSelItems.GetSize() - 1)
			return -1;
		return (int)m_aSelItems.GetAt(aIndex + 1);
	}

	UINT CListUI::GetListType()
	{
		return LT_LIST;
	}

	LRESULT CListUI::GetAttribute(LPCTSTR pstrName, LPARAM lParam, WPARAM wParam)
	{
		if( _tcsicmp(pstrName, _T("ListInfo")) == 0 ) {
			return (LRESULT)&m_ListInfo;
		}
		return 0;
	}

	bool CListUI::IsDelayedDestroy() const
	{
		return m_pList->IsDelayedDestroy();
	}

	void CListUI::SetDelayedDestroy(bool bDelayed)
	{
		m_pList->SetDelayedDestroy(bDelayed);
	}

	int CListUI::GetChildPadding() const
	{
		return m_pList->GetChildPadding();
	}

	void CListUI::SetChildPadding(int iPadding)
	{
		m_pList->SetChildPadding(iPadding);
	}

	void CListUI::SetItemFont(int index)
	{
		m_ListInfo.nFont = index;
		NeedUpdate();
	}

	void CListUI::SetItemTextStyle(UINT uStyle)
	{
		m_ListInfo.uTextStyle = uStyle;
		NeedUpdate();
	}

	void CListUI::SetItemTextPadding(RECT rc)
	{
		m_ListInfo.rcTextPadding = rc;
		NeedUpdate();
	}

	RECT CListUI::GetItemTextPadding() const
	{
		RECT rect = m_ListInfo.rcTextPadding;
		GetManager()->GetDPIObj()->Scale(&rect);
		return rect;
	}

	void CListUI::SetItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetItemBkImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sBkImage = pStrImage;
		Invalidate();
	}

	void CListUI::SetAlternateBk(bool bAlternateBk)
	{
		m_ListInfo.bAlternateBk = bAlternateBk;
		Invalidate();
	}

	DWORD CListUI::GetItemTextColor() const
	{
		return m_ListInfo.dwTextColor;
	}

	DWORD CListUI::GetItemBkColor() const
	{
		return m_ListInfo.dwBkColor;
	}

	LPCTSTR CListUI::GetItemBkImage() const
	{
		return m_ListInfo.sBkImage;
	}

	bool CListUI::IsAlternateBk() const
	{
		return m_ListInfo.bAlternateBk;
	}

	void CListUI::SetSelectedItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwSelectedTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetSelectedItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwSelectedBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetSelectedItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sSelectedImage = pStrImage;
		Invalidate();
	}

	DWORD CListUI::GetSelectedItemTextColor() const
	{
		return m_ListInfo.dwSelectedTextColor;
	}

	DWORD CListUI::GetSelectedItemBkColor() const
	{
		return m_ListInfo.dwSelectedBkColor;
	}

	LPCTSTR CListUI::GetSelectedItemImage() const
	{
		return m_ListInfo.sSelectedImage;
	}

	void CListUI::SetHotItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwHotTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetHotItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwHotBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetHotItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sHotImage = pStrImage;
		Invalidate();
	}

	DWORD CListUI::GetHotItemTextColor() const
	{
		return m_ListInfo.dwHotTextColor;
	}
	DWORD CListUI::GetHotItemBkColor() const
	{
		return m_ListInfo.dwHotBkColor;
	}

	LPCTSTR CListUI::GetHotItemImage() const
	{
		return m_ListInfo.sHotImage;
	}

	void CListUI::SetDisabledItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetDisabledItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwDisabledBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetDisabledItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sDisabledImage = pStrImage;
		Invalidate();
	}

	DWORD CListUI::GetDisabledItemTextColor() const
	{
		return m_ListInfo.dwDisabledTextColor;
	}

	DWORD CListUI::GetDisabledItemBkColor() const
	{
		return m_ListInfo.dwDisabledBkColor;
	}

	LPCTSTR CListUI::GetDisabledItemImage() const
	{
		return m_ListInfo.sDisabledImage;
	}

	DWORD CListUI::GetItemLineColor() const
	{
		return m_ListInfo.dwLineColor;
	}

	void CListUI::SetItemLineColor(DWORD dwLineColor)
	{
		m_ListInfo.dwLineColor = dwLineColor;
		Invalidate();
	}
	void CListUI::SetItemShowRowLine(bool bShowLine)
	{
		m_ListInfo.bShowRowLine = bShowLine;
		Invalidate();
	}
	void CListUI::SetItemShowColumnLine(bool bShowLine)
	{
		m_ListInfo.bShowColumnLine = bShowLine;
		Invalidate();
	}
	bool CListUI::IsItemShowHtml()
	{
		return m_ListInfo.bShowHtml;
	}

	void CListUI::SetItemShowHtml(bool bShowHtml)
	{
		if (m_ListInfo.bShowHtml == bShowHtml) return;

		m_ListInfo.bShowHtml = bShowHtml;
		NeedUpdate();
	}

	bool CListUI::IsItemRSelected()
	{
		return m_ListInfo.bRSelected;
	}

	void CListUI::SetItemRSelected(bool bSelected)
	{
		if (m_ListInfo.bRSelected == bSelected) return;

		m_ListInfo.bRSelected = bSelected;
		NeedUpdate();
	}

	void CListUI::SetMultiExpanding(bool bMultiExpandable)
	{
		m_ListInfo.bMultiExpandable = bMultiExpandable;
	}

	bool CListUI::ExpandItem(int iIndex, bool bExpand /*= true*/)
	{
		if (m_iExpandedItem >= 0 && !m_ListInfo.bMultiExpandable) {
			CControlUI* pControl = GetItemAt(m_iExpandedItem);
			if (pControl != NULL) {
				IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if (pItem != NULL) pItem->Expand(false);
			}
			m_iExpandedItem = -1;
		}
		if (bExpand) {
			CControlUI* pControl = GetItemAt(iIndex);
			if (pControl == NULL) return false;
			if (!pControl->IsVisible()) return false;
			IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if (pItem == NULL) return false;
			m_iExpandedItem = iIndex;
			if (!pItem->Expand(true)) {
				m_iExpandedItem = -1;
				return false;
			}
		}
		NeedUpdate();
		return true;
	}

	int CListUI::GetExpandedItem() const
	{
		return m_iExpandedItem;
	}

	void CListUI::EnsureVisible(int iIndex)
	{
		if (m_iCurSel < 0) return;
		RECT rcItem = m_pList->GetItemAt(iIndex)->GetPos();
		RECT rcList = m_pList->GetPos();
		m_pList->ApplyInsetToRect(rcList);

		CScrollBarUI* pHorizontalScrollBar = m_pList->GetHorizontalScrollBar();
		if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

		int iPos = m_pList->GetScrollPos().cy;
		if (rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom) return;
		int dx = 0;
		if (rcItem.top < rcList.top) dx = rcItem.top - rcList.top;
		if (rcItem.bottom > rcList.bottom) dx = rcItem.bottom - rcList.bottom;
		Scroll(0, dx);
	}

	void CListUI::Scroll(int dx, int dy)
	{
		if (dx == 0 && dy == 0) return;
		SIZE sz = m_pList->GetScrollPos();
		m_pList->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
	}

	void CListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("header")) == 0) GetHeader()->SetVisible(_tcsicmp(pstrValue, _T("hidden")) != 0);
		else if (_tcsicmp(pstrName, _T("headerbkimage")) == 0) GetHeader()->SetBkImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("scrollselect")) == 0) SetScrollSelect(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("fixedscrollbar")) == 0) SetFixedScrollbar(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("multiexpanding")) == 0) SetMultiExpanding(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("itemfont")) == 0) m_ListInfo.nFont = _ttoi(pstrValue);
		else if (_tcsicmp(pstrName, _T("itemalign")) == 0) {
			if (_tcsstr(pstrValue, _T("left")) != NULL) {
				m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_LEFT;
			}
			if (_tcsstr(pstrValue, _T("center")) != NULL) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_CENTER;
			}
			if (_tcsstr(pstrValue, _T("right")) != NULL) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_ListInfo.uTextStyle |= DT_RIGHT;
			}
		}
		else if (_tcsicmp(pstrName, _T("itemvalign")) == 0) {
			if (_tcsstr(pstrValue, _T("top")) != NULL) {
				m_ListInfo.uTextStyle &= ~(DT_VCENTER | DT_BOTTOM);
				m_ListInfo.uTextStyle |= DT_TOP;
			}
			if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
				m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM | DT_WORDBREAK);
				m_ListInfo.uTextStyle |= DT_VCENTER | DT_SINGLELINE;
			}
			if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
				m_ListInfo.uTextStyle &= ~(DT_TOP | DT_VCENTER);
				m_ListInfo.uTextStyle |= DT_BOTTOM;
			}
		}
		else if (_tcsicmp(pstrName, _T("itemendellipsis")) == 0) {
			if (_tcsicmp(pstrValue, _T("true")) == 0) m_ListInfo.uTextStyle |= DT_END_ELLIPSIS;
			else m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS;
		}
		else if (_tcsicmp(pstrName, _T("itemtextpadding")) == 0) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetItemTextPadding(rcTextPadding);
		}
		else if (_tcsicmp(pstrName, _T("itemtextcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetItemTextColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itembkcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetItemBkColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itembkimage")) == 0) SetItemBkImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("itemaltbk")) == 0) SetAlternateBk(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("itemselectedtextcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetSelectedItemTextColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itemselectedbkcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetSelectedItemBkColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itemselectedimage")) == 0) SetSelectedItemImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("itemhottextcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetHotItemTextColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itemhotbkcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetHotItemBkColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itemhotimage")) == 0) SetHotItemImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("itemdisabledtextcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetDisabledItemTextColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itemdisabledbkcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetDisabledItemBkColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itemdisabledimage")) == 0) SetDisabledItemImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("itemlinecolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetItemLineColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("itemshowrowline")) == 0) SetItemShowRowLine(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("itemshowcolumnline")) == 0) SetItemShowColumnLine(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("itemshowhtml")) == 0) SetItemShowHtml(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("multiselect")) == 0) SetMultiSelect(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("itemrselected")) == 0) SetItemRSelected(_tcscmp(pstrValue, _T("true")) == 0);
		else CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	IListCallbackUI* CListUI::GetTextCallback() const
	{
		return m_pCallback;
	}

	void CListUI::SetTextCallback(IListCallbackUI* pCallback)
	{
		m_pCallback = pCallback;
	}

	SIZE CListUI::GetScrollPos() const
	{
		return m_pList->GetScrollPos();
	}

	SIZE CListUI::GetScrollRange() const
	{
		return m_pList->GetScrollRange();
	}

	bool CListUI::SetScrollPos(SIZE szPos, bool bMsg)
	{
		return m_pList->SetScrollPos(szPos, bMsg);
	}

	bool CListUI::LineUp()
	{
		return m_pList->LineUp();
	}

	bool CListUI::LineDown()
	{
		return m_pList->LineDown();
	}

	void CListUI::PageUp()
	{
		m_pList->PageUp();
	}

	void CListUI::PageDown()
	{
		m_pList->PageDown();
	}

	void CListUI::HomeUp()
	{
		m_pList->HomeUp();
	}

	void CListUI::EndDown()
	{
		m_pList->EndDown();
	}

	bool CListUI::LineLeft()
	{
		return m_pList->LineLeft();
	}

	bool CListUI::LineRight()
	{
		return m_pList->LineRight();
	}

	void CListUI::PageLeft()
	{
		m_pList->PageLeft();
	}

	void CListUI::PageRight()
	{
		m_pList->PageRight();
	}

	void CListUI::HomeLeft()
	{
		m_pList->HomeLeft();
	}

	void CListUI::EndRight()
	{
		m_pList->EndRight();
	}

	void CListUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
	}

	CScrollBarUI* CListUI::GetVerticalScrollBar() const
	{
		return m_pList->GetVerticalScrollBar();
	}

	CScrollBarUI* CListUI::GetHorizontalScrollBar() const
	{
		return m_pList->GetHorizontalScrollBar();
	}

	BOOL CListUI::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
	{
		if (!m_pList)
			return FALSE;
		return m_pList->SortItems(pfnCompare, dwData);
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListBodyUI::CListBodyUI(CListUI* pOwner) : m_pOwner(pOwner)
	{
		ASSERT(m_pOwner);
	}

	BOOL CListBodyUI::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
	{
		//if (!pfnCompare)
		//	return FALSE;
		//m_pCompareFunc = pfnCompare;
		//m_compareData = dwData;
		//CControlUI **pData = (CControlUI **)m_items.GetData();
		//qsort_s(m_items.GetData(), m_items.GetSize(), sizeof(CControlUI*), CListBodyUI::ItemComareFunc, this);
		//IListItemUI *pItem = NULL;
		//for (int i = 0; i < m_items.GetSize(); ++i)
		//{
		//	pItem = (IListItemUI*)(static_cast<CControlUI*>(m_items[i])->GetInterface(TEXT("ListItem")));
		//	if (pItem)
		//	{
		//		pItem->SetIndex(i);
		//		pItem->Select(false);
		//	}
		//}
		//m_pOwner->SelectItem(-1);
		//if (_manager)
		//{
		//	SetPos(GetPos());
		//	Invalidate();
		//}

		return TRUE;
	}

	int __cdecl CListBodyUI::ItemComareFunc(void *pvlocale, const void *item1, const void *item2)
	{
		CListBodyUI *pThis = (CListBodyUI*)pvlocale;
		if (!pThis || !item1 || !item2)
			return 0;
		return pThis->ItemComareFunc(item1, item2);
	}

	int __cdecl CListBodyUI::ItemComareFunc(const void *item1, const void *item2)
	{
		CControlUI *pControl1 = *(CControlUI**)item1;
		CControlUI *pControl2 = *(CControlUI**)item2;
		return m_pCompareFunc((UINT_PTR)pControl1, (UINT_PTR)pControl2, m_compareData);
	}

	int CListBodyUI::GetScrollStepSize() const
	{
		if (m_pOwner != NULL) return m_pOwner->GetScrollStepSize();

		return CVerticalLayoutUI::GetScrollStepSize();
	}

	bool CListBodyUI::SetScrollPos(SIZE szPos, bool bMsg)
	{
		int cx = 0;
		int cy = 0;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		RECT rcPos;
		for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) continue;

			rcPos = pControl->GetPos();
			rcPos.left -= cx;
			rcPos.right -= cx;
			rcPos.top -= cy;
			rcPos.bottom -= cy;
			pControl->SetPos(rcPos, true);
		}

		Invalidate();
		if (m_pOwner) {
			TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
			if (!pInfo) return cx||cy;
			CListHeaderUI* pHeader = pInfo->headerView;
			if (pHeader == NULL) return cx||cy;
			pInfo->nColumns = pHeader->GetCount();

			if (!pHeader->IsVisible()) {
				//for (int it = 0; it < pHeader->GetCount(); it++) {
				//	static_cast<CControlUI*>(pHeader->GetItemAt(it))->SetInternVisible(true);
				//}
			}
			for (int i = 0; i < pInfo->nColumns; i++) {
				CControlUI* pControl = static_cast<CControlUI*>(pHeader->GetItemAt(i));
				if (!pControl->IsVisible()) continue;
				if (pControl->IsFloat()) continue;

				RECT rcPos = pControl->GetPos();
				rcPos.left -= cx;
				rcPos.right -= cx;
				pControl->SetPos(rcPos);
				//pInfo->rcColumn[i] = pControl->GetPos();
			}
			if (!pHeader->IsVisible()) {
				//for (int it = 0; it < pHeader->GetCount(); it++) {
				//	static_cast<CControlUI*>(pHeader->GetItemAt(it))->SetInternVisible(false);
				//}
			}
		}

		return cx||cy;
	}

	void CListBodyUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if (m_pOwner->IsFixedScrollbar() && m_pVerticalScrollBar) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		else if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

		int cxNeeded = 0;
		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for (int it1 = 0; it1 < m_items.GetSize(); it1++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if (sz.cy == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;

			RECT rcPadding = pControl->GetPadding();
			sz.cx = MAX(sz.cx, 0);
			if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
			if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
			cxNeeded = MAX(cxNeeded, sz.cx);
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;

		if (m_pOwner) {
			CListHeaderUI* pHeader = m_pOwner->GetHeader();
			if (pHeader != NULL && pHeader->GetCount() > 0) {
				cxNeeded = MAX(0, pHeader->EstimateSize(CDuiSize(rc.right - rc.left, rc.bottom - rc.top)).cx);
				if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
				{
					int nOffset = m_pHorizontalScrollBar->GetScrollPos();
					RECT rcHeader = pHeader->GetPos();
					rcHeader.left = rc.left - nOffset;
					pHeader->SetPos(rcHeader);
				}
			}
		}

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		if (nAdjustables > 0) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
			iPosY -= m_pVerticalScrollBar->GetScrollPos();
		}
		int iPosX = rc.left;
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		}
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) {
				SetFloatPos(it2);
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
			szRemaining.cy -= rcPadding.top;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if (sz.cy == 0) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if (iAdjustable == nAdjustables) {
					sz.cy = MAX(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				}
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = MAX(cxNeeded, szAvailable.cx - rcPadding.left - rcPadding.right);

			if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
			if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();

			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
			pControl->SetPos(rcCtrl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

		if (m_pHorizontalScrollBar != NULL) {
			if (cxNeeded > rc.right - rc.left) {
				if (m_pHorizontalScrollBar->IsVisible()) {
					m_pHorizontalScrollBar->SetScrollRange(cxNeeded - (rc.right - rc.left));
				}
				else {
					m_pHorizontalScrollBar->SetVisible(true);
					m_pHorizontalScrollBar->SetScrollRange(cxNeeded - (rc.right - rc.left));
					m_pHorizontalScrollBar->SetScrollPos(0);
					rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
				}
			}
			else {
				if (m_pHorizontalScrollBar->IsVisible()) {
					m_pHorizontalScrollBar->SetVisible(false);
					m_pHorizontalScrollBar->SetScrollRange(0);
					m_pHorizontalScrollBar->SetScrollPos(0);
					rc.bottom += m_pHorizontalScrollBar->GetFixedHeight();
				}
			}
		}
		UINT uListType = m_pOwner->GetListType();
		if (uListType == LT_LIST) {
			// 计算横向尺寸
			int nItemCount = m_items.GetSize();
			if (nItemCount > 0)
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[0]);
				int nFixedWidth = pControl->GetFixedWidth();
				if (nFixedWidth > 0)
				{
					int nRank = (rc.right - rc.left) / nFixedWidth;
					if (nRank > 0)
					{
						cyNeeded = ((nItemCount - 1) / nRank + 1) * pControl->GetFixedHeight();
					}
				}
			}
		}
		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void CListBodyUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pOwner != NULL) m_pOwner->DoEvent(event);
			else CVerticalLayoutUI::DoEvent(event);
			return;
		}

		CVerticalLayoutUI::DoEvent(event);
	}
} // namespace DuiLib
