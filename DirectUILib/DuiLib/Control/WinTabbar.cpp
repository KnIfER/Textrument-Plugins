#include "StdAfx.h"
#include "Core\InsituDebug.h"
#include "WindowsEx\comctl32.h"

extern void TAB_Register();

namespace DuiLib {
	IMPLEMENT_DUICONTROL(WinTabbar)

	WinTabbar::WinTabbar()
		: CControlUI()
	{
		m_dwBackColor = RGB(0, 0, 255);
		_isDirectUI = true;
	}

	LPCTSTR WinTabbar::GetClass() const
	{
		return L"WinTabbar";
	}

	LPVOID WinTabbar::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, L"WinTabbar") == 0 ) return static_cast<WinTabbar*>(this);
		return __super::GetInterface(pstrName);
	}

	struct DemoData
	{
		const TCHAR* title;
		int image;
	};

	DemoData demoData[]{
		{L"happy", 0}
		,{L"happy for what", 1}
		,{L"sad", 0}
		,{L"sad for what", 1}
		,{L"Values，何为价值观", 2}
		,{L"成功秘诀.pdf", 3}
		,{L"happy", 0}
		,{L"happy", 0}
		,{L"SecretToHappiness.pdf", 4}
		,{L"MasterCPP.pdf", 4}
		,{L"happy", 0}
		,{L"happy", 0}
		,{L"Your photo.png", 5}
		,{L"happy", 0}
		,{L"happy", 0}
	};
	int size=0;
	void addTab(HWND hWnd, const TCHAR *text, int image)
	{
		TCITEM data;
		data.iImage = image;
		data.mask = TCIF_TEXT;
		data.pszText = (TCHAR *)text;
		data.mask = TCIF_TEXT | TCIF_IMAGE;
		SendMessage(hWnd, TCM_INSERTITEM, size++, reinterpret_cast<LPARAM>(&data));
	}

	void WinTabbar::Init()
	{
		_hParent = m_pParent->GetHWND();

		TAB_Register();
		//LogIs("_hParent::%d", _hParent);

		int style = WS_CHILD | WS_VISIBLE 
			| TCS_MULTILINE
			//| TCS_BUTTONS
			| TCS_FOCUSNEVER
			| WS_CLIPCHILDREN 

			| TCS_FLICKERFREE 

			;

		_hWnd = ::CreateWindowEx(
			0,
			L"MyTabControl32",
			TEXT("Tab"),
			style,
			0, 0, 800, 100,
			_hParent,
			NULL,
			CPaintManagerUI::GetInstance(),
			0);
		for (size_t i = 0; i < 15; i++)
		{
			DemoData & dd = demoData[i];
			addTab(_hWnd, dd.title, dd.image);
		}
		TCHAR buffer[64];
		for (size_t i = 0; i < 80; i++)
		{
			swprintf_s(buffer, L"happy#%d", i);
			addTab(_hWnd, buffer, 0);
		}

		//设置字体
		LOGFONT logFont;
		HFONT hFont;
		HDC hdc;
		hdc = GetDC(_hWnd);
		logFont.lfHeight = MulDiv(13, GetDeviceCaps(hdc, LOGPIXELSY), 72);//13是字号大小
		ReleaseDC(_hWnd, hdc);
		logFont.lfWidth = 0;
		logFont.lfEscapement = 0;
		logFont.lfOrientation = 0;
		logFont.lfWeight = FW_REGULAR;
		logFont.lfItalic = 0;
		logFont.lfUnderline = 0;
		logFont.lfStrikeOut = 0;
		logFont.lfCharSet = GB2312_CHARSET;
		logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logFont.lfQuality = PROOF_QUALITY;
		logFont.lfPitchAndFamily = VARIABLE_PITCH  | FF_ROMAN;
		lstrcpy(logFont.lfFaceName, L"华文细黑"); 
		lstrcpy(logFont.lfFaceName, L"宋体"); 
		hFont = CreateFontIndirect(&logFont);
		SendMessage(_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);

		//::ShowWindow(_hWnd, TRUE);
		//::SetWindowText(_hWnd, TEXT("TEST"));
		//if (!dynamic_cast<WinFrame*>(m_pParent))
		//{
		//	GetRoot()->_WNDList.push_back(this);
		//}
	}

	void WinTabbar::SetPos(RECT rc, bool bNeedInvalidate) 
	{
		//__super::SetPos(rc, bNeedInvalidate);
		m_rcItem = rc;
		resize();
	}

	void WinTabbar::resize() 
	{
		if(_hWnd) {
			RECT rcPos = m_rcItem;

			//::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			
			::MoveWindow(_hWnd, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, TRUE);

		}
	}
	

} // namespace DuiLib
