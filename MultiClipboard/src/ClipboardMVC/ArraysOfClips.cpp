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
#include "ArraysOfClips.h"
#include <iterator>
#include "McOptions.h"
#endif


#define MULTICLIPBOARD_SESSION_FILENAME TEXT("\\MultiClipboardSession.dat")
#define MULTICLIPBOARD_SESSION_FILE_VERSION 1

extern TCHAR configPath[MAX_PATH];


struct SessionFileHeader
{
	unsigned int FileHeaderSize;
	unsigned int Version;
	unsigned int NumberDataOfClip;

	SessionFileHeader( unsigned int inNumberDataOfClip = 0 )
		: FileHeaderSize( sizeof(SessionFileHeader) )
		, Version( MULTICLIPBOARD_SESSION_FILE_VERSION )
		, NumberDataOfClip( inNumberDataOfClip )
	{}
};


DataOfClip::DataOfClip()
{
}


DataOfClip::DataOfClip( const TextItem & textItem )
: TextItem( textItem )
{
	UpdateColumnText();
}


bool DataOfClip::operator==( const TextItem & rhs ) const
{
	return text == rhs.text && textMode == rhs.textMode;
}


void DataOfClip::UpdateColumnText()
{
	MakeColumnText( columnText );
}


ArraysOfClips::ArraysOfClips()
: MaxListSize( 20 )
, MaxDisplaySize( 10 )
, bSaveClipboardSession( false )
{
}

bool ArraysOfClips::AddText( const TextItem & textItem )
{
	if ( IsTextAvailable( textItem.text ) )
	{
		// Text already in list, don't add double entry
		return false;
	}

	DataOfClip clipboardItem( textItem );
	textList.push_front( clipboardItem );

	// Check if max list size is exceeded
	while ( GetNumText() > GetMaxListSize() )
	{
		// If so, remove the last one from the list
		textList.pop_back();
	}

	OnModified();
	return true;
}


void ArraysOfClips::RemoveText( const unsigned int index )
{
	TextListIterator iter = GetIterAtIndex( index );
	if ( iter == textList.end() )
	{
		return;
	}
	textList.erase( iter );
	OnModified();
}


void ArraysOfClips::RemoveAllTexts()
{
	textList.clear();
	OnModified();
}


const DataOfClip & ArraysOfClips::GetText( const unsigned int index )
{
	TextListIterator iter = GetIterAtIndex( index );
	if ( iter == textList.end() )
	{
		return NullStruct;
	}

	return *iter;
}


const DataOfClip & ArraysOfClips::PasteText( const unsigned int index )
{
	TextListIterator iter = GetIterAtIndex( index );
	if ( iter == textList.end() )
	{
		return NullStruct;
	}

	// Cut it and move it to the front of the list
	textList.push_front( *iter );
	textList.erase( iter );

	OnModified();
	return *(textList.begin());
}


bool ArraysOfClips::EditText( const int index, const std::wstring & newText )
{
	TextListIterator iter = GetIterAtIndex( index );
	if ( iter == textList.end() )
	{
		return false;
	}

	iter->text = newText;
	iter->UpdateColumnText();
	OnModified();
	return true;
}


bool ArraysOfClips::ModifyTextItem( const TextItem & fromTextItem, const TextItem & toTextItem )
{
	TextListIterator iter;
	for ( iter = textList.begin(); iter != textList.end(); ++iter )
	{
		if ( *iter == fromTextItem )
		{
			*iter = toTextItem;
			iter->UpdateColumnText();
			OnModified();
			return true;
		}
	}
	return false;
}


void ArraysOfClips::SetTextNewIndex( const unsigned int index, const unsigned int newIndex )
{
	if ( index    < 0 || index    >= GetNumText() ||
		 newIndex < 0 || newIndex >= GetNumText() ||
		 newIndex == index )
	{
		return;
	}

	TextListIterator CurrPosition = GetIterAtIndex( index );
	TextListIterator NewPosition;
	if ( index < newIndex )
	{
		NewPosition = GetIterAtIndex( newIndex+1 );
	}
	else
	{
		NewPosition = GetIterAtIndex( newIndex );
	}
	textList.insert( NewPosition, *CurrPosition );
	textList.erase( CurrPosition );
	OnModified();
}


bool ArraysOfClips::IsTextAvailable( const std::wstring & text ) const
{
	ConstTextListIterator iter;
	for ( iter = textList.begin(); iter != textList.end(); ++iter )
	{
		if ( iter->text == text )
		{
			return true;
		}
	}
	return false;
}


int ArraysOfClips::GetTextItemIndex( const TextItem & text ) const
{
	int textIndex = 0;
	ConstTextListIterator iter;
	for ( iter = textList.begin(); iter != textList.end(); ++iter, ++textIndex )
	{
		if ( *iter == text )
		{
			return textIndex;
		}
	}
	return -1;
}


unsigned int ArraysOfClips::GetNumText() const
{
	return (unsigned int)textList.size();
}

unsigned int ArraysOfClips::GetNumDisplay() const
{
	return min(textList.size(), MaxDisplaySize);
}


