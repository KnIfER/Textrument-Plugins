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

#ifndef MULTI_CLIP_PASTE_MENU_H
#define MULTI_CLIP_PASTE_MENU_H

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "ModelViewController.h"
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRA_LEAN
#include <windows.h>

#include "MultiClipboardProxy.h"
#endif


class MultiClipPasteMenu : public IController, public MouseListener, public KeyListener
{
public:
	MultiClipPasteMenu();
	virtual void Init( IModel * pNewModel, MultiClipboardProxy * pClipboardProxy, McOptionsManager * pSettings );

	void show();
	bool isEnabled() { return bUsePasteMenu; }

	virtual BOOL OnMouseEvent( MouseEventType eventType, MouseEventFlags eventFlags,
		INT mouseX, INT mouseY, INT mouseDelta );
	virtual BOOL OnKeyEvent( KeyEventType eventType, INT keyCode );

	virtual void OnModelModified();

	virtual void OnObserverAdded( McOptionsManager * SettingsManager );
	virtual void OnSettingsChanged( const stringType & GroupName, const stringType & SettingName );

private:
	// For caching the paste menu handle
	HMENU hPasteMenu;
	// Whether to used numbered mnemonic in paste menu
	bool bNumberedPasteList;
	// Length of text to display in paste menu
	unsigned int MenuTextLength;
	// Whether to use middle click for pasting
	bool bUseMiddleClickPaste;
	// Whether MultiClipboard plugin will use this (true) or MultiClipCyclicPaste (false)
	bool bUsePasteMenu;
	// Whether the user last fired off a keyboard or mouse event. Used to determine whether to position
	// the menu at the caret or mouse cursor
	bool bMouseLastUsed;

	void RecreateCopyMenu();
	void CreateMenuText( const std::wstring & InClipText, std::wstring & OutMenuString, const int index );
	void PasteClipboardItem( unsigned int MenuItemID );
	BOOL OnMiddleClick( bool bIsShift );
};


#endif