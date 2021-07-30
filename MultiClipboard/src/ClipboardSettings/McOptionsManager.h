/*
This file is part of McOptionsManager project
Copyright (C) 2009 LoonyChewy
http://www.peepor.net/loonchew/index.php?p=loonylib

This project is dedicated to the public domain.
For details, see http://creativecommons.org/licenses/publicdomain/
*/

#ifndef LOONY_SETTINGS_MANAGER
#define LOONY_SETTINGS_MANAGER


#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include <map>
#include <string>
#include <list>
#endif

// Define string types
#ifdef UNICODE
	typedef std::wstring stringType;
	typedef wchar_t TCHAR;
#else
	typedef std::string stringType;
	typedef char TCHAR;
#endif //UNICODE

#ifndef TEXT
	#ifdef UNICODE
		#define __TEXT(quote) L##quote
	#else
		#define __TEXT(quote) ##quote
	#endif //UNICODE
	#define TEXT(quote) __TEXT(quote)
#endif	// TEXT


enum SettingValueType
{
	SVT_INVALID,
	SVT_BOOL,
	SVT_INT,
	SVT_FLOAT,
	SVT_STRING
};

struct SettingValue
{
	SettingValueType Type;
	union
	{
		bool BoolValue;
		int IntValue;
		float FloatValue;
	} UnionValue;
	// Can't have std string in union, so its placed outside for a bit of wasted space
	stringType StringValue;

	SettingValue() : Type( SVT_INVALID )
	{
		::memset( &UnionValue, 0, sizeof( UnionValue ) );
	}
};


typedef std::map< stringType, SettingValue * > SettingsGroupType;
typedef std::map< stringType, SettingsGroupType * > SettingsType;


// Forward declare this class
class SettingsObserver;
typedef std::list< SettingsObserver * > SettingsObserversListType;
typedef SettingsObserversListType::iterator SettingsObserversIterator;

class McOptionsManager
{
public:
	McOptionsManager( TCHAR * RootName, TCHAR * Version = TEXT("1.0") );
	virtual ~McOptionsManager();
	virtual bool LoadSettings( TCHAR * FilePath );
	virtual void SaveSettings( TCHAR * FilePath );
	virtual void LoadDefaultSettings();

	virtual bool IsSettingExists( const stringType & GroupName, const stringType & SettingName );
	virtual bool GetBoolSetting( const stringType & GroupName, const stringType & SettingName );
	virtual int GetIntSetting( const stringType & GroupName, const stringType & SettingName );
	virtual float GetFloatSetting( const stringType & GroupName, const stringType & SettingName );
	virtual stringType GetStringSetting( const stringType & GroupName, const stringType & SettingName );

	virtual void SetBoolSetting( const stringType & GroupName, const stringType & SettingName, const bool Value );
	virtual void SetIntSetting( const stringType & GroupName, const stringType & SettingName, const int Value );
	virtual void SetFloatSetting( const stringType & GroupName, const stringType & SettingName, const float Value );
	virtual void SetStringSetting( const stringType & GroupName, const stringType & SettingName, const stringType & Value );

	/*
		Iterating settings can be done as such:

		for ( bool LoopGroups = g_SettingsManager.FirstSettingsGroup(); LoopGroups; LoopGroups = g_SettingsManager.NextSettingsGroup())
		{
			stringType GroupName = g_SettingsManager.GetCurrentSettingsGroupName();
			for ( bool LoopValues = g_SettingsManager.FirstSettingValue(); LoopValues; LoopValues = g_SettingsManager.NextSettingValue())
			{
				stringType ValueName = g_SettingsManager.GetCurrentSettingValueName();
				SettingValue * Value = g_SettingsManager.GetCurrentSettingValue();
			}
		}
	*/
	// Iterator for setting groups - Always call FirstSettingsGroup first before iterating
	virtual bool FirstSettingsGroup();
	virtual bool NextSettingsGroup();
	virtual stringType GetCurrentSettingsGroupName();

	// Iterator for setting values - Always call FirstSettingValue first before iterating
	virtual bool FirstSettingValue();
	virtual bool NextSettingValue();
	virtual stringType GetCurrentSettingValueName();
	virtual const SettingValue * GetCurrentSettingValue();

	// Interaction with settings observers
	virtual void AddSettingsObserver( SettingsObserver * pObserver );
	virtual void RemoveSettingsObserver( SettingsObserver * pObserver );

protected:
	SettingsType Settings;
	stringType VersionString;		// Version number to set in xml file
	stringType RootElementName;		// Name of the root element in xml file

	// Internal iterators for iterating the setting groups and values
	SettingsType::iterator CurrentSettingsGroupIter;
	SettingsGroupType::iterator CurrentSettingValueIter;

	virtual SettingValue * GetSetting( const stringType & GroupName, const stringType & SettingName );
	virtual SettingValue * AddNewSetting( const stringType & GroupName, const stringType & SettingName );

	// Settings observers
	SettingsObserversListType SettingsObserversList;
	virtual void UpdateSettingsObservers( const stringType & GroupName, const stringType & SettingName );
};


class SettingsObserver
{
public:
	SettingsObserver();
	virtual void OnObserverAdded( McOptionsManager * SettingsManager );
	virtual void OnSettingsChanged( const stringType & GroupName, const stringType & SettingName );

protected:
	McOptionsManager * pSettingsManager;
};


#endif