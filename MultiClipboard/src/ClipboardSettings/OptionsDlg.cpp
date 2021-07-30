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
#include "OptionsDlg.h"
#include "McOptionsManager.h"
#include "McOptions.h"
#include "NppDarkMode.h"
#include "NativeLang_def.h"
#include "resource.h"
#include <windowsx.h>
#include <sstream>
#include <vector>
#endif

extern HINSTANCE g_hInstance;
extern McOptionsManager g_SettingsManager;
extern NppData				g_NppData;
#define NM_MOUSE_OVER_CONTROL 1000


// This is the subclassed wnd proc for the children control of the settings dialog.
// It is used to trap the mouse move before the message is being sent to the child controls
LRESULT CALLBACK MCBSettingsChildCtrlDlgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
	case WM_NCHITTEST:
		HWND hDlgParent = ::GetParent( hwnd );
		int OwnID = ::GetDlgCtrlID( hwnd );

		::SendMessage( hDlgParent, WM_COMMAND, MAKEWPARAM( OwnID, NM_MOUSE_OVER_CONTROL ), (LPARAM)hwnd );
		break;
	}
	WNDPROC OwnWndProc = reinterpret_cast<WNDPROC>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
	return ::CallWindowProc( OwnWndProc, hwnd, msg, wParam, lParam );
}


LRESULT CALLBACK StaticTextChildCtrlDlgDlgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
	case WM_NCHITTEST:
		HWND hDlgParent = ::GetParent( hwnd );
		int OwnID = ::GetDlgCtrlID( hwnd );

		::SendMessage( hDlgParent, WM_COMMAND, MAKEWPARAM( OwnID, NM_MOUSE_OVER_CONTROL ), (LPARAM)hwnd );
		// Static text needs to return this or Windows will return HTTRANSPARENT and pass this message away
		return HTCLIENT;
	}
	WNDPROC OwnWndProc = reinterpret_cast<WNDPROC>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
	return ::CallWindowProc( OwnWndProc, hwnd, msg, wParam, lParam );
}


void OptionsDlg::Init( HINSTANCE hInst, HWND hNpp )
{
	Window::init( hInst, hNpp );
	CurrentMouseOverID = 0;
}


void OptionsDlg::ShowDialog( bool Show )
{
	if ( !isCreated() )
	{
		create( IDD_OPTIONS_DLG );
		LoadSettingsControlMap();
		SubclassAllChildControls();
	}
	display( Show );
	if ( Show )
	{
		LoadMultiClipboardSettings();
		goToCenter();
	}

}

