//this file is part of notepad++
//Copyright (C) 2011 AustinYoung<pattazl@gmail.com>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"

//
// put the headers you need here
//
#include <stdlib.h>
#include <time.h>
#include <shlwapi.h>
#include "LNhistoryDlg.h"


LocationNavigateDlg _LNhistory;

extern int BufferIdBeforeClick;

extern bool WindowOpaqueMsg;

deque<LocationInfo>LocationList;
deque<LocationInfo>LocationSave;
long LocationPos=0;
bool PositionSetting = false;
int MaxOffset=100;
int MaxList = 50;
bool AutoClean = false;
HWND curScintilla=0;
bool AlwaysRecord=false;
bool SaveRecord = false;
bool InCurr = false;
bool AlwaysRefreshBtns = false;
bool ShowFNOnly = true;
bool skipClosed = false;
bool pinMenu = false;
bool bIsPaused = false;
bool bAutoRecord = true;
bool NeedMark = false;
MarkType ByBookMark = MarkHightLight;
long MarkColor = DefaultColor;
long SaveColor = DefaultSaveColor;

#ifdef UNICODE 
	#define generic_itoa _itow
#else
	#define generic_itoa itoa
#endif

FuncItem funcItem[nbFunc];
bool menuState[nbFunc];
HWND hToolbar;
int IconID[nbFunc];
// The data of Notepad++ that you can use in your plugin commands

toolbarIcons		g_TBPrevious{0,0,0x666,0,IDI_ICON_PREV,IDI_ICON_PREV_ACT,IDI_ICON_PREV_OFF,IDB_BITMAP1};
toolbarIcons		g_TBNext{0,0,0x666,0,IDI_ICON_NEXT,IDI_ICON_NEXT_ACT,IDI_ICON_NEXT_OFF,IDB_BITMAP2};
toolbarIcons		g_TBPreviousChg{0,0,0x666,0,IDI_ICON_PREV1,IDI_ICON_PREV1_ACT,IDI_ICON_PREV1_OFF,IDB_BITMAP3};
toolbarIcons		g_TBNextChg{0,0,0x666,0,IDI_ICON_NEXT1,IDI_ICON_NEXT1_ACT,IDI_ICON_NEXT1_OFF,IDB_BITMAP4};

TCHAR iniFilePath[MAX_PATH];
//bool SaveRecording = false;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
	// Initialize dialog
	_LNhistory.init((HINSTANCE)hModule, NULL);
	g_hModule = hModule;

	//InitializeCriticalSection(&criCounter); 
}

// Here do the clean up, save the parameters if any
void pluginCleanUp()
{
	//DeleteCriticalSection(&criCounter);
	//if(1) return;
	TCHAR str[500]={0};	
	wsprintf(str,TEXT("%d"),MaxOffset);
	::WritePrivateProfileString(sectionName, strMaxOffset,str , iniFilePath);
	wsprintf(str,TEXT("%d"),MaxList);
	::WritePrivateProfileString(sectionName, strMaxList, str, iniFilePath);
	wsprintf(str,TEXT("%d"),AutoClean?1:0);
	::WritePrivateProfileString(sectionName, strAutoClean, str, iniFilePath);
	wsprintf(str,TEXT("%d"),bIsPaused?1:0);
	::WritePrivateProfileString(sectionName, srtPaused, str, iniFilePath);
	wsprintf(str,TEXT("%d"),AlwaysRecord?1:0);
	::WritePrivateProfileString(sectionName, strAlwaysRecord, str, iniFilePath);
	wsprintf(str,TEXT("%d"),SaveRecord?1:0);
	::WritePrivateProfileString(sectionName, strSaveRecord, str, iniFilePath);
	wsprintf(str,TEXT("%d"),InCurr?1:0);
	::WritePrivateProfileString(sectionName, strInCurr, str, iniFilePath);
	wsprintf(str,TEXT("%d"),skipClosed?1:0);
	::WritePrivateProfileString(sectionName, strSkipClosed, str, iniFilePath);
	wsprintf(str,TEXT("%d"),pinMenu?1:0);
	::WritePrivateProfileString(sectionName, strPinMenu, str, iniFilePath);
	wsprintf(str,TEXT("%d"),bAutoRecord?1:0);
	::WritePrivateProfileString(sectionName, strAutoRecord, str, iniFilePath);
	wsprintf(str,TEXT("%d"),AlwaysRefreshBtns?1:0);
	::WritePrivateProfileString(sectionName, strAlwaysRefreshBtn, str, iniFilePath);
	wsprintf(str,TEXT("%d"),ShowFNOnly?1:0);
	::WritePrivateProfileString(sectionName, strShowFNOnly, str, iniFilePath);
	
	wsprintf(str,TEXT("%d"),NeedMark?1:0);
	::WritePrivateProfileString(sectionName, strNeedMark, str, iniFilePath);
	wsprintf(str,TEXT("%d"),ByBookMark);
	::WritePrivateProfileString(sectionName, strByBookMark, str, iniFilePath);
	wsprintf(str,TEXT("%d"),MarkColor);
	::WritePrivateProfileString(sectionName, strMarkColor, str, iniFilePath);
	wsprintf(str,TEXT("%d"),SaveColor);
	::WritePrivateProfileString(sectionName, strSaveColor, str, iniFilePath);

}

