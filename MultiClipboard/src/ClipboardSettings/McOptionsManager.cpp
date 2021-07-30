/*
This file is part of McOptionsManager project
Copyright (C) 2009 LoonyChewy
http://www.peepor.net/loonchew/index.php?p=loonylib

This project is dedicated to the public domain
For details, see http://creativecommons.org/licenses/publicdomain/
*/

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "McOptionsManager.h" // LnooySettingsManager
#include "../TinyXML/tinyxml.h"
#include <vector>
#include <algorithm>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


void ConvertWCharToChar( const stringType & str, std::vector< char > & buffer )
{
#ifdef UNICODE
	int strLength = ::WideCharToMultiByte( CP_ACP, 0, str.c_str(), -1, 0, 0, 0, 0 );
	if ( strLength == 0 )
	{
		// Something when wrong during the conversion
		return;
	}
	buffer.reserve( strLength );
	::WideCharToMultiByte( CP_ACP, 0, str.c_str(), -1, &buffer[0], strLength, 0, 0 );
#else
	buffer.reserve( str.length() );
	::strcpy( &buffer[0], str.c_str() );
#endif
}


void ConvertCharToWChar( const char * pInStr, stringType & OutStr )
{
#ifdef UNICODE
	std::vector< wchar_t > buffer( 64 );
	int strLength = ::MultiByteToWideChar( CP_ACP, 0, pInStr, -1, 0, 0 );
	if ( strLength == 0 )
	{
		// Something when wrong during the conversion
		return;
	}
	buffer.reserve( strLength );
	::MultiByteToWideChar( CP_ACP, 0, pInStr, -1, &buffer[0], strLength );
	OutStr = &buffer[0];
#else
	OutStr = pInStr;
#endif
}


McOptionsManager::McOptionsManager( TCHAR * RootName, TCHAR * Version )
: RootElementName( RootName )
, VersionString( Version )
{
	CurrentSettingsGroupIter = Settings.end();
}


McOptionsManager::~McOptionsManager()
{
	for ( SettingsType::iterator i1 = Settings.begin(); i1 != Settings.end(); ++i1 )
	{
		SettingsGroupType * SettingsGroup = i1->second;
		for ( SettingsGroupType::iterator i2 = SettingsGroup->begin(); i2 != SettingsGroup->end(); ++i2 )
		{
			delete i2->second;
		}
		SettingsGroup->empty();
		delete SettingsGroup;
	}
	Settings.empty();
}


bool McOptionsManager::LoadSettings( TCHAR * FilePath )
{
	std::vector< char > tempBuffer(128);
	ConvertWCharToChar( FilePath, tempBuffer );

	TiXmlDocument SettingsDoc( &tempBuffer[0] );
	if ( !SettingsDoc.LoadFile() )
	{
		return false;
	}

	// Try to find the xml declaration
	TiXmlNode * pChildNode = SettingsDoc.FirstChild();
	TiXmlDeclaration * pDecl = 0;
	while ( pChildNode )
	{
		pDecl = pChildNode->ToDeclaration();
		if ( pDecl )
		{
			// Xml declaration node found
			break;
		}
		pChildNode = pChildNode->NextSibling();
	}
	ConvertWCharToChar( VersionString, tempBuffer );
	if ( !pDecl || 0 != strcmp( pDecl->Version(), &tempBuffer[0] ) )
	{
		return false;
	}

	// Get the root node
	TiXmlElement * pRoot = SettingsDoc.FirstChildElement()->ToElement();
	ConvertWCharToChar( RootElementName, tempBuffer );
	// should always have a valid root but handle gracefully if it does
	if ( !pRoot || 0 != strcmp( pRoot->Value(), &tempBuffer[0] ) )
	{
		return false;
	}

	// Iterate the groups
	TiXmlElement * pCurrentGroup = pRoot->FirstChildElement( "Group" );
	while ( pCurrentGroup )
	{
		const char * pGroupName = pCurrentGroup->Attribute( "name" );
		stringType GroupName;
		ConvertCharToWChar( pGroupName, GroupName );

		// Iterate the values in this group
		TiXmlElement * pCurrentValue = pCurrentGroup->FirstChildElement( "Value" );
		while ( pCurrentValue )
		{
			const char * pValueName = pCurrentValue->Attribute( "name" );
			const char * pValueType = pCurrentValue->Attribute( "type" );
			const char * pValueText = pCurrentValue->GetText();

			if ( !pValueName || !pValueType || !pValueText )
			{
				// Advance to the next value
				pCurrentValue = pCurrentValue->NextSiblingElement( "Value" );
				continue;
			}

			stringType SettingName;
			ConvertCharToWChar( pValueName, SettingName );
			if ( 0 == strcmp( pValueType, "bool" ) )
			{
				if ( 0 == strcmp( pValueText, "true" ) )
				{
					SetBoolSetting( GroupName, SettingName, true );
				}
				else if ( 0 == strcmp( pValueText, "false" ) )
				{
					SetBoolSetting( GroupName, SettingName, false );
				}
			}
			else if ( 0 == strcmp( pValueType, "int" ) )
			{
				int ValueInt = ::atoi( pValueText );
				SetIntSetting( GroupName, SettingName, ValueInt );
			}
			else if ( 0 == strcmp( pValueType, "float" ) )
			{
				float ValueFloat = (float)::atof( pValueText );
				SetFloatSetting( GroupName, SettingName, ValueFloat );
			}
			else if ( 0 == strcmp( pValueType, "string" ) )
			{
				stringType ValueString;
				ConvertCharToWChar( pValueText, ValueString );
				SetStringSetting( GroupName, SettingName, ValueString );
			}

			// Advance to the next value
			pCurrentValue = pCurrentValue->NextSiblingElement( "Value" );
		}

		// Advance to the next group
		pCurrentGroup = pCurrentGroup->NextSiblingElement( "Group" );
	}
	return true;
}