INT_PTR CALLBACK OptionsDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
		case WM_INITDIALOG:
		{
			// Change language
			bool enlarge_text = ::SendMessage(g_NppData._nppHandle, NPPM_GETENLARGETEXT, 0, 0);
			if(enlarge_text) {
				//::MessageBox(NULL, TEXT("enlarge_text"), TEXT(""), MB_OK);

				LOGFONT logfont{};
				//ZeroMemory(&logfont, sizeof(LOGFONT));
				logfont.lfCharSet = GB2312_CHARSET;
				//logfont.lfWeight = 550;
				//HFONT hFont = (HFONT)lParam;
				//SendMessage(_hSelf,WM_SETFONT,(WPARAM)hFont,0);
				logfont.lfHeight = -21;
				auto hFont = CreateFontIndirect(&logfont);

				setWindowFont(_hSelf, hFont);
			}

			NLChangeDialog( _hInst, g_NppData._nppHandle, _hSelf, TEXT("Options") );

			return TRUE;
		}

		case WM_COMMAND:
		{
			if ( ( HIWORD(wParam) == NM_MOUSE_OVER_CONTROL ) && ( LOWORD(wParam) != 0 ) )
			{
				if ( CurrentMouseOverID != LOWORD(wParam) )
				{
					CurrentMouseOverID = LOWORD(wParam);
					DisplayMouseOverIDHelp( CurrentMouseOverID );
				}
				break;
			}

			switch ( wParam )
			{
			case IDOK:
				SaveMultiClipboardSettings();
				// fall through
			case IDCANCEL:
				display(FALSE);
				return TRUE;

			default :
				break;
			}
		}


		case WM_MOUSEMOVE:
			if ( CurrentMouseOverID != 0 )
			{
				CurrentMouseOverID = 0;
				DisplayMouseOverIDHelp( CurrentMouseOverID );
			}
			break;

		case WM_CTLCOLOREDIT:
		{
			if (NppDarkMode::isEnabled())
			{
				return NppDarkMode::onCtlColorSofter(reinterpret_cast<HDC>(wParam));
			}
			break;
		}

		case WM_CTLCOLORLISTBOX:
		{
			if (NppDarkMode::isEnabled())
			{
				return NppDarkMode::onCtlColor(reinterpret_cast<HDC>(wParam));
			}
			break;
		}

		case WM_CTLCOLORDLG:
		case WM_CTLCOLORSTATIC:
		{
			if (NppDarkMode::isEnabled())
			{
				return NppDarkMode::onCtlColorDarker(reinterpret_cast<HDC>(wParam));
			}
			break;
		}

		case WM_PRINTCLIENT:
		{
			if (NppDarkMode::isEnabled())
			{
				return TRUE;
			}
			break;
		}

		case NPPN_DARKCONF_CHANGED:
		{
			NppDarkMode::autoSubclassAndThemeChildControls(_hSelf);
			NppDarkMode::setDarkScrollBar(_hSelf);
			return TRUE;
		}

		case WM_ERASEBKGND:
		{
			if (!NppDarkMode::isEnabled())
			{
				break;
			}

			RECT rc = { 0 };
			getClientRect(rc);
			FillRect((HDC)wParam, &rc, NppDarkMode::getBackgroundBrush());
			return 1;
		}
	}
	return FALSE;
}


void OptionsDlg::SetIntValueToDialog( const std::wstring & GroupName, const std::wstring & SettingName, const int DlgItemID )
{
	int intValue = g_SettingsManager.GetIntSetting( GroupName, SettingName );
	::SetDlgItemInt( _hSelf, DlgItemID, intValue, FALSE );
}


void OptionsDlg::SetBoolValueToDialog( const std::wstring & GroupName, const std::wstring & SettingName, const int DlgItemID )
{
	bool boolValue = g_SettingsManager.GetBoolSetting( GroupName, SettingName );
	::CheckDlgButton( _hSelf, DlgItemID, boolValue ? BST_CHECKED : BST_UNCHECKED );
}


void OptionsDlg::GetIntValueFromDialog( const std::wstring & GroupName, const std::wstring & SettingName, const int DlgItemID )
{
	int intValue = ::GetDlgItemInt( _hSelf, DlgItemID, NULL, FALSE );
	g_SettingsManager.SetIntSetting( GroupName, SettingName, intValue );
}


void OptionsDlg::GetBoolValueFromDialog( const std::wstring & GroupName, const std::wstring & SettingName, const int DlgItemID )
{
	bool boolValue = BST_CHECKED == ::IsDlgButtonChecked( _hSelf, DlgItemID );
	g_SettingsManager.SetBoolSetting( GroupName, SettingName, boolValue );
}


void OptionsDlg::LoadMultiClipboardSettings()
{
	for ( unsigned int i = 0; i < SettingsControlMap.size(); ++i )
	{
		switch ( SettingsControlMap[i].SettingType )
		{
		case SCTE_BOOL:
			SetBoolValueToDialog( SettingsControlMap[i].GroupName, SettingsControlMap[i].SettingName, SettingsControlMap[i].ControlID );
			break;

		case SCTE_INT:
			SetIntValueToDialog( SettingsControlMap[i].GroupName, SettingsControlMap[i].SettingName, SettingsControlMap[i].ControlID );
			break;

		default:
			break;;
		}
	}
}