void ArraysOfClips::SetMaxListSize( const int NewSize )
{
	if ( NewSize <= 0 )
	{
		return;
	}

	MaxListSize = NewSize;

	bool NeedToTrim = GetNumText() > GetMaxListSize();
	// Trim clipboard if necessary to new size
	while ( GetNumText() > GetMaxListSize() )
	{
		textList.pop_back();
	}

	if ( NeedToTrim )
	{
		// Inform all clipboard viewers that the text list has changed
		OnModified();
	}
}


void ArraysOfClips::SaveClipboardSession()
{
	if ( bSaveClipboardSession )
	{
		TCHAR SessionFilePath[MAX_PATH];
		lstrcpy( SessionFilePath, configPath );
		lstrcat( SessionFilePath, MULTICLIPBOARD_SESSION_FILENAME );

		HANDLE hSessionFile = ::CreateFile( SessionFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hSessionFile != INVALID_HANDLE_VALUE )
		{
			SessionFileHeader header( GetNumText() );
			DWORD BytesWritten = 0;
			::WriteFile( hSessionFile, &header, header.FileHeaderSize, &BytesWritten, NULL );

			// Use a reverse iterator here because when loading, items will be added in reverse order
			ConstReverseTextListIterator iter;
			for ( iter = textList.rbegin(); iter != textList.rend(); ++iter )
			{
				unsigned int textSize = iter->text.size();
				unsigned int textMode = iter->textMode;
				::WriteFile( hSessionFile, &textSize, sizeof(textSize), &BytesWritten, NULL );
				::WriteFile( hSessionFile, &textMode, sizeof(textMode), &BytesWritten, NULL );
				::WriteFile( hSessionFile, iter->text.c_str(), sizeof(std::wstring::value_type)*textSize, &BytesWritten, NULL );
			}

			::CloseHandle( hSessionFile );
		}
	}
}


void ArraysOfClips::LoadClipboardSession()
{
	if ( bSaveClipboardSession )
	{
		TCHAR SessionFilePath[MAX_PATH];
		lstrcpy( SessionFilePath, configPath );
		lstrcat( SessionFilePath, MULTICLIPBOARD_SESSION_FILENAME );

		HANDLE hSessionFile = ::CreateFile( SessionFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hSessionFile != INVALID_HANDLE_VALUE )
		{
			SessionFileHeader header;
			DWORD BytesRead = 0;
			::ReadFile( hSessionFile, &header, header.FileHeaderSize, &BytesRead, NULL );

			for ( unsigned int textIndex = 0; textIndex < header.NumberDataOfClip; ++textIndex )
			{
				unsigned int textSize = 0;
				unsigned int textMode = 0;
				::ReadFile( hSessionFile, &textSize, sizeof(textSize), &BytesRead, NULL );
				::ReadFile( hSessionFile, &textMode, sizeof(textMode), &BytesRead, NULL );
				// Copy the text item to a buffer
				std::vector< unsigned char > buffer( sizeof(std::wstring::value_type)*textSize );
				::ReadFile( hSessionFile, &buffer[0], buffer.size(), &BytesRead, NULL );
				// Add the text to the clipboard list
				TextItem textItem;
				textItem.textMode = (TextCopyModeEnum) textMode;
				textItem.text.assign( (std::wstring::value_type*) &buffer[0], textSize );
				AddText( textItem );
			}

			::CloseHandle( hSessionFile );
		}
	}
}


ArraysOfClips::TextListType::iterator ArraysOfClips::GetIterAtIndex( const unsigned int index )
{
	if ( index >= GetNumText() )
	{
		return textList.end();
	}

	TextListType::iterator iter = textList.begin();
	std::advance( iter, index );
	return iter;
}


void ArraysOfClips::OnObserverAdded( McOptionsManager * SettingsManager )
{
	SettingsObserver::OnObserverAdded( SettingsManager );

	// Add default settings if it doesn't exists
	SET_SETTINGS_INT( SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_MAX_CLIPBOARD_ITEMS, MaxListSize )
	SET_SETTINGS_INT( SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_MAX_DISPLAY_ITEMS, MaxDisplaySize )
	SET_SETTINGS_BOOL( SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_SAVE_CLIPBOARD_SESSION, bSaveClipboardSession )
}


void ArraysOfClips::OnSettingsChanged( const stringType & GroupName, const stringType & SettingName )
{
	if ( GroupName != SETTINGS_GROUP_CLIPBOARDLIST )
	{
		return;
	}

	int NewMaxClipboardItems = 0;
	IF_SETTING_CHANGED_INT( SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_MAX_CLIPBOARD_ITEMS, NewMaxClipboardItems )
	IF_SETTING_CHANGED_INT( SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_MAX_DISPLAY_ITEMS, MaxDisplaySize )
	else IF_SETTING_CHANGED_BOOL( SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_SAVE_CLIPBOARD_SESSION, bSaveClipboardSession )
	if ( NewMaxClipboardItems > 0 )
	{
		SetMaxListSize( NewMaxClipboardItems );
	}
}