// Here do the clean up (especially for the shortcut)
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
	delete funcItem[menuPrevious]._pShKey;
	delete funcItem[menuNext]._pShKey;
	delete funcItem[menuChgPrevious]._pShKey;
	delete funcItem[menuChgNext]._pShKey;
	delete funcItem[menuOption]._pShKey;
	delete funcItem[menuInCurr]._pShKey;
	delete funcItem[menuNeedMark]._pShKey;

	if (SaveRecord)
	{
		//SaveRecording = true;
		TCHAR iniContent[RecordConentMax]={0};
		// 如果要保存记录，那么将 locationlist 保存
		for (int i=0;i<LocationSave.size();i++)
		{
			TCHAR tmp[300]={0};
			wsprintf(tmp,TEXT("%d<%d>%s|"),LocationSave[i].changed?1:0,LocationSave[i].position,LocationSave[i].FilePath);
			lstrcat(iniContent,tmp);
		}
		::WritePrivateProfileString(sectionName, strRecordContent, iniContent, iniFilePath);
	} else {
		::WritePrivateProfileString(sectionName, strRecordContent, NULL, iniFilePath);
	}
}

//----------------------------------------------//
//-- ASSOCIATED FUNCTIONS START------------------//
//----------------------------------------------//

void PreviousLocation()
{
	BufferIdBeforeClick=1;
	if (LocationPos > 0)
	{
		if ( InCurr )
		{
			int pos = LocationPos;
			while ( --pos >= 0 )
			{
				if ( lstrcmp(LocationList[pos].FilePath,currFile) == 0 )
				{
					// 当前位置的文件名和当前文件一样,找位置不一样的
					if( LocationList[pos].position != LocationList[LocationPos].position)						{
						LocationPos = pos;
						break;
					}
				}
			}
			SetPosByIndex(0);
		} else {
			SetPosByIndex(-1);
		}
		_LNhistory.refreshDlg(0);
	}
	BufferIdBeforeClick=0;
}
void NextLocation()
{
	BufferIdBeforeClick=1;
	if (LocationPos < LocationList.size()-1)
	{
		// 需要查找当前文件的下一个
		if ( InCurr )
		{
			int pos = LocationPos;
			while ( ++pos <= LocationList.size()-1)
			{
				if ( lstrcmp(LocationList[pos].FilePath,currFile) == 0 )
				{
					// 当前位置的文件名和当前文件一样,找位置不一样的
					if( LocationList[pos].position != LocationList[LocationPos].position)						{
						LocationPos = pos;
						break;
					};
				}
			}
			SetPosByIndex(0);
		} else {
			SetPosByIndex(1);
		}
		_LNhistory.refreshDlg(0);
	}
	BufferIdBeforeClick=0;
}
void PreviousChangedLocation()
{
	BufferIdBeforeClick=1;
	int tmpPos = LocationPos;
	int len =  LocationList.size()-1;
	if (tmpPos > len )
	{
		tmpPos = len;
	}
	bool PositionSet=0;
	while ( tmpPos >0)
	{
		// 需要往前走一个
		int pos = --tmpPos;
		if ( LocationList[pos].changed )
		{
			// 需要判断文件是否一致
			bool samefile = true;
			if ( InCurr )
			{
				samefile = (lstrcmp(LocationList[pos].FilePath,currFile) == 0);
				if( samefile )
				{
					//if ( lstrcmp(LocationList[LocationPos].FilePath,currFile)==0 )
					// 当前位置的文件名和当前文件一样,找位置不一样的
					samefile = ( LocationList[pos].position != LocationList[LocationPos].position);
				}
			}
			if ( samefile )
			{
				//LocationPos = pos;
				if(!SetPosByIndex(pos-LocationPos, false)) {
					continue;
				}
				PositionSet=1;
				_LNhistory.refreshDlg(0);
				break;
			}
		}
	}
	if(!PositionSet) {
		EnableTBButton(menuChgPrevious, false);
	}
	BufferIdBeforeClick=0;
}
void NextChangedLocation()
{
	BufferIdBeforeClick=1;
	int tmpPos = LocationPos;
	int len =  LocationList.size()-1;
	if (tmpPos > len )
	{
		tmpPos = len;
	}
	bool PositionSet=0;
	while ( tmpPos < len)
	{
		// 需要往后走一个
		int pos = ++tmpPos;
		if ( LocationList[pos].changed )
		{
			bool samefile = true;
			if ( InCurr )
			{
				samefile = (lstrcmp(LocationList[pos].FilePath,currFile) == 0);
				if( samefile )
				{
					// 当前位置的文件名和当前文件一样,找位置不一样的
					samefile = ( LocationList[pos].position != LocationList[LocationPos].position);
				}
			}
			if ( samefile )
			{
				//LocationPos = pos;
				if(!SetPosByIndex(pos-LocationPos, false)) {
					continue;
				}
				PositionSet=1;
				_LNhistory.refreshDlg(0);
				break;
			}
		}
	}
	if(!PositionSet) {
		EnableTBButton(menuChgNext, false);
	}
	BufferIdBeforeClick=0;
}