void OptionsDlg::SaveMultiClipboardSettings()
{
	for ( unsigned int i = 0; i < SettingsControlMap.size(); ++i )
	{
		switch ( SettingsControlMap[i].SettingType )
		{
		case SCTE_BOOL:
			GetBoolValueFromDialog( SettingsControlMap[i].GroupName, SettingsControlMap[i].SettingName, SettingsControlMap[i].ControlID );
			break;

		case SCTE_INT:
			GetIntValueFromDialog( SettingsControlMap[i].GroupName, SettingsControlMap[i].SettingName, SettingsControlMap[i].ControlID );
			break;

		default:
			break;;
		}
	}
}

int MupleCBlastHelp=0;

void OptionsDlg::DisplayMouseOverIDHelp( int ControlID )
{
	if ( ControlID == 0 || MupleCBlastHelp==ControlID)
	{
		//::SetDlgItemText( _hSelf, IDC_OPTION_EXPLANATION, TEXT("") );
		return;
	}
	MupleCBlastHelp = ControlID;
	std::wostringstream HelpNativeLangIndex;
	HelpNativeLangIndex << ControlID << TEXT("_HELP");
	std::vector< TCHAR > HelpText(512);
	int len = NLGetText( g_hInstance, g_NppData._nppHandle, HelpNativeLangIndex.str().c_str(), &HelpText[0], HelpText.capacity() );
	if ( len == 0 )
	{
		::SetWindowText( ::GetDlgItem( _hSelf, IDC_OPTION_EXPLANATION ), GetControlHelpText( ControlID ) );
	}
	else
	{
		::SetWindowText( ::GetDlgItem( _hSelf, IDC_OPTION_EXPLANATION ), &HelpText[0] );
	}
}


void OptionsDlg::SubclassChildControl( const int ControlID )
{
	HWND hChild = GetDlgItem( _hSelf, ControlID );
	WNDPROC ChildWndProc = (WNDPROC) SetWindowLongPtr( hChild, GWLP_WNDPROC, (LONG_PTR) MCBSettingsChildCtrlDlgProc );
	
	
	::SetWindowLongPtr( hChild, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ChildWndProc) );

}


void OptionsDlg::SubclassStaticTextChildControl( const int ControlID )
{
	HWND hChild = GetDlgItem( _hSelf, ControlID );
	WNDPROC ChildWndProc = (WNDPROC) SetWindowLongPtr( hChild, GWLP_WNDPROC, (LONG_PTR) StaticTextChildCtrlDlgDlgProc );
	::SetWindowLongPtr( hChild, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ChildWndProc) );
}


void OptionsDlg::SubclassAllChildControls()
{
	for ( unsigned int i = 0; i < SettingsControlMap.size(); ++i )
	{
		SubclassChildControl( SettingsControlMap[i].ControlID );
		if ( SettingsControlMap[i].ControlStaticTextID > 0 )
		{
			SubclassStaticTextChildControl( SettingsControlMap[i].ControlStaticTextID );
		}
	}
}


void OptionsDlg::GetSettingsGroupAndName( const int Control, std::wstring & GroupName, std::wstring & SettingName )
{
	for ( unsigned int i = 0; i < SettingsControlMap.size(); ++i )
	{
		if ( Control == SettingsControlMap[i].ControlID ||
			 Control == SettingsControlMap[i].ControlStaticTextID )
		{
			GroupName = SettingsControlMap[i].GroupName;
			SettingName = SettingsControlMap[i].SettingName;
		}
	}
}


LPCTSTR OptionsDlg::GetControlHelpText( int ControlID )
{
	for ( unsigned int i = 0; i < SettingsControlMap.size(); ++i )
	{
		if ( ControlID == SettingsControlMap[i].ControlID ||
			 ControlID == SettingsControlMap[i].ControlStaticTextID )
		{
			return SettingsControlMap[i].SettingHelp.c_str();
		}
	}
	return TEXT("");
}


