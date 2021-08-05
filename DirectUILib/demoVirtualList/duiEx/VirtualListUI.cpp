#include "stdafx.h"
#include "VirtualListUI.h"


//////////////////////////////////////////////////////////////////////////
// CVirtualListUI
//////////////////////////////////////////////////////////////////////////

CVirtualListUI::CVirtualListUI() :
	m_pDataProvider(NULL),
	m_nOwnerElementHeight(0),
	m_nOwnerItemCount(0),
	m_nOldYScrollPos(0),
	m_bArrangedOnce(false),
	m_bForceArrange(false),
	m_iCurlShowBeginIndex(0),
	m_iCurlShowEndIndex(0)
{
	if (CListUI::m_pList != NULL)
	{
		CListUI::m_pList->SetVisible(false);
	}
// 
	m_pList = new CVirListBodyUI(this);
	CVerticalLayoutUI::Add(m_pList);
}


CVirtualListUI::~CVirtualListUI()
{

}

LPCTSTR CVirtualListUI::GetClass() const
{
	return L"VirtualList";
}

LPVOID CVirtualListUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, _T("VirtualList")) == 0)
	{
		return static_cast<CVirtualListUI*>(this);
	}
	else
	{
		return CListUI::GetInterface(pstrName);
	}
}


void CVirtualListUI::SetItemCount(int nCount)
{
	if (m_pList)
	{
		m_pList->SetItemCount(nCount);
	}
	m_nOwnerItemCount = nCount;
}

void CVirtualListUI::SetDataProvider(IVirtualDataProvider * pDataProvider)
{
	if (m_pList)
	{
		m_pList->SetDataProvider(pDataProvider);
	}
	m_pDataProvider = pDataProvider;
}

IVirtualDataProvider * CVirtualListUI::GetDataProvider()
{
	return m_pDataProvider;
}

void CVirtualListUI::SetElementHeight(int nHeight)
{
	if (m_pList)
	{
		m_pList->SetElementHeight(nHeight);
	}

	m_nOwnerElementHeight = nHeight;
}

void CVirtualListUI::InitElement(int nMaxItemCount)
{
	if (m_pList)
	{
		m_pList->InitElement(nMaxItemCount);
		return;
	}
	ASSERT(m_pDataProvider);
	ASSERT(m_nOwnerElementHeight);
	m_nOwnerItemCount = nMaxItemCount;

	int nCount = GetElementCount();
	if (nCount > nMaxItemCount)
		nCount = nMaxItemCount;

	for (int i = 0; i < nCount; i++) {
		CControlUI *pControl = CreateElement();
		//this->Add(pControl);
		m_pList->Add(pControl);
		if (pControl->GetManager() == NULL)
		{
			OutputDebugString(L"pControl->GetManager() == NULL");
		}
		FillElement(pControl, i);
	}
}

void CVirtualListUI::RemoveAll()
{
	//__super::RemoveAll();
	if (m_pList)
	{
		m_pList->RemoveAll();
	}
	else
	{
		__super::RemoveAll();
	}
	if (m_pVerticalScrollBar)
		m_pVerticalScrollBar->SetScrollPos(0);

	delete m_pDataProvider;
	m_pDataProvider = NULL;
	m_nOldYScrollPos = 0;
	m_bArrangedOnce = false;
	m_bForceArrange = false;
}

void CVirtualListUI::SetForceArrange(bool bForce)
{
	m_bForceArrange = bForce;
}

void CVirtualListUI::GetDisplayCollection(std::vector<int>& collection)
{
	collection.clear();

	if (GetCount() == 0)
		return;

	//	RECT rcThis = this->GetPos(false);
	RECT rcThis = this->GetPos();

	int min = GetScrollPos().cy / m_nOwnerElementHeight;
	int max = min + ((rcThis.bottom - rcThis.top) / m_nOwnerElementHeight);
	int nCount = GetElementCount();
	if (max >= nCount)
		max = nCount - 1;

	for (auto i = min; i <= max; i++)
		collection.push_back(i);
}

std::vector<int> CVirtualListUI::GetSelectIndex()
{
	return m_pList->GetSelectIndex();
}

bool CVirtualListUI::Remove(CControlUI* pControl)
{
	if (m_pList)
	{
		return m_pList->Remove(pControl);
	}
	else
	{
		return CListUI::Remove(pControl);
	}
}

bool CVirtualListUI::RemoveAt(int iIndex)
{
	if (m_pList)
	{
		return m_pList->RemoveAt(iIndex);
	}
	else
	{
		return CListUI::RemoveAt(iIndex);
	}
}

