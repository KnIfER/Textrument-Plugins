#include "StdAfx.h"
#include "Core\InsituDebug.h"

void BUTTON_Register(HINSTANCE hInst);

namespace DuiLib {
	IMPLEMENT_DUICONTROL(WinButton)

	WinButton::WinButton()
		: CControlUI()
	{
		m_dwBackColor = RGB(0, 0, 255);
		_isDirectUI = false;
	}

	LPCTSTR WinButton::GetClass() const
	{
		return L"WinButton";
	}

	LPVOID WinButton::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, L"WinButton") == 0 ) return static_cast<WinButton*>(this);
		return __super::GetInterface(pstrName);
	}

	void WinButton::Init()
	{
		if (!_hWnd)
		{
			BUTTON_Register(CPaintManagerUI::GetInstance());
			//_isDirectUI = false;
			__hParent = _hParent;
			_hWnd = ::CreateWindow(
				//WC_BUTTON
				TEXT("MyBUTTON")
				//, TEXT("PUSHBUTTON")
				, TEXT("MyBUTTON")
				, WS_CHILD 
				| WS_VISIBLE 
				//| BS_CHECKBOX //| BS_PUSHLIKE
				, 0, 0
				, 64,64
				, _hParent
				, NULL
				, CPaintManagerUI::GetInstance()
				,  NULL) ;
			::ShowWindow(_hWnd, SW_SHOW);
			::SetWindowText(_hWnd, TEXT("TEST"));
		}
		if (_hParent != __hParent)
		{
			__hParent = _hParent;
			::SetParent(_hWnd, _hParent);
			::InvalidateRect(_hWnd, NULL, TRUE);
		}
	}

	void WinButton::SetPos(RECT rc, bool bNeedInvalidate) 
	{
		//__super::SetPos(rc, bNeedInvalidate);

		m_rcItem = rc;
		resize();
	}

	void WinButton::resize() 
	{
		if(_hWnd) {
			RECT rcPos = m_rcItem;

			//::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			
			::MoveWindow(_hWnd, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, TRUE);
			//::UpdateWindow(_hWnd);
		}
	}
	

} // namespace DuiLib
