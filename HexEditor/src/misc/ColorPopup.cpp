/*
this file is part of notepad++
Copyright (C)2003 Don HO < donho@altern.org >

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "ColorPopup.h"
#include "NativeLang_def.h"
//#include "SysMsg.h"

DWORD colorItems[] = {
	RGB(  0,   0,   0),	RGB( 64,   0,   0),	RGB(128,   0,   0),	RGB(128,  64,  64),	RGB(255,   0,   0),	RGB(255, 128, 128),
	RGB(255, 255, 128),	RGB(255, 255,   0),	RGB(255, 128,  64),	RGB(255, 128,   0),	RGB(128,  64,   0),	RGB(128, 128,   0),
	RGB(128, 128,  64),	RGB(  0,  64,   0),	RGB(  0, 128,   0),	RGB(  0, 255,   0),	RGB(128, 255,   0),	RGB(128, 255, 128),
	RGB(  0, 255, 128),	RGB(  0, 255,  64),	RGB(  0, 128, 128),	RGB(  0, 128,  64),	RGB(  0,  64,  64),	RGB(128, 128, 128),
	RGB( 64, 128, 128),	RGB(  0,   0, 128),	RGB(  0,   0, 255),	RGB(  0,  64, 128),	RGB(  0, 255, 255), RGB(128, 255, 255),
	RGB(  0, 128, 255),	RGB(  0, 128, 192),	RGB(128, 128, 255),	RGB(  0,   0, 160),	RGB(  0,   0,  64),	RGB(192, 192, 192),
	RGB( 64,   0,  64),	RGB( 64,   0,  64),	RGB(128,   0, 128),	RGB(128,   0,  64),	RGB(128, 128, 192),	RGB(255, 128, 192),
	RGB(255, 128, 255),	RGB(255,   0, 255), RGB(255,   0, 128),	RGB(128,   0, 255), RGB( 64,   0, 128),	RGB(255, 255, 255),
};

void systemMessage(const TCHAR *title)
{
	LPVOID lpMsgBuf;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		::GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );// Process any inserts in lpMsgBuf.
	MessageBox( NULL, (LPTSTR)lpMsgBuf, title, MB_OK | MB_ICONSTOP);
	::LocalFree(lpMsgBuf);
}

void ColorPopup::create(int dialogID) 
{
	_hSelf = ::CreateDialogParam(_hInst, MAKEINTRESOURCE(dialogID), _hParent,  (DLGPROC)dlgProc, (LPARAM)this);
	
	if (!_hSelf)
	{
		systemMessage(_T("ColorPopup"));
		throw int(696);
	}
	Window::getClientRect(_rc);
	display();
}

BOOL CALLBACK ColorPopup::dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message) 
	{
		case WM_MEASUREITEM:
		{
			RECT rc;
			LPMEASUREITEMSTRUCT lpmis =  (LPMEASUREITEMSTRUCT) lParam; 
			::GetWindowRect(::GetDlgItem(hwnd, lpmis->CtlID), &rc);
			lpmis->itemHeight = (rc.bottom-rc.top)/6; 
			lpmis->itemWidth = (rc.right-rc.left)/8;
			return TRUE;
		}

		case WM_INITDIALOG :
		{
			ColorPopup *pColorPopup = (ColorPopup *)(lParam);
			pColorPopup->_hSelf = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (long)lParam);
			pColorPopup->run_dlgProc(message, wParam, lParam);
			return TRUE;
		}

		default :
		{
			ColorPopup *pColorPopup = reinterpret_cast<ColorPopup *>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if (!pColorPopup)
				return FALSE;
			return pColorPopup->run_dlgProc(message, wParam, lParam);
		}
	}
}

INT_PTR CALLBACK ColorPopup::run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam)
{

	switch (Message)
	{
		case WM_INITDIALOG:
		{
			int nColor;
			for (nColor = 0 ; nColor < int(sizeof(colorItems)/sizeof(DWORD)) ; nColor++)
			{
				::SendDlgItemMessage(_hSelf, IDC_COLOR_LIST, LB_ADDSTRING, nColor, (LPARAM) "");
				::SendDlgItemMessage(_hSelf, IDC_COLOR_LIST, LB_SETITEMDATA , nColor, (LPARAM) colorItems[nColor]);
			}

			NLChangeDialog(_hInst, _hNpp, _hSelf, _T("ColorPopup"));

			return TRUE;
		}
		
		case WM_CTLCOLORLISTBOX:
			return (BOOL)((HBRUSH)::CreateSolidBrush(GetSysColor(COLOR_3DFACE)));

		case WM_DRAWITEM:
		{
			HDC hdc;
			COLORREF	cr;
			HBRUSH		hbrush;
	
			DRAWITEMSTRUCT *pdis = (DRAWITEMSTRUCT *)lParam;
			hdc = pdis->hDC;
			RECT rc = pdis->rcItem;
	
			// Transparent.
			SetBkMode(hdc,TRANSPARENT);
	
			// NULL object
			if (pdis->itemID == UINT(-1)) return 0; 

			switch (pdis->itemAction)
			{
				case ODA_DRAWENTIRE:
					switch (pdis->CtlID)
					{
						case IDC_COLOR_LIST:
							rc = pdis->rcItem;
							cr = (COLORREF) pdis->itemData;
							InflateRect(&rc, -3, -3);
							hbrush = CreateSolidBrush((COLORREF)cr);
							FillRect(hdc, &rc, hbrush);
							DeleteObject(hbrush);
							FrameRect(hdc, &rc, (HBRUSH) GetStockObject(GRAY_BRUSH));
							break;
					}
					// *** FALL THROUGH ***
				case ODA_SELECT:
					rc = pdis->rcItem;
					if (pdis->itemState & ODS_SELECTED)
					{
						rc.bottom --;
						rc.right --;
						// Draw the lighted side.
						HPEN hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
						HPEN holdPen = (HPEN)SelectObject(hdc, hpen);
						MoveToEx(hdc, rc.left, rc.bottom, NULL);
						LineTo(hdc, rc.left, rc.top);
						LineTo(hdc, rc.right, rc.top);
						SelectObject(hdc, holdPen);
						DeleteObject(hpen);
						// Draw the darkened side.
						hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
						holdPen = (HPEN)SelectObject(hdc, hpen);
						LineTo(hdc, rc.right, rc.bottom);
						LineTo(hdc, rc.left, rc.bottom);
						SelectObject(hdc, holdPen);
						DeleteObject(hpen);
					}
					else 
					{
						hbrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
						FrameRect(hdc, &rc, hbrush);
						DeleteObject(hbrush);
					}
					break;
				case ODA_FOCUS:
					rc = pdis->rcItem;
					InflateRect(&rc, -2, -2);
					DrawFocusRect(hdc, &rc);
					break;
				default:
					break;
			}
			return TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam))
            {
                case IDOK :
			    {
					isColorChooserLaunched = true;
					CHOOSECOLOR cc;                 // common dialog box structure 
					static COLORREF acrCustClr[16] = {
						RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),\
						RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),\
						RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),\
						RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),\
					}; // array of custom colors 

					// Initialize CHOOSECOLOR 
					::ZeroMemory(&cc, sizeof(cc));
					cc.lStructSize = sizeof(cc);
					cc.hwndOwner = _hParent;

					cc.lpCustColors = (LPDWORD) acrCustClr;
					cc.rgbResult = _color;
					cc.Flags = CC_FULLOPEN | CC_RGBINIT;

					display(false);
					 
					if (ChooseColor(&cc)==TRUE) 
					{
						::SendMessage(_hParent, HEXM_PICKUP_COLOR, cc.rgbResult, 0);
					}
					else
					{
						::SendMessage(_hParent, HEXM_PICKUP_CANCEL, 0, 0);
					}

				    return TRUE;
			    }

                case IDC_COLOR_LIST :
                {
			        if (HIWORD(wParam) == LBN_SELCHANGE)
		            {
                        int i = (int)::SendMessage((HWND)lParam, LB_GETCURSEL, 0L, 0L);
                        _color = (COLORREF)::SendMessage((HWND)lParam, LB_GETITEMDATA, i, 0L);

                        ::SendMessage(_hParent, HEXM_PICKUP_COLOR, _color, 0);
					    return TRUE;
		            }
                }
			    
                default :
                    return FALSE;
            }
		
		case WM_ACTIVATE :
        {
			if (LOWORD(wParam) == WA_INACTIVE)
				if (!isColorChooserLaunched)
					::SendMessage(_hParent, HEXM_PICKUP_CANCEL, 0, 0);
			return TRUE;
		}
		
	}
	return FALSE;
}


