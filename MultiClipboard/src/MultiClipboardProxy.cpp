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
#include "Scintilla.h"
#include "MultiClipboardProxy.h"
#include <cstdio>
#include <vector>
#endif

extern NppData				g_NppData;
extern SciSubClassWrp		g_ScintillaMain, g_ScintillaSecond;


unsigned int ColumnModeText::GetNumRows() const
{
	return rowTexts.size();
}


unsigned int ColumnModeText::GetNumColumns() const
{
	// Use the width of the first column, ignoring the possibility of uneven with rows
	return ( rowTexts.size() > 0 ) ? rowTexts[0].size() : 0;
}


TextItem::TextItem()
: textMode(TCM_NORMAL)
{
}


TextItem::TextItem( std::wstring inText, TextCopyModeEnum inMode )
: text(inText)
, textMode(inMode)
{
}


void TextItem::MakeColumnText( ColumnModeText & columnText ) const
{
	SplitTextIntoRows( columnText.rowTexts );
}


void TextItem::SplitTextIntoRows( std::vector< std::wstring > & rowTexts ) const
{
	rowTexts.clear();
	unsigned int strLen = text.size();

	unsigned int lastFindOffset = 0;
	unsigned int currentFindOffset = text.find_first_of( TEXT("\r\n"), lastFindOffset );
	while ( currentFindOffset >=lastFindOffset && currentFindOffset<strLen )
	{
		// Store each row of text
		rowTexts.push_back( text.substr( lastFindOffset, currentFindOffset-lastFindOffset ) );

		if ( text[currentFindOffset] == TEXT('\r') && currentFindOffset+1 < strLen && text[currentFindOffset+1] == TEXT('\n') )
		{
			lastFindOffset = currentFindOffset + 2;
		}
		else
		{
			// For CR or LF line endings
			lastFindOffset = currentFindOffset + 1;
		}
		currentFindOffset = text.find_first_of( TEXT("\r\n"), lastFindOffset );
	}
}


MultiClipboardProxy::MultiClipboardProxy()
: isCyclicPasteUndoAction( false )
, ignoreClipTextCount( 0 )
{
}


void MultiClipboardProxy::Init()
{
	// Copied from scintilla's source code
	// There does not seem to be a real standard for indicating that the clipboard
	// contains a rectangular selection, so copy Developer Studio.
	cfColumnSelect = ::RegisterClipboardFormat( TEXT("MSDEVColumnSelect") );

	// Likewise for line-copy (copies a full line when no text is selected)
	cfLineSelect = ::RegisterClipboardFormat( TEXT("MSDEVLineSelect") );
}


void MultiClipboardProxy::Destroy()
{
	if ( clipboardListeners.size() > 0 )
	{
		// Remove Notepad++ from the clipboard chain if it is in it
		::ChangeClipboardChain( g_NppData._nppHandle, hNextClipboardViewer );
	}
}


void MultiClipboardProxy::RegisterClipboardListener( ClipboardListener * pListener )
{
	if ( !pListener )
	{
		return;
	}

	if ( clipboardListeners.size() == 0 )
	{
		// Put Notepad++ into the clipboard chain if not already done so
		hNextClipboardViewer = ::SetClipboardViewer( g_NppData._nppHandle );
	}

	// Add one more clipboard listener
	clipboardListeners.push_back( pListener );
}


void MultiClipboardProxy::OnNewClipboardText( const TextItem & textItem )
{
	if ( ignoreClipTextCount > 0 )
	{
		// Suppose to ignore this clipboard text, due to Notepad++ text format conversion
		--ignoreClipTextCount;
		return;
	}

	for ( unsigned int i = 0; i < clipboardListeners.size(); ++i )
	{
		clipboardListeners[i]->OnNewClipboardText( textItem );
	}
}


void MultiClipboardProxy::OnTextPastedInNpp()
{
	for ( unsigned int i = 0; i < clipboardListeners.size(); ++i )
	{
		clipboardListeners[i]->OnTextPasted();
	}
}


