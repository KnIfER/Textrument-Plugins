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

// include files
#ifndef UNITY_BUILD_SINGLE_INCLUDE

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRA_LEAN
#include "PluginDefinition.h"

#include "SciSubClassWrp.h"
#include "MCSubClassWndProc.h"
#include "MultiClipboardProxy.h"
#include "McOptionsManager.h"
#include "OptionsDlg.h"
#include "AboutDialog.h"
#include "NativeLang_def.h"

#include "ArraysOfClips.h"
#include "OSClipboardController.h"
#include "MultiClipDlg.h"
#include "MultiClipPasteMenu.h"
#include "MultiClipCyclicPaste.h"
#include "SelectedTextAutoCopier.h"

#include "wutils.h"
#endif


HWND mainAppWnd;

// information for notepad++
CONST TCHAR	PLUGIN_NAME[] = TEXT("&MultiClipboard");

// global values
MultiClipboardProxy	g_ClipboardProxy;
McOptionsManager g_SettingsManager( TEXT("McOptions") );
toolbarIcons		g_TBWndMgr{0,0,0x666,0,IDR_MULTICLIPBOARD_ICO,0,0,IDB_EX_MULTICLIPBOARD};
SciSubClassWrp		g_ScintillaMain, g_ScintillaSecond;

// dialog classes
AboutDialog			_aboutDlg;
OptionsDlg _optDlg;

// settings

// ini file name
CONST TCHAR configFileName[] = TEXT( "\\MultiClipboard.xml" );
TCHAR configPath[MAX_PATH];
TCHAR SettingsFilePath[MAX_PATH];

// MVC components for plugin
ArraysOfClips clipboardList;
OSClipboardController OSClipboard;
MultiClipViewerDialog _mcViewer;
MultiClipPasteMenu _pasteMenu;
MultiClipCyclicPaste _cyclicPaste;
SelectedTextAutoCopier autoCopier;

// Function prototypes for this plugin
void ShutDownPlugin();

// load and save properties from/into settings file
void LoadSettings();
void SaveSettings();

// menu functions
void ToggleView();
void ShowClipPasteMenu();
void ShowAboutDlg();
void ShowOptionsDlg();

// main function of dll
BOOL APIENTRY DllMain( HINSTANCE hModule,
                       DWORD  reasonForCall,
                       LPVOID lpReserved )
{
	g_hInstance = hModule;

	switch (reasonForCall)
	{
		case DLL_PROCESS_ATTACH:
		{
			_mcViewer.init((HINSTANCE)hModule, NULL);
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			delete funcItem[0]._pShKey;
			delete funcItem[1]._pShKey;

			// save settings
			SaveSettings();

			ShutDownPlugin();
			break;
		}
		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
	}

	return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	// For OLE drag and drop
	OleInitialize( NULL );

	// stores notepad data
	g_NppData = notpadPlusData;

	g_ClipboardProxy.Init();

	// Subclass the Notepad++ windows procedure
	g_NppWndProc = (WNDPROC) SetWindowLongPtr( g_NppData._nppHandle, GWLP_WNDPROC, (LONG_PTR) MCSubClassNppWndProc );
	// As well as the 2 scintilla windows'
	g_ScintillaMain.Init( g_NppData._scintillaMainHandle, MCSubClassSciWndProc );
	g_ScintillaSecond.Init( g_NppData._scintillaSecondHandle, MCSubClassSciWndProc );

	// load data of plugin
	LoadSettings();

	// initial dialogs
	_aboutDlg.Init( g_hInstance, g_NppData );
	_optDlg.Init( g_hInstance, mainAppWnd=g_NppData._nppHandle );

	// Initialisation of MVC components
	g_SettingsManager.AddSettingsObserver( &clipboardList );
	clipboardList.LoadClipboardSession();
	OSClipboard.Init( &clipboardList, &g_ClipboardProxy, &g_SettingsManager );

	_mcViewer.Init( &clipboardList, &g_ClipboardProxy, &g_SettingsManager );

	_pasteMenu.Init( &clipboardList, &g_ClipboardProxy, &g_SettingsManager );
	_cyclicPaste.Init( &clipboardList, &g_ClipboardProxy, &g_SettingsManager );
	autoCopier.Init( &clipboardList, &g_ClipboardProxy, &g_SettingsManager );


	// Set function pointers
	funcItem[0]._pFunc = ToggleView;
	funcItem[1]._pFunc = ShowClipPasteMenu;
	funcItem[2]._pFunc = ShowOptionsDlg;
	funcItem[3]._pFunc = ShowAboutDlg;

	// Fill menu names
	lstrcpy( funcItem[0]._itemName, TEXT("&MultiClip Viewer...") );
	lstrcpy( funcItem[1]._itemName, TEXT("MultiClipboard &Paste") );
	lstrcpy( funcItem[2]._itemName, TEXT("&Options...") );
	lstrcpy( funcItem[3]._itemName, TEXT("&About...") );

	// Set shortcuts
	funcItem[0]._pShKey = new ShortcutKey;
	funcItem[0]._pShKey->_isAlt		= true;
	funcItem[0]._pShKey->_isCtrl	= true;
	funcItem[0]._pShKey->_isShift	= false;
	funcItem[0]._pShKey->_key		= 'V';
	funcItem[1]._pShKey = new ShortcutKey;
	funcItem[1]._pShKey->_isAlt		= false;
	funcItem[1]._pShKey->_isCtrl	= true;
	funcItem[1]._pShKey->_isShift	= true;
	funcItem[1]._pShKey->_key		= 'V';
	funcItem[2]._pShKey = NULL;
	funcItem[3]._pShKey = NULL;


}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(INT *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

/***
 *	beNotification()
 *
 *	This function is called, if a notification in Scantilla/Notepad++ occurs
 */
bool deferredUpdateRequested = false;
bool NPPRunning = false;
int EditorBG;
void refreshDarkModeIfEditorBgChanged()
{
	int editorBg = SendMessage(nppData._nppHandle, NPPM_GETEDITORDEFAULTBACKGROUNDCOLOR, 0, 0);
	if (EditorBG!=editorBg)
	{
		_mcViewer.refreshDarkMode();
		EditorBG = editorBg;
	}
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{

	int code = notifyCode->nmhdr.code;
	//int NeedUpdate=0;
	switch (code) 
	{
		case NPPN_TBMODIFICATION:
		if (notifyCode->nmhdr.hwndFrom == g_NppData._nppHandle)
		{
			auto HRO = (HINSTANCE)g_hInstance;

			long version = ::SendMessage(g_NppData._nppHandle, NPPM_GETNOTMADVERSION, 0, 0);

			legacy = version<0x666;

			g_TBWndMgr.HRO = HRO;
			if(legacy) {
				g_TBWndMgr.hToolbarBmp = (HBITMAP)::LoadImage(HRO, MAKEINTRESOURCE(IDB_EX_MULTICLIPBOARD), IMAGE_BITMAP, 0, 0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));
				g_TBWndMgr.hToolbarIcon = (HICON)::LoadIcon(HRO, MAKEINTRESOURCE(IDR_MULTICLIPBOARD_ICO));
			}
			::SendMessage(g_NppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[MULTICLIPBOARD_DOCKABLE_WINDOW_INDEX]._cmdID, (LPARAM)&g_TBWndMgr);

			fetchFontStack(g_NppData._nppHandle);

			// Change menu language
			NLChangeNppMenu( (HINSTANCE)g_hInstance, g_NppData._nppHandle, (LPTSTR)PLUGIN_NAME, funcItem, nbFunc );
		}  break;

		case SCN_UPDATEUI:
		{
			//static int paint_cc = 0; LogIs("SCN_UPDATEUI %d %d ", paint_cc++, notifyCode->updated);
			if (legacy && 9==notifyCode->updated) // SC_UPDATE_CONTENT
			{
				refreshDarkModeIfEditorBgChanged();
			}
		}
		break;

		case NPPN_READY:
		{
			NPPRunning=true;
		}
		break;

		case NPPN_SHUTDOWN:
		{
			NPPRunning=0;
		}
		break;

		// 切换黑暗模式
		case NPPN_DARKCONF_CHANGED:
		{
			//LogIs(2, "NPPN_DARKCONF_CHANGED");
			if(NPPRunning)
			{
				_mcViewer.refreshDarkMode();
			}
		}
		break;
	}
}

/***
 *	messageProc()
 *
 *	This function is called, if a notification from Notepad occurs
 */
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}