void FlipCheckMenu(bool *val, int mid) {
	*val = !*val;
	::CheckMenuItem(::GetMenu(nppData._nppHandle), 
		funcItem[mid]._cmdID, MF_BYCOMMAND | (*val?MF_CHECKED:MF_UNCHECKED));
}

void AutoRecord()
{
	// 刷新菜单
	FlipCheckMenu(&bAutoRecord, menuAutoRecord);
	::EnableMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuManualRecord]._cmdID,MF_BYCOMMAND|(bAutoRecord?MF_GRAYED:MF_ENABLED ));
}
void ManualRecord()
{
	//
	AddList(true);
}
void ClearAllRecords()
{
	if(LocationList.size() && IDOK==::MessageBox(_LNhistory.getHSelf(), TEXT("Clear All Navigation History?"), TEXT(""), MB_OKCANCEL | MB_TASKMODAL)) {
		ClearLocationList();
		_LNhistory.refreshDlg(1);
	}
}
void SkipClosed()
{
	FlipCheckMenu(&skipClosed, menuSkipClosed);
}
void FlipRefreshBtns()
{
	FlipCheckMenu(&AlwaysRefreshBtns, menuAlwaysRefresh);
}
void FlipAutoClean()
{
	FlipCheckMenu(&AutoClean, menuClearOnClose);
	// 刷新配置界面
	if ( _LNhistory.isCreated() )
	{
		::SendMessage( _LNhistory._hAuto, BM_SETCHECK ,(LPARAM)(AutoClean?1:0),0);
	}
}
void PauseRecording()
{
	FlipCheckMenu(&bIsPaused, menuPause);
}
void PinMenu()
{
	FlipCheckMenu(&pinMenu, menuPinMenu);
}
void NavigateInCurr()
{
	FlipCheckMenu(&InCurr, menuInCurr);
	// 刷新配置界面
	_LNhistory.refreshDlg(0);
	if ( _LNhistory.isCreated() )
	{
		::SendMessage( _LNhistory._hInCurr, BM_SETCHECK ,(LPARAM)(InCurr?1:0),0);
	}
}
void MarkChange()
{
	FlipCheckMenu(&NeedMark, menuNeedMark);
	// 刷新配置界面
	if ( _LNhistory.isCreated() )
	{
		::SendMessage( _LNhistory._hMark, BM_SETCHECK ,(LPARAM)(NeedMark?1:0),0);
	}
}