void MultiClipboardProxy::GetTextInSystemClipboard( TextItem & textItem )
{
	if ( !::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
	{
		return;
	}
	if ( !::OpenClipboard( g_NppData._nppHandle ) )
	{
		return;
	}

	HGLOBAL hglb = ::GetClipboardData( CF_UNICODETEXT );
	if ( hglb != NULL )
	{
		textItem.text = LPWSTR( ::GlobalLock( hglb ) );
		::GlobalUnlock( hglb );
		textItem.textMode = GetCurrentClipboardFormat();
	}
	::CloseClipboard();
}


void MultiClipboardProxy::SetTextToSystemClipboard( const TextItem & textItem )
{
	// First, allocate and copy text to a system memory
	int DataSize = (textItem.text.size()+1) * sizeof( std::wstring::value_type );
	HGLOBAL hGlobal = GlobalAlloc( GHND | GMEM_SHARE, DataSize );
	PTSTR pGlobal = (PTSTR)GlobalLock( hGlobal );
	::CopyMemory( pGlobal, textItem.text.c_str(), DataSize );
	GlobalUnlock( hGlobal );
	// Clear clipboard and set our text to it
	OpenClipboard( g_NppData._nppHandle );
	EmptyClipboard();
	SetClipboardData( CF_UNICODETEXT, hGlobal );
	// Also set the text selection mode
	if ( TCM_COLUMN == textItem.textMode )
	{
		::SetClipboardData( cfColumnSelect, 0 );
	}
	else if ( TCM_LINE == textItem.textMode )
	{
		::SetClipboardData( cfLineSelect, 0 );
	}
	CloseClipboard();
}


void MultiClipboardProxy::OnNppTextFormatConversion( UniMode NewFormat )
{
	UniMode CurrentUniMode = GetCurrentEncoding( GetCurrentScintilla() );
	if ( NewFormat == CurrentUniMode )
	{
		// Not converting between Ansi and Unicode/UTF, ignore
		return;
	}

	// Notepad++ uses the system clipboard to hold text temporary when converting
	// So we must ignore the next clipboard text change
	if ( ::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
	{
		// Already contain text in system clipboard, Notepad++ will restore back this text
		// So must ignore two clipboard text changes instead
		ignoreClipTextCount = 2;
	}
	else
	{
		ignoreClipTextCount = 1;
	}
}


TextCopyModeEnum MultiClipboardProxy::GetCurrentClipboardFormat()
{
	if ( ::IsClipboardFormatAvailable( cfLineSelect ) != 0 )
	{
		return TCM_LINE;
	}
	else if ( ::IsClipboardFormatAvailable( cfColumnSelect ) != 0 )
	{
		return TCM_COLUMN;
	}
	return TCM_NORMAL;
}


void MultiClipboardProxy::AddTimer( MVCTimer * pTimer )
{
	if ( timers.find( pTimer->TimerID ) != timers.end() )
	{
		// Timer with this ID already exists.
		// Assume it is being restarted with a new time value,
		// so kill it and re-add
		DeleteTimer( pTimer );
	}

	SetTimer( g_NppData._nppHandle, pTimer->TimerID, pTimer->Time, 0 );
	timers[ pTimer->TimerID ] = pTimer;
}


void MultiClipboardProxy::DeleteTimer( MVCTimer * pTimer )
{
	if ( timers.find( pTimer->TimerID ) == timers.end() )
	{
		// Timer with this ID not found
		return;
	}

	::KillTimer( g_NppData._nppHandle, pTimer->TimerID );
	timers.erase( pTimer->TimerID );
}


BOOL MultiClipboardProxy::OnTimer( UINT EventID )
{
	if ( timers.find( EventID ) == timers.end() )
	{
		// Timer with this ID not found
		return FALSE;
	}

	MVCTimer * pTimer = timers[ EventID ];
	pTimer->OnTimer();
	return TRUE;
}


void MultiClipboardProxy::AddMouseListener( MouseListener * listener )
{
	std::vector< MouseListener * >::iterator mouseIter;
	for ( mouseIter = mouseListeners.begin(); mouseIter != mouseListeners.end(); ++mouseIter )
	{
		if ( listener == *mouseIter )
		{
			// If mouse listener is already registered, then stop
			return;
		}
	}

	// Add this mouse listener
	mouseListeners.push_back( listener );
}


BOOL MultiClipboardProxy::OnMouseEvent( MouseListener::MouseEventType eventType, MouseListener::MouseEventFlags eventFlags,
										INT mouseX, INT mouseY, INT mouseDelta )
{
	BOOL result = FALSE;
	std::vector< MouseListener * >::iterator mouseIter;
	for ( mouseIter = mouseListeners.begin(); mouseIter != mouseListeners.end(); ++mouseIter )
	{
		BOOL thisResult = (*mouseIter)->OnMouseEvent( eventType, eventFlags, mouseX, mouseY, mouseDelta );
		if ( thisResult )
		{
			// This listener wants to trap the message
			result = TRUE;
		}
	}

	return result;
}


void MultiClipboardProxy::AddKeyListener( KeyListener * listener )
{
	std::vector< KeyListener * >::iterator keyIter;
	for ( keyIter = keyListeners.begin(); keyIter != keyListeners.end(); ++keyIter )
	{
		if ( listener == *keyIter )
		{
			// If key listener is already registered, then stop
			return;
		}
	}

	// Add this key listener
	keyListeners.push_back( listener );
}


BOOL MultiClipboardProxy::OnKeyEvent( KeyListener::KeyEventType eventType, INT keyCode )
{
	BOOL result = FALSE;
	std::vector< KeyListener * >::iterator keyIter;
	for ( keyIter = keyListeners.begin(); keyIter != keyListeners.end(); ++keyIter )
	{
		BOOL thisResult = (*keyIter)->OnKeyEvent( eventType, keyCode );
		if ( thisResult )
		{
			// This listener wants to trap the message
			result = TRUE;
		}
	}

	return result;
}


BOOL MultiClipboardProxy::IsNppForegroundWindow()
{
	HWND hForeground = ::GetForegroundWindow();
	if ( hForeground == g_NppData._nppHandle )
	{
		return TRUE;
	}
	return FALSE;
}


void MultiClipboardProxy::SetFocusToDocument()
{
	::SetFocus( GetCurrentScintilla()->hWnd );
}


POINT MultiClipboardProxy::GetMouseCursorPosition()
{
	POINT pt;
	int currentPos = ::GetCursorPos( &pt );

	return pt;
}


POINT MultiClipboardProxy::GetCurrentCaretPosition()
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();

	POINT pt;
	int currentPos = pCurrentScintilla->execute( SCI_GETCURRENTPOS, 0, 0 );
	pt.x = pCurrentScintilla->execute( SCI_POINTXFROMPOSITION, 0, (LPARAM)currentPos );
	pt.y = pCurrentScintilla->execute( SCI_POINTYFROMPOSITION, 0, (LPARAM)currentPos );
	::ClientToScreen( pCurrentScintilla->hWnd, &pt );

	return pt;
}


void MultiClipboardProxy::SetCaretPosition( int pos, HWND hwnd )
{
	SciSubClassWrp * pScintilla = GetScintillaFromHwnd( hwnd );
	pScintilla->execute( SCI_GOTOPOS, pos );
}


int MultiClipboardProxy::GetPositionFromPoint( const POINT & point, HWND hwnd )
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();
	if ( !hwnd )
	{
		hwnd = pCurrentScintilla->hWnd;
	}
	POINT clientPoint = point;
	::ScreenToClient( hwnd, &clientPoint );
	return pCurrentScintilla->execute( SCI_POSITIONFROMPOINT, clientPoint.x, clientPoint.y );
}


