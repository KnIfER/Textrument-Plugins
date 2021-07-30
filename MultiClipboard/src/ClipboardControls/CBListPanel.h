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

#ifndef MULTI_CLIPBOARD_LIST_BOX_H
#define MULTI_CLIPBOARD_LIST_BOX_H


#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "ToolbarPanel.h"
#include <string>
#endif


// Notification to parent that delete key is pressed
#define LBN_DELETEITEM LB_MSGMAX+0x100


class CBListPanel : public ToolbarPanel
{
public:
	virtual void init(HINSTANCE hInst, HWND parent);
	virtual void destroy();

	virtual void AddItem( std::wstring item );
	virtual void ClearAll();
	virtual INT GetItemCount();

	virtual INT GetCurrentSelectionIndex();
	// Selects the specified list box item.
	// If index out of bounds, and bStrictSelect is TRUE, nothing is selected
	// else the last item in the list, if available, is selected
	virtual void SetCurrentSelectedItem( INT NewSelectionIndex, BOOL bStrictSelect=TRUE );

	int _lbBgColor;
	int _lbFgColor;

	LRESULT runProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) override;
};


#endif