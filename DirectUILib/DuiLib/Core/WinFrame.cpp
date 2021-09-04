#include "StdAfx.h"
#include "Core\InsituDebug.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL(WinFrame)

	class WndBase : public CWindowWnd, public INotifyUI
	{
	public:
		WndBase(){
		};
		LPCTSTR GetWindowClassName() const { 
			return _T("WndFrame"); 
		};
		UINT GetClassStyle() const {
			return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; 
		};

		void Init(WinFrame* pOwner) {
			m_pOwner = pOwner;
			RECT rcPos = pOwner->GetPos();
			UINT uStyle = uStyle = WS_CHILD;
			Create(m_pOwner->GetParent()->GetHWND(), NULL, uStyle, 0, rcPos);

			//Styls
			//LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
			//styleValue |= pOwner->GetWindowStyls();
			//::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);

			//Styls
			::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
			::SetFocus(m_hWnd);

			m_bInit = true;
		};

		void Notify(TNotifyUI& msg){
			if( msg.sType == _T("click") ) 
			{
			}
		}

		void draw(CControlUI* UI){
			if (UI)
			{
				UI->SetManager(&m_pm, NULL, false);
				m_pm.AttachDialog(UI);
				m_pm.AddNotifier(this);
				m_pOwner->SetAutoDestroy(false);
				m_pOwner->RemoveAll();
				UI->SetManager(&m_pm, NULL, 1);
			}
		}

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam){
			if( uMsg == WM_CREATE ) {
				m_pm.Init(m_hWnd);
				m_pm.reInit(m_pOwner->GetManager());
				//draw(m_pOwner);
				draw(m_pOwner->GetItemAt(0));
				::UpdateWindow(m_hWnd);
				return 0;
			}
			else if( isModal && uMsg == WM_DESTROY ) 
			{
			}
			else if( uMsg == WM_CLOSE  ) {
			}
			else if( uMsg == WM_NCACTIVATE ) {
				//if( !::IsIconic(*this) ) return (wParam == 0) ? TRUE : FALSE;
			}
			LRESULT lRes = 0;
			if( m_pm.GetRoot() && m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
			return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		}

		void OnFinalMessage(HWND hWnd) {
		}

		//LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		//LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//protected:
		CPaintManagerUI m_pm;
		WinFrame* m_pOwner;
		bool m_bInit;
	};

	WinFrame::WinFrame()
		: CContainerUI()
	{
		m_dwBackColor = RGB(0, 0, 255);
		_isDirectUI = false;
	}

	LPCTSTR WinFrame::GetClass() const
	{
		return L"WinFrame";
	}

	LPVOID WinFrame::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, L"WinFrame") == 0 ) return static_cast<WinFrame*>(this);
		return __super::GetInterface(pstrName);
	}

	bool WinFrame::Add(CControlUI* pControl)
	{
		bool ret = __super::Add(pControl);
		//if (ret)
		//{
		//	((WndBase*)_WND)->draw(pControl);
		//}
		return ret;   
	}

	void WinFrame::Init()
	{
		_hParent = m_pParent->GetHWND();
		//LogIs("_hParent::%d", _hParent);

		WndBase*_WND = new WndBase;

		_WND->Init(this);

		_hWnd = _WND->GetHWND();

		this->_WND = _WND;

		//if (!dynamic_cast<WinFrame*>(m_pParent))
		//{
		//	GetRoot()->_WNDList.push_back(this);
		//}
	}

	bool WinFrame::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		if (!_isDirectUI)
		{
			return true;
		}

		//::OffsetRect((LPRECT)&rcPaint,  -GetX(),  -GetY());

		//::OffsetRect((LPRECT)&rcPaint, 0, 10);


		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return true;
		
		CRenderClip clip;
		CRenderClip::GenerateClip(hDC, rcTemp, clip);
		CControlUI::DoPaint(hDC, rcPaint, pStopControl);

		if( m_items.GetSize() > 0 ) {
			RECT rcInset = GetInset();
			RECT rc = m_rcItem;
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
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
				CRenderClip::GenerateClip(hDC, rcTemp, childClip);
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( pControl == pStopControl ) return false;
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						CRenderClip::UseOldClipBegin(hDC, childClip);
						if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
						CRenderClip::UseOldClipEnd(hDC, childClip);
					}
					else {
						if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
						if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
					}
				}
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

	void WinFrame::SetPos(RECT rc, bool bNeedInvalidate) 
	{
		//__super::SetPos(rc, bNeedInvalidate);
		m_rcItem = rc;

		//if (_hWnd!=_hParent)
		if (::IsWindow(::GetParent(_hWnd)))
		{
			::MoveWindow(_hWnd, rc.left, rc.top, rc.right - rc.left, 
				rc.bottom - rc.top, FALSE);
			((WndBase*)_WND)->m_pm.GetRoot()->SetPos({0, 0, rc.right - rc.left, rc.bottom - rc.top});
		}

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( pControl->IsVisible() )  //   && pControl->IsDirectUI()
			{
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
	}

	void WinFrame::resize() 
	{
		if(_hWnd) {
			RECT rc = m_rcItem;
			//::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
			//	rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			
			::MoveWindow(_hWnd, rc.left, rc.top, rc.right - rc.left, 
				rc.bottom - rc.top, FALSE);

			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( pControl->IsVisible() && !pControl->IsDirectUI() ) {
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
		}
	}
	

} // namespace DuiLib