void ToggleHistoryPanel()
{
	_LNhistory.setParent( nppData._nppHandle );
	tTbData data = {0};

	if ( !_LNhistory.isCreated() )
	{
		WindowOpaqueMsg = 0;
		_LNhistory.create( &data );
		WindowOpaqueMsg = 1;
		// define the default docking behaviour
		data.uMask          = DWS_DF_CONT_RIGHT | DWS_ICONTAB;
		data.pszModuleName = _LNhistory.getPluginFileName();
		// the dlgDlg should be the index of funcItem where the current function pointer is
		data.dlgID = menuOption;
		data.hIconTab       = ( HICON )::LoadImage( _LNhistory.getHinst(),
			MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 14, 14,
			LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT );
		::SendMessage( nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, ( LPARAM )&data );

		_LNhistory.display();
		::SendMessage( nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[menuOption]._cmdID, true );
	} else {
		bool NeedShowDlg = !_LNhistory.isVisible();

		_LNhistory.display(NeedShowDlg);

		::SendMessage( nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[menuOption]._cmdID, NeedShowDlg );
	}
}

void ShowAbout()
{
	::MessageBox(nppData._nppHandle, TEXT(" You can use Ctrl+ - jump to previous cursor position \n You can use Ctrl+Shift+ - jump to next cursor position \n You can use Ctrl+Alt+ Z jump to previous changed position \n You can use Ctrl+Alt+ Y jump to next changed position \n 'Auto clear when close'- Will remove the file's record when file closed.\n 'Always record'- Will always record the position even after you jumped.\n 'Save record when App exit'- Record data when application exit and it will be loaded in next run \n 'In Curr'- If checked, navigate only in current file\n 'Mark'- If checked, modified line will be marked by bookmark or color\n 'Mark Color/Save Color'- Available if not select mark with bookmark, you could mark with different symbol.  \n\nBundled with Textrument (v0.1.1). \n\n (Save/Restore Currently unavailable!)  Version: Original Author: Austin Young<pattazl@gmail.com>"), TEXT("About Location Navigate"), MB_OK);
}

//----------------------------------------------//
//-- ASSOCIATED FUNCTIONS END-------------------//
//----------------------------------------------//

bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
	if (index >= nbFunc)
		return false;

	if (!pFunc)
		return false;

	lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = check0nInit;
	funcItem[index]._pShKey = sk;

	return true;
}

