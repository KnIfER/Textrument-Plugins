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

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "SplitterPanel.h"
#include "InsituDebug.h"
#include "NppDarkMode.h"
#include <windowsx.h>
#include <commctrl.h>
#include "resource.h"
#endif

#define SPLITTER_PANEL_CLASS_NAME TEXT( "SplitterPanel" )


SplitterPanel::SplitterPanel()
: Orientation( ESPO_VERTICAL )
, SplitterBarPositionPercent( 0.5f )
, SplitterBarThickness( 6 )
, pChildWin1( 0 )
, pChildWin2( 0 )
, hSplitterCursorUpDown( 0 )
, hSplitterCursorLeftRight( 0 )
, hSplitterBrush( 0 )
, hSplitterPen( 0 )
, DragListMessage( 0 )
{
}


void SplitterPanel::init( HINSTANCE hInst, HWND parent )
{
	Window::init( hInst, parent );

	WNDCLASS wndclass;
	ZeroMemory( &wndclass, sizeof(WNDCLASS) );
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = StaticSplitterPanelProc;
	wndclass.hInstance = _hInst;
	wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOWFRAME);
	wndclass.lpszClassName = SPLITTER_PANEL_CLASS_NAME;

	if ( !::RegisterClass( &wndclass ) )
	{
		DWORD dwErr = GetLastError();
		// Check if class is already registered, if not then we have some other errors
		if ( ERROR_CLASS_ALREADY_EXISTS != dwErr )
		{
			TCHAR errText[512] = TEXT("");
			wsprintf( errText, TEXT("Cannot register window class %s, error code (%d)\r\nPlease remove this plugin and contact the plugin developer with this message"), SPLITTER_PANEL_CLASS_NAME, dwErr );
			::MessageBox( parent, errText, TEXT("MultiClipboard plugin error"), MB_OK );
			return;
		}
		return;
	}

	_hSelf = CreateWindow( SPLITTER_PANEL_CLASS_NAME, 0, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, _hParent, 0, _hInst, 0 );
	if ( !_hSelf )
	{
		DWORD dwErr = GetLastError();
		TCHAR errText[512] = TEXT("");
		wsprintf( errText, TEXT("Cannot create window class %s, error code (%d)\r\nPlease remove this plugin and contact the plugin developer with this message"), SPLITTER_PANEL_CLASS_NAME, dwErr );
		::MessageBox( parent, errText, TEXT("MultiClipboard plugin error"), MB_OK );
	}

	// Store the instance pointer within the window user data
	::SetWindowLongPtr( _hSelf, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this) );

	// Load the cursors for moving the splitter
	hSplitterCursorUpDown    = ::LoadCursor( _hInst, MAKEINTRESOURCE(IDC_UPDOWN) );
	hSplitterCursorLeftRight = ::LoadCursor( _hInst, MAKEINTRESOURCE(IDC_LEFTRIGHT) );
	// Make splitter same colour as the dialog pane
	hSplitterBrush = ::CreateSolidBrush( ::GetSysColor(COLOR_BTNFACE) );
	hSplitterPen = ::CreatePen( PS_SOLID, 0, ::GetSysColor(COLOR_BTNFACE) );

	DragListMessage = ::RegisterWindowMessage( DRAGLISTMSGSTRING );
}

void SplitterPanel::SetSplitterPanelOrientation( ESplitterPanelOrientation NewOrientation )
{
	if ( Orientation != NewOrientation )
	{
		Orientation = NewOrientation;

		// Resize child windows
		ResizeChildren();
	}
}

