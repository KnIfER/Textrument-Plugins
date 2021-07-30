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

#ifndef MULTI_CLIPBOARD_SETTINGS_DIALOG
#define MULTI_CLIPBOARD_SETTINGS_DIALOG

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "StaticDialog.h"
#include <string>
#include <vector>
#endif


class OptionsDlg : public StaticDialog
{
public:
	void Init( HINSTANCE hInst, HWND hNpp );
	void ShowDialog( bool Show = TRUE );
	virtual void create(int dialogID, bool isRTL = false, bool msgDestParent = true) {
        StaticDialog::create(dialogID, isRTL, msgDestParent);
		run_dlgProc( NPPN_DARKCONF_CHANGED, 0, 0);
    };

	void destroy() {
		// A Ajouter les fils...
	};
protected:
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void LoadMultiClipboardSettings();
	void SaveMultiClipboardSettings();

	void SetIntValueToDialog( const std::wstring & GroupName, const std::wstring & SettingName, const int DlgItemID );
	void SetBoolValueToDialog( const std::wstring & GroupName, const std::wstring & SettingName, const int DlgItemID );
	void GetIntValueFromDialog( const std::wstring & GroupName, const std::wstring & SettingName, const int DlgItemID );
	void GetBoolValueFromDialog( const std::wstring & GroupName, const std::wstring & SettingName, const int DlgItemID );

	void SubclassChildControl( const int ControlID );
	void SubclassStaticTextChildControl( const int ControlID );
	void SubclassAllChildControls();
	void GetSettingsGroupAndName( const int Control, std::wstring & GroupName, std::wstring & SettingName );

	// ID of child control the mouse cursor is current over, for displaying context help
	int CurrentMouseOverID;
	void DisplayMouseOverIDHelp( const int ControlID );
	LPCTSTR GetControlHelpText( int ControlID );

	// Handles mapping from settings to dialog item control ID and vice versa
	// All mappings shall be defined in this function
	void LoadSettingsControlMap();
	enum SettingControlTypeEnum
	{
		SCTE_BOOL,
		SCTE_INT
	};
	struct SettingsControlMapStruct
	{
		int ControlID;
		int ControlStaticTextID;
		SettingControlTypeEnum SettingType;
		std::wstring GroupName;
		std::wstring SettingName;
		std::wstring SettingHelp;

		SettingsControlMapStruct(
			int controlID, int controlStaticTextID, SettingControlTypeEnum settingType,
			std::wstring groupName, std::wstring settingName, std::wstring settingHelp )
			: ControlID( controlID ), ControlStaticTextID( controlStaticTextID ), SettingType( settingType )
			, GroupName( groupName ), SettingName( settingName ), SettingHelp( settingHelp ) {}

		SettingsControlMapStruct(
			int controlID, SettingControlTypeEnum settingType,
			std::wstring groupName, std::wstring settingName, std::wstring settingHelp )
			: ControlID( controlID ), ControlStaticTextID( -1 ), SettingType( settingType )
			, GroupName( groupName ), SettingName( settingName ), SettingHelp( settingHelp ) {}
	};
	typedef std::vector< SettingsControlMapStruct > SettingsControlMapType;
	typedef SettingsControlMapType::iterator SettingsControlMapIter;
	SettingsControlMapType SettingsControlMap;
};


#endif