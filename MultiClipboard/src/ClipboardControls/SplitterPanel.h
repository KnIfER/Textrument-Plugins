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

#ifndef SPLITTER_PANEL_H
#define SPLITTER_PANEL_H

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include <windows.h>
#include "Window.h"
#endif

enum ESplitterPanelOrientation
{
	ESPO_VERTICAL,
	ESPO_HORIZONTAL
};

class SplitterPanel : public Window
{
public:
	SplitterPanel();

	virtual void init(HINSTANCE hInst, HWND parent);
	virtual void destroy() {}

	virtual ESplitterPanelOrientation GetSplitterPanelOrientation() { return Orientation; }
	virtual void SetSplitterPanelOrientation( ESplitterPanelOrientation NewOrientation );
	Window * pChildWin1;
	Window * pChildWin2;

private :
	ESplitterPanelOrientation Orientation;
	FLOAT SplitterBarPositionPercent;
	INT SplitterBarThickness;
	BOOL IsDraggingSplitter;
	HCURSOR hSplitterCursorUpDown;
	HCURSOR hSplitterCursorLeftRight;
	HBRUSH hSplitterBrush;
	HPEN hSplitterPen;
	// For passing through drag list box messages
	UINT DragListMessage;

	void GetSplitterBarRect( RECT &SplitterPosRect );
	void GetPanel1Rect( RECT &Panel1Rect );
	void GetPanel2Rect( RECT &Panel2Rect );
	void ResizeChildren();
	void SetCurrentCursor();

	LRESULT CALLBACK SplitterPanelProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

	static LRESULT CALLBACK StaticSplitterPanelProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
};

#endif //SPLITTER_PANEL_H
