#include "StdAfx.h"
#include "UIVerticalLayout.h"
#include "core/InsituDebug.h"

namespace DuiLib
{
	IMPLEMENT_QKCONTROL(CVerticalLayoutUI)

	CVerticalLayoutUI::CVerticalLayoutUI() : CContainerUI()
	{
	}

	LPCTSTR CVerticalLayoutUI::GetClass() const
	{
		return _T("VerticalLayoutUI");
	}

	LPVOID CVerticalLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_VERTICALLAYOUT) == 0 ) return static_cast<CVerticalLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	SIZE CVerticalLayoutUI::EstimateSize(const SIZE & szAvailable)
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
			int estimateCy = 0;
			
			int cxNeeded = 0;
			int nAdjustables = 0;
			int cyFixed = 0;
			int nEstimateNum = 0;
			SIZE szControlAvailable;
			for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
				if (pControl->IsVisible() && !pControl->IsFloat())
				{
					szControlAvailable = szAvailable;
					RECT rcPadding = pControl->GetPadding();
					szControlAvailable.cx -= rcPadding.left + rcPadding.right;
					if (szControlAvailable.cx > pControl->GetMaxAvailWidth()) szControlAvailable.cx = pControl->GetMaxAvailWidth();
					if (szControlAvailable.cy > pControl->GetMaxAvailHeight()) szControlAvailable.cy = pControl->GetMaxAvailHeight();
					
					SIZE sz = pControl->EstimateSize(szControlAvailable);

					if( sz.cy == 0 ) {
						nAdjustables++;
					}
					else {
						if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
						if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
					}
					cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;

					sz.cx = MAX(sz.cx, 0);
					if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
					if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
					cxNeeded = MAX(cxNeeded, sz.cx + rcPadding.left + rcPadding.right);
					nEstimateNum++;
				}
			}
			cyFixed += (nEstimateNum - 1) * m_iChildPadding;
			cyFixed += rcInnerPadding.top + rcInnerPadding.bottom;
			cxNeeded += rcInnerPadding.left + rcInnerPadding.right;
			m_cxyFixedLast.cy = m_bAutoCalcHeight?cyFixed:max(cyFixed,availHeight);
			m_cxyFixedLast.cx = m_bAutoCalcWidth?cyFixed:max(cxNeeded,availWidth);

		}
		return m_cxyFixedLast;
	}

	void CVerticalLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;

		ApplyInsetToRect(rc);
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		if( m_items.GetSize() == 0 || _bSupressingChildLayout) {
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

		// WTF???
		/////////////////////////////////////////////////
		//if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		//	szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
		//if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
		//	szAvailable.cy += m_pVerticalScrollBar->GetScrollRange();
		//
		//LogIs("rc::%d %d %d", rc.bottom , rc.top, szAvailable.cy);
		///////////////////////////////////////////////

		int cxNeeded = 0;
		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		SIZE szControlAvailable;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			szControlAvailable = szAvailable;
			RECT rcPadding = pControl->GetPadding();
			szControlAvailable.cx -= rcPadding.left + rcPadding.right;
			if (szControlAvailable.cx > pControl->GetMaxAvailWidth()) szControlAvailable.cx = pControl->GetMaxAvailWidth();
			if (szControlAvailable.cy > pControl->GetMaxAvailHeight()) szControlAvailable.cy = pControl->GetMaxAvailHeight();
			
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cy == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;

			sz.cx = MAX(sz.cx, 0);
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			cxNeeded = MAX(cxNeeded, sz.cx + rcPadding.left + rcPadding.right);
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			iPosY -= m_pVerticalScrollBar->GetScrollPos();
		}
		else {
			// 子控件垂直对其方式
			if(nAdjustables <= 0) {
				UINT iChildAlign = GetChildVAlign(); 
				if (iChildAlign == DT_VCENTER) {
					iPosY += (szAvailable.cy -cyFixed) / 2;
				}
				else if (iChildAlign == DT_BOTTOM) {
					iPosY += (szAvailable.cy - cyFixed);
				}
			}
		}
		int iEstimate = 0;
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}

			iEstimate += 1;
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cy -= rcPadding.top;

			szControlAvailable = szRemaining;
			szControlAvailable.cx -= rcPadding.left + rcPadding.right;
			if (szControlAvailable.cx > pControl->GetMaxAvailWidth()) szControlAvailable.cx = pControl->GetMaxAvailWidth();
			if (szControlAvailable.cy > pControl->GetMaxAvailHeight()) szControlAvailable.cy = pControl->GetMaxAvailHeight();
			cyFixedRemaining = cyFixedRemaining - (rcPadding.top + rcPadding.bottom);
			if (iEstimate > 1) cyFixedRemaining = cyFixedRemaining - m_iChildPadding;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cy == 0 ) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cy = MAX(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				} 
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = MAX(sz.cx, 0);
			if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if( !m_pHorizontalScrollBar && sz.cx > szControlAvailable.cx ) sz.cx = szControlAvailable.cx;
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();

			UINT iChildAlign = GetChildAlign(); 
			if (iChildAlign == DT_CENTER) 
			{
				int iPosX = (rc.right + rc.left) / 2;
				if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
					iPosX += m_pHorizontalScrollBar->GetScrollRange() / 2;
					iPosX -= m_pHorizontalScrollBar->GetScrollPos();
				}
				RECT rcCtrl = { iPosX - sz.cx/2, iPosY + rcPadding.top, iPosX + sz.cx - sz.cx/2, iPosY + sz.cy + rcPadding.top };
				pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_RIGHT) 
			{
				int iPosX = rc.right;
				if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
					iPosX += m_pHorizontalScrollBar->GetScrollRange();
					iPosX -= m_pHorizontalScrollBar->GetScrollPos();
				}
				RECT rcCtrl = { iPosX - rcPadding.right - sz.cx, iPosY + rcPadding.top, iPosX - rcPadding.right, iPosY + sz.cy + rcPadding.top };
				pControl->SetPos(rcCtrl, false);
			}
			else 
			{
				int iPosX = rc.left;
				if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
					iPosX -= m_pHorizontalScrollBar->GetScrollPos();
				}
				RECT rcCtrl = { iPosX + rcPadding.left
					, iPosY + rcPadding.top
					, iPosX + rcPadding.left + (iChildAlign==DT_INTERNAL?szAvailable:sz).cx
					, iPosY + sz.cy + rcPadding.top };
				pControl->SetPos(rcCtrl, false);
			}

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

		// Process the scrollbar
		scrollbars_set_cnt++;
		if (scrollbars_set_cnt<3) ProcessScrollBar(rc, cxNeeded, cyNeeded);
		scrollbars_set_cnt--;
	}

	//void CVerticalLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	//{
	//	CContainerUI::SetAttribute(pstrName, pstrValue);
	//}
}