void MultiClipboardProxy::GetCurrentSelectionPosition( int & start, int & end )
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();

	start = pCurrentScintilla->execute( SCI_GETSELECTIONSTART, 0, 0 );
	end = pCurrentScintilla->execute( SCI_GETSELECTIONEND, 0, 0 );
}


void MultiClipboardProxy::SetCurrentSelectionPosition( int start, int end, HWND hwnd )
{
	SciSubClassWrp * pScintilla = GetScintillaFromHwnd( hwnd );
	pScintilla->execute( SCI_SETSEL, start, end );
}


int MultiClipboardProxy::SetRectangularSelection( int start, int width, int rows, HWND hwnd )
{
	// Get active scintilla
	SciSubClassWrp * pScintilla = GetScintillaFromHwnd( hwnd );

	int startLine = pScintilla->execute( SCI_LINEFROMPOSITION, start );
	int startCol = pScintilla->execute( SCI_GETCOLUMN, start );
	int end = pScintilla->execute( SCI_FINDCOLUMN, startLine + rows - 1, startCol + width );

	// Select the rectangle from the bottom right to top left, so that it can be replaced in placed
	pScintilla->execute( SCI_SETRECTANGULARSELECTIONANCHOR, end );
	pScintilla->execute( SCI_SETRECTANGULARSELECTIONCARET, start );

	return end;
}