#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
	return TRUE;
}
#endif //UNICODE


/***
 *	LoadSettings()
 *
 *	Load the parameters of plugin
 */
void LoadSettings(void)
{
	// initialize the config directory
	::SendMessage( g_NppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)configPath );

	// Test if config path exist
	if ( ::PathFileExists( configPath ) == FALSE )
	{
		::CreateDirectory( configPath, NULL );
	}

	lstrcpy( SettingsFilePath, configPath );
	lstrcat( SettingsFilePath, configFileName );
	if ( ::PathFileExists(SettingsFilePath) == FALSE )
	{
		::CloseHandle( ::CreateFile( SettingsFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) );
	}

	g_SettingsManager.LoadSettings( SettingsFilePath );
}

/***
 *	SaveSettings()
 *
 *	Saves the parameters of plugin
 */
void SaveSettings(void)
{
	g_SettingsManager.SaveSettings( SettingsFilePath );

	clipboardList.SaveClipboardSession();
}

void ShutDownPlugin()
{
	_mcViewer.Shutdown();
	g_ClipboardProxy.Destroy();
	// Shutdown COM for OLE drag drop
	OleUninitialize();
}

/**************************************************************************
 *	Interface functions
 */
void ToggleView(void)
{
	//LogIs(2, "ToggleView");
	// get menu and test if dockable dialog is open
	bool nxtShow = 1;

	nxtShow = !_mcViewer.isVisible();

	HMENU hMenu = ::GetMenu(g_NppData._nppHandle);

	::CheckMenuItem(hMenu, funcItem[MULTICLIPBOARD_DOCKABLE_WINDOW_INDEX]._cmdID, MF_BYCOMMAND | (nxtShow?MF_CHECKED:MF_UNCHECKED));

	_mcViewer.ShowDialog(nxtShow);
}


void ShowAboutDlg(void)
{
	_aboutDlg.doDialog();
}


void ShowOptionsDlg()
{
	_optDlg.ShowDialog();
}


void ShowClipPasteMenu()
{
	if ( _pasteMenu.isEnabled() )
	{
		_pasteMenu.show();
	}
	else
	{
		_cyclicPaste.DoCyclicPaste();
	}
}