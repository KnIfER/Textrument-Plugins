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

	void WinTabbar::SetEventListener(Listener & _listener)
	{
		WND_SetListener(_hWnd, &_listener);
	}

	void WinTabbar::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
				m_uWndState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
				if(IsRichEvent()) m_pManager->SendNotify(this, DUI_MSGTYPE_BUTTONDOWN);
			}
			return;
		}	
		if( event.Type == UIEVENT_BUTTONUP )
		{ // 不可达
			//if( (m_uWndState & UISTATE_CAPTURED) != 0 ) 
			{
				m_uWndState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
				//if( ::PtInRect(&m_rcItem, event.ptMouse) ) 
				{
					m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
				}			
			}
			return;
		}

	}

	LRESULT CALLBACK WinTabbar::TabWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		WinTabbar* tabbar = (WinTabbar*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		switch(message)
		{
			case WM_LBUTTONUP:
			{
				//tabbar->m_pManager->SendNotify(tabbar, L"click");

				break;
			}
		}
		return CallWindowProc(tabbar->_defaultProc, tabbar->_hWnd, message, wParam, lParam);
	}

	void WinTabbar::Init()
	{
		_hParent = m_pParent->GetHWND();
		//LogIs("_hParent::%d", _hParent);
		TAB_Register();
		DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
			//| TCS_MULTILINE
			//| TCS_BUTTONS | TCS_FLATBUTTONS
			//| TCS_BOTTOM
			//| TCS_VERTICAL
			| TCS_FOCUSNEVER
			| WS_CLIPCHILDREN 
			//| TCS_FIXEDWIDTH 
			;
		DWORD styleEx = 0
			| TCS_EX_HOTTRACKDRAW 
			| TCS_EX_FLICKERFREE 
			| TCS_EX_FIXEDBASELINE 
			//| TCS_EX_PLAINBUTTONS | TCS_EX_FLATSEPARATORS
			;
		_hWnd = ::CreateWindowEx(
			styleEx,
			L"MyTabControl32",
			TEXT("Tab"),
			style,
			0, 0, 800, 100,
			_hParent,
			NULL,
			CPaintManagerUI::GetInstance(),
			0);
		SetWindowLongPtr(_hWnd, GWLP_USERDATA, (LONG_PTR)this);
		_defaultProc = (WNDPROC)SetWindowLongPtr(_hWnd, GWLP_WNDPROC, (LONG_PTR)TabWndProc);
	}

	void WinTabbar::setTabPadding(int paddingX, int paddingY)
	{
		TabCtrl_SetPadding(_hWnd, paddingX, paddingY);
	}

	void WinTabbar::setTabCloseImages(int inactive, int selected, int hovered, int pushed)
	{
		TabCtrl_SetCloseImage(_hWnd, MAKELONG(inactive, selected), MAKELONG(hovered, pushed));
	}

	HIMAGELIST WinTabbar::setImageList(int capacity, int bytesPerPixel, int iconWidth, int iconHeight)
	{
		UINT flags = ILC_MASK;
		if (bytesPerPixel==3)
		{
			flags |= ILC_COLOR24;
		}
		else if (bytesPerPixel==4)
		{
			flags |= ILC_COLOR32;
		}
		else if (bytesPerPixel==2)
		{
			flags |= ILC_COLOR16;
		}
		else if (bytesPerPixel==1)
		{
			flags |= ILC_COLOR8;
		}
		_hImageList = ImageList_Create(iconWidth, iconHeight, flags, capacity, 1);
		return TabCtrl_SetImageList(_hWnd, _hImageList);
	}

	int WinTabbar::addImageToList(HBITMAP hBitmap, COLORREF maskColor)
	{
		if (_hImageList)
		{
			return ImageList_AddMasked(_hImageList, hBitmap, RGB(255, 255, 255));
		}
		return -1;
	}

	void WinTabbar::addTab(int position, const TCHAR *text, LPARAM lParam, int image)
	{
		TCITEM tab{};
		tab.iImage = image;
		tab.mask = TCIF_TEXT;
		tab.pszText = (TCHAR *)text;
		tab.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
		tab.lParam = lParam;
		TabCtrl_InsertItem(_hWnd, position, reinterpret_cast<LPARAM>(&tab));
	}
	
	bool WinTabbar::closeTabAt(int position)
	{
		if (TabCtrl_DeleteItem(_hWnd, position))
		{
			//TabCtrl_SetCurSel();
			return true;
		}
		return false;
	}
	
	LPARAM WinTabbar::GetTabDataAt(int position)
	{
		return TabCtrl_GetItemExtra(_hWnd, position);
	}

	void WinTabbar::SetMultiLine(bool enabled, int maxLns)
	{
		DWORD style = GetWindowLong(GetHWND(), GWL_STYLE);
		if (enabled)
		{
			style |= TCS_MULTILINE;
		}
		else
		{
			style &= ~TCS_MULTILINE;
		}
		SetWindowLong(GetHWND(), GWL_STYLE, style);
		TabCtrl_SetMaxRows(GetHWND(), maxLns);
	}

	bool WinTabbar::setTabFont(int fontSize, TCHAR* fontName)
	{
		//设置字体
		LOGFONT logFont;
		HFONT hFont;
		HDC hdc;
		hdc = GetDC(_hWnd);
		logFont.lfHeight = MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
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
		if (fontName)
		{
			lstrcpy(logFont.lfFaceName, fontName); 
		}
		hFont = CreateFontIndirect(&logFont);
		if (hFont)
		{
			SendMessage(_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			return true;
		}
		return false;
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
			//::InflateRect(&rcPos, -10, -10);
			//::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			//ShowWindow(_hWnd, SW_SHOW);
			::MoveWindow(_hWnd, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, TRUE);
			::UpdateWindow(_hWnd);

			TabCtrl_AdjustRect(_hWnd, false, &rcPos);

			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( pControl->IsVisible() ) { // && !pControl->IsDirectUI()
					if( pControl->IsFloat() ) {
						SetFloatPos(it);
					}
					else { 
						pControl->SetPos(rcPos, true);
					}
				}
			}
		}
	}
	
	bool WinTabbar::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		//RECT rcTemp = { 0 };
		//if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return true;
		//
		//PAINTSTRUCT ps;
		//hDC = BeginPaint(_hWnd, &ps);
		//
		//CRenderClip clip;
		//CRenderClip::GenerateClip(hDC, rcTemp, clip);
		////CControlUI::DoPaint(hDC, rcPaint, pStopControl);
		//
		//if( m_items.GetSize() > 0 ) {
		//	RECT rcInset = GetInset();
		//	RECT rc = m_rcItem;
		//	rc.left += rcInset.left;
		//	rc.top += rcInset.top;
		//	rc.right -= rcInset.right;
		//	rc.bottom -= rcInset.bottom;
		//
		//	if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
		//		for( int it = 0; it < m_items.GetSize(); it++ ) {
		//			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
		//			if( pControl == pStopControl ) return false;
		//			if( !pControl->IsVisible() ) continue;
		//			if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
		//			if( pControl ->IsFloat() ) {
		//				if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
		//				if( !pControl->Paint(hDC, m_rcItem, pStopControl) ) return false;
		//			}
		//		}
		//	}
		//	else {
		//		CRenderClip childClip;
		//		CRenderClip::GenerateClip(hDC, rcTemp, childClip);
		//		for( int it = 0; it < m_items.GetSize(); it++ ) {
		//			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
		//			if( pControl == pStopControl ) return false;
		//			if( !pControl->IsVisible() ) continue;
		//			if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
		//			if( pControl->IsFloat() ) {
		//				if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
		//				CRenderClip::UseOldClipBegin(hDC, childClip);
		//				if( !pControl->Paint(hDC, m_rcItem, pStopControl) ) return false;
		//				CRenderClip::UseOldClipEnd(hDC, childClip);
		//			}
		//			else {
		//				if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
		//				if( !pControl->Paint(hDC, m_rcItem, pStopControl) ) return false;
		//			}
		//		}
		//	}
		//}
		// 
		//
		//::EndPaint(_hWnd, &ps);
		return true;
	}
} // namespace DuiLib