void MultiClipboardProxy::GetSelectionText( TextItem & textItem )
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();

	// Find the length of the text
	int textLength = pCurrentScintilla->execute( SCI_GETSELTEXT, 0, 0 );

	// Create the buffer that will hold the selection text
	std::vector< char > buffer( textLength );
	// And fill it up
	pCurrentScintilla->execute( SCI_GETSELTEXT, 0, (LPARAM)&buffer[0] );

	// Get code page of scintilla
	UINT codePage = ( uni8Bit == GetCurrentEncoding( pCurrentScintilla ) ) ? CP_ACP : CP_UTF8;

	// Create the buffer that will hold the selection text converted into wide char
	std::vector< wchar_t > wbuffer( textLength );
	::MultiByteToWideChar( codePage, 0, &buffer[0], -1, &wbuffer[0], textLength );

	// Set the return value
	textItem.text = &wbuffer[0];

	// Get the current selection mode
	int selMode = pCurrentScintilla->execute( SCI_GETSELECTIONMODE );
	switch ( selMode )
	{
	case SC_SEL_RECTANGLE:
		textItem.textMode = TCM_COLUMN;
		break;

	case SC_SEL_LINES:
		textItem.textMode = TCM_LINE;
		break;

	case SC_SEL_STREAM:
	default:
		textItem.textMode = TCM_NORMAL;
		break;
	}
}


void MultiClipboardProxy::ReplaceSelectionText( const TextItem & textItem )
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();

	if ( textItem.textMode == TCM_COLUMN )
	{
		ReplaceSelectionWithColumnText( textItem.columnText );
	}
	else
	{
		// Create the buffer that will hold the converted text
		std::vector< char > buffer;
		ConvertWideCharToMultiByte( textItem.text, buffer );
		// Paste the text into the editor
		pCurrentScintilla->execute( SCI_REPLACESEL, 0, (LPARAM)&buffer[0] );
	}
}


// Special case text replacement functionality for column text
void MultiClipboardProxy::ReplaceSelectionWithColumnText( const ColumnModeText & columnText )
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();

	BeginUndoAction();

	// Delete the current selection first
	pCurrentScintilla->execute( SCI_REPLACESEL, 0, (LPARAM)"" );
	int cursorPos = pCurrentScintilla->execute( SCI_GETCURRENTPOS );
	InsertColumnTextAtPos( columnText, cursorPos );

	EndUndoAction();
}


void MultiClipboardProxy::InsertTextAtPos( const TextItem & textItem, int pos, HWND hwnd )
{
	// Get active scintilla
	SciSubClassWrp * pScintilla = GetScintillaFromHwnd( hwnd );

	if ( textItem.textMode == TCM_COLUMN )
	{
		InsertColumnTextAtPos( textItem.columnText, pos, hwnd );
	}
	else
	{
		// Create the buffer that will hold the converted text
		std::vector< char > buffer;
		ConvertWideCharToMultiByte( textItem.text, buffer );
		// Paste the text into the editor
		pScintilla->execute( SCI_INSERTTEXT, pos, (LPARAM)&buffer[0] );
	}
}