void commandMenuInit()
{
	// Initialization of your plugin commands
	// Firstly we get the parameters from your plugin config file (if any)
	// get path of plugin configuration
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)iniFilePath);

	// if config path doesn't exist, we create it
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CreateDirectory(iniFilePath, NULL);
	}

	// make your plugin config file full file path name
	PathAppend(iniFilePath, configFileName);

	// get the parameter value from plugin config
	MaxOffset = ::GetPrivateProfileInt(sectionName, strMaxOffset, 100, iniFilePath);
	MaxList = ::GetPrivateProfileInt(sectionName, strMaxList, 50, iniFilePath) ;
	AutoClean = (::GetPrivateProfileInt(sectionName, strAutoClean, 0, iniFilePath)== 1) ;
	bIsPaused = (::GetPrivateProfileInt(sectionName, srtPaused, 0, iniFilePath)== 1) ;
	AlwaysRecord = (::GetPrivateProfileInt(sectionName, strAlwaysRecord, 0, iniFilePath)== 1) ;
	SaveRecord   = (::GetPrivateProfileInt(sectionName, strSaveRecord, 0, iniFilePath)== 1) ;
	InCurr   = (::GetPrivateProfileInt(sectionName, strInCurr, 0, iniFilePath)== 1) ;
	skipClosed   = (::GetPrivateProfileInt(sectionName, strSkipClosed, 0, iniFilePath)== 1) ;
	pinMenu   = (::GetPrivateProfileInt(sectionName, strPinMenu, 0, iniFilePath)== 1) ;
	bAutoRecord   = (::GetPrivateProfileInt(sectionName, strAutoRecord, 1, iniFilePath)== 1) ;
	AlwaysRefreshBtns   = (::GetPrivateProfileInt(sectionName, strAlwaysRefreshBtn, 0, iniFilePath)== 1) ;
	ShowFNOnly   = (::GetPrivateProfileInt(sectionName, strShowFNOnly, 1, iniFilePath)== 1) ;
	NeedMark = (::GetPrivateProfileInt(sectionName, strNeedMark, 0, iniFilePath)== 1) ;
	ByBookMark = (MarkType)::GetPrivateProfileInt(sectionName, strByBookMark, 0, iniFilePath);
	MarkColor = ::GetPrivateProfileInt(sectionName, strMarkColor, DefaultColor, iniFilePath);
	SaveColor = ::GetPrivateProfileInt(sectionName, strSaveColor, DefaultSaveColor, iniFilePath);
	//--------------------------------------------//
	//-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
	//--------------------------------------------//
	// with function :
	// setCommand(int index,                      // zero based number to indicate the order of command
	//            TCHAR *commandName,             // the command name that you want to see in plugin menu
	//            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
	//            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
	//            bool check0nInit                // optional. Make this menu item be checked visually
	//            );
	#define VK_OEM_MINUS      0xBD
	ShortcutKey *PreviousKey = new ShortcutKey{1,0,0,VK_OEM_MINUS};
	ShortcutKey *NextKey = new ShortcutKey{1,0,1,VK_OEM_MINUS};
	ShortcutKey *PreChgKey = new ShortcutKey{1,1,0,0x5A};//VK_Z
	ShortcutKey *NextChgKey = new ShortcutKey{1,1,0,0x59};//VK_Y
	ShortcutKey *optionsKey = new ShortcutKey{1,1,1,VK_OEM_MINUS};
	ShortcutKey *AutoKey = new ShortcutKey{1,0,1,VK_F9};
	ShortcutKey *ManualKey = new ShortcutKey{0,0,0,VK_F9};
	ShortcutKey *ClearRecordsKey = new ShortcutKey{1,1,1,VK_F9};
	ShortcutKey *incurrKey = new ShortcutKey{0,1,0,VK_OEM_MINUS};
	ShortcutKey *markKey = new ShortcutKey{1,1,0,0x4D};// VK_M

	setCommand(menuPrevious, TEXT("Previous Location"), PreviousLocation, PreviousKey, false);
	setCommand(menuNext, TEXT("Next Location"), NextLocation, NextKey, false);
	setCommand(menuChgPrevious, TEXT("Previous Changed"), PreviousChangedLocation, PreChgKey, false);
	setCommand(menuChgNext, TEXT("Next Changed"), NextChangedLocation, NextChgKey, false);
	
	setCommand(menuOption, TEXT("Show List and Option"), ToggleHistoryPanel, optionsKey, false);

	setCommand(menuSeparator0, TEXT("-SEPARATOR-"),NULL, NULL, false);
	setCommand(menuAlwaysRefresh, TEXT("Always Update Icons"), FlipRefreshBtns, NULL, false);
	setCommand(menuAutoRecord, TEXT("Auto Record"), AutoRecord, AutoKey, false);
	setCommand(menuManualRecord, TEXT("Record"), ManualRecord, ManualKey, false);
	setCommand(menuClearRecords, TEXT("Clear All Records"), ClearAllRecords, ClearRecordsKey, false);

	setCommand(menuInCurr, TEXT("In Current File"), NavigateInCurr, incurrKey, false);
	setCommand(menuNeedMark, TEXT("Mark Changed Line"), MarkChange, markKey, false);


	setCommand(menuSeparator1, TEXT("-SEPARATOR-"),NULL, NULL, false);

	setCommand(menuSkipClosed, TEXT("Skip Closed File"), SkipClosed, NULL, false);
	setCommand(menuClearOnClose, TEXT("Auto Clear On Close"), FlipAutoClean, NULL, false);
	setCommand(menuPause, TEXT("Pause"), PauseRecording, NULL, bIsPaused);


	setCommand(menuSeparator2, TEXT("-SEPARATOR-"),NULL, NULL, false);
	setCommand(menuPinMenu, TEXT("Pin Menu"), PinMenu, NULL, false);
	setCommand(menuAbout, TEXT("About Location Navigate"), ShowAbout, NULL, false);
}