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
#include "CBEditbox.h"
#include <commctrl.h>
#include <vector>
#endif


void CBEditbox::init(HINSTANCE hInst, HWND parent)
{
	hNewFont = 0;

	Window::init( hInst, parent );

	_hSelf = CreateWindow( WC_EDIT, NULL,
		WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
		ES_LEFT | ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
		0, 0, 1, 1, parent, 0, hInst, NULL );

	if ( !_hSelf )
	{
		return;
	}

	// associate this class instance with the listbox instance
	::SetWindowLongPtr( _hSelf, GWLP_USERDATA, (LONG_PTR)this );

	// subclass the listbox control
	oldWndProc = (WNDPROC)::SetWindowLongPtr( _hSelf, GWLP_WNDPROC, (LONG_PTR)StaticListboxProc );


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
	//::SendMessage( _hSelf, WM_SETFONT, (WPARAM)hNewFont, 1 );

	// Set tab width, 16 units is approximately 4 characters wide
	int TabWidth = 16;
	::SendMessage( _hSelf, EM_SETTABSTOPS, 1, (LPARAM)&TabWidth );

	runProc(_hSelf, NPPN_DARKCONF_CHANGED, 0, 0);
}


LRESULT CBEditbox::runProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	}

	return ::CallWindowProc( oldWndProc, hwnd, message, wParam, lParam );
}

void CBEditbox::destroy()
{
	::DeleteObject( hNewFont );
}


void CBEditbox::SetText( const std::wstring & text )
{
	::SetWindowText( _hSelf, text.c_str() );
}


std::wstring CBEditbox::GetText()
{
	// Use stl's vector to store the text from editbox, instead of new and delete array
	// because it is safer against memory leaks

	// Get length of text in edit box
	INT textLength = ::GetWindowTextLength( _hSelf );
	// Use stl's vector to store the text, make sure to reserve space for null terminator
	std::vector< std::wstring::value_type > data( textLength + 1, 0 );
	// Get the window text into the vector
	::GetWindowText( _hSelf, &data[0], (int)data.capacity() );

	// return the text
	return std::wstring( &data[0] );
}


void CBEditbox::GetText( std::wstring & text )
{
	// Use stl's vector to store the text from editbox, instead of new and delete array
	// because it is safer against memory leaks

	// Get length of text in edit box
	INT textLength = ::GetWindowTextLength( _hSelf );
	// Use stl's vector to store the text, make sure to reserve space for null terminator
	std::vector< std::wstring::value_type > data( textLength + 1, 0 );
	// Get the window text into the vector
	::GetWindowText( _hSelf, &data[0], (int)data.capacity() );

	// return the text
	text.assign( &data[0] );
}


void CBEditbox::SetEditBoxReadOnly( const BOOL bReadOnly )
{
	::SendMessage( _hSelf, EM_SETREADONLY, (WPARAM) bReadOnly, 0 );
}


void CBEditbox::EnableEditBox( const BOOL bEnable )
{
	::EnableWindow( _hSelf, bEnable );
}


BOOL CBEditbox::IsEditBoxEnabled()
{
	return ::IsWindowEnabled( _hSelf );
}