void SplitterPanel::GetSplitterBarRect( RECT &SplitterPosRect )
{
	RECT rect;

	// Calculate splitter position
	getClientRect( rect );
	if ( Orientation == ESPO_VERTICAL )
	{
		SplitterPosRect.top = (INT) ( (rect.bottom-rect.top) * SplitterBarPositionPercent );
		SplitterPosRect.bottom = SplitterPosRect.top + SplitterBarThickness;
		SplitterPosRect.left = 0;
		SplitterPosRect.right = rect.right - rect.left;
	}
	else
	{
		SplitterPosRect.top = 0;
		SplitterPosRect.bottom = rect.bottom - rect.top;
		SplitterPosRect.left = (INT) ( (rect.right-rect.left) * SplitterBarPositionPercent );
		SplitterPosRect.right = SplitterPosRect.left + SplitterBarThickness;
	}
}
void SplitterPanel::GetPanel1Rect( RECT &Panel1Rect )
{
	RECT SplitterBarRect;

	// Calculate splitter position
	GetSplitterBarRect( SplitterBarRect );

	if ( Orientation == ESPO_VERTICAL )
	{
		// Put ChildWin1 directly above splitter bar
		Panel1Rect.top = 0;
		Panel1Rect.bottom = SplitterBarRect.top;
		Panel1Rect.left = 0;
		Panel1Rect.right = SplitterBarRect.right;
	}
	else
	{
		// Put ChildWin1 directly left of splitter bar
		Panel1Rect.top = 0;
		Panel1Rect.bottom = SplitterBarRect.bottom;
		Panel1Rect.left = 0;
		Panel1Rect.right = SplitterBarRect.left;
	}
}

void SplitterPanel::GetPanel2Rect( RECT &Panel2Rect )
{
	RECT ClientRect, SplitterBarRect;

	getClientRect( ClientRect );
	// Calculate splitter position
	GetSplitterBarRect( SplitterBarRect );
	if ( Orientation == ESPO_VERTICAL )
	{
		// Put ChildWin2 directly below splitter bar
		Panel2Rect.top = SplitterBarRect.bottom;
		Panel2Rect.bottom = ClientRect.bottom;
		Panel2Rect.left = 0;
		Panel2Rect.right = ClientRect.right;
	}
	else
	{
		// Put ChildWin1 directly right of splitter bar
		Panel2Rect.top = 0;
		Panel2Rect.bottom = ClientRect.bottom;
		Panel2Rect.left = SplitterBarRect.right;
		Panel2Rect.right = ClientRect.right;
	}
}


void SplitterPanel::ResizeChildren()
{
	if ( pChildWin1 )
	{
		RECT rect;
		GetPanel1Rect( rect );
		rect.right = rect.right - rect.left;
		rect.bottom = rect.bottom - rect.top;
		pChildWin1->reSizeTo( rect );
	}
	if ( pChildWin2 )
	{
		RECT rect;
		GetPanel2Rect( rect );
		rect.right = rect.right - rect.left;
		rect.bottom = rect.bottom - rect.top;
		pChildWin2->reSizeTo( rect );
	}
}


void SplitterPanel::SetCurrentCursor()
{
	if ( IsDraggingSplitter )
	{
		if ( Orientation == ESPO_VERTICAL )
		{
			SetCursor( hSplitterCursorUpDown );
		}
		else
		{
			SetCursor( hSplitterCursorLeftRight );
		}
	}
	else
	{
		RECT SplitterBarRect;
		GetSplitterBarRect( SplitterBarRect );
		POINT MousePos;
		GetCursorPos( &MousePos );
		ScreenToClient( _hSelf, &MousePos );

		if ( PtInRect( &SplitterBarRect, MousePos ) )
		{
			if ( Orientation == ESPO_VERTICAL )
			{
				SetCursor( hSplitterCursorUpDown );
			}
			else
			{
				SetCursor( hSplitterCursorLeftRight );
			}
		}
		else
		{
			SetCursor( LoadCursor( NULL, IDC_ARROW) );
		}
	}
}