void MultiClipboardProxy::InsertColumnTextAtPos( const ColumnModeText & columnText, int pos, HWND hwnd )
{
	// Get active scintilla
	SciSubClassWrp * pScintilla = GetScintillaFromHwnd( hwnd );

	BeginUndoAction();
		
	int cursorCol = pScintilla->execute( SCI_GETCOLUMN, pos );
	int cursorLine = pScintilla->execute( SCI_LINEFROMPOSITION, pos );
	int docEndPos = pScintilla->execute( SCI_GETLENGTH );
	int docEndLine = pScintilla->execute( SCI_LINEFROMPOSITION, docEndPos );

	// Ensure that there are enough rows in the document to fit in the column text. Add newlines if not enough
	if ( (unsigned int)(docEndLine - cursorLine) < columnText.GetNumRows() )
	{
		std::wstring eolChar = GetEOLString( hwnd );
		// Get the multibyte representation of the eol characters
		std::vector< char > eolText;
		ConvertWideCharToMultiByte( eolChar, eolText );
		// Add the eol characters to the end of the doc
		unsigned int extraLinesNeeded = columnText.GetNumRows() - ( docEndLine - cursorLine ) - 1;
		for ( unsigned int extraLines = 0; extraLines < extraLinesNeeded; ++extraLines )
		{
			// Length is the number of eolChar characters, while the actual buffer is eolText
			pScintilla->execute( SCI_APPENDTEXT, eolChar.size(), (LPARAM)&eolText[0] );
		}
	}

	// Insert the column of text into the document row by row
	for ( unsigned int row = 0; row < columnText.GetNumRows(); row++)
	{
		// Find the document position of the start and end of this line
		int lineBegin = pScintilla->execute( SCI_POSITIONFROMLINE, cursorLine + row );
		int lineEnd = pScintilla->execute( SCI_GETLINEENDPOSITION, cursorLine + row );
		// And the number of columns in this line
		int lineEndCol = pScintilla->execute( SCI_GETCOLUMN, lineEnd );

		std::wstring rowTextToInsert( columnText.rowTexts[row] );
		// If the column to insert the text is beyond the end of the line...
		int insertPos = lineBegin + cursorCol;
		if ( lineEndCol < cursorCol )
		{
			// ...append spaces to the end of the line
			rowTextToInsert.insert( 0, cursorCol - lineEndCol, TEXT(' ') );
			insertPos = lineBegin + lineEndCol;
		}

		// The buffer that will hold the converted text
		std::vector< char > rowBuffer;
		ConvertWideCharToMultiByte( rowTextToInsert, rowBuffer );
		pScintilla->execute( SCI_INSERTTEXT, insertPos, (LPARAM)&rowBuffer[0] );
	}

	EndUndoAction();
}


void MultiClipboardProxy::BeginUndoAction()
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();
	pCurrentScintilla->execute( SCI_BEGINUNDOACTION );
}


void MultiClipboardProxy::EndUndoAction()
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();
	pCurrentScintilla->execute( SCI_ENDUNDOACTION );
}


void MultiClipboardProxy::AddCyclicPasteListener( CyclicPasteListener * listener )
{
	std::vector< CyclicPasteListener * >::iterator cyclicPasteIter;
	for ( cyclicPasteIter = cyclicPasteListeners.begin(); cyclicPasteIter != cyclicPasteListeners.end(); ++cyclicPasteIter )
	{
		if ( listener == *cyclicPasteIter )
		{
			// If cyclic paste listener is already registered, then stop
			return;
		}
	}

	// Add this mouse listener
	cyclicPasteListeners.push_back( listener );
}


void MultiClipboardProxy::CyclicPasteBegin()
{
	if ( !isCyclicPasteUndoAction )
	{
		// Get active scintilla
		BeginUndoAction();
		isCyclicPasteUndoAction = true;

		// Notify the listeners
		std::vector< CyclicPasteListener * >::iterator cyclicPasteIter;
		for ( cyclicPasteIter = cyclicPasteListeners.begin(); cyclicPasteIter != cyclicPasteListeners.end(); ++cyclicPasteIter )
		{
			(*cyclicPasteIter)->OnCyclicPasteBegin();
		}
	}
}


void MultiClipboardProxy::CyclicPasteEnd()
{
	if ( isCyclicPasteUndoAction )
	{
		EndUndoAction();
		isCyclicPasteUndoAction = false;

		// Notify the listeners
		std::vector< CyclicPasteListener * >::iterator cyclicPasteIter;
		for ( cyclicPasteIter = cyclicPasteListeners.begin(); cyclicPasteIter != cyclicPasteListeners.end(); ++cyclicPasteIter )
		{
			(*cyclicPasteIter)->OnCyclicPasteEnd();
		}
	}
}


EolMode MultiClipboardProxy::GetEOLMode( HWND hwnd )
{
	// Get active scintilla
	SciSubClassWrp * pScintilla = GetScintillaFromHwnd( hwnd );

	switch ( pScintilla->execute( SCI_GETEOLMODE ) )
	{
	case SC_EOL_CRLF:
		return eolCRLF;

	case SC_EOL_CR:
		return eolCR;

	case SC_EOL_LF:
		return eolLF;
	}
	return eolCRLF;
}


std::wstring MultiClipboardProxy::GetEOLString( HWND hwnd )
{
	switch ( GetEOLMode( hwnd ) )
	{
	case eolCRLF: return TEXT("\r\n");
	case eolCR:   return TEXT("\r");
	default:      return TEXT("\n");
	}
}


void MultiClipboardProxy::PasteTextToNpp( const TextItem & textItem )
{
	// Get active scintilla
	SciSubClassWrp * pCurrentScintilla = GetCurrentScintilla();

	// Put text into system clipboard
	SetTextToSystemClipboard( textItem );

	// Paste text into scintilla
	pCurrentScintilla->execute( SCI_PASTE );
}


void MultiClipboardProxy::PrintText( char * format, ... )
{
	char textBuffer[1024];
	va_list args;

	va_start( args, format );
		vsprintf( textBuffer, format, args );
		perror( textBuffer );
	va_end( args );

	GetCurrentScintilla()->execute( SCI_INSERTTEXT, 0, (LPARAM)textBuffer );
}


SciSubClassWrp * MultiClipboardProxy::GetCurrentScintilla()
{
	int currentEdit;
	::SendMessage( g_NppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM) &currentEdit );
	return ( currentEdit == 0 ) ? &g_ScintillaMain : &g_ScintillaSecond;
}


SciSubClassWrp * MultiClipboardProxy::GetScintillaFromHwnd( HWND hwnd )
{
	if ( hwnd && hwnd == g_NppData._scintillaMainHandle )
	{
		return &g_ScintillaMain;
	}
	else if ( hwnd && hwnd == g_NppData._scintillaSecondHandle )
	{
		return &g_ScintillaSecond;
	}
	else
	{
		return GetCurrentScintilla();
	}
}


UniMode MultiClipboardProxy::GetCurrentEncoding( SciSubClassWrp * pScintilla )
{
	if ( !pScintilla )
	{
		return uniEnd;
	}
	if ( pScintilla->execute( SCI_GETCODEPAGE ) == 0 )
	{
		return uni8Bit;
	}
	return uniUTF8;
}


void MultiClipboardProxy::ConvertWideCharToMultiByte( const std::wstring & wideCharText, std::vector<char> & multiByteText )
{
	// Get code page of scintilla
	UINT codePage = ( uni8Bit == GetCurrentEncoding( GetCurrentScintilla() ) ) ? CP_ACP : CP_UTF8;

	// Find the length of the text after conversion
	int textLength = ::WideCharToMultiByte( codePage, 0, wideCharText.c_str(), -1, 0, 0, 0, 0 );
	// Ensure the buffer have sufficient capacity to hold the converted text
	multiByteText.resize( textLength );
	// And fill it up
	::WideCharToMultiByte( codePage, 0, wideCharText.c_str(), -1, &multiByteText[0], textLength, 0, 0 );
}