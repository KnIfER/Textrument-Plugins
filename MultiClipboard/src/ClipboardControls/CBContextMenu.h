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
/*
Note: Most of the code below is taken from
"Writing your own menu-like window" by Raymond Chen
http://blogs.msdn.com/oldnewthing/archive/2004/08/20/217684.aspx
Direct download of the code is from "FakeMenu.zip"
http://www.mvps.org/user32/rc/FakeMenu.zip
*/


#ifndef MULTI_CLIPBOARD_CONTEXT_MENU_H
#define MULTI_CLIPBOARD_CONTEXT_MENU_H


#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "Window.h"
#include <string>
#endif


class CBContextMenu : public Window
{
public:
	virtual void init( HINSTANCE hInst, HWND parent );
	virtual void destroy();
	virtual void ShowContextMenu( int x, int y );

	virtual void AddItem( std::wstring item );

private:
	HFONT hNewFont;

	LRESULT CALLBACK ContextMenuProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

	static LRESULT CALLBACK StaticContextMenuProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
};


#endif