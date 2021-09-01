#include "StdAfx.h"
#include "Core\InsituDebug.h"
#include "WindowsEx\comctl32.h"

extern void TAB_Register();

namespace DuiLib {
	IMPLEMENT_DUICONTROL(WinTabbar)

	WinTabbar::WinTabbar()
		: CContainerUI()
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
		TCITEM tab{};
		tab.iImage = image;
		tab.mask = TCIF_TEXT;
		tab.pszText = (TCHAR *)text;
		tab.mask = TCIF_TEXT | TCIF_IMAGE;
		//tab.lParam;
		TabCtrl_InsertItem(hWnd, size++, reinterpret_cast<LPARAM>(&tab));
	}

	void WinTabbar::Init()
	{
		_hParent = m_pParent->GetHWND();

		TAB_Register();
		//LogIs("_hParent::%d", _hParent);

		DWORD style = WS_CHILD | WS_VISIBLE 
			//| TCS_MULTILINE
			//| TCS_BUTTONS
			//| TCS_FLATBUTTONS
			//| TCS_BOTTOM
			//| TCS_VERTICAL
			| TCS_FOCUSNEVER
			| WS_CLIPCHILDREN 
			| TCS_HOTTRACKDRAW 
			//| TCS_FIXEDWIDTH 

			| TCS_FLICKERFREE 

			| TCS_FIXEDBASELINE 

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


		TabCtrl_SetPadding(_hWnd, 12, 3);
		TabCtrl_SetCloseImage(_hWnd, MAKELONG(6, 7), MAKELONG(8, 7));

		HIMAGELIST hImageList = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, 3, 1);
		//auto bmp = m_pManager->GetImage(L"tab_def.bmp");
		auto bmp = CRenderEngine::LoadImageStr(L"tab_def.bmp", NULL, 0xFFFFFFFF, NULL, 3);
		if (bmp)
		{
			//HBITMAP hBitmap = LoadBitmap(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDB_TOOLBAR));
			HBITMAP hBitmap = bmp->hBitmap;
			ImageList_AddMasked(hImageList, hBitmap, RGB(255, 255, 255));
			DeleteObject(hBitmap);
			//SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList); // 正常显示时的图像列表
			TabCtrl_SetImageList(_hWnd, hImageList);
		}
		LogIs("bmp::%d", bmp);

		for (size_t i = 0; i < 15; i++)
		{
			DemoData & dd = demoData[i];
			addTab(_hWnd, dd.title, dd.image);
		}
		TCHAR buffer[64];
		for (size_t i = 0; i < 80; i++)
		{
			swprintf_s(buffer, L"happy#%d", i+15);
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

			TabCtrl_AdjustRect(_hWnd, false, &rcPos);

			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( pControl->IsVisible() ) { // && !pControl->IsDirectUI()
					if( pControl->IsFloat() ) {
						SetFloatPos(it);
					}
					else { 
						pControl->SetPos(rcPos, false);
					}
				}
			}
		}
	}
	

} // namespace DuiLib
