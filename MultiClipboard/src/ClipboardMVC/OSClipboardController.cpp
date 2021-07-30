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
#include "OSClipboardController.h"
#include "ArraysOfClips.h"
#include "McOptions.h"
#endif

extern MultiClipboardProxy	g_ClipboardProxy;


OSClipboardController::OSClipboardController()
: bGetClipTextFromOS( FALSE )
, bOnlyWhenPastedInNpp( FALSE )
, bIgnoreLargeClipboardText( FALSE )
, LargeClipboardTextSize( 10000 )
{
}


void OSClipboardController::Init( IModel * pNewModel, MultiClipboardProxy * pClipboardProxy, McOptionsManager * pSettings )
{
	IController::Init( pNewModel, pClipboardProxy, pSettings );
	pClipboardProxy->RegisterClipboardListener( this );
}


void OSClipboardController::OnNewClipboardText( const TextItem & textItem )
{
	BOOL isNppForeground = g_ClipboardProxy.IsNppForegroundWindow();
	if ( !bGetClipTextFromOS && !isNppForeground )
	{
		// Get text only when N++ is active application when bGetClipTextFromOS is FALSE
		return;
	}

	if ( bIgnoreLargeClipboardText && textItem.text.size() > LargeClipboardTextSize )
	{
		// Don't store text larger than this size in clipboard list.
		return;
	}

	if ( isNppForeground || !bOnlyWhenPastedInNpp  )
	{
		ArraysOfClips * pClipboardList = (ArraysOfClips *)GetModel();
		if ( !pClipboardList )
		{
			return;
		}
		// Add text to clipboard list
		pClipboardList->AddText( textItem );
	}
}


void OSClipboardController::OnTextPasted()
{
	if ( !(bGetClipTextFromOS && bOnlyWhenPastedInNpp) )
	{
		// Only allow this when we can get text from other applications,
		// and text is now being pasted into N++
		return;
	}

	ArraysOfClips * pClipboardList = (ArraysOfClips *)GetModel();
	if ( !pClipboardList )
	{
		return;
	}

	TextItem textItem;
	g_ClipboardProxy.GetTextInSystemClipboard( textItem );

	if ( textItem.text.size() > 0 )
	{
		// Add text to clipboard list
		pClipboardList->AddText( textItem );
	}
}


void OSClipboardController::OnObserverAdded( McOptionsManager * SettingsManager )
{
	SettingsObserver::OnObserverAdded( SettingsManager );

	// Add default settings if it doesn't exists
	SET_SETTINGS_BOOL( SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_COPY_FROM_OTHER_PROGRAMS, bGetClipTextFromOS )
	SET_SETTINGS_BOOL( SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_ONLY_WHEN_PASTED_IN_NPP, bOnlyWhenPastedInNpp )
	SET_SETTINGS_BOOL( SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_IGNORE_LARGE_TEXT, bIgnoreLargeClipboardText )
	SET_SETTINGS_INT( SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_LARGE_TEXT_SIZE, LargeClipboardTextSize )
}


void OSClipboardController::OnSettingsChanged( const stringType & GroupName, const stringType & SettingName )
{
	if ( GroupName != SETTINGS_GROUP_OSCLIPBOARD )
	{
		return;
	}

	IF_SETTING_CHANGED_BOOL( SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_COPY_FROM_OTHER_PROGRAMS, bGetClipTextFromOS )
	else IF_SETTING_CHANGED_BOOL( SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_ONLY_WHEN_PASTED_IN_NPP, bOnlyWhenPastedInNpp )
	else IF_SETTING_CHANGED_BOOL( SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_IGNORE_LARGE_TEXT, bIgnoreLargeClipboardText )
	else IF_SETTING_CHANGED_INT( SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_LARGE_TEXT_SIZE, LargeClipboardTextSize )
}