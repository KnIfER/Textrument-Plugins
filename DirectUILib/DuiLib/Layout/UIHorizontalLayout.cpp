#include "StdAfx.h"
#include "UIHorizontalLayout.h"

namespace DuiLib
{
	IMPLEMENT_QKCONTROL(CHorizontalLayoutUI)

	CHorizontalLayoutUI::CHorizontalLayoutUI() : CContainerUI()
	{
	}

	LPCTSTR CHorizontalLayoutUI::GetClass() const
	{
		return _T("HorizontalLayoutUI");
	}

	LPVOID CHorizontalLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_HORIZONTALLAYOUT) == 0 ) return static_cast<CHorizontalLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	SIZE CHorizontalLayoutUI::EstimateSize(const SIZE & szAvailable)
	{
		if (_manager && _LastScaleProfile!=_manager->GetDPIObj()->ScaleProfile())
			OnDPIChanged();
		const RECT & rcInnerPadding = m_rcInsetScaled;

		UINT availWidth = szAvailable.cx - rcInnerPadding.left - rcInnerPadding.right;
		UINT availHeight = szAvailable.cy - rcInnerPadding.top - rcInnerPadding.bottom;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) availHeight -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) availWidth -= m_pHorizontalScrollBar->GetFixedHeight();

		m_cxyFixedLast = GetFixedSize();
		if(m_cxyFixed.cx < 0) m_cxyFixedLast.cx = szAvailable.cx;
		if(m_cxyFixed.cy < 0) m_cxyFixedLast.cy = szAvailable.cy;
		if(_manager != NULL)
			_manager->GetDPIObj()->Scale(&m_cxyFixedLast);
		if (m_bAutoCalcHeight || m_bAutoCalcWidth)
		{
			int cyNeeded = 0;
			int nAdjustables = 0;
			int cxFixed = 0;
			int nEstimateNum = 0;
			SIZE szControlAvailable;
			for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
				if( !pControl->IsVisible() ) continue;
				if( pControl->IsFloat() ) continue;
				szControlAvailable = szAvailable;
				RECT rcPadding = pControl->GetPadding();
				szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
				if (szControlAvailable.cx > pControl->GetMaxAvailWidth()) szControlAvailable.cx = pControl->GetMaxAvailWidth();
				if (szControlAvailable.cy > pControl->GetMaxAvailHeight()) szControlAvailable.cy = pControl->GetMaxAvailHeight();
				SIZE sz = pControl->EstimateSize(szControlAvailable);
				if( sz.cx == 0 ) {
					nAdjustables++;
				}
				else {
					if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
					if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				}
				cxFixed += sz.cx + pControl->GetPadding().left + pControl->GetPadding().right;

				sz.cy = MAX(sz.cy, 0);
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyNeeded = MAX(cyNeeded, sz.cy + rcPadding.top + rcPadding.bottom);
				nEstimateNum++;
			}
			cxFixed += (nEstimateNum - 1) * m_iChildPadding;
			cyNeeded += rcInnerPadding.top + rcInnerPadding.bottom;
			cxFixed += rcInnerPadding.left + rcInnerPadding.right;
			m_cxyFixedLast.cy = m_bAutoCalcHeight?cyNeeded:MAX(cyNeeded, availHeight);
			m_cxyFixedLast.cx = m_bAutoCalcWidth?cxFixed:MAX(cxFixed, availWidth);

		}
		// m_cxyFixedLast.cx = 100000; // 当同时设置了 autocalcwidth 与 width, VBOX处width被当作maxWidh，于是此数值过大时无效
		// 当废除 autocalcwidth 变量，使得 width 等于 -2 时代表计算内容尺寸。
		//m_cxyFixedLast.cx = 0;
		return m_cxyFixedLast;
	}

	void CHorizontalLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;

		// Adjust for inset
		ApplyInsetToRect(rc);

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		if( m_items.GetSize() == 0 || _bSupressingChildLayout ) {
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		//if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		//	szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
			szAvailable.cy += m_pVerticalScrollBar->GetScrollRange();

		int cyNeeded = 0;
		int nAdjustables = 0;
		int cxFixed = 0;
		int nEstimateNum = 0;
		SIZE szControlAvailable;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			szControlAvailable = szAvailable;
			RECT rcPadding = pControl->GetPadding();
			szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
			if (szControlAvailable.cx > pControl->GetMaxAvailWidth()) szControlAvailable.cx = pControl->GetMaxAvailWidth();
			if (szControlAvailable.cy > pControl->GetMaxAvailHeight()) szControlAvailable.cy = pControl->GetMaxAvailHeight();
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cx == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			cxFixed += sz.cx + pControl->GetPadding().left + pControl->GetPadding().right;

			sz.cy = MAX(sz.cy, 0);
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			cyNeeded = MAX(cyNeeded, sz.cy + rcPadding.top + rcPadding.bottom);
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * m_iChildPadding;
		// Place elements
		int cxNeeded = 0;
		int cxExpand = 0;
		if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosX = rc.left;
		
		// 滚动条
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		}
		else {
			// 子控件横向对其方式
			if(nAdjustables <= 0) {
				UINT iChildAlign = GetChildAlign(); 
				if (iChildAlign == DT_CENTER) {
					iPosX += (szAvailable.cx -cxFixed) / 2;
				}
				else if (iChildAlign == DT_RIGHT) {
					iPosX += (szAvailable.cx - cxFixed);
				}
			}
		}
		int iEstimate = 0;
		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			
			iEstimate += 1;
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cx -= rcPadding.left;

			szControlAvailable = szRemaining;
			szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
			if (szControlAvailable.cx > pControl->GetMaxAvailWidth()) szControlAvailable.cx = pControl->GetMaxAvailWidth();
			if (szControlAvailable.cy > pControl->GetMaxAvailHeight()) szControlAvailable.cy = pControl->GetMaxAvailHeight();
			cxFixedRemaining = cxFixedRemaining - (rcPadding.left + rcPadding.right);
			if (iEstimate > 1) cxFixedRemaining = cxFixedRemaining - m_iChildPadding;

			SIZE sz = pControl->EstimateSize(szControlAvailable);

			if( sz.cx == 0 ) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				} 
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixedRemaining -= sz.cx;
			}

			//sz.cy = pControl->GetMaxHeight();

			if( sz.cy == 0 ) sz.cy = szAvailable.cy - rcPadding.top - rcPadding.bottom;
			if( sz.cy < 0 ) sz.cy = 0;
			if( sz.cy > szControlAvailable.cy ) sz.cy = szControlAvailable.cy;
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();

			UINT iChildAlign = GetChildVAlign(); 
			if (iChildAlign == DT_VCENTER) {
				int iPosY = (rc.bottom + rc.top) / 2;
				if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
					iPosY += m_pVerticalScrollBar->GetScrollRange() / 2;
					iPosY -= m_pVerticalScrollBar->GetScrollPos();
				}
				RECT rcCtrl = { iPosX + rcPadding.left, iPosY - sz.cy/2, iPosX + sz.cx + rcPadding.left, iPosY + sz.cy - sz.cy/2 };
				pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_BOTTOM) {
				int iPosY = rc.bottom;
				if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
					iPosY += m_pVerticalScrollBar->GetScrollRange();
					iPosY -= m_pVerticalScrollBar->GetScrollPos();
				}
				RECT rcCtrl = { iPosX + rcPadding.left, iPosY - rcPadding.bottom - sz.cy, iPosX + sz.cx + rcPadding.left, iPosY - rcPadding.bottom };
				pControl->SetPos(rcCtrl, false);
			}
			else 
			{
				int iPosY = rc.top;
				if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
					iPosY -= m_pVerticalScrollBar->GetScrollPos();
				}
				RECT rcCtrl = { iPosX + rcPadding.left
					, iPosY + rcPadding.top
					, iPosX + sz.cx + rcPadding.left
					, iPosY + (iChildAlign==DT_INTERNAL?szAvailable:sz).cy + rcPadding.top };
				pControl->SetPos(rcCtrl, false);
			}

			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
		}
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;

		// Process the scrollbar
		scrollbars_set_cnt++;
		if (scrollbars_set_cnt<3) ProcessScrollBar(rc, cxNeeded, cyNeeded);
		scrollbars_set_cnt--;
	}

	//void CHorizontalLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	//{
	//	if( _tcsicmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
	//	else if( _tcsicmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcsicmp(pstrValue, _T("true")) == 0);
	//	else CContainerUI::SetAttribute(pstrName, pstrValue);
	//}
}
