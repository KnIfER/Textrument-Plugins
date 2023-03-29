#include "StdAfx.h"
#include "../Core/InsituDebug.h"

namespace DuiLib
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_QKCONTROL(CContainerUI)

		CContainerUI::CContainerUI()
		: CControlUI(), m_iChildPadding(0),
		m_iChildAlign(DT_LEFT),
		m_iChildVAlign(DT_TOP),
		_clipchildren(true),
		m_bMouseChildEnabled(true),
		m_pVerticalScrollBar(NULL),
		m_pHorizontalScrollBar(NULL),
		m_nScrollStepSize(0)
		,scrollbars_set_cnt(0)
		,_scrollSpeed(0)
		,_scrollTarget(0)
		,_bUseSmoothScroll(false)
		,_smoothScrolling(false)
		,_scrollX(0)
		,_scrollY(0)
		,_bSupressingChildLayout(false)
		,_vscrollEatInset(true)
		,_hscrollEatInset(true)
	{
		_view_states |= VIEWSTATEMASK_IsViewGroup;
	}

	CContainerUI::~CContainerUI()
	{
		if( m_pVerticalScrollBar ) {
			delete m_pVerticalScrollBar;
			m_pVerticalScrollBar = NULL;
		}
		if( m_pHorizontalScrollBar ) {
			delete m_pHorizontalScrollBar;
			m_pHorizontalScrollBar = NULL;
		}
	}

	LPCTSTR CContainerUI::GetClass() const
	{
		return _T("ContainerUI");
	}

	LPVOID CContainerUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("IContainer")) == 0 ) return static_cast<IContainerUI*>(this);
		else if( _tcsicmp(pstrName, DUI_CTR_CONTAINER) == 0 ) return static_cast<CContainerUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	int CContainerUI::GetChildPadding() const
	{
		if (_manager) return _manager->GetDPIObj()->Scale(m_iChildPadding);
		return m_iChildPadding;
	}


	void CContainerUI::SetChildPadding(int iPadding)
	{
		m_iChildPadding = iPadding;
		NeedUpdate();
	}

	void CContainerUI::SetClipChildren(bool value, LPCTSTR handyStr)
	{
		if (handyStr)
		{
			value = _tcsicmp(handyStr, _T("true")) == 0;
		}
		_clipchildren = value;
		if (!handyStr) NeedUpdate();
	}

	UINT CContainerUI::GetChildAlign() const
	{
		return m_iChildAlign;
	}

	void CContainerUI::SetChildAlign(UINT iAlign)
	{
		m_iChildAlign = iAlign;
		NeedUpdate();
	}

	UINT CContainerUI::GetChildVAlign() const
	{
		return m_iChildVAlign;
	}

	void CContainerUI::SetChildVAlign(UINT iVAlign)
	{
		m_iChildVAlign = iVAlign;
		NeedUpdate();
	}

	bool CContainerUI::IsMouseChildEnabled() const
	{
		return m_bMouseChildEnabled;
	}

	void CContainerUI::SetMouseChildEnabled(bool bEnable)
	{
		m_bMouseChildEnabled = bEnable;
	}

	void CContainerUI::SetVisible(bool bVisible)
	{
		if( m_bVisible == bVisible ) return;
		CControlUI::SetVisible(bVisible);
		//for( int it = 0; it < m_items.GetSize(); it++ ) {
		//	static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
		//}
	}

	void CContainerUI::SetEnabled(bool bEnabled)
	{
		if( bEnabled != m_bEnabled  )
		{
			if(bEnabled) _view_states |= VIEWSTATEMASK_Enabled;
			else _view_states &= ~VIEWSTATEMASK_Enabled;

			// todo optimize
			for( int it = 0; it < m_items.GetSize(); it++ ) {
				static_cast<CControlUI*>(m_items[it])->SetEnabled(bEnabled);
			}

			Invalidate();
		}
	}

	void CContainerUI::SetMouseEnabled(bool bEnabled)
	{
		if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetMouseEnabled(bEnabled);
		if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetMouseEnabled(bEnabled);
		CControlUI::SetMouseEnabled(bEnabled);
	}

	static INT64 TicksLastDraw, TicksPerSecond;

	void CContainerUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( _parent != NULL ) _parent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused_YES;
			return;
		}
		if( event.Type == UIEVENT_TIMER ) 
		{
			if (event.wParam==0x100) //  && _bUseSmoothScroll
			{
				int scrollY = 0;
				if (_scrollY)
				{
					INT64 tk;
					::QueryPerformanceCounter((LARGE_INTEGER*)&tk);
					LogIs("time elapsed=%d", tk-TicksLastDraw);
					scrollY = _scrollY*10*1.2/25;
					scrollY = _scrollY*(10*1.2/31);
					//if(scrollY*_scrollY<0)LogIs("213213_scrollY=%d %d",_scrollY,scrollY);
					//scrollY = _scrollY>0?1:-1;
					//if(tk/10000<1000) {
					//	scrollY = (tk-TicksLastDraw)*1.0/10000/16*scrollY;
					//}
					TicksLastDraw = tk;

					if (std::abs(scrollY)<1)
					{
						scrollY = _scrollY>0?1:-1;
					}
					if ((_scrollY-scrollY>0) ^ (_scrollY>0))
					{
						scrollY = _scrollY;
						_scrollY = 0;
					}
					else
					{
						_scrollY -= scrollY;
					}
				}
				//if(scrollY*_scrollY<0) LogIs("213213");
				if(scrollY)
				DoScroll(0, scrollY);

				if (!_scrollY)
				{
					KillTimer(0x100);
				}
			}
			if (event.wParam==0x112) //  && _bUseSmoothScroll
			{
				SetPos(GetPos(), false);
				//KillTimer(0x112);
			}
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused_NO;
			return;
		}
		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled() )
		{
			if( event.Type == UIEVENT_KEYDOWN ) 
			{
				switch( event.chKey ) {
				case VK_DOWN:
					LineDown();
					return;
				case VK_UP:
					LineUp();
					return;
				case VK_NEXT:
					PageDown();
					return;
				case VK_PRIOR:
					PageUp();
					return;
				case VK_HOME:
					HomeUp();
					return;
				case VK_END:
					EndDown();
					return;
				}
			}
			else if( event.Type == UIEVENT_SCROLLWHEEL )
			{
				if (event.wParam&MK_SHIFT)
				{
					// scroll horizontally if possible.
				}
				else
				{
					if (!((short)HIWORD(event.wParam)<0?LineDown():LineUp()))
						CControlUI::DoEvent(event);
					return;
				}
			}
		}
		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled() ) {
			if( event.Type == UIEVENT_KEYDOWN ) 
			{
				switch( event.chKey ) {
				case VK_DOWN:
					LineRight();
					return;
				case VK_UP:
					LineLeft();
					return;
				case VK_NEXT:
					PageRight();
					return;
				case VK_PRIOR:
					PageLeft();
					return;
				case VK_HOME:
					HomeLeft();
					return;
				case VK_END:
					EndRight();
					return;
				}
			}
			else if( event.Type == UIEVENT_SCROLLWHEEL )
			{
				if ((short)HIWORD(event.wParam)<0?LineRight():LineLeft())
					return;
			}
		}
		CControlUI::DoEvent(event);
	}

	SIZE CContainerUI::GetScrollPos() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) sz.cy = m_pVerticalScrollBar->GetScrollPos();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) sz.cx = m_pHorizontalScrollBar->GetScrollPos();
		return sz;
	}

	SIZE CContainerUI::GetScrollRange() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) sz.cy = m_pVerticalScrollBar->GetScrollRange();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) sz.cx = m_pHorizontalScrollBar->GetScrollRange();
		return sz;
	}

	bool CContainerUI::SetScrollPos(SIZE szPos, bool bMsg, bool seeking)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( cx || cy) 
		{
			if (_bUseSmoothScroll)
			{
				_scrollX += cx;
				_scrollY += cy;
				SetTimer(0x100, 10);
			}
			else
			{
				DoScroll(cx, cy);
			}

			if(m_pVerticalScrollBar)
			{
				// 发送滚动消息
				if( _manager != NULL && bMsg ) {
					int nPage = (m_pVerticalScrollBar->GetScrollPos() + m_pVerticalScrollBar->GetLineSize()) / m_pVerticalScrollBar->GetLineSize();
					_manager->SendNotify(this, DUI_MSGTYPE_SCROLL, (WPARAM)nPage);
				}
			}
			return true;
		}
		return false;
	}

	void CContainerUI::DoScroll(int x, int y)
	{
		RECT rcPos;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() || pControl->IsFloat() ) continue;

			rcPos = pControl->GetPos();
			rcPos.left -= x;
			rcPos.right -= x;
			rcPos.top -= y;
			rcPos.bottom -= y;
			pControl->SetPos(rcPos, false);
		}

		Invalidate();
	}

	void CContainerUI::SetScrollStepSize(int nSize)
	{
		if (nSize >0)
			m_nScrollStepSize = nSize;
	}

	int CContainerUI::GetScrollStepSize() const
	{
		int stepSize = m_nScrollStepSize;
		if (stepSize == 0) {
			stepSize = 8;
			if( _manager ) stepSize = ::GetSystemMetrics(SM_CYVSCROLL) * 3 * 2 + 8;
		}
		//if(_manager )
		//	stepSize = _manager->GetDPIObj()->Scale(stepSize);

		return stepSize;

		//return ::GetSystemMetrics(SM_CYVSCROLL);
	}

	bool CContainerUI::LineUp()
	{
		SIZE sz = GetScrollPos();
		sz.cy -= GetScrollStepSize();
		return SetScrollPos(sz);
	}

	bool CContainerUI::LineDown()
	{
		SIZE sz = GetScrollPos();
		sz.cy += GetScrollStepSize();
		return SetScrollPos(sz);
	}

	void CContainerUI::PageUp()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy -= iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::PageDown()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy += iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::HomeUp()
	{
		SIZE sz = GetScrollPos();
		sz.cy = 0;
		SetScrollPos(sz);
	}

	void CContainerUI::EndDown()
	{
		if(_manager) {
			::UpdateWindow(_manager->GetPaintWindow());
		}
		SIZE sz = GetScrollPos();
		sz.cy = GetScrollRange().cy;
		SetScrollPos(sz);
	}

	bool CContainerUI::LineLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx -= GetScrollStepSize();
		return SetScrollPos(sz);
	}

	bool CContainerUI::LineRight()
	{
		SIZE sz = GetScrollPos();
		sz.cx += GetScrollStepSize();
		return SetScrollPos(sz);
	}

	void CContainerUI::PageLeft()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx -= iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::PageRight()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx += iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::HomeLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx = 0;
		SetScrollPos(sz);
	}

	void CContainerUI::EndRight()
	{
		if(_manager) {
			::UpdateWindow(_manager->GetPaintWindow());
		}
		SIZE sz = GetScrollPos();
		sz.cx = GetScrollRange().cx;
		SetScrollPos(sz);
	}

	void CContainerUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		if( bEnableVertical && !m_pVerticalScrollBar ) {
			m_pVerticalScrollBar = new CScrollBarUI;
			m_pVerticalScrollBar->SetOwner(this);
			m_pVerticalScrollBar->SetManager(_manager, NULL, false);
			if ( _manager ) {
				Style* pDefaultAttributes = _manager->GetDefaultAttributeList(_T("VScrollBar"));
				if( pDefaultAttributes ) {
					m_pVerticalScrollBar->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableVertical && m_pVerticalScrollBar ) {
			delete m_pVerticalScrollBar;
			m_pVerticalScrollBar = NULL;
		}

		if( bEnableHorizontal && !m_pHorizontalScrollBar ) {
			m_pHorizontalScrollBar = new CScrollBarUI;
			m_pHorizontalScrollBar->SetHorizontal(true);
			m_pHorizontalScrollBar->SetOwner(this);
			m_pHorizontalScrollBar->SetManager(_manager, NULL, false);

			if ( _manager ) {
				Style* pDefaultAttributes = _manager->GetDefaultAttributeList(_T("HScrollBar"));
				if( pDefaultAttributes ) {
					m_pHorizontalScrollBar->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableHorizontal && m_pHorizontalScrollBar ) {
			delete m_pHorizontalScrollBar;
			m_pHorizontalScrollBar = NULL;
		}

		NeedUpdate();
	}

	CScrollBarUI* CContainerUI::GetVerticalScrollBar() const
	{
		return m_pVerticalScrollBar;
	}

	CScrollBarUI* CContainerUI::GetHorizontalScrollBar() const
	{
		return m_pHorizontalScrollBar;
	}

	int CContainerUI::FindSelectable(int iIndex, bool bForward /*= true*/) const
	{
		// NOTE: This is actually a helper-function for the list/combo/ect controls
		//       that allow them to find the next enabled/available selectable item
		if( GetCount() == 0 ) return -1;
		iIndex = CLAMP(iIndex, 0, GetCount() - 1);
		if( bForward ) {
			for( int i = iIndex; i < GetCount(); i++ ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return -1;
		}
		else {
			for( int i = iIndex; i >= 0; --i ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return FindSelectable(0, true);
		}
	}

	RECT CContainerUI::GetClientPos() const
	{
		RECT rc = m_rcItem;
		ApplyInsetToRect(rc);

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			rc.top -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom += m_pVerticalScrollBar->GetScrollRange();
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			rc.left -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right += m_pHorizontalScrollBar->GetScrollRange();
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}
		return rc;
	}

	void CContainerUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CControlUI::Move(szOffset, bNeedInvalidate);
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) m_pVerticalScrollBar->Move(szOffset, false);
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) m_pHorizontalScrollBar->Move(szOffset, false);
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( pControl != NULL && pControl->IsVisible() ) pControl->Move(szOffset, false);
		}
	}

	void CContainerUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		if( m_items.IsEmpty() ) return;

		rc = m_rcItem;
		ApplyInsetToRect(rc);

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			rc.top -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom += m_pVerticalScrollBar->GetScrollRange();
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			rc.left -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right += m_pHorizontalScrollBar->GetScrollRange();
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
			}
			else { 
				SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
				pControl->SetPos(rcCtrl, false);
			}
		}
	}

	void CContainerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("mousechild")) == 0 ) SetMouseChildEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("vscrollbar")) == 0 ) {
			EnableScrollBar(_tcsicmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);
		}
		else if( _tcsicmp(pstrName, _T("vscrollbarstyle")) == 0 ) {
			m_sVerticalScrollBarStyle = pstrValue;
			EnableScrollBar(TRUE, GetHorizontalScrollBar() != NULL);
			if( GetVerticalScrollBar() ) {
				Style* pStyle = _manager->GetStyleForId(m_sVerticalScrollBarStyle);
				if( pStyle ) {
					GetVerticalScrollBar()->ApplyAttributeList(pStyle);
				}
				else {
					GetVerticalScrollBar()->ApplyAttributeList(pstrValue);
				}
			}
		}
		else if( _tcsicmp(pstrName, _T("hscrollbar")) == 0 ) {
			EnableScrollBar(GetVerticalScrollBar() != NULL, _tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("hscrollbarstyle")) == 0 ) {
			m_sHorizontalScrollBarStyle = pstrValue;
			//todo 
			EnableScrollBar(TRUE, GetHorizontalScrollBar() != NULL);
			if( GetHorizontalScrollBar() ) {
				Style* pStyle = _manager->GetStyleForId(m_sHorizontalScrollBarStyle);
				if( pStyle ) {
					GetHorizontalScrollBar()->ApplyAttributeList(pStyle);
				}
				else {
					GetHorizontalScrollBar()->ApplyAttributeList(pstrValue);
				}
			}
		}
		else if( _tcsicmp(pstrName, _T("childpadding")) == 0 ) SetChildPadding(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("clipchild")) == 0 ) SetClipChildren(true, pstrValue);
		else if( _tcscmp(pstrName, _T("childalign")) == 0 ) {
			if( _tcscmp(pstrValue, _T("left")) == 0 ) m_iChildAlign = DT_LEFT;
			else if( _tcscmp(pstrValue, _T("center")) == 0 ) m_iChildAlign = DT_CENTER;
			else if( _tcscmp(pstrValue, _T("right")) == 0 ) m_iChildAlign = DT_RIGHT;
			else if( _tcscmp(pstrValue, _T("fit")) == 0 ) m_iChildAlign = DT_INTERNAL;
		}
		else if( _tcscmp(pstrName, _T("childvalign")) == 0 ) {
			if( _tcscmp(pstrValue, _T("top")) == 0 ) m_iChildVAlign = DT_TOP;
			else if( _tcscmp(pstrValue, _T("vcenter")) == 0 ) m_iChildVAlign = DT_VCENTER;
			else if( _tcscmp(pstrValue, _T("bottom")) == 0 ) m_iChildVAlign = DT_BOTTOM;
			else if( _tcscmp(pstrValue, _T("fit")) == 0 ) m_iChildVAlign = DT_INTERNAL;
		}
		else if( _tcsicmp(pstrName, _T("scrollstepsize")) == 0 ) SetScrollStepSize(_ttoi(pstrValue));
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CContainerUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit, bool setChild)
	{
		_manager = pManager;
		if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetManager(pManager, this, bInit, setChild);
		if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetManager(pManager, this, bInit, setChild);
		CControlUI::SetManager(pManager, pParent, bInit, setChild);
	}

	CControlUI* CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
		if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
		if( (uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
		if( (uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL ) return NULL;

		CControlUI* pResult = NULL;
		if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
		}
		if( pResult == NULL && m_pVerticalScrollBar != NULL ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags&~UIFIND_FOCUSABLE);
		}
		if( pResult == NULL && m_pHorizontalScrollBar != NULL ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags&~UIFIND_FOCUSABLE);
		}
		if( pResult != NULL ) return pResult;

		if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled() ) {
			RECT rc = m_rcItem;
			ApplyInsetToRect(rc);
			bool topFirst = uFlags & UIFIND_TOP_FIRST;
			int length = m_items.GetSize() - 1;

			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

			for( int it = length; it >= 0; it-- ) {
				pResult = static_cast<CControlUI*>(m_items[topFirst?it:(length-it)])->FindControl(Proc, pData, uFlags);
				if( pResult != NULL ) {
					if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
						continue;
					else 
						return pResult;
				}          
			}
		}

		//if( (uFlags & UIFIND_FOCUSABLE) != 0 && !m_bFocusable ) return NULL;
		if( (uFlags & UIFIND_HITTEST) != 0 && !m_bMouseEnabled ) return NULL;
		pResult = NULL;
		if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
		}
		return pResult;
	}

	bool CContainerUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		if (!m_bIsDirectUI)
		{
			return true;
		}
		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return true;

		bool clipchildren = _clipchildren;
		CRenderClip clip;
		if(clipchildren)CRenderClip::GenerateClip(hDC, rcTemp, clip);
		CControlUI::DoPaint(hDC, rcPaint, pStopControl);

		if( m_items.GetSize() > 0 ) {
			RECT rc = m_rcItem;
			if(clipchildren)
				ApplyInsetToRect(rc);

			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

			if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( pControl == pStopControl ) return false;
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
					}
				}
			}
			else {
				CRenderClip childClip;
				if(clipchildren)
				{
#ifdef MODULE_SKIA_RENDERER
					//_manager->GetSkiaCanvas()->save();
					//SkRect rect = {rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.bottom};
					//_manager->GetSkiaCanvas()->clipRect(rect);
#endif
					CRenderClip::GenerateClip(hDC, rcTemp, childClip);
				}
				//bool stopped = false; // { stopped=true; break; }
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( pControl == pStopControl ) return false;
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						if(clipchildren)CRenderClip::UseOldClipBegin(hDC, childClip);
						if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
						if(clipchildren)CRenderClip::UseOldClipEnd(hDC, childClip);
					}
					else {
						if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
						if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
					}
				}
#ifdef MODULE_SKIA_RENDERER
				//if(clipchildren)
				//{
				//	_manager->GetSkiaCanvas()->restore();
				//}
#endif
			}
		}

		if( m_pVerticalScrollBar != NULL && (LONG_PTR)m_pVerticalScrollBar != 0xdddddddd && m_pVerticalScrollBar->IsVisible() ) {
			if( m_pVerticalScrollBar == pStopControl ) return false;
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
				if( !m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
			}
		}

		if( m_pHorizontalScrollBar != NULL && (LONG_PTR)m_pHorizontalScrollBar != 0xdddddddd  && m_pHorizontalScrollBar->IsVisible() ) {
			if( m_pHorizontalScrollBar == pStopControl ) return false;
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
				if( !m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
			}
		}
		return true;
	}

	void CContainerUI::SetFloatPos(int iIndex)
	{
		// 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return;

		CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);

		if( !pControl->IsVisible() ) return;
		if( !pControl->IsFloat() ) return;

		SIZE szXY = pControl->GetFixedXY();
		SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};
		if(sz.cx==-1)
		{
			sz.cx = GetWidth();
		}
		if(sz.cy==-1)
		{
			sz.cy = GetHeight();
		}
		int nParentWidth = m_rcItem.right - m_rcItem.left;
		int nParentHeight = m_rcItem.bottom - m_rcItem.top;
		if(sz.cx==-2 || sz.cy==-2)
		{
			SIZE szEst = pControl->EstimateSize({nParentWidth, nParentHeight});
			if(pControl->GetAutoCalcWidth()) sz.cx = szEst.cx;
			if(pControl->GetAutoCalcHeight()) sz.cy = szEst.cy;
		}

		UINT uAlign = pControl->GetFloatAlign();
		if(uAlign != 0) {
			RECT rcCtrl = {0, 0, sz.cx, sz.cy};
			if((uAlign & DT_CENTER) != 0) {
				::OffsetRect(&rcCtrl, (nParentWidth - sz.cx) / 2, 0);
			}
			else if((uAlign & DT_RIGHT) != 0) {
				::OffsetRect(&rcCtrl, nParentWidth - sz.cx, 0);
			}
			else {
				::OffsetRect(&rcCtrl, szXY.cx, 0);
			}

			if((uAlign & DT_VCENTER) != 0) {
				::OffsetRect(&rcCtrl, 0, (nParentHeight - sz.cy) / 2);
			}
			else if((uAlign & DT_BOTTOM) != 0) {
				::OffsetRect(&rcCtrl, 0, nParentHeight - sz.cy);
			}
			else {
				::OffsetRect(&rcCtrl, 0, szXY.cy);
			}
			::OffsetRect(&rcCtrl, m_rcItem.left, m_rcItem.top);
			//PRINT_RECT(SetFloatPos, rcCtrl, 1);
			pControl->SetPos(rcCtrl, false);
		}
		else {
			TPercentInfo rcPercent = pControl->GetFloatPercent();
			LONG width = m_rcItem.right - m_rcItem.left;
			LONG height = m_rcItem.bottom - m_rcItem.top;
			LONG left = szXY.cx < 0 ? m_rcItem.right : m_rcItem.left;
			LONG top = szXY.cy < 0 ? m_rcItem.bottom : m_rcItem.top;
			RECT rcCtrl = { 0 };
			rcCtrl.left = (LONG)(width*rcPercent.left) + szXY.cx + left;
			rcCtrl.top = (LONG)(height*rcPercent.top) + szXY.cy + top;
			rcCtrl.right = (LONG)(width*rcPercent.right) + szXY.cx + sz.cx + left;
			rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY.cy + sz.cy + top;
			pControl->SetPos(rcCtrl, false);
		}
	}

	void CContainerUI::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
	{
		while (m_pHorizontalScrollBar)
		{
			int available = rc.right - rc.left;
			// Scroll needed
			if (cxRequired > available && !m_pHorizontalScrollBar->IsVisible())
			{
				m_pHorizontalScrollBar->SetVisible(true);
				m_pHorizontalScrollBar->SetScrollRange(cxRequired - available);
				m_pHorizontalScrollBar->SetScrollPos(0);
				SetPos(m_rcItem);
				break;
			}

			// No scrollbar required
			if (!m_pHorizontalScrollBar->IsVisible()) break;

			// Scroll not needed anymore?
			int cxScroll = cxRequired - available;
			if ( cxScroll <= 0)
			{
				m_pHorizontalScrollBar->SetVisible(false);
				m_pHorizontalScrollBar->SetScrollPos(0);
				m_pHorizontalScrollBar->SetScrollRange(0);
				SetPos(m_rcItem);
				break;
			}

			//ReAddInsetToRect(rc);
			//RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
			RECT rcScrollBarPos = { m_rcItem.left, m_rcItem.bottom - m_pHorizontalScrollBar->GetFixedHeight(), m_rcItem.right, m_rcItem.bottom };

			if (_borderSizeType)
			{
				rcScrollBarPos.top -=    _borderInsetScaled.bottom + _rcBorderSizeScaled.bottom;
				rcScrollBarPos.bottom -= _borderInsetScaled.bottom + _rcBorderSizeScaled.bottom;
				rcScrollBarPos.left +=   _borderInsetScaled.left + _rcBorderSizeScaled.left;
				rcScrollBarPos.right -=  _borderInsetScaled.right + _rcBorderSizeScaled.right;
			}
			if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
			{
				rcScrollBarPos.right -=  m_pVerticalScrollBar->GetFixedWidth();
			}
			//ApplyInsetToRect(rc);
			m_pHorizontalScrollBar->SetPos(rcScrollBarPos);

			if (m_pHorizontalScrollBar->GetScrollRange() != cxScroll) 
			{
				int iScrollPos = m_pHorizontalScrollBar->GetScrollPos();
				m_pHorizontalScrollBar->SetScrollRange(::abs(cxScroll)); // if scrollpos>range then scrollpos=range
				if(iScrollPos > m_pHorizontalScrollBar->GetScrollPos()) 
				{
					SetPos(m_rcItem);
				}
			}

			break;
		}

		while (m_pVerticalScrollBar)
		{
			// Scroll needed
			int available = rc.bottom - rc.top;
			if (cyRequired > available && !m_pVerticalScrollBar->IsVisible()) 
			{
				m_pVerticalScrollBar->SetVisible(true);
				m_pVerticalScrollBar->SetScrollRange(cyRequired - (available));
				m_pVerticalScrollBar->SetScrollPos(0);
				SetPos(m_rcItem);
				break;
			}

			// No scrollbar required
			if (!m_pVerticalScrollBar->IsVisible()) break;

			// Scroll not needed anymore?
			int cyScroll = cyRequired - (available);
			if (cyScroll <= 0) 
			{
				m_pVerticalScrollBar->SetVisible(false);
				m_pVerticalScrollBar->SetScrollPos(0);
				m_pVerticalScrollBar->SetScrollRange(0);
				SetPos(m_rcItem);
				break;
			}

			//ReAddInsetToRect(rc);
			//RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
			RECT rcScrollBarPos = { m_rcItem.right-m_pVerticalScrollBar->GetFixedWidth(), m_rcItem.top, m_rcItem.right, m_rcItem.bottom };

			if (_borderSizeType)
			{
				rcScrollBarPos.left -=   _borderInsetScaled.right + _rcBorderSizeScaled.right;
				rcScrollBarPos.right -=  _borderInsetScaled.right + _rcBorderSizeScaled.right;
				rcScrollBarPos.top +=    _borderInsetScaled.top + _rcBorderSizeScaled.top;
				rcScrollBarPos.bottom -= _borderInsetScaled.bottom + _rcBorderSizeScaled.bottom;
			}
			if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
			{
				rcScrollBarPos.bottom -=  m_pHorizontalScrollBar->GetFixedHeight();
			}
			m_pVerticalScrollBar->SetPos(rcScrollBarPos);
			//LogIs("rcScrollBarPos::%ld, %ld, %ld, %ld",  rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom);
			//LogIs("m_rcItem::%ld, %ld, %ld, %ld",  m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);

			if (m_pVerticalScrollBar->GetScrollRange() != cyScroll)
			{
				int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
				m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll)); // if scrollpos>range then scrollpos=range
				if(iScrollPos > m_pVerticalScrollBar->GetScrollPos()) 
				{
					SetPos(m_rcItem);
				}
			}
			break;
		}
	}

	bool CContainerUI::SetSubControlText( LPCTSTR pstrSubControlName,LPCTSTR pstrText )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL) {
			pSubControl->SetText(pstrText);
			return TRUE;
		}
		else return FALSE;
	}

	bool CContainerUI::SetSubControlFixedHeight( LPCTSTR pstrSubControlName,int cy )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL) {
			pSubControl->SetFixedHeight(cy);
			return TRUE;
		}
		else return FALSE;
	}

	bool CContainerUI::SetSubControlFixedWdith( LPCTSTR pstrSubControlName,int cx )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL) {
			pSubControl->SetFixedWidth(cx);
			return TRUE;
		}
		else return FALSE;
	}

	bool CContainerUI::SetSubControlUserData( LPCTSTR pstrSubControlName,LPCTSTR pstrText )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL) {
			pSubControl->SetUserData(pstrText);
			return TRUE;
		}
		else return FALSE;
	}

	DuiLib::QkString CContainerUI::GetSubControlText( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetText();
	}

	int CContainerUI::GetSubControlFixedHeight( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL) return -1;
		else return pSubControl->GetFixedHeight();
	}

	int CContainerUI::GetSubControlFixedWdith( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL) return -1;
		else return pSubControl->GetFixedWidth();
	}

	const QkString CContainerUI::GetSubControlUserData( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL) return _T("");
		else return pSubControl->GetUserData();
	}

	CControlUI* CContainerUI::FindSubControl( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl = static_cast<CControlUI*>(GetManager()->FindSubControlByName(this, pstrSubControlName));
		return pSubControl;
	}

} // namespace DuiLib
