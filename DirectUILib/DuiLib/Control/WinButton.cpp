#include "StdAfx.h"

#ifdef QkWinButton
#include "Core\InsituDebug.h"

#include "WindowsEx\edit.h"
#include "WindowsEx\combo.h"

void BUTTON_Register();

namespace DuiLib {
	IMPLEMENT_QKCONTROL(WinButton)

	WinButton::WinButton()
		: CControlUI()
	{
		m_dwBackColor = RGB(0, 0, 255);
		_view_states &= ~VIEWSTATEMASK_IsDirectUI;
		_type = 0;
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
		if (!_hWnd && _hParent)
		{
			BUTTON_Register();
			EDIT_Register();
			COMBO_Register();
			//_isDirectUI = false;
			__hParent = _hParent;
			if(1)
			_hWnd = ::CreateWindow(
				WC_BUTTON
				//TEXT("MyBUTTON")
				//, TEXT("PUSHBUTTON")
				, TEXT("MyBUTTON")
				, WS_CHILD 
				| WS_VISIBLE 
		//		| BS_MULTILINE  
				//| BS_CENTER 
				| BS_VCENTER 
				| BS_CENTER 
				| _type
				//| 0x0000000EL // BS_COMMANDLINK 
				//| BS_AUTO3STATE 
				//| BS_AUTOCHECKBOX  | BS_PUSHLIKE
				, 0, 0
				, 64,64
				, _hParent
				, NULL
				, CPaintManagerUI::GetInstance()
				,  NULL) ;
			if(0)
			_hWnd = ::CreateWindow(
				//WC_COMBOBOX
				//WC_EDIT
				TEXT("MyEdit")
				//TEXT("MyBUTTON")
				//, TEXT("PUSHBUTTON")
				, TEXT("MyBUTTON")
				, WS_CHILD 
				| WS_VISIBLE 
				| ES_MULTILINE  
				//| WS_VSCROLL  
				| ES_AUTOHSCROLL  
				| ES_AUTOVSCROLL  
				//| WS_BORDER  
				| WS_HSCROLL  
				| WS_VSCROLL  
				//| 0x00000200  
				, 0, 0
				, 64,64
				, _hParent
				, NULL
				, CPaintManagerUI::GetInstance()
				,  NULL) ;
			if(false)
			_hWnd = ::CreateWindow(
				//WC_COMBOBOX
				TEXT("MyComboBox")
				, TEXT("")
				, WS_CHILD 
				| WS_VISIBLE 

				| CBS_DROPDOWN | CBS_HASSTRINGS

				//| CBS_DROPDOWNLIST


				//| CBS_SIMPLE

				, 0, 0
				, 64,64
				, _hParent
				, NULL
				, CPaintManagerUI::GetInstance()
				,  NULL) ;

			TCHAR szMessage[20] = L"Hello";
			ComboBox_AddString(_hWnd, szMessage);
			ComboBox_AddString(_hWnd, szMessage);
			ComboBox_AddString(_hWnd, szMessage);

			//SendMessage(_hWnd, BCM_FIRST + 0x0009, 0, (LPARAM)L"with note"); // BCM_SETNOTE
			//SendMessage(_hWnd, EM_SETLIMITTEXT, 16, (LPARAM)0);
			SendMessage(_hWnd, WM_SETFONT, (WPARAM)_manager->GetFont(GetFont()), 0);
			::ShowWindow(_hWnd, SW_SHOW);
			//::SetWindowText(_hWnd, TEXT("TEST\nWRAP"));
			//::SetWindowText(_hWnd, TEXT("Run...Run...Run... 123 "));

			//::SetWindowText(_hWnd, GetText().GetData());
		}
	}

	void WinButton::SetPos(RECT rc, bool bNeedInvalidate) 
	{
		if (_hWnd && _hParent != __hParent)
		{
			__hParent = _hParent;
			::SetParent(_hWnd, _hParent);
			::InvalidateRect(_hWnd, NULL, TRUE);
		}

		m_rcItem = rc;
		if(_hWnd) {
			RECT rcPos = m_rcItem;

			//::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);

			::MoveWindow(_hWnd, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, TRUE);
			//::UpdateWindow(_hWnd);
		}
		__super::SetPos(rc, bNeedInvalidate);
	}

	void WinButton::SetText(LPCTSTR pstrText)
	{
		__super::SetText(pstrText);
		if(_hWnd && IsWindow(_hWnd))
			::SetWindowText(GetHWND(), (LPCWSTR)GetText().GetData());
		NeedParentUpdate();
	}

	void WinButton::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("type")) == 0 ) SetType(pstrValue);
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void WinButton::SetType(LPCTSTR pstrText)
	{
		int type = 0;
		if (pstrText)
		{
			int length = lstrlen(pstrText);
			if (length>=1)
			{
				TCHAR tc = pstrText[0];
				bool auto_ = false;
				if (tc==L'_' && length>1)
				{
					auto_ = true;
					tc = pstrText[1];
				}
				if(tc>=L'a'&&tc<=L'z') tc = _toupper(tc);
				switch(tc)
				{
				case L'P':
					type = auto_?BS_DEFPUSHBUTTON:BS_PUSHBUTTON;
					break;
				case L'C':
					type = auto_?BS_AUTOCHECKBOX:BS_CHECKBOX;
					break;
				case L'R':
					type = auto_?BS_AUTORADIOBUTTON:BS_RADIOBUTTON;
					break;
				case L'3':
					type = auto_?BS_AUTO3STATE:BS_3STATE;
					break;
				case L'G':
					type = BS_GROUPBOX;
					break;
				case L'U':
					type = BS_USERBUTTON;
					break;
				case L'S':
					type = auto_?0x0000000DL:0x0000000CL;
					break;
				case L'L':
					type = 0x0000000EL;
					break;
				default:
					type = BS_OWNERDRAW;
					break;
				}
			}
		}
		_type = type;
		//if(_hWnd && IsWindow(_hWnd)) ...
	}

} // namespace DuiLib


#endif