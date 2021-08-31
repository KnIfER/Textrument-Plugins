#include "StdAfx.h"
#include "Core\InsituDebug.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL(WinButton)

	WinButton::WinButton()
		: CControlUI()
	{
		m_dwBackColor = RGB(0, 0, 255);
		_isDirectUI = true;
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
		_hParent = m_pParent->GetHWND();
		//LogIs("_hParent::%d", _hParent);
		_hWnd = ::CreateWindow(WC_BUTTON
			, TEXT("PUSHBUTTON")
			, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON
			, 0, 0
			, 64,64
			, _hParent
			, NULL
			, CPaintManagerUI::GetInstance()
			,  NULL) ;
		::ShowWindow(_hWnd, TRUE);
		::SetWindowText(_hWnd, TEXT("TEST"));
		if (!dynamic_cast<WinFrame*>(m_pParent))
		{
			GetRoot()->_WNDList.push_back(this);
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

		}
	}
	

} // namespace DuiLib