void McOptionsManager::SaveSettings( TCHAR * FilePath )
{
	std::vector< char > tempBuffer(128);
	TiXmlDocument SettingsDoc;

	ConvertWCharToChar( VersionString, tempBuffer );
	SettingsDoc.LinkEndChild( new TiXmlDeclaration( &tempBuffer[0], "", "" ) );
	ConvertWCharToChar( RootElementName, tempBuffer );
	TiXmlElement * pRootElement = new TiXmlElement( &tempBuffer[0] );
	SettingsDoc.LinkEndChild( pRootElement );

	// New elements to be created for the settings
	TiXmlElement * pCurrentGroup = 0;
	TiXmlElement * pCurrentValue = 0;
	TiXmlText * pValueText = 0;

	for ( bool LoopGroups = FirstSettingsGroup(); LoopGroups; LoopGroups = NextSettingsGroup())
	{
		stringType GroupName = GetCurrentSettingsGroupName();
		ConvertWCharToChar( GroupName, tempBuffer );

		// Create a new xml node for this settting group
		pCurrentGroup = new TiXmlElement( "Group" );
		pCurrentGroup->SetAttribute( "name", &tempBuffer[0] );
		pRootElement->LinkEndChild( pCurrentGroup );

		for ( bool LoopValues = FirstSettingValue(); LoopValues; LoopValues = NextSettingValue())
		{
			stringType ValueName = GetCurrentSettingValueName();
			const SettingValue * pValue = GetCurrentSettingValue();
			ConvertWCharToChar( ValueName, tempBuffer );

			// Create a new xml node for this value
			pCurrentValue = new TiXmlElement( "Value" );
			pCurrentValue->SetAttribute( "name", &tempBuffer[0] );
			switch ( pValue->Type )
			{
			case SVT_BOOL:
				pCurrentValue->SetAttribute( "type", "bool" );
				break;

			case SVT_INT:
				pCurrentValue->SetAttribute( "type", "int" );
				break;

			case SVT_FLOAT:
				pCurrentValue->SetAttribute( "type", "float" );
				break;

			case SVT_STRING:
				pCurrentValue->SetAttribute( "type", "string" );
				break;
			}
			// Add the value under the group node
			pCurrentGroup->LinkEndChild( pCurrentValue );

			// Create a new xml node for the actual value itself
			switch ( pValue->Type )
			{
			case SVT_BOOL:
				pValueText = new TiXmlText( pValue->UnionValue.BoolValue ? "true" : "false" );
				break;

			case SVT_INT:
				::_itoa( pValue->UnionValue.IntValue, &tempBuffer[0], 10 );
				pValueText = new TiXmlText( &tempBuffer[0] );
				break;

			case SVT_FLOAT:
				::sprintf( &tempBuffer[0], "%f", pValue->UnionValue.FloatValue );
				pValueText = new TiXmlText( &tempBuffer[0] );
				break;

			case SVT_STRING:
				ConvertWCharToChar( pValue->StringValue, tempBuffer );
				pValueText = new TiXmlText( &tempBuffer[0] );
				break;
			}
			// Add the actual value under the value node
			pCurrentValue->LinkEndChild( pValueText );
		}
	}

	ConvertWCharToChar( FilePath, tempBuffer );
	SettingsDoc.SaveFile( &tempBuffer[0] );
}


