/*
This file is part of MultiClipboard Plugin for Notepad++
Copyright (C) 2009 LoonyChewy

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "PluginDefinition.h"
#include "CBListbox.h"
#include <commctrl.h>
#endif

const COLORREF white     	            = RGB(0xFF, 0xFF, 0xFF);
const COLORREF black     	            = RGB(0,       0,    0);

void CBListbox::init(HINSTANCE hInst, HWND parent)
{
	hNewFont = 0;

	Window::init( hInst, parent );

	_hSelf = CreateWindow( WC_LISTBOX, L"asd",
		 WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD ^ LBS_SORT 
		| LBS_NOINTEGRALHEIGHT
		| LBS_HASSTRINGS
		//| CBS_OWNERDRAWVARIABLE
		,
		0, 0, 1, 1, parent, 0, hInst, NULL );


	if ( !_hSelf )
	{
		return;
	}

	// associate this class instance with the listbox instance
	::SetWindowLongPtr( _hSelf, GWLP_USERDATA, (LONG_PTR)this );

	// subclass the listbox control
	oldWndProc = (WNDPROC)::SetWindowLongPtr( _hSelf, GWLP_WNDPROC, (LONG_PTR)StaticListboxProc );


	// Make items draggable
	MakeDragList( _hSelf );

	hNewFont = (HFONT)::SendMessage( _hSelf, WM_GETFONT, 0, 0 );
	if ( hNewFont == NULL )
	{
		hNewFont = (HFONT)::GetStockObject( SYSTEM_FONT );
	}
	LOGFONT lf;
	::GetObject( hNewFont, sizeof( lf ), &lf );
	lf.lfHeight = 15;
	lf.lfWidth = 0;
	lf.lfWeight = FW_NORMAL;
	lstrcpy( lf.lfFaceName, TEXT("Courier New") );
	hNewFont = ::CreateFontIndirect( &lf );
	::SendMessage( _hSelf, WM_SETFONT, (WPARAM)hNewFont, 1 );


	runProc(_hSelf, NPPN_DARKCONF_CHANGED, 0, 0);
}


void CBListbox::destroy()
{
	::DeleteObject( hNewFont );
}


void CBListbox::AddItem( const std::wstring & item )
{
	auto textToAdd = item.c_str();
	if ( item.size() > 127 )
	{
		lstrcpyn(buffer, textToAdd, 127);
		buffer[127] = '\0';
		::SendMessage( _hSelf, LB_ADDSTRING, 0, (LPARAM) buffer);
	} else {
		::SendMessage( _hSelf, LB_ADDSTRING, 0, (LPARAM)textToAdd );
	}
}


void CBListbox::ClearAll()
{
	::SendMessage( _hSelf, LB_RESETCONTENT, 0, 0 );
}


INT CBListbox::GetItemCount()
{
	return (INT)::SendMessage( _hSelf, LB_GETCOUNT, 0, 0 );
}


INT CBListbox::GetCurrentSelectionIndex()
{
	return (INT)::SendMessage( _hSelf, LB_GETCURSEL, 0, 0 );
}


void CBListbox::SetCurrentSelectedItem( INT NewSelectionIndex, BOOL bStrictSelect )
{
	if ( bStrictSelect )
	{
		::SendMessage( _hSelf, LB_SETCURSEL, NewSelectionIndex, 0 );
	}
	else
	{
		INT ItemCount = GetItemCount();
		if ( ItemCount == LB_ERR || ItemCount <= 0)
		{
			// Error, can't get item count or no items, don't select anything
			return;
		}
		if ( ItemCount > NewSelectionIndex )
		{
			// Requested index is valid, select it
			::SendMessage( _hSelf, LB_SETCURSEL, NewSelectionIndex, 0 );
		}
		else
		{
			// Requested index out of bounds, select the last item
			::SendMessage( _hSelf, LB_SETCURSEL, ItemCount-1, 0 );
		}
	}
}

void CBListbox::drawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//LogIs(2, "DrawText");
	if (lpDrawItemStruct->itemID >= GetItemCount())
		return;

	//printStr(TEXT("OK"));
	COLORREF fgColor = _lbFgColor == -1?black:_lbFgColor; // fg black by default
	COLORREF bgColor = _lbBgColor == -1?white:_lbBgColor; // bg white by default
	
	::SendMessage( _hSelf, LB_GETTEXT, lpDrawItemStruct->itemID, (LPARAM)buffer );
	buffer[127] = '\0';
	TCHAR *ptStr = buffer;
	//ptStr = (TCHAR *)L"ASDASD";
	// 
	//printStr(ptStr);
	::SetTextColor(lpDrawItemStruct->hDC, fgColor);
	::SetBkColor(lpDrawItemStruct->hDC, bgColor);

	::DrawText(lpDrawItemStruct->hDC, ptStr, lstrlen(ptStr), &(lpDrawItemStruct->rcItem), DT_SINGLELINE | DT_VCENTER | DT_LEFT);

}

LRESULT CBListbox::runProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
		case WM_CTLCOLORLISTBOX:
		{
			LogIs(2, "WM_CTLCOLORLISTBOX ");
			if (_lbBgColor != -1)
				return reinterpret_cast<LRESULT>(::CreateSolidBrush(_lbBgColor));
			break;
		}

		case NPPN_DARKCONF_CHANGED :
		{
			if (NppDarkMode::isEnabled())
			{
				_lbBgColor = NppDarkMode::getBackgroundColor();
				_lbFgColor = white;
			} else {
				_lbBgColor = white;
				_lbFgColor = black;
			}
			NppDarkMode::setDarkScrollBar(_hSelf);
			return true;
		}
		
		case WM_MEASUREITEM :
		{
			LogIs(2, "WM_MEASUREITEM ");
			return true;
		}

		case WM_DRAWITEM:
		{
			drawItem(reinterpret_cast<DRAWITEMSTRUCT *>(lParam));
			break;
		}

		case WM_ERASEBKGND:
		{
			if (!NppDarkMode::isEnabled())
			{
				break;
			}
		
			RECT rc = { 0 };
			getClientRect(rc);
			FillRect((HDC)wParam, &rc, NppDarkMode::getBackgroundBrush());
			return 1;
		}

		case WM_KEYUP:
			switch ( wParam )
			{
			case VK_DELETE:
				SendMessage( _hParent, WM_COMMAND, MAKEWPARAM(0, LBN_DELETEITEM), (LPARAM)_hSelf );
				return 0;
			}
	}

	return ::CallWindowProc( oldWndProc, hwnd, message, wParam, lParam );
}