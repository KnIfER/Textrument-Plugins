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
#pragma once
#ifndef LNHISTORY_DLG_H
#define LNHISTORY_DLG_H

#include "DockingDlgInterface.h"
#include "PluginDefinition.h"
#include "SelfCtrl.h"
#include "resource.h"
#include <deque>
#include "menuCmdID.h"
#include "ToolbarPanel.h"

#define SELF_REFRESH WM_USER+9

using namespace std;

struct LocationInfo
{
	//long line;
	//long col;
	long position;// 文档中的位置
	long bufferID;// 文档编号
	//TCHAR FilePath[MAX_PATH];
	TCHAR* FilePath;
	bool changed; // 是否修改过
};

enum MarkType{
	// 需要和 MarkTypeArr 统一
	MarkHightLight = 0,
	MarkUnderLine  ,
	MarkRect ,
	MarkRoundrect ,
	MarkCircle ,
	MarkTriangle,
	MarkArrow ,
	////////////////此处为列表的次序
	MarkBookmark, // 此表示使用书签，定义的值不在 SC_MARK 中
};
const TCHAR sectionName[] = TEXT("Setting");
const TCHAR strMaxOffset[] = TEXT("MaxOffset");
const TCHAR strMaxList[] = TEXT("MaxList");
const TCHAR strAutoClean[] = TEXT("AutoClean");
const TCHAR srtPaused[] = TEXT("Paused");
const TCHAR strAlwaysRecord[] = TEXT("AlwaysRecord");
const TCHAR strSaveRecord[] = TEXT("SaveRecord");
const TCHAR strInCurr[] = TEXT("InCurr");
const TCHAR strSkipClosed[] = TEXT("SkNill");
const TCHAR strPinMenu[] = TEXT("PinMnu");
const TCHAR strAutoRecord[] = TEXT("AutoRecord");
const TCHAR strAlwaysRefreshBtn[] = TEXT("RefreshBtn");
const TCHAR strShowFNOnly[] = TEXT("NameOnly");
const TCHAR strNeedMark[] = TEXT("NeedMark");
const TCHAR strByBookMark[] = TEXT("ByBookMark");
const TCHAR strMarkColor[] = TEXT("MarkColor");
const TCHAR strSaveColor[] = TEXT("SaveColor");
const TCHAR strRecordContent[] = TEXT("RecordContent");
const TCHAR configFileName[] = TEXT("LocationNavigate.ini");
extern TCHAR iniFilePath[MAX_PATH];

extern void InitBookmark();
extern vector<MarkData> MarkHistory;

extern deque<LocationInfo>LocationList;
extern deque<LocationInfo>LocationSave;// 用于关闭前保存数据的数组
extern long LocationPos;  // 队列中的位置
extern bool PositionSetting; // 是否进行跳转
extern int MaxOffset;
extern int MaxList ;
extern bool AutoClean ;
extern bool AlwaysRecord;
extern bool SaveRecord;
extern bool InCurr;
extern bool AlwaysRefreshBtns;
extern bool ShowFNOnly;
extern bool skipClosed;
extern bool pinMenu;
extern bool bIsPaused;
extern bool bAutoRecord;
extern bool NeedMark;
extern MarkType ByBookMark;
extern long MarkColor;
extern long SaveColor;

extern bool legacy ;

extern HWND curScintilla;
//extern CRITICAL_SECTION criCounter;
extern FuncItem funcItem[nbFunc];
extern bool menuState[nbFunc];
extern HWND hToolbar;
extern int IconID[nbFunc];
extern NppData nppData;

extern void AddList(bool flag=false);
extern void ClearLocationList();

bool SetPosByIndex(int delta, bool doit=true);
void EnableTBButton(menuList flagIndex, bool state, bool force=false);
#define TB_ENABLEBUTTON         (WM_USER + 1)
class LocationNavigateDlg : public DockingDlgInterface
{
public :
	LocationNavigateDlg() : DockingDlgInterface(IDD_LOCATIONNAVIGATE) {};

    virtual void display(bool toShow = true); 

	virtual void setClosed(bool toClose);

	void refreshDlg(bool updateList);

	void setParent(HWND parent2set){
		_hParent = parent2set;
	};

	HWND _hListBox,_hUG_T,_hUG_E,_hUG_OK,_hMark,_hUG_T2,_hUG_E2,_hABOUT,_hAuto,_hClear,_hAlways,_hSaveRecord,_hInCurr,_hBookmark,_hColor,_hSaveColor;
protected :
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	SelfCtrl _color,_savecolor;
private :
	int skFlags=0;
	bool hasChanged;
	void refreshValue();
	ToolbarPanel ListBoxPanel;
	//Window ListBoxWrap;
	ToolBar toolBar;
	void OnToolBarCommand( UINT Cmd );
};

#endif //LNHISTORY_DLG_H
