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
#include "MCSubClassWndProc.h"
#include "SciSubClassWrp.h"
#include "MultiClipboardProxy.h"
#include "../../PowerEditor/src/menuCmdID.h"
#endif


extern SciSubClassWrp		g_ScintillaMain, g_ScintillaSecond;
extern WNDPROC				g_NppWndProc;
extern MultiClipboardProxy	g_ClipboardProxy;


// Function to translate keyboard events to a form the clipboard proxy's key listeners can understand
BOOL ProcessKeyEvent( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	KeyListener::KeyEventType ket = (KeyListener::KeyEventType)0;
	switch ( msg )
	{
	case WM_CHAR:
		ket = KeyListener::KET_Char;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		ket = KeyListener::KET_KeyUp;
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		ket = KeyListener::KET_KeyDown;
		break;
	}

	return g_ClipboardProxy.OnKeyEvent( ket, wp );
}


// Function to translate mouse events to a form the clipboard proxy's mouse listeners can understand
BOOL ProcessMouseEvent( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	MouseListener::MouseEventType met = (MouseListener::MouseEventType)0;
	INT mouseDelta = 0;
	switch( msg )
	{
	case WM_MOUSEMOVE:
		met = MouseListener::EMET_MouseMove;
		break;
	case WM_LBUTTONDOWN:
		met = MouseListener::EMET_LButtonDown;
		break;
	case WM_LBUTTONUP:
		met = MouseListener::EMET_LButtonUp;
		break;
	case WM_LBUTTONDBLCLK:
		met = MouseListener::EMET_LButtonDblClk;
		break;
	case WM_MBUTTONDOWN:
		met = MouseListener::EMET_MButtonDown;
		break;
	case WM_MBUTTONUP:
		met = MouseListener::EMET_MButtonUp;
		break;
	case WM_MBUTTONDBLCLK:
		met = MouseListener::EMET_MButtonDblClk;
		break;
	case WM_RBUTTONDOWN:
		met = MouseListener::EMET_RButtonDown;
		break;
	case WM_RBUTTONUP:
		met = MouseListener::EMET_RButtonUp;
		break;
	case WM_RBUTTONDBLCLK:
		met = MouseListener::EMET_RButtonDblClk;
		break;
	case WM_MOUSEWHEEL:
		met = MouseListener::EMET_MouseWheel;
		mouseDelta = HIWORD(wp);
		break;
	}
	int mef = 0;
	mef |= (wp & MK_CONTROL) ? MouseListener::EMEF_CtrlDown : 0;
	mef |= (wp & MK_SHIFT) ? MouseListener::EMEF_ShiftDown : 0;
	mef |= (wp & MK_LBUTTON) ? MouseListener::EMEF_LMBDown : 0;
	mef |= (wp & MK_MBUTTON) ? MouseListener::EMEF_MMBDown : 0;
	mef |= (wp & MK_RBUTTON) ? MouseListener::EMEF_RMBDown : 0;

	return g_ClipboardProxy.OnMouseEvent( met, (MouseListener::MouseEventFlags)mef, LOWORD(lp), HIWORD(lp), mouseDelta );
}


LRESULT CALLBACK MCSubClassNppWndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch ( msg )
	{
	// When clipboard data has changed
	case WM_DRAWCLIPBOARD:
		{
			if ( !::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
			{
				break;
			}

			if ( !::OpenClipboard( hwnd ) )
			{
				break;
			}
			// Let OS convert text to unicode format for us
			HGLOBAL hGlobal = ::GetClipboardData( CF_UNICODETEXT );
			if ( hGlobal != NULL )
			{
				LPWSTR pGlobal = (LPWSTR)::GlobalLock( hGlobal );
				TextItem textItem( pGlobal, g_ClipboardProxy.GetCurrentClipboardFormat() );
				g_ClipboardProxy.OnNewClipboardText( textItem );
				::GlobalUnlock( hGlobal );
			}
			::CloseClipboard();

		}
		if ( g_ClipboardProxy.hNextClipboardViewer )
		{
			::SendMessage( g_ClipboardProxy.hNextClipboardViewer, msg, wp, lp );
		}
		break;

	// When clipboard viewer list has changed
	case WM_CHANGECBCHAIN:
		if ( (HWND)wp == g_ClipboardProxy.hNextClipboardViewer )
		{
			g_ClipboardProxy.hNextClipboardViewer = (HWND)lp ;
		}
		else if ( g_ClipboardProxy.hNextClipboardViewer )
		{
			::SendMessage( g_ClipboardProxy.hNextClipboardViewer, msg, wp, lp );
		}
		break;

	case WM_TIMER:
		if ( g_ClipboardProxy.OnTimer( UINT(wp) ) )
		{
			// Timer is processed by plugin
			return TRUE;
		}
		break;

	case WM_COMMAND:
		switch ( LOWORD( wp ) )
		{
		case IDM_FORMAT_CONV2_ANSI:
			// Handle converting Notepad++ text format to ANSI
			g_ClipboardProxy.OnNppTextFormatConversion( uni8Bit );
			break;

		case IDM_FORMAT_CONV2_AS_UTF_8:
		case IDM_FORMAT_CONV2_UTF_8:
		case IDM_FORMAT_CONV2_UTF_16BE: 
		case IDM_FORMAT_CONV2_UTF_16LE:
			// Handle converting Notepad++ text format to UTF/Unicode
			g_ClipboardProxy.OnNppTextFormatConversion( uniUTF8 );
			break;
		}
		break;
	}
	// Call Notepad++'s window procedure
	return CallWindowProc( g_NppWndProc, hwnd, msg, wp, lp );
}


LRESULT CALLBACK MCSubClassSciWndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch ( msg )
	{
	case WM_KEYDOWN:
		// Look for hardcoded Ctrl-V or Shift-Insert as paste command
		if ( 'V' == wp && 0x8000 & ::GetKeyState( VK_CONTROL ) ||
			VK_INSERT == wp && 0x8000 & ::GetKeyState( VK_SHIFT ) )
		{
			g_ClipboardProxy.OnTextPastedInNpp();
		}
		if ( ProcessKeyEvent( hwnd, msg, wp, lp ) )
		{
			// Key event is processed by the plugin
			return TRUE;
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		if ( wp == VK_SHIFT || wp == VK_CONTROL )
		{
			g_ClipboardProxy.CyclicPasteEnd();
		}
		// Fall through
	case WM_SYSKEYDOWN:
	case WM_CHAR:
		if ( ProcessKeyEvent( hwnd, msg, wp, lp ) )
		{
			// Key event is processed by the plugin
			return TRUE;
		}
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		if ( ProcessMouseEvent( hwnd, msg, wp, lp ) )
		{
			// Key event is processed by the plugin
			return true;
		}
		break;
	}

	// Call scintilla view's window procedure
	if ( g_ScintillaMain.hWnd == hwnd )	// Main Scintilla Window
	{
		return g_ScintillaMain.CallScintillaWndProc( hwnd, msg, wp, lp );
	}
	else if ( g_ScintillaSecond.hWnd == hwnd )	// Second Scintilla Window
	{
		return g_ScintillaSecond.CallScintillaWndProc( hwnd, msg, wp, lp );
	}
	else
	{
		// Should not be reaching here!
		return TRUE;
	}
}