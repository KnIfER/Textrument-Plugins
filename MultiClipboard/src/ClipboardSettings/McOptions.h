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

#ifndef MULTI_CLIPBOARD_SETTINGS_H
#define MULTI_CLIPBOARD_SETTINGS_H

#include "../../PowerEditor/src/wutils.h"

// Settings group and value names
#define SETTINGS_GROUP_CLIPBOARDLIST		TEXT("ClipboardList")
#define SETTINGS_MAX_CLIPBOARD_ITEMS		TEXT("MaxClipboardItems")
#define SETTINGS_MAX_DISPLAY_ITEMS			TEXT("MaxDisplayItems")
#define SETTINGS_SAVE_CLIPBOARD_SESSION		TEXT("SaveClipboardSession")

#define SETTINGS_GROUP_OSCLIPBOARD			TEXT("OSClipboard")
#define SETTINGS_COPY_FROM_OTHER_PROGRAMS	TEXT("CopyFromOtherPrograms")
#define SETTINGS_ONLY_WHEN_PASTED_IN_NPP	TEXT("OnlyWhenPastedInNpp")
#define SETTINGS_IGNORE_LARGE_TEXT			TEXT("IgnoreLargeText")
#define SETTINGS_LARGE_TEXT_SIZE			TEXT("IgnoreLargeTextSize")

#define SETTINGS_GROUP_PASTE_MENU			TEXT("PasteMenu")
#define SETTINGS_USE_PASTE_MENU				TEXT("UsePasteMenu")
#define SETTINGS_SHOW_NUMBERED_PASTE_MENU	TEXT("ShowNumberedPasteMenu")
#define SETTINGS_PASTE_MENU_WIDTH			TEXT("PasteMenuWidth")
#define SETTINGS_MIDDLE_CLICK_PASTE			TEXT("MiddleClickPaste")

#define SETTINGS_GROUP_AUTO_COPY			TEXT("AutoCopy")
#define SETTINGS_AUTO_COPY_TEXT_SELECTION	TEXT("AutoCopyTextSelection")
#define SETTINGS_AUTO_COPY_UPDATE_TIME		TEXT("AutoCopyUpdateTime")

#define SETTINGS_GROUP_MULTI_CLIP_VIEWER	TEXT("MultiClipViewer")
#define SETTINGS_NO_EDIT_LARGE_TEXT			TEXT("NoEditLargeText")
#define SETTINGS_NO_EDIT_LARGE_TEXT_SIZE	TEXT("NoEditLargeTextSize")
#define SETTINGS_LARGE_TEXT_DISPLAY_SIZE	TEXT("LargeTextDisplaySize")
#define SETTINGS_PASTE_ALL_REVERSE			TEXT("PasteAllReverse")
#define SETTINGS_PASTE_ALL_NEWLINE_BETWEEN	TEXT("PasteAllNewLine")


#define SET_SETTINGS_BOOL( SETTING_GROUP, SETTING_NAME, SETTING_VARIABLE ) \
	if ( !pSettingsManager->IsSettingExists( SETTING_GROUP, SETTING_NAME ) ) \
		pSettingsManager->SetBoolSetting( SETTING_GROUP, SETTING_NAME, SETTING_VARIABLE != FALSE ); \
	else \
		OnSettingsChanged( SETTING_GROUP, SETTING_NAME );

#define SET_SETTINGS_INT( SETTING_GROUP, SETTING_NAME, SETTING_VARIABLE ) \
	if ( !pSettingsManager->IsSettingExists( SETTING_GROUP, SETTING_NAME ) ) \
		pSettingsManager->SetIntSetting( SETTING_GROUP, SETTING_NAME, SETTING_VARIABLE ); \
	else \
		OnSettingsChanged( SETTING_GROUP, SETTING_NAME );

#define IF_SETTING_CHANGED_BOOL( SETTING_GROUP, SETTING_NAME, SETTING_VARIABLE ) \
	if ( SettingName == SETTING_NAME ) \
		SETTING_VARIABLE = pSettingsManager->GetBoolSetting( SETTING_GROUP, SETTING_NAME );

#define IF_SETTING_CHANGED_INT( SETTING_GROUP, SETTING_NAME, SETTING_VARIABLE ) \
	if ( SettingName == SETTING_NAME ) \
		SETTING_VARIABLE = pSettingsManager->GetIntSetting( SETTING_GROUP, SETTING_NAME );


#endif