void CVirtualListUI::SetPos(RECT rc, bool bNeedInvalidate)
{

	CVerticalLayoutUI::SetPos(rc, bNeedInvalidate);

	if (m_pHeader == NULL) return;
	// Determine general list information and the size of header columns
	m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
	// The header/columns may or may not be visible at runtime. In either case
	// we should determine the correct dimensions...

	if (!m_pHeader->IsVisible()) {
		for (int it = 0; it < m_pHeader->GetCount(); it++) {
			static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
		}
		m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), bNeedInvalidate);
	}

	for (int i = 0; i < m_ListInfo.nColumns; i++) {
		CControlUI* pControl = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
		if (!pControl->IsVisible()) continue;
		if (pControl->IsFloat()) continue;
		RECT rcPos = pControl->GetPos();
		m_ListInfo.rcColumn[i] = pControl->GetPos();
	}
	if (!m_pHeader->IsVisible()) {
		for (int it = 0; it < m_pHeader->GetCount(); it++) {
			static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
		}
	}
	m_pList->SetPos(m_pList->GetPos(), bNeedInvalidate);
}


CControlUI* CVirtualListUI::CreateElement()
{
	if (m_pDataProvider)
		return m_pDataProvider->CreateElement();

	return nullptr;
}

void CVirtualListUI::FillElement(CControlUI *pControl, int iIndex)
{
	if (m_pDataProvider)
		m_pDataProvider->FillElement(pControl, iIndex);
}


int CVirtualListUI::GetElementCount()
{
	if (m_pDataProvider)
		return m_pDataProvider->GetElementtCount();

	return 0;
}

void CVirtualListUI::EnableScrollBar(bool bEnableVertical /*= true*/, bool bEnableHorizontal /*= false*/)
{
	m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
}

void CVirtualListUI::DoEvent(TEventUI& event)
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
		if (m_pParent != NULL) m_pParent->DoEvent(event);
		else CVerticalLayoutUI::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_SETFOCUS)
	{
		m_bFocused = true;
		return;
	}
	if (event.Type == UIEVENT_KILLFOCUS)
	{
		m_bFocused = false;
		return;
	}

	switch (event.Type) 
	{
	case UIEVENT_KEYDOWN:
		switch (event.chKey) 
		{
		case VK_UP:
		{
			m_pList->LineUp();
// 			if (m_aSelItems.GetSize() > 0) {
// 				int index = GetMinSelItemIndex() - 1;
// 				UnSelectAllItems();
// 				index > 0 ? SelectItem(index, true) : SelectItem(0, true);
// 			}
		}
		return;

		case VK_DOWN:
		{
			m_pList->LineDown();
// 			if (m_aSelItems.GetSize() > 0) {
// 				int index = GetMaxSelItemIndex() + 1;
// 				UnSelectAllItems();
// 				index + 1 > m_pList->GetCount() ? SelectItem(GetCount() - 1, true) : SelectItem(index, true);
// 			}
		}
		return;

		case VK_PRIOR:
			m_pList->PageUp();
			return;

		case VK_NEXT:
			m_pList->PageDown();
			return;

		case VK_HOME:
		{
			SIZE sz = { 0,0 };
			m_pList->SetScrollPos(sz);
		}
		//由于虚拟列表初始化会设置选中   此处可屏蔽
// 			SelectItem(FindSelectable(0, false), true);
 			return;
		case VK_END:
		{
			SIZE sz = { 0, m_pList->GetVerticalScrollBar()->GetScrollRange() };
			m_pList->SetScrollPos(sz);
		}
// 			SelectItem(FindSelectable(GetCount() - 1, true), true);
 			return;
		case VK_RETURN:
			if (m_iCurSel != -1) GetItemAt(m_iCurSel)->Activate();
			return;
		case 0x41:// Ctrl+A
		{
			if (IsMultiSelect() && (GetKeyState(VK_CONTROL) & 0x8000))
			{
				SelectAllItems();
			}
			return;
		}
		}
		break;
	case UIEVENT_SCROLLWHEEL:
	{
		switch (LOWORD(event.wParam)) 
		{
		case SB_LINEUP:
			m_pList->LineUp();
// 			if (m_bScrollSelect && !IsMultiSelect()) SelectItem(FindSelectable(m_iCurSel - 1, false), true);
// 			else m_pList->LineUp();
			return;
		case SB_LINEDOWN:
			m_pList->LineDown();
// 			if (m_bScrollSelect && !IsMultiSelect()) SelectItem(FindSelectable(m_iCurSel + 1, true), true);
// 			else m_pList->LineDown();
			return;
		}
	}
	break;
	}
	CVerticalLayoutUI::DoEvent(event);
}

bool CVirtualListUI::SelectItem(int iIndex, bool bTakeFocus /*= false*/)
{
	return m_pList->SelectItem(iIndex, bTakeFocus);	
}

bool CVirtualListUI::SelectMultiItem(int iIndex, bool bTakeFocus /*= false*/)
{
	return m_pList->SelectMultiItem(iIndex, bTakeFocus /*= false*/);
}

void CVirtualListUI::SelectAllItems()
{
	m_pList->SelectAllItems();
}

void CVirtualListUI::UnSelectAllItems()
{
	m_pList->UnSelectAllItems();
}

bool CVirtualListUI::SelectRange(int iIndex, bool bTakeFocus /*= false*/)
{
	return m_pList->SelectRange(iIndex, bTakeFocus);
}
