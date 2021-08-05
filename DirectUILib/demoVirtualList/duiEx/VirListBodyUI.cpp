#include "stdafx.h"
#include "VirListBodyUI.h"

CVirListBodyUI::CVirListBodyUI(CListUI* pOwner) : CListBodyUI(pOwner)
{
	m_pOwner = pOwner;
	m_vSelIndex.clear();
}

CVirListBodyUI::~CVirListBodyUI()
{

}

void CVirListBodyUI::SetVerScrollRange(int nRange)
{
	m_pVerticalScrollBar->SetScrollRange(nRange);
}

void CVirListBodyUI::SetItemCount(int nCount)
{
	//m_i
}

void CVirListBodyUI::RedrawItems(int iItemFrom, int iItemTo)
{
	m_iCurlShowBeginIndex = iItemFrom;
	m_iCurlShowEndIndex = iItemTo;
}

void CVirListBodyUI::SetDataProvider(IVirtualDataProvider * pDataProvider)
{
	m_pDataProvider = pDataProvider;
}

IVirtualDataProvider * CVirListBodyUI::GetDataProvider()
{
	return m_pDataProvider;
}

void CVirListBodyUI::SetElementHeight(int nHeight)
{
	m_nOwnerElementHeight = nHeight;
}

void CVirListBodyUI::InitElement(int nMaxItemCount /*= 50*/)
{
	ASSERT(m_pDataProvider);
	ASSERT(m_nOwnerElementHeight);
	m_nOwnerItemCount = nMaxItemCount;

	int nCount = GetElementCount();
	if (nCount > nMaxItemCount)
		nCount = nMaxItemCount;

	for (int i = 0; i < nCount; i++) 
	{
		CControlUI *pControl = CreateElement();	
		FillElement(pControl, i);
		this->Add(pControl);
		static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")))->SetOwner(m_pOwner);
		static_cast<IListItemUI*>(GetItemAt(i)->GetInterface(_T("ListItem")))->SetIndex(i);
	}
}

void CVirListBodyUI::Refresh()
{

	int nElementCount = GetElementCount();
	int nItemCount = GetCount();

	if (nItemCount > nElementCount) 
	{
		int n = nItemCount - nElementCount;
		for (int i = 0; i < n; i++)
			this->RemoveAt(0);
	}
	else if (nItemCount < nElementCount)
	{
		int n = 0;
		if (nElementCount <= m_nOwnerItemCount) 
		{
			n = nElementCount - nItemCount;
		}
		else {
			n = m_nOwnerItemCount - nItemCount;
		}

		for (int i = 0; i < n; i++) 
		{
			CControlUI *pControl = CreateElement();
			this->Add(pControl);
		}
	}

	if (UseDefaultLayout())
	{
		// 刚从虚拟列表转换到普通模式时，存在布局错误的情况（虚拟列表滚动条接近底部，
		// 然后数据减少，变成普通模式）
		// 		if (nItemCount == m_nOwnerItemCount)
		// 		{
		// 			this->GetLayout()->ArrangeChild(m_items, m_rcItem);
		// 		}

		for (int i = 0; i < (int)m_items.GetSize(); i++)
		{
			FillElement((CControlUI*)m_items[i], i);
			static_cast<IListItemUI*>(GetItemAt(i)->GetInterface(_T("ListItem")))->SetIndex(i);
		}
	}
	else
	{
		ASSERT(nElementCount);
		if (nElementCount <= 0)
			return;
		ReArrangeChild(true);
		Arrange();
	}
}

void CVirListBodyUI::RemoveAll()
{
	__super::RemoveAll();

	if (m_pVerticalScrollBar)
		m_pVerticalScrollBar->SetScrollPos(0);

	m_nOldYScrollPos = 0;
	m_bArrangedOnce = false;
	m_bForceArrange = false;
}

void CVirListBodyUI::SetForceArrange(bool bForce)
{
	m_bForceArrange = bForce;
}

void CVirListBodyUI::GetDisplayCollection(std::vector<int>& collection)
{
	collection.clear();
	if (GetCount() == 0)
		return;

	RECT rcThis = this->GetPos();
	int min = GetScrollPos().cy / m_nOwnerElementHeight;
	int max = min + ((rcThis.bottom - rcThis.top) / m_nOwnerElementHeight);
	int nCount = GetElementCount();
	if (max >= nCount)
		max = nCount - 1;

	for (auto i = min; i <= max; i++)
		collection.push_back(i);
}

void CVirListBodyUI::EnsureVisible(int iIndex, bool bToTop /*= false*/)
{
	if (iIndex < 0 || iIndex >= GetElementCount())
		return;

	int nPos = m_pVerticalScrollBar->GetScrollPos();
	int nTopIndex = nPos / m_nOwnerElementHeight;
	int nNewPos = 0;

	if (bToTop) 
	{
		nNewPos = CalcElementsHeight(iIndex);
		if (nNewPos >= m_pVerticalScrollBar->GetScrollRange())
			return;
	}
	else 
	{
		if (IsElementDisplay(iIndex))
			return;

		if (iIndex > nTopIndex)
		{
			// 向下
			int height = CalcElementsHeight(iIndex + 1);
			nNewPos = height - (m_rcItem.bottom - m_rcItem.top);
		}
		else 
		{
			// 向上
			nNewPos = CalcElementsHeight(iIndex);
		}
	}
	DuiLib::CDuiSize sz(0, nNewPos);

	SetScrollPos(sz);
}

void CVirListBodyUI::EnsureVisible(int iIndex)
{
	if (iIndex < 0 || iIndex >= GetElementCount())
		return;

	int nPos = m_pVerticalScrollBar->GetScrollPos();
	int nTopIndex = nPos / m_nOwnerElementHeight;
	int nNewPos = 0;


	if (false)
	{
		nNewPos = CalcElementsHeight(iIndex);
		if (nNewPos >= m_pVerticalScrollBar->GetScrollRange())
			return;
	}
	else 
	{
		if (IsElementDisplay(iIndex))
			return;

		if (iIndex > nTopIndex) 
		{
			// 向下
			int height = CalcElementsHeight(iIndex + 1);
			nNewPos = height - (m_rcItem.bottom - m_rcItem.top);
		}
		else
		{
			// 向上
			nNewPos = CalcElementsHeight(iIndex);
		}
	}
	DuiLib::CDuiSize sz(0, nNewPos);

	SetScrollPos(sz);
}

void CVirListBodyUI::SetScrollPos(SIZE szPos, bool bMsg)
{
	m_nOldYScrollPos = m_pVerticalScrollBar->GetScrollPos();
	
	if (szPos.cy < 0)
	{
		szPos.cy = 0;
	}

	if (szPos.cy > m_pVerticalScrollBar->GetScrollRange())
	{
		szPos.cy = m_pVerticalScrollBar->GetScrollRange();
	}

	if (m_pVerticalScrollBar)
	{
		if (szPos.cy == 0)
		{
			OutputDebugString(L"szPos.cy == 0 \r\n");
		}
		m_pVerticalScrollBar->SetScrollPos(szPos.cy);
	}
	if (m_pHorizontalScrollBar)
	{
		m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
	}
	
	if (UseDefaultLayout())
		return;

	ReArrangeChild(false);
}

void CVirListBodyUI::SetPos(RECT rc, bool bNeedInvalidate /*= true*/)
{
	CControlUI::SetPos(rc, bNeedInvalidate);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
		rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
		rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	// Determine the minimum size
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
	{
		szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
	}

	int cxNeeded = 0;
	int nAdjustables = 0;
	int cyFixed = 0;
	int nEstimateNum = 0;
	for (int it1 = 0; it1 < m_items.GetSize(); it1++)
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
		if (!pControl->IsVisible()) continue;
		if (pControl->IsFloat()) continue;
		SIZE sz = pControl->EstimateSize(szAvailable);
		if (0 == sz.cy)
		{
			nAdjustables++;
		}
		else 
		{
			if (sz.cy < pControl->GetMinHeight())
			{
				sz.cy = pControl->GetMinHeight();
			}
			else if (sz.cy > pControl->GetMaxHeight())
			{
				sz.cy = pControl->GetMaxHeight();
			}
		}

		cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;

		RECT rcPadding = pControl->GetPadding();
		sz.cx = MAX(sz.cx, 0);
		if (sz.cx < pControl->GetMinWidth())
		{
			sz.cx = pControl->GetMinWidth();
		}
		else if (sz.cx > pControl->GetMaxWidth())
		{
			sz.cx = pControl->GetMaxWidth();
		}
		cxNeeded = MAX(cxNeeded, sz.cx);
		nEstimateNum++;
	}
	cyFixed += (nEstimateNum - 1) * m_iChildPadding;

	if (m_pOwner) 
	{
		CListHeaderUI* pHeader = m_pOwner->GetHeader();
		if (pHeader != NULL && pHeader->GetCount() > 0) 
		{
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
	if (nAdjustables > 0)
	{
		cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
	}

	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosY = rc.top;
	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) 
	{
		iPosY -= m_pVerticalScrollBar->GetScrollPos();
	}

	int iPosX = rc.left;
	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) 
	{
		iPosX -= m_pHorizontalScrollBar->GetScrollPos();
	}

	int iAdjustable = 0;
	int cyFixedRemaining = cyFixed;
	for (int it2 = 0; it2 < m_items.GetSize(); it2++)
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
		if (!pControl->IsVisible())
		{
			continue;
		}

		if (pControl->IsFloat())
		{
			SetFloatPos(it2);
			continue;
		}

		RECT rcPadding = pControl->GetPadding();
		szRemaining.cy -= rcPadding.top;
		SIZE sz = pControl->EstimateSize(szRemaining);
		if (0 == sz.cy)
		{
			iAdjustable++;
			sz.cy = cyExpand;
			// Distribute remaining to last element (usually round-off left-overs)
			if (iAdjustable == nAdjustables) 
			{
				sz.cy = MAX(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
			}
			if (sz.cy < pControl->GetMinHeight())
			{
				sz.cy = pControl->GetMinHeight();
			}
			if (sz.cy > pControl->GetMaxHeight())
			{
				sz.cy = pControl->GetMaxHeight();
			}
		}
		else 
		{
			if (sz.cy < pControl->GetMinHeight())
			{
				sz.cy = pControl->GetMinHeight();
			}
			if (sz.cy > pControl->GetMaxHeight())
			{
				sz.cy = pControl->GetMaxHeight();
			}
			cyFixedRemaining -= sz.cy;
		}

		sz.cx = MAX(cxNeeded, szAvailable.cx - rcPadding.left - rcPadding.right);

		if (sz.cx < pControl->GetMinWidth())
		{
			sz.cx = pControl->GetMinWidth();
		}
		if (sz.cx > pControl->GetMaxWidth())
		{
			sz.cx = pControl->GetMaxWidth();
		}

		RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
		pControl->SetPos(rcCtrl);

		iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
		cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
		szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
	}
	cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

	if (m_pHorizontalScrollBar != NULL) 
	{
		if (cxNeeded > rc.right - rc.left)
		{
			if (m_pHorizontalScrollBar->IsVisible())
			{
				m_pHorizontalScrollBar->SetScrollRange(cxNeeded - (rc.right - rc.left));
			}
			else
			{
				m_pHorizontalScrollBar->SetVisible(true);
				m_pHorizontalScrollBar->SetScrollRange(cxNeeded - (rc.right - rc.left));
				m_pHorizontalScrollBar->SetScrollPos(0);
				rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
			}
		}
		else
		{
			if (m_pHorizontalScrollBar->IsVisible()) 
			{
				m_pHorizontalScrollBar->SetVisible(false);
				m_pHorizontalScrollBar->SetScrollRange(0);
				m_pHorizontalScrollBar->SetScrollPos(0);
				rc.bottom += m_pHorizontalScrollBar->GetFixedHeight();
			}
		}
	}

	//放这个位置是因为容器大小改变后  滚动条的 Range 会改变
	SetPosInternally(rc);

// 	bool bChange = false;
// 	CDuiRect rcItem(m_rcItem);
// 
// 	if (m_pVerticalScrollBar)
// 	{
// 		rc.right += m_pVerticalScrollBar->GetWidth();
// 	}
// 	if (!rcItem.Equal(rc))
// 		bChange = true;
// 
// 	if (bChange) 
// 	{
// 		if (UseDefaultLayout())
// 			return;
		ReArrangeChild(true);
//	}
}

void CVirListBodyUI::SetPosInternally(RECT rc)
{
	rc = m_rcItem;
	rc.bottom = GetElementCount()*m_nOwnerElementHeight + rc.top;
	ProcessScrollBar(m_rcItem, rc.right - rc.left, rc.bottom - rc.top);
}

void CVirListBodyUI::ReArrangeChild(bool bForce)
{
	ScrollDirection direction = kScrollUp;
	if (!bForce && !m_bForceArrange) 
	{
		if (!NeedReArrange(direction))//如果没有到最后一个元素, 则不需要重新加载
			return;
	}
	//UnSelectAllItems();

	int nElementCount = GetElementCount();
	if (nElementCount <= 0)
	{
		return;
	}
	int nTopIndexBottom = 0;//相当于滚动条位置11111
	int nTopIndex = GetTopElementIndex(nTopIndexBottom);
	CDuiRect rcItem = m_rcItem;
	for (int i = 0; i < (int)m_items.GetSize(); i++)//重新改变元素在容器中的位置
	{
		rcItem.top = m_rcItem.top + i*m_nOwnerElementHeight;
		rcItem.bottom = rcItem.top + m_nOwnerElementHeight;

		GetItemAt(i)->SetPos(rcItem);
		int nElementIndex = nTopIndex + i;
		if (nElementIndex < nElementCount)//
		{ 
			FillElement(GetItemAt(i), nElementIndex);//填充元素
			static_cast<IListItemUI*>(GetItemAt(i)->GetInterface(_T("ListItem")))->SetIndex(nElementIndex);//设置索引
			static_cast<IListItemUI*>(GetItemAt(i)->GetInterface(_T("ListItem")))->Select(false);//Re 
		}
	}
	//初始化选中项
	InitSelectItem();
}

void CVirListBodyUI::AddElement(int iIndex)
{
	//应该是判断该索引是否存在加载区域, 如果在加载区域, 增加数据到数据源中对应索引的位置 重新刷新  否则只需要增加数据到数据源 无需刷新
	int nCount = GetElementCount();
	if (nCount <= m_nOwnerItemCount) 
	{
		CControlUI *pControl = CreateElement();
		this->AddAt(pControl, iIndex);
		FillElement(pControl, iIndex);
		//static_cast<IListItemUI*>(GetItemAt(iIndex)).SetIndex(iIndex);
	}
	else 
	{
		ReArrangeChild(true);
		Arrange();
	}
}

void CVirListBodyUI::RemoveElement(int iIndex)
{ 
	//应该是判断该索引是否存在加载区域, 如果在加载区域, 删掉数据源中对应索引的数据 重新刷新  否则只需要删掉对应索引的数据 无需刷新
	int nCount = GetElementCount();
	if (nCount < m_nOwnerItemCount) 
	{
		this->RemoveAt(iIndex);
	}
	else
	{
		ReArrangeChild(true);
		Arrange();
	}
}

DuiLib::CControlUI* CVirListBodyUI::CreateElement()
{
	if (m_pDataProvider)
	{
		return m_pDataProvider->CreateElement();
	}

	return nullptr;
}

void CVirListBodyUI::FillElement(CControlUI *pControl, int iIndex)
{
	if (m_pDataProvider)
	{
		m_pDataProvider->FillElement(pControl, iIndex);
	}
}

int CVirListBodyUI::GetElementCount()
{
	if (m_pDataProvider)
	{
		return m_pDataProvider->GetElementtCount();
	}

	return 0;
}

bool CVirListBodyUI::UseDefaultLayout()
{
	return GetElementCount() <= GetCount();
}

int CVirListBodyUI::CalcElementsHeight(int nCount)
{
	if (nCount < 0)
	{
		nCount = GetElementCount();
	}

	return nCount * m_nOwnerElementHeight;
}

int CVirListBodyUI::GetTopElementIndex(int &bottom)
{
	if (m_pVerticalScrollBar)
	{
		int nPos = m_pVerticalScrollBar->GetScrollPos();
		int iIndex = nPos / m_nOwnerElementHeight;
		bottom = iIndex * m_nOwnerElementHeight;

		if (0 == iIndex)
		{
			OutputDebugString(L" 0 == iIndex \r\n");
		}

		return iIndex;
	}
	return 0;
}

bool CVirListBodyUI::IsElementDisplay(int iIndex)
{
	if (iIndex < 0)
		return false;

	int nPos = m_pVerticalScrollBar->GetScrollPos();
	int nElementPos = CalcElementsHeight(iIndex);
	if (nElementPos >= nPos)
	{
		int nHeight = this->GetHeight();
		if (nElementPos + m_nOwnerElementHeight <= nPos + nHeight)
			return true;
	}

	return false;
}

bool CVirListBodyUI::NeedReArrange(ScrollDirection &direction)
{
	direction = kScrollUp;
	if (!m_bArrangedOnce) 
	{
		m_bArrangedOnce = true;
		return true;
	}

	int nCount = GetCount();
	if (nCount <= 0)
		return false;

	if (GetElementCount() < nCount)
		return false;

	CDuiRect rcThis = this->GetPos();
	if (rcThis.GetWidth() <= 0)
		return false;

	//int nPos = m_pVerticalScrollBar->GetScrollPos();
	int nPos = m_pVerticalScrollBar->GetScrollPos();
	CDuiRect rcItem;

	// 补救措施
	// 情况一：通讯录列表，一开始不可见，切换后可见，如果提前布局，
	// 则Element宽度为0，因此，必须重新布局；
	// 情况二：写信窗口联系人列表，列表宽度会不断变化，因此，需要在宽度变化后
	// 重新布局，否则，导致最终Element布局时的宽度不正确
	//rcItem = static_cast<CControlUI*> (/*m_items[0]*/GetItemAt(0))->GetPos();
	// modified by zqw, 2016/10/12
	// 针对情况二，解决方法是，列表宽度变化 或者 拖动写信窗口右侧，列表position改变，
	// 此时，在SetPos中强制重新布局
	//if (u.GetWidth() != rect.GetWidth()) {
	//    return true;
	//}

	if (nPos >= m_nOldYScrollPos)//当前滚动条位置大于之前的滚动条位置 
	{
		// 下
// 		rcItem = GetItemAt(nCount - 1)->GetPos();//最后一个元素位置
// 		int nSub = (rcItem.bottom - rcThis.top) - (nPos + rcThis.GetHeight());//如果最后一个元素位置 与 控件顶部的距离 
// 		//小于 当前第一个元素位置加上控件高度 说明 滚动条移动后显示区域 有没有控件的区域 这个高度为 nSub
// 		if (nSub < 0)
// 		{
			direction = kScrollDown;
			return true;
//		}
	}
	else 
	{
		// 上
// 		rcItem = GetItemAt(0)->GetPos();
// 		if (nPos < (rcItem.top - rcThis.top)) 
// 		{
			direction = kScrollUp;
			return true;
//		}
	}

	return false;
}

void CVirListBodyUI::Arrange()
{
	if (!IsVisible())
		return;

	// 	m_bReEstimateSize = true;
	// 	m_bIsArranged = true;
	Invalidate();
}

void CVirListBodyUI::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
{
	while (m_pHorizontalScrollBar)
	{
		// Scroll needed
		if (cxRequired > rc.right - rc.left && !m_pHorizontalScrollBar->IsVisible())
		{
			m_pHorizontalScrollBar->SetVisible(true);
			m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
			m_pHorizontalScrollBar->SetScrollPos(0);
			SetPos(m_rcItem);
			break;
		}

		// No scrollbar required
		if (!m_pHorizontalScrollBar->IsVisible()) break;

		// Scroll not needed anymore?
		int cxScroll = cxRequired - (rc.right - rc.left);
		if (cxScroll <= 0)
		{
			m_pHorizontalScrollBar->SetVisible(false);
			m_pHorizontalScrollBar->SetScrollPos(0);
			m_pHorizontalScrollBar->SetScrollRange(0);
			SetPos(m_rcItem);
		}
		else
		{
			RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
			m_pHorizontalScrollBar->SetPos(rcScrollBarPos);

			if (m_pHorizontalScrollBar->GetScrollRange() != cxScroll)
			{
				int iScrollPos = m_pHorizontalScrollBar->GetScrollPos();
				m_pHorizontalScrollBar->SetScrollRange(::abs(cxScroll)); // if scrollpos>range then scrollpos=range
				if (iScrollPos > m_pHorizontalScrollBar->GetScrollPos())
				{
					SetPos(m_rcItem);
				}
			}
		}
		break;
	}

	while (m_pVerticalScrollBar)
	{
		// Scroll needed
		rc.left += GetPadding().left;
		rc.top += GetPadding().top;
		rc.right -= GetPadding().right;
		rc.bottom -= GetPadding().bottom;
		int nHeight = rc.bottom - rc.top;
		if (cyRequired > nHeight && !m_pVerticalScrollBar->IsVisible())
		{
			m_pVerticalScrollBar->SetVisible(true);
			m_pVerticalScrollBar->SetScrollRange(cyRequired - nHeight);
			m_pVerticalScrollBar->SetScrollPos(0);
			m_bScrollProcess = true;
			SetPos(m_rcItem);
			m_bScrollProcess = false;
			RECT rcScrollPos = m_pVerticalScrollBar->GetPos();
			break;
		}

		// No scrollbar required
		if (!m_pVerticalScrollBar->IsVisible()) break;

		// Scroll not needed anymore?
		int cyScroll = cyRequired - (rc.bottom - rc.top);
		if (cyScroll <= 0 && !m_bScrollProcess)
		{
			m_pVerticalScrollBar->SetVisible(false);
			m_pVerticalScrollBar->SetScrollPos(0);
			m_pVerticalScrollBar->SetScrollRange(0);
			SetPos(m_rcItem);
			break;
		}

		RECT rcScrollBarPos = { rc.right - m_pVerticalScrollBar->GetFixedWidth(), rc.top, rc.right , rc.bottom };
		m_pVerticalScrollBar->SetPos(rcScrollBarPos);

		if (m_pVerticalScrollBar->GetScrollRange() != cyScroll)
		{
			int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll)); // if scrollpos>range then scrollpos=range
			if (iScrollPos > m_pVerticalScrollBar->GetScrollPos() && !m_bScrollProcess)
			{
				SetPos(m_rcItem);
			}
		}
		break;
	}
}

void CVirListBodyUI::InitSelectItem()
{
	int nHeight = 0;
	int nTopIndex = GetTopElementIndex(nHeight);//
	for (int i = 0; i < m_vSelIndex.size(); ++i)
	{
		int iIndex = m_vSelIndex[i] - nTopIndex;
		if (iIndex >= 0 && iIndex < GetCount())
		{
			static_cast<IListItemUI*>(GetItemAt(iIndex)->GetInterface(L"ListItem"))->SelectMulti(true);
		}
	}
}

bool CVirListBodyUI::SelectItem(int iIndex, bool bTakeFocus /*= false*/)
{
	if (m_iSelIndex == iIndex)
	{
		return false;
	}
	// 取消所有选择项
	UnSelectAllItems();
	// 判断是否合法列表项

	int nHeight = 0;
	int nTopIndex = GetTopElementIndex(nHeight);//
	int iSelIndex = iIndex - nTopIndex;
	if (iSelIndex < 0 || iSelIndex>=GetCount()) 
		return false;
	CControlUI* pControl = GetItemAt(iSelIndex);
	if (pControl == NULL) 
		return false;
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
	if (pListItem == NULL) 
		return false;
	if (!pListItem->Select(true)) 
	{
		return false;
	}

	m_vSelIndex.push_back(iIndex);
	m_iSelIndex = iIndex;
	m_pOwner->EnsureVisible(iSelIndex);
	if (bTakeFocus)
	{
		pControl->SetFocus();
	}
	if (m_pManager != NULL /*&& iLastSel != m_iCurSel*/) 
	{
		m_pManager->SendNotify(m_pOwner, DUI_MSGTYPE_ITEMSELECT, iIndex);
	}

	return true;
}

bool CVirListBodyUI::SelectMultiItem(int iIndex, bool bTakeFocus /*= false*/)
{
	if (NULL == m_pOwner)
	{
		return false;
	}
	if (!m_pOwner->IsMultiSelect()) //判断是否有多选属性
	{
		return SelectItem(iIndex, bTakeFocus);
	}

	bool bRes = true;

	do 
	{
		if (iIndex < 0)//
		{
			bRes = false;
			break;
		}

		int nHeight = 0;
		int nTopIndex = GetTopElementIndex(nHeight);//获取可见区域顶部索引
		int iSelIndex = iIndex - nTopIndex;//控件在容器中的索引 ,第iSelIndex + 1个控件元素

		if (iIndex >=0 && iIndex < GetElementCount())
		{
			std::vector<int>::iterator it = std::find(m_vSelIndex.begin(), m_vSelIndex.end(), iIndex);
			if (it == m_vSelIndex.end())//没有选中状态的控件 避免一个索引缓存多次, 初始化时效率较低
			{
				m_vSelIndex.push_back(iIndex);//增加到选中容器
				bRes = true;
			}
		}
		else
		{
			bRes = false;
			break;
		}

		if (iSelIndex >=0 && iSelIndex <  GetCount())//
		{
			//在容器中显示的控件 
			CControlUI* pControl = GetItemAt(iSelIndex);//iSelIndex >=0 && iSelIndex < GetCount()
			if (NULL == pControl)
			{
				bRes = false;
				break;
			}
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if (NULL == pListItem)
			{
				bRes = false;
				break;
			}


			if (!pListItem->SelectMulti(true))//选中
			{
				bRes = false;
				break;  
			}

// 			m_iSelIndex = iIndex;
// 			m_vSelIndex.push_back(iIndex);//添加到选中索引容器中
// 										  //EnsureVisible(iIndex);
			if (bTakeFocus)
			{
				pControl->SetFocus();
			}
		}

		
	} while (false);

	if (bRes)
	{
		if (m_pManager != NULL)
		{
			m_pManager->SendNotify(m_pOwner, DUI_MSGTYPE_ITEMSELECT, iIndex);
		}
	}
	return bRes;
}
//全选
void CVirListBodyUI::SelectAllItems()
{
	//将容器中的所有控件都设置选中状态
	for (int i = 0; i < GetCount(); i++)
	{
		CControlUI* pControl = GetItemAt(i);
		if (NULL == pControl)
			return ;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if (NULL == pListItem)
			return ;
		if (!pListItem->SelectMulti(true))
		{
			return ;
		}
	}
	m_vSelIndex.clear();//清除掉
	for (int i = 0; i < GetElementCount(); i++)//将所有元素索引都增加到选中列表
	{
		m_vSelIndex.push_back(i);
	}
}

//全部不选中
void CVirListBodyUI::UnSelectAllItems()
{
	int nHeight = 0;
	int nTopIndex = GetTopElementIndex(nHeight);//出现重复
	for (int i = 0; i< m_vSelIndex.size(); ++i)//将容器中选中的设置为不选中
	{
		int iIndex = m_vSelIndex[i] - nTopIndex;
		if (iIndex >= 0 && iIndex < GetCount())//将容器中选中的设置为不选中
		{
			static_cast<IListItemUI*>(GetItemAt(iIndex)->GetInterface(L"ListItem"))->SelectMulti(false);
		}
	}
	m_vSelIndex.clear();//清空选中列表
}

bool CVirListBodyUI::SelectRange(int iIndex, bool bTakeFocus /*= false*/)
{
	if (-1 == iIndex)
	{
		return false;
	}
	if (iIndex == m_iSelIndex)
	{
		return true;
	}
	int iSelBegin;
	int iSelEnd;
	int iOldSel = m_iSelIndex;
	if (iIndex > m_iSelIndex)
	{
		if (-1 == m_iSelIndex)
		{
			iSelBegin = 0;
			//return true;
		}
		else
		{
			iSelBegin = m_iSelIndex;
		}
		iSelEnd = iIndex;
	}
	else
	{
		iSelBegin = iIndex;
		iSelEnd = m_iSelIndex;
	}
	UnSelectAllItems();//清空选中

	int nHeight = 0;
	int nTopIndex = GetTopElementIndex(nHeight);//出现重复
	for (int i = iSelBegin; i <= iSelEnd; i++)
	{
		int iIndex = m_vSelIndex[i] - nTopIndex;
		if (iIndex >= 0 && iIndex < GetCount())//将容器中选中的设置为不选中
		{
			static_cast<IListItemUI*>(GetItemAt(iIndex)->GetInterface(L"ListItem"))->SelectMulti(true);
		}
		m_vSelIndex.push_back(i);
	}
	m_iSelIndex = iOldSel;
	return true;
}

std::vector<int> CVirListBodyUI::GetSelectIndex()
{
	return m_vSelIndex;
}