// All settings to be defined here, and the rest of the functions will take care of the rest
void OptionsDlg::LoadSettingsControlMap()
{
	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_EDIT_MAX_CLIPLIST_SIZE, IDC_TEXT_MAX_CLIPLIST_SIZE, SCTE_INT,
		SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_MAX_CLIPBOARD_ITEMS,
		TEXT("剪贴板“存储”的最大条目数量") ) );
	
	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_EDIT_MAX_CLIPLIST_SIZE_1, IDC_TEXT_MAX_CLIPLIST_SIZE_1, SCTE_INT,
		SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_MAX_DISPLAY_ITEMS,
		TEXT("上下文菜单“显示”的最大条目数量") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_COPY_FROM_OTHER_PROGRAMS, SCTE_BOOL,
		SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_COPY_FROM_OTHER_PROGRAMS,
		TEXT("获得来自其他应用程序的剪贴数据") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_ONLY_WHEN_PASTE_IN_NPP, SCTE_BOOL,
		SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_ONLY_WHEN_PASTED_IN_NPP,
		TEXT("仅在粘贴到图创文本时，才触发“获得来自其他应用程序的剪贴数据”这一操作 ") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_USE_PASTE_MENU, SCTE_BOOL,
		SETTINGS_GROUP_PASTE_MENU, SETTINGS_USE_PASTE_MENU,
		TEXT("用Ctrl-Shift-V粘贴时，显示上下文菜单，而不是直接粘贴然后轮番切换。") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_NUMBERED_PASTE_MENU, SCTE_BOOL,
		SETTINGS_GROUP_PASTE_MENU, SETTINGS_SHOW_NUMBERED_PASTE_MENU,
		TEXT("上下文菜单显示数字而用非首字母作为加速键") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_EDIT_PASTE_MENU_WIDTH, IDC_TEXT_PASTE_MENU_WIDTH, SCTE_INT,
		SETTINGS_GROUP_PASTE_MENU, SETTINGS_PASTE_MENU_WIDTH,
		TEXT("显示宽度") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_MIDDLE_CLICK_PASTE, SCTE_BOOL,
		SETTINGS_GROUP_PASTE_MENU, SETTINGS_MIDDLE_CLICK_PASTE,
		TEXT("点击鼠标中键进行粘贴. 按住Shift显示上下文菜单") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_AUTO_COPY_SELECTION, SCTE_BOOL,
		SETTINGS_GROUP_AUTO_COPY, SETTINGS_AUTO_COPY_TEXT_SELECTION,
		TEXT("自动复制选中的文本") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_NO_COPY_LARGE_TEXT, SCTE_BOOL,
		SETTINGS_GROUP_OSCLIPBOARD, SETTINGS_IGNORE_LARGE_TEXT,
		TEXT("略过非常大的文本，较少内存占用。Improves performance when copy and pasting very large text") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_NO_LARGE_TEXT_EDIT, SCTE_BOOL,
		SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_NO_EDIT_LARGE_TEXT,
		TEXT("禁止编辑大文本。 Improves performance when using the MultiClip Viewer") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_PASTE_ALL_REVERSE, SCTE_BOOL,
		SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_PASTE_ALL_REVERSE,
		TEXT("粘贴全部：使用逆序") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_PASTE_ALL_NEWLINE_BETWEEN, SCTE_BOOL,
		SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_PASTE_ALL_NEWLINE_BETWEEN,
		TEXT("粘贴全部：每行隔开") ) );

	SettingsControlMap.push_back( SettingsControlMapStruct(
		IDC_CHECK_PERSIST_CLIPBOARD_LIST, SCTE_BOOL,
		SETTINGS_GROUP_CLIPBOARDLIST, SETTINGS_SAVE_CLIPBOARD_SESSION,
		TEXT("存储到会话。（WIP）Persist the clipboard list across Editor sessions") ) );
}