void McOptionsManager::LoadDefaultSettings()
{
	// Child classes can override this
}


bool McOptionsManager::IsSettingExists( const stringType & GroupName, const stringType & SettingName )
{
	return 0 != GetSetting( GroupName, SettingName );
}


bool McOptionsManager::GetBoolSetting( const stringType & GroupName, const stringType & SettingName )
{
	SettingValue * Value = GetSetting( GroupName, SettingName );
	if ( Value == 0 || Value->Type != SVT_BOOL )
	{
		return false;
	}
	return Value->UnionValue.BoolValue;
}


int McOptionsManager::GetIntSetting( const stringType & GroupName, const stringType & SettingName )
{
	SettingValue * Value = GetSetting( GroupName, SettingName );
	if ( Value == 0 || Value->Type != SVT_INT )
	{
		return 0;
	}
	return Value->UnionValue.IntValue;
}


float McOptionsManager::GetFloatSetting( const stringType & GroupName, const stringType & SettingName )
{
	SettingValue * Value = GetSetting( GroupName, SettingName );
	if ( Value == 0 || Value->Type != SVT_FLOAT )
	{
		return 0.0f;
	}
	return Value->UnionValue.FloatValue;
}


stringType McOptionsManager::GetStringSetting( const stringType & GroupName, const stringType & SettingName )
{
	SettingValue * Value = GetSetting( GroupName, SettingName );
	if ( Value == 0 || Value->Type != SVT_STRING )
	{
		return stringType();
	}
	return Value->StringValue;
}


void McOptionsManager::SetBoolSetting( const stringType & GroupName, const stringType & SettingName, const bool Value )
{
	SettingValue * ValueToSet = GetSetting( GroupName, SettingName );
	if ( ValueToSet == 0 )
	{
		ValueToSet = AddNewSetting( GroupName, SettingName );
		if ( ValueToSet == 0 )
		{
			return;
		}
	}
	ValueToSet->Type = SVT_BOOL;
	ValueToSet->UnionValue.BoolValue = Value;

	// Inform the observers
	UpdateSettingsObservers( GroupName, SettingName );
}


void McOptionsManager::SetIntSetting( const stringType & GroupName, const stringType & SettingName, const int Value )
{
	SettingValue * ValueToSet = GetSetting( GroupName, SettingName );
	if ( ValueToSet == 0 )
	{
		ValueToSet = AddNewSetting( GroupName, SettingName );
		if ( ValueToSet == 0 )
		{
			return;
		}
	}
	ValueToSet->Type = SVT_INT;
	ValueToSet->UnionValue.IntValue = Value;

	// Inform the observers
	UpdateSettingsObservers( GroupName, SettingName );
}


void McOptionsManager::SetFloatSetting( const stringType & GroupName, const stringType & SettingName, const float Value )
{
	SettingValue * ValueToSet = GetSetting( GroupName, SettingName );
	if ( ValueToSet == 0 )
	{
		ValueToSet = AddNewSetting( GroupName, SettingName );
		if ( ValueToSet == 0 )
		{
			return;
		}
	}
	ValueToSet->Type = SVT_FLOAT;
	ValueToSet->UnionValue.FloatValue = Value;

	// Inform the observers
	UpdateSettingsObservers( GroupName, SettingName );
}


void McOptionsManager::SetStringSetting( const stringType & GroupName, const stringType & SettingName, const stringType & Value )
{
	SettingValue * ValueToSet = GetSetting( GroupName, SettingName );
	if ( ValueToSet == 0 )
	{
		ValueToSet = AddNewSetting( GroupName, SettingName );
		if ( ValueToSet == 0 )
		{
			return;
		}
	}
	ValueToSet->Type = SVT_STRING;
	ValueToSet->StringValue = Value;

	// Inform the observers
	UpdateSettingsObservers( GroupName, SettingName );
}


bool McOptionsManager::FirstSettingsGroup()
{
	CurrentSettingsGroupIter = Settings.begin();
	return CurrentSettingsGroupIter != Settings.end();
}


bool McOptionsManager::NextSettingsGroup()
{
	++CurrentSettingsGroupIter;
	return CurrentSettingsGroupIter != Settings.end();
}


