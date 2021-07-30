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
#include "SelectedTextAutoCopier.h"
#include "ArraysOfClips.h"
#include "McOptions.h"
#endif

extern MultiClipboardProxy	g_ClipboardProxy;


SelectedTextAutoCopier::SelectedTextAutoCopier()
: PrevSelStart(-1)
, PrevSelEnd(-1)
, MVCTimer(15000, 250)
, IsCyclicPasteActive(false)
{
}


void SelectedTextAutoCopier::Init( IModel * pNewModel, MultiClipboardProxy * pClipboardProxy, McOptionsManager * pSettings )
{
	IController::Init( pNewModel, pClipboardProxy, pSettings );
	pClipboardProxy->RegisterClipboardListener( this );
	pClipboardProxy->AddCyclicPasteListener( this );
}


void SelectedTextAutoCopier::EnableAutoCopy()
{
	IsEnableAutoCopy = true;
	g_ClipboardProxy.AddTimer( this );
}


void SelectedTextAutoCopier::DisableAutoCopy()
{
	IsEnableAutoCopy = false;
	g_ClipboardProxy.DeleteTimer( this );
}


void SelectedTextAutoCopier::OnNewClipboardText( const TextItem & textItem )
{
	// Do nothing
}


void SelectedTextAutoCopier::OnTextPasted()
{
	if ( IsCyclicPasteActive )
	{
		// Don't do anything when cyclic paste is in action
		return;
	}

	// Get the current text selection position
	int CurrSelStart = -1, CurrSelEnd = -1;
	g_ClipboardProxy.GetCurrentSelectionPosition( CurrSelStart, CurrSelEnd );
	// If pasting over the current selection...
	if ( IsEnableAutoCopy && CurrSelStart != CurrSelEnd )
	{
		// ...then restore the backup clipboard entry to paste over
		g_ClipboardProxy.SetTextToSystemClipboard( SystemClipboardBackup );

		// Move the corresponding clipboard list entry to the top
		ArraysOfClips * pClipboardList = (ArraysOfClips*)GetModel();
		int clipboardItemIndex = pClipboardList->GetTextItemIndex( SystemClipboardBackup );
		if ( clipboardItemIndex >= 0 )
		{
			pClipboardList->PasteText( clipboardItemIndex );
		}

		// Actual pasting will be done after this function returns
	}
}


bool SelectedTextAutoCopier::IsSelectionOverlapping( const int CurrSelStart, const int CurrSelEnd )
{
	if ( CurrSelStart == CurrSelEnd )
	{
		// No text selected now, must be no overlapping
		return false;
	}

	if ( PrevSelStart == PrevSelEnd )
	{
		// Previously no text selected, no overlapping possible
		return false;
	}

	if ( PrevSelStart == CurrSelStart && PrevSelEnd == CurrSelEnd )
	{
		// Optimisation: if same region selected, then don't copy text as it has already been copied
		return false;
	}

	// This test catches "underlaps" as well as overlaps
	if ( CurrSelStart <= PrevSelEnd && CurrSelEnd >= PrevSelStart )
	{
		return true;
	}

	// Finally, there's no overlapping
	return false;
}


void SelectedTextAutoCopier::OnTimer()
{
	if ( IsCyclicPasteActive )
	{
		// Don't do anything when cyclic paste is in action
		return;
	}

	// Get the current text selection position
	int CurrSelStart = -1, CurrSelEnd = -1;
	g_ClipboardProxy.GetCurrentSelectionPosition( CurrSelStart, CurrSelEnd );

	// Check if selection has changed from previous time tick
	if ( !( CurrSelStart == PrevSelStart && CurrSelEnd == PrevSelEnd ) )
	{
		// Yes, there is a change, check if the change overlaps with the current selection
		if ( IsSelectionOverlapping( CurrSelStart, CurrSelEnd ) )
		{
			TextItem CurrentSelectionText;
			g_ClipboardProxy.GetSelectionText( CurrentSelectionText );
			ArraysOfClips * pClipboardList = (ArraysOfClips*)GetModel();
			// Update the clipboard list entry with the new selection
			if ( pClipboardList->ModifyTextItem( PreviousSelectionText, CurrentSelectionText ) )
			{
				PreviousSelectionText = CurrentSelectionText;
			}
			// Set the new selection to the system clipboard
			g_ClipboardProxy.SetTextToSystemClipboard( CurrentSelectionText );
		}
		else
		{
			if ( CurrSelStart != CurrSelEnd )
			{
				// Backup system clipboard in case of implicit pasting of selection text
				g_ClipboardProxy.GetTextInSystemClipboard( SystemClipboardBackup );
				// Copy selection text, set it to the system clipboard too
				g_ClipboardProxy.GetSelectionText( PreviousSelectionText );
				g_ClipboardProxy.SetTextToSystemClipboard( PreviousSelectionText );
			}
		}

		// Save selection position
		PrevSelStart = CurrSelStart;
		PrevSelEnd   = CurrSelEnd;
	}
}


void SelectedTextAutoCopier::OnCyclicPasteBegin()
{
	IsCyclicPasteActive = true;
}


void SelectedTextAutoCopier::OnCyclicPasteEnd()
{
	IsCyclicPasteActive = false;
}


void SelectedTextAutoCopier::OnObserverAdded( McOptionsManager * SettingsManager )
{
	SettingsObserver::OnObserverAdded( SettingsManager );

	// Add default settings if it doesn't exists
	SET_SETTINGS_BOOL( SETTINGS_GROUP_AUTO_COPY, SETTINGS_AUTO_COPY_TEXT_SELECTION, IsEnableAutoCopy )
	SET_SETTINGS_INT( SETTINGS_GROUP_AUTO_COPY, SETTINGS_AUTO_COPY_UPDATE_TIME, Time )
}


void SelectedTextAutoCopier::OnSettingsChanged( const stringType & GroupName, const stringType & SettingName )
{
	if ( GroupName != SETTINGS_GROUP_AUTO_COPY )
	{
		return;
	}

	if ( SettingName == SETTINGS_AUTO_COPY_TEXT_SELECTION )
	{
		if ( pSettingsManager->GetBoolSetting( SETTINGS_GROUP_AUTO_COPY, SETTINGS_AUTO_COPY_TEXT_SELECTION ) )
		{
			EnableAutoCopy();
		}
		else
		{
			DisableAutoCopy();
		}
	}
	else if ( SettingName == SETTINGS_AUTO_COPY_UPDATE_TIME )
	{
		Time = pSettingsManager->GetIntSetting( SETTINGS_GROUP_AUTO_COPY, SETTINGS_AUTO_COPY_UPDATE_TIME );
		if ( IsEnableAutoCopy )
		{
			// Refresh the timer update interval
			g_ClipboardProxy.AddTimer( this );
		}
	}
}