LRESULT SplitterPanel::SplitterPanelProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( message == DragListMessage )
	{
		// Being a container window which doesn't care about child window control notifications,
		// forward the child window controls' notifications to the parent window for processing
		return ::SendMessage( _hParent, message, wParam, lParam );
	}

	switch ( message )
	{
		case WM_MOUSEMOVE:
		{
			POINT MousePos;
			MousePos.x = GET_X_LPARAM( lParam );
			MousePos.y = GET_Y_LPARAM( lParam );

			if ( IsDraggingSplitter )
			{
				RECT ClientRect;
				getClientRect( ClientRect );

				if ( Orientation == ESPO_VERTICAL )
				{
					int height = ClientRect.bottom - ClientRect.top;
					if ( MousePos.y > 0 && ( MousePos.y + SplitterBarThickness ) < height )
					{
						SplitterBarPositionPercent = (FLOAT) MousePos.y / height;
					}
				}
				else
				{
					int width = ClientRect.right - ClientRect.left;
					if ( MousePos.x > 0 && ( MousePos.x + SplitterBarThickness ) < width )
					{
						SplitterBarPositionPercent = (FLOAT) MousePos.x / width;
					}
				}
				ResizeChildren();
				InvalidateRect( _hSelf, 0, TRUE );
			}
			else
			{
				SetCurrentCursor();
			}
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			RECT SplitterBarRect;
			GetSplitterBarRect( SplitterBarRect );
			POINT MousePos;
			MousePos.x = LOWORD( lParam );
			MousePos.y = HIWORD( lParam );

			if ( PtInRect( &SplitterBarRect, MousePos ) )
			{
				IsDraggingSplitter = TRUE;
				SetCapture( _hSelf );
				InvalidateRect( _hSelf, 0, TRUE );
				return 0;
			}
		} break;

		case WM_LBUTTONUP:
		{
			if ( IsDraggingSplitter )
			{
				IsDraggingSplitter = FALSE;
				ReleaseCapture();
				InvalidateRect( _hSelf, 0, TRUE );
				return 0;
			}
		} break;

		case WM_ERASEBKGND:
		{
			if (!NppDarkMode::isEnabled())
			{
				break;
			}

			RECT rc = { 0 };
			getClientRect(rc);

			FillRect((HDC)wParam, &rc, NppDarkMode::getSofterBackgroundBrush());

			return 1;
		}

		case WM_MOVE:
		case WM_SIZE:
			ResizeChildren();
			return 0;

		case WM_PAINT:
		{
			if (NppDarkMode::isEnabled())
			{
				break;
			}

			HDC hdc;
			PAINTSTRUCT ps;
			RECT SplitterBarRect;
			HBRUSH hOldBrush;
			HPEN hOldPen;

			SetCurrentCursor();

			GetSplitterBarRect( SplitterBarRect );
			hdc = ::BeginPaint( _hSelf, &ps );

			hOldBrush = (HBRUSH)::SelectObject( hdc, hSplitterBrush );
			hOldPen = (HPEN)::SelectObject( hdc, hSplitterPen );
			::Rectangle( hdc, SplitterBarRect.left, SplitterBarRect.top, SplitterBarRect.right, SplitterBarRect.bottom );

			// clean up
			::SelectObject( hdc, hOldBrush );
			::SelectObject( hdc, hOldPen );

			::EndPaint( _hSelf, &ps );
			return 0;
		}

		case WM_COMMAND:
		case WM_NOTIFY:
			// Being a container window which doesn't care about child window control notifications,
			// forward the child window controls' notifications to the parent window for processing
			return ::SendMessage( _hParent, message, wParam, lParam );

		case WM_DESTROY:
			::DeleteBrush( hSplitterBrush );
			::DeletePen( hSplitterPen );
			return 0;
	}
	return ::DefWindowProc( hwnd, message, wParam, lParam );
}


LRESULT CALLBACK SplitterPanel::StaticSplitterPanelProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	SplitterPanel * pSplitterPanel = reinterpret_cast<SplitterPanel *>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
	if ( !pSplitterPanel )
	{
		return ::DefWindowProc( hwnd, message, wParam, lParam );
	}
	return pSplitterPanel->SplitterPanelProc( hwnd, message, wParam, lParam );
}