stringType McOptionsManager::GetCurrentSettingsGroupName()
{
	if ( CurrentSettingsGroupIter == Settings.end() )
	{
		return stringType();
	}
	return CurrentSettingsGroupIter->first;
}


bool McOptionsManager::FirstSettingValue()
{
	if ( CurrentSettingsGroupIter == Settings.end() )
	{
		return false;
	}
	CurrentSettingValueIter = CurrentSettingsGroupIter->second->begin();
	return CurrentSettingValueIter != CurrentSettingsGroupIter->second->end();
}


bool McOptionsManager::NextSettingValue()
{
	if ( CurrentSettingsGroupIter == Settings.end() )
	{
		return false;
	}
	++CurrentSettingValueIter;
	return CurrentSettingValueIter != CurrentSettingsGroupIter->second->end();
}


stringType McOptionsManager::GetCurrentSettingValueName()
{
	if ( CurrentSettingsGroupIter == Settings.end() )
	{
		return stringType();
	}
	if ( CurrentSettingValueIter == CurrentSettingsGroupIter->second->end() )
	{
		return stringType();
	}
	return CurrentSettingValueIter->first;
}


const SettingValue * McOptionsManager::GetCurrentSettingValue()
{
	if ( CurrentSettingsGroupIter == Settings.end() )
	{
		return 0;
	}
	if ( CurrentSettingValueIter == CurrentSettingsGroupIter->second->end() )
	{
		return 0;
	}
	return CurrentSettingValueIter->second;
}


SettingValue * McOptionsManager::GetSetting( const stringType & GroupName, const stringType & SettingName )
{
	SettingsType::iterator GroupIter = Settings.find( GroupName );
	if ( GroupIter == Settings.end() )
	{
		return 0;
	}

	SettingsGroupType * SettingsGroup = GroupIter->second;
	SettingsGroupType::iterator SettingsIter = SettingsGroup->find( SettingName );
	if ( SettingsIter == SettingsGroup->end() )
	{
		return 0;
	}
	return SettingsIter->second;
}


SettingValue * McOptionsManager::AddNewSetting( const stringType & GroupName, const stringType & SettingName )
{
	typedef std::pair< stringType, SettingsGroupType *  > SettingsGroupPair;
	typedef std::pair< stringType, SettingValue * > SettingValuePair;

	SettingsType::iterator GroupIter = Settings.find( GroupName );
	if ( GroupIter == Settings.end() )
	{
		SettingsGroupType * NewGroup = new SettingsGroupType;
		GroupIter = Settings.insert( Settings.begin(), SettingsGroupPair( GroupName, NewGroup ) );
	}

	SettingsGroupType * SettingsGroup = GroupIter->second;
	SettingsGroupType::iterator SettingsIter = SettingsGroup->find( SettingName );
	if ( SettingsIter == SettingsGroup->end() )
	{
		SettingValue * NewValue = new SettingValue;
		SettingsIter = SettingsGroup->insert( SettingsGroup->begin(), SettingValuePair( SettingName, NewValue ) );
		return NewValue;
	}
	return 0;
}


void McOptionsManager::AddSettingsObserver( SettingsObserver * pObserver )
{
	if ( !pObserver )
	{
		return;
	}
	SettingsObserversList.push_back( pObserver );
	pObserver->OnObserverAdded( this );
}


void McOptionsManager::RemoveSettingsObserver( SettingsObserver * pObserver )
{
	if ( !pObserver )
	{
		return;
	}
	SettingsObserversIterator iter;
	iter = std::find( SettingsObserversList.begin(), SettingsObserversList.end(), pObserver );
	if ( iter != SettingsObserversList.end() )
	{
		SettingsObserversList.erase( iter );
	}
}


void McOptionsManager::UpdateSettingsObservers( const stringType & GroupName, const stringType & SettingName )
{
	for ( SettingsObserversIterator iter = SettingsObserversList.begin(); iter != SettingsObserversList.end(); ++iter )
	{
		(*iter)->OnSettingsChanged( GroupName, SettingName );
	}
}



SettingsObserver::SettingsObserver()
: pSettingsManager(0)
{
}


void SettingsObserver::OnObserverAdded( McOptionsManager * SettingsManager )
{
	pSettingsManager = SettingsManager;
}


void SettingsObserver::OnSettingsChanged( const stringType & GroupName, const stringType & SettingName )
{
}