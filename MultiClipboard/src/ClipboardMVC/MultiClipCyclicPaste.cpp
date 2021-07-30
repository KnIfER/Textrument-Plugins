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
#include "MultiClipCyclicPaste.h"
#include "ArraysOfClips.h"
#include "MultiClipboardProxy.h"
#include "McOptions.h"
#endif


extern MultiClipboardProxy g_ClipboardProxy;


MultiClipCyclicPaste::MultiClipCyclicPaste()
: selectionPosStart( 0 )
, selectionPosEnd( 0 )
, nextPasteIndex( 0 )
{
}


void MultiClipCyclicPaste::Init( IModel * pNewModel, MultiClipboardProxy * pClipboardProxy, McOptionsManager * pSettings )
{
	IController::Init( pNewModel, pClipboardProxy, pSettings );
	pClipboardProxy->AddCyclicPasteListener( this );
}


void MultiClipCyclicPaste::DoCyclicPaste()
{
	ArraysOfClips * pClipboardList = (ArraysOfClips*)GetModel();
	if ( !pClipboardList || pClipboardList->GetNumText() <= 0 )
	{
		return;
	}

	// begin the undo action if not already so, to prevent unnecessary undos for the cyclic pastes
	g_ClipboardProxy.CyclicPasteBegin();

	// get scintilla selection pos
	int currentPosStart = 0, currentPosEnd = 0;
	g_ClipboardProxy.GetCurrentSelectionPosition( currentPosStart, currentPosEnd );

	// compare with current pos
	if ( !( currentPosStart == selectionPosStart && currentPosEnd == selectionPosEnd ) )
	{
		// if different, reset text index
		ResetPasteIndex();
	}

	// paste text into current selection pos
	const DataOfClip & itemToPaste = pClipboardList->GetText( nextPasteIndex );
	g_ClipboardProxy.ReplaceSelectionText( itemToPaste );

	// Select this newly pasted text
	if ( itemToPaste.textMode == TCM_COLUMN )
	{
		currentPosEnd = g_ClipboardProxy.SetRectangularSelection( currentPosStart, itemToPaste.columnText.GetNumColumns(), itemToPaste.columnText.GetNumRows() );
	}
	else
	{
		currentPosEnd = currentPosStart + pClipboardList->GetText( nextPasteIndex ).text.size();
		g_ClipboardProxy.SetCurrentSelectionPosition( currentPosStart, currentPosEnd );
	}

	// Update next paste index
	++nextPasteIndex;
	if ( (unsigned int) nextPasteIndex >= pClipboardList->GetNumText() )
	{
		ResetPasteIndex();
	}

	// update stored selection pos
	selectionPosStart = currentPosStart;
	selectionPosEnd = currentPosEnd;
}


void MultiClipCyclicPaste::ResetPasteIndex()
{
	nextPasteIndex = 0;
}


void MultiClipCyclicPaste::OnCyclicPasteBegin()
{
}


void MultiClipCyclicPaste::OnCyclicPasteEnd()
{
	int currPasteIndex = nextPasteIndex - 1;
	ResetPasteIndex();

	ArraysOfClips * pClipboardList = (ArraysOfClips*)GetModel();
	if ( !pClipboardList || pClipboardList->GetNumText() <= 0 )
	{
		return;
	}
	if ( currPasteIndex < 0 )
	{
		// Handle wrap around
		currPasteIndex = pClipboardList->GetNumText() - 1;
	}
	pClipboardList->PasteText( (unsigned int)currPasteIndex );
	selectionPosStart = -1;
	selectionPosEnd = -1;
}


void MultiClipCyclicPaste::OnModelModified()
{
	ResetPasteIndex();
}


void MultiClipCyclicPaste::OnObserverAdded( McOptionsManager * SettingsManager )
{
	SettingsObserver::OnObserverAdded( SettingsManager );
}


void MultiClipCyclicPaste::OnSettingsChanged( const stringType & GroupName, const stringType & SettingName )
{
}