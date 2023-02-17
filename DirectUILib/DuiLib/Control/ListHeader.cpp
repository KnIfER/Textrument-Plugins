#include "StdAfx.h"

#ifdef QkListHeader

namespace DuiLib {
	IMPLEMENT_QKCONTROL(CListHeaderUI)

	CListHeaderUI::CListHeaderUI() :
		m_bIsScaleHeader(false)
	{
	}

	CListHeaderUI::~CListHeaderUI()
	{
	}

	LPCTSTR CListHeaderUI::GetClass() const
	{
		return _T("ListHeaderUI");
	}

	LPVOID CListHeaderUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_LISTHEADER) == 0) return this;
		return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	SIZE CListHeaderUI::EstimateSize(const SIZE & szAvailable)
	{
		int size = m_items.GetSize();
		if (_LastScaleProfile!=_manager->GetDPIObj()->ScaleProfile())
			OnDPIChanged();
		SIZE cXY = { 0, m_cxyFixed.cy };
		if (cXY.cy == 0 && _manager != NULL) {
			for (int it = 0; it < size; it++) {
				cXY.cy = MAX(cXY.cy, static_cast<CControlUI*>(m_items[it])->EstimateSize(szAvailable).cy);
			}
			int nMin = _manager->GetDefaultFontInfo()->tm.tmHeight + 6;
			cXY.cy = MAX(cXY.cy, nMin);
		}

		for (int it = 0; it < m_items.GetSize(); it++) {
			cXY.cx += static_cast<CControlUI*>(m_items[it])->EstimateSize(szAvailable).cx;
		}
		if (cXY.cx < szAvailable.cx) cXY.cx = szAvailable.cx;
		return cXY;
	}

	void CListHeaderUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;
		// Adjust for inset
		ApplyInsetToRect(rc);

		if (m_items.GetSize() == 0) {
			return;
		}

		// Determine the width of elements that are sizeable
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		int nAdjustables = 0;
		int cxFixed = 0;
		int nEstimateNum = 0;
		for (int it1 = 0; it1 < m_items.GetSize(); it1++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if (sz.cx == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
			}
			cxFixed += sz.cx + pControl->GetPadding().left + pControl->GetPadding().right;
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * m_iChildPadding;

		int cxExpand = 0;
		int cxNeeded = 0;
		if (nAdjustables > 0) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);

		int nHeaderWidth = rc.right - rc.left;
		CListUI *pList = static_cast<CListUI*>(GetParent());
		if (pList != NULL) {
			CScrollBarUI* pVScroll = pList->GetVerticalScrollBar();
			if (pVScroll != NULL) {
				nHeaderWidth -= pVScroll->GetWidth();
				szAvailable.cx = nHeaderWidth;
			}
		}

		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosX = rc.left;
		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;

		for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) {
				SetFloatPos(it2);
				continue;
			}
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cx -= rcPadding.left;

			SIZE sz = { 0,0 };
			if (m_bIsScaleHeader) {
				CListHeaderItemUI* pHeaderItem = static_cast<CListHeaderItemUI*>(pControl);
				sz.cx = int(nHeaderWidth * (float)pHeaderItem->GetScale() / 100);
			}
			else {
				sz = pControl->EstimateSize(szRemaining);
			}

			if (sz.cx == 0) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if (iAdjustable == nAdjustables) {
					sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				}
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();

				cxFixedRemaining -= sz.cx;
			}

			sz.cy = pControl->GetFixedHeight();
			if (sz.cy == 0) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
			if (sz.cy < 0) sz.cy = 0;
			if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
			if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy };
			pControl->SetPos(rcCtrl);
			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
		}
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
	}

	void CListHeaderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("scaleheader")) == 0) SetScaleHeader(_tcsicmp(pstrValue, _T("true")) == 0);
		else CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CListHeaderUI::SetScaleHeader(bool bIsScale)
	{
		m_bIsScaleHeader = bIsScale;
	}

	bool CListHeaderUI::IsScaleHeader() const
	{
		return m_bIsScaleHeader;
	}

	void CListHeaderUI::DoInit()
	{

	}

	void CListHeaderUI::DoPostPaint(HDC hDC, const RECT& rcPaint) 
	{

	}
}

#endif