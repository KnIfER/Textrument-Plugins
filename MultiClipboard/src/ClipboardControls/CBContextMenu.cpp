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
/*
Note: Most of the code below is taken from
"Writing your own menu-like window" by Raymond Chen
http://blogs.msdn.com/oldnewthing/archive/2004/08/20/217684.aspx
Direct download of the code is from "FakeMenu.zip"
http://www.mvps.org/user32/rc/FakeMenu.zip
*/

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "CBContextMenu.h"
#endif

#define CONTEXT_MENU_CLASS_NAME TEXT( "ToolbarPanel" )


void CBContextMenu::init( HINSTANCE hInst, HWND parent )
{
	hNewFont = 0;

	Window::init( hInst, parent );

	WNDCLASS wndclass;
	ZeroMemory( &wndclass, sizeof(WNDCLASS) );
	wndclass.lpfnWndProc = StaticContextMenuProc;
	wndclass.hInstance = hInst;
	wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wndclass.lpszClassName = CONTEXT_MENU_CLASS_NAME;

	if ( !::RegisterClass( &wndclass ) )
	{
		DWORD dwErr = GetLastError();
		// Check if class is already registered, if not then we have some other errors
		if ( ERROR_CLASS_ALREADY_EXISTS != dwErr )
		{
			TCHAR errText[512] = TEXT("");
			wsprintf( errText, TEXT("Cannot register window class %s, error code (%d)\r\nPlease remove this plugin and contact the plugin developer with this message"), CONTEXT_MENU_CLASS_NAME, dwErr );
			::MessageBox( parent, errText, TEXT("MultiClipboard plugin error"), MB_OK );
			return;
		}
	}

	_hSelf = CreateWindow( TEXT("CBContextMenu"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER ,
		0, 0, 1, 1, parent, 0, hInst, NULL );

	if ( !_hSelf )
	{
		return;
	}

	hNewFont = (HFONT)::SendMessage( _hSelf, WM_GETFONT, 0, 0 );
	if ( hNewFont == NULL )
	{
		hNewFont = (HFONT)::GetStockObject( SYSTEM_FONT );
	}
	LOGFONT lf;
	::GetObject( hNewFont, sizeof( lf ), &lf );
	lf.lfHeight = 16;
	lf.lfWidth = 0;
	lf.lfWeight = FW_NORMAL;
	lstrcpy( lf.lfFaceName, TEXT("Courier New") );
	hNewFont = ::CreateFontIndirect( &lf );
	::SendMessage( _hSelf, WM_SETFONT, (WPARAM)hNewFont, 1 );
}


void CBContextMenu::destroy()
{
	::DeleteObject( hNewFont );
}


void CBContextMenu::ShowContextMenu( int x, int y )
{
}


void CBContextMenu::AddItem( std::wstring item )
{
}


LRESULT CALLBACK CBContextMenu::ContextMenuProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return ::DefWindowProc( hwnd, message, wParam, lParam );
}


LRESULT CALLBACK CBContextMenu::StaticContextMenuProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CBContextMenu * pContextMenu = reinterpret_cast<CBContextMenu *>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
	if ( !pContextMenu )
	{
		return ::DefWindowProc( hwnd, message, wParam, lParam );
	}
	return pContextMenu->ContextMenuProc( hwnd, message, wParam, lParam );
}