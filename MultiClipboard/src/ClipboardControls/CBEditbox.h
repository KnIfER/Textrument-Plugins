/*
This file is part of MultiClipboard Plugin for Notepad++
Copyright (C) 2009 LoonyChewy

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

#ifndef MULTI_CLIPBOARD_EDIT_BOX_H
#define MULTI_CLIPBOARD_EDIT_BOX_H


#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "Window.h"
#include <string>
#endif


class CBEditbox : public Window
{
public:
	virtual void init(HINSTANCE hInst, HWND parent);
	virtual void destroy();

	virtual void SetText( const std::wstring & text );
	virtual std::wstring GetText();
	// This should be slightly more efficient due to not pass text around too much
	virtual void GetText( std::wstring & text );

	virtual void SetEditBoxReadOnly( const BOOL bReadOnly = TRUE );
	virtual void EnableEditBox( const BOOL bEnable = TRUE );
	virtual BOOL IsEditBoxEnabled();

private:
	WNDPROC oldWndProc;
	// Subclass the list box's wnd proc for customised behavior
	static LRESULT CALLBACK StaticListboxProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		return ((CBEditbox *)(::GetWindowLongPtr(hwnd, GWLP_USERDATA)))->runProc( hwnd, message, wParam, lParam );
	};

	LRESULT runProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

	HFONT hNewFont;
};


#endif