#include "StdAfx.h"
#include "Core\InsituDebug.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL(WinFrame)

	class WndBase : public CWindowWnd
	{
	public:
		WndBase(){
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

		LPCTSTR GetWindowClassName() const{
			return L"WNDBASE";
		};

		void OnFinalMessage(HWND hWnd) {
		}

		//LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		//LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		WinFrame* m_pOwner;
		bool m_bInit;
	};

	WinFrame::WinFrame()
		: CContainerUI()
	{
		m_dwBackColor = RGB(0, 0, 255);
		_isDirectUI = true;
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

	void WinFrame::Init()
	{
		_hParent = m_pParent->GetHWND();
		//LogIs("_hParent::%d", _hParent);

		WndBase*_WND = new WndBase;

		_WND->Init(this);

		_hWnd = _WND->GetHWND();

		this->_WND = _WND;

		if (!dynamic_cast<WinFrame*>(m_pParent))
		{
			GetRoot()->_WNDList.push_back(this);
		}
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
