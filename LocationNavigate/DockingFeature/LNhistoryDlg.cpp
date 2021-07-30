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

#include "LNhistoryDlg.h"
#include "PluginDefinition.h"

bool bSupressSel=false;

void LocationNavigateDlg::display(bool toShow){
	DockingDlgInterface::display(toShow);
	if (toShow)
		::SetFocus(::GetDlgItem(_hSelf, IDOK));

	setClosed(false);

	::SendMessage( _hSelf, SELF_REFRESH, 0, 1);
};

void LocationNavigateDlg::setClosed(bool toClose) {
	_isClosed = toClose;
	::CheckMenuItem(::GetMenu(nppData._nppHandle), 
		funcItem[menuOption]._cmdID, MF_BYCOMMAND | (toClose?MF_UNCHECKED:MF_CHECKED));
}

void LocationNavigateDlg::refreshDlg(bool updateList) {
	skFlags=0;
	hasChanged=0;
	if (isCreated() && isVisible())
	{
		::SendMessage( _hSelf, SELF_REFRESH, AlwaysRefreshBtns, updateList);
		if(!AlwaysRefreshBtns&&!updateList) {
			hasChanged=1;
		}
	} else {
		if(AlwaysRefreshBtns) {
			::SendMessage( _hSelf, SELF_REFRESH, 1, 0);
		} else {
			hasChanged=1;
		}
	}
	skFlags=~skFlags;

	int size=LocationList.size();
	EnableTBButton(menuNext, (skFlags&0x1)&&size&&LocationPos<size-1);
	EnableTBButton(menuPrevious, (skFlags&0x2)&&size&&LocationPos >0);
	EnableTBButton(menuChgNext, hasChanged&&(skFlags&0x4)&&size&&LocationPos<size-1);
	EnableTBButton(menuChgPrevious, hasChanged&&(skFlags&0x8)&&size&&LocationPos > 0);
};


void LocationNavigateDlg::refreshValue()
{
	TCHAR strHint[500]={0};	
	wsprintf(strHint,TEXT("%d"),MaxOffset);
	::SendMessage( _hUG_E, WM_SETTEXT ,0, (WPARAM)strHint);
	wsprintf(strHint,TEXT("%d"),MaxList);
	::SendMessage( _hUG_E2, WM_SETTEXT,0,(WPARAM)strHint);
	//BM_SETCHECK BM_SETCHECK:MF_UNCHECKED
	::SendMessage( _hAuto, BM_SETCHECK ,(LPARAM)(AutoClean?1:0),0);
	::SendMessage( _hAlways, BM_SETCHECK ,(LPARAM)(AlwaysRecord?1:0),0);
	::SendMessage( _hSaveRecord, BM_SETCHECK ,(LPARAM)(SaveRecord?1:0),0);
	::SendMessage( _hInCurr, BM_SETCHECK ,(LPARAM)(InCurr?1:0),0);
	::SendMessage( _hMark, BM_SETCHECK ,(LPARAM)(NeedMark?1:0),0);


	//::SendMessage( _hBookmark, BM_SETCHECK ,(LPARAM)(ByBookMark?1:0),0);
	::SendMessage(_hBookmark, CB_SETCURSEL, ByBookMark, 0);
}

void EnableTBButton(menuList flagIndex, bool state, bool force)
{
	//::MessageBox(NULL, TEXT("111"), TEXT(""), MB_OK);
	if (force||menuState[flagIndex]!=state)
	{
		//TCHAR buffer[100]={0};
		//wsprintf(buffer,TEXT("position=%d,%d"),flagIndex,state);
		//::MessageBox(NULL, buffer, TEXT(""), MB_OK);
		// 状态不符合，需要改变

		if(legacy) {
			::EnableMenuItem(::GetMenu(nppData._nppHandle), funcItem[flagIndex]._cmdID,MF_BYCOMMAND|(state?MF_ENABLED:MF_GRAYED));
			if ( IconID[flagIndex] !=-1)
			{
				if(!IsWindow(hToolbar)) {
					HWND hReBar;
					hReBar = FindWindowEx( nppData._nppHandle, hReBar, TEXT( "ReBarWindow32" ), NULL );
					hToolbar = FindWindowEx( hReBar, NULL, TEXT( "ToolbarWindow32" ), NULL );
				}
				::SendMessage(hToolbar,TB_ENABLEBUTTON,(WPARAM)IconID[flagIndex],MAKELONG((state?TRUE:FALSE), 0));
			}
		} else {
			::SendMessage(nppData._nppHandle,NPPM_ENABLEMENUITEM,(WPARAM)IconID[flagIndex],MAKELONG((state?TRUE:FALSE), 0));
		}
		menuState[flagIndex] = state;
	}
}
bool SetPosByIndex(int delta, bool doit)
{
	long maxLen = LocationList.size()-1;

	if(delta) {
		LocationPos+=delta;
	}

	long res = 0;

	if (maxLen==-1 || LocationPos<0 || LocationPos>maxLen)
	{
		return true;
	}
	PositionSetting = true;

	//TCHAR buffer[300]={0};
	//wsprintf(buffer,TEXT("fN=%s"), LocationList[LocationPos].FilePath);
	//::MessageBox(NULL, buffer, TEXT(""), MB_OK);

	if(delta) {
		if(skipClosed) {
			while(LocationPos>=0 && LocationPos<=maxLen
				&& 1!=(res=SendMessage(nppData._nppHandle, NPPM_SWITCHTOFILE, 0 , (LPARAM)LocationList[LocationPos].FilePath))) {
				if(doit) {
					LocationPos+=delta;
				} else {
					return false;
				}
			}
			if(LocationPos<0) {
				LocationPos=0;
			} else if(LocationPos>maxLen) {
				LocationPos=maxLen;
			}
		}
	}

	long pos = LocationList[LocationPos].position;
	
	if(res!=1) {
		res = ::SendMessage(nppData._nppHandle, skipClosed?NPPM_SWITCHTOFILE:NPPM_DOOPEN, 
			0 , (LPARAM)LocationList[LocationPos].FilePath);
	}
	
	if (pos != -1 && res ==1)
	{
		// Get the current scintilla
		//int which = -1;
		//::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
		//if (which == -1)
		//	return ;
		//HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

		// 此处为简单行定位，后续改为中央定位
		//long line = ::SendMessage(curScintilla, SCI_LINEFROMPOSITION, pos , 0);
		//::SendMessage(curScintilla, SCI_ENSUREVISIBLE, line, 0);

		// 需要垂直居中，中央定位
		// 获取开始显示的行数,有些不准确
		long beginLines = ::SendMessage(curScintilla, SCI_GETFIRSTVISIBLELINE , 0 , 0);
		// 获取当前显示的行数
		//long currLines = ::SendMessage(curScintilla, SCI_GETCURLINE , 0 , 0);
		//long position = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
		long targetLine = ::SendMessage(curScintilla, SCI_LINEFROMPOSITION, pos, 0);
		// 获取屏幕高度
		long screenLines = ::SendMessage(curScintilla, SCI_LINESONSCREEN , 0 , 0);
		

		// 根据当前位置和目标位置进行计算，滚动到中央
		long currCenterLine = (int)(beginLines+screenLines/2);
		long scrollLine = 0; // currLines2-(int)(beginLines+screenLines/2)
		// 直接根据当前的中心行和目标行的距离进行滚动设置，但是由于中心行定位不准，暂不用
		//scrollLine = targetLine - currCenterLine ;
		//::SendMessage(curScintilla, SCI_LINESCROLL , 0 , scrollLine); 
		// 采用
		if(targetLine < beginLines){
			scrollLine = -screenLines/2;
		}else if ( targetLine > beginLines+screenLines)
		{
			scrollLine = screenLines/2;
		}
		// 定位到具体位置
		::SendMessage(curScintilla, SCI_GOTOPOS, pos, 0);
		// 滚动半个屏幕
		::SendMessage(curScintilla, SCI_LINESCROLL , 0 , scrollLine ); 
		// 防止滚动错误，让这个位置确保处于可见范围
		::SendMessage(curScintilla, SCI_SCROLLCARET , 0 , 0);
		
		//::SendMessage(curScintilla, SCI_GOTOLINE, line-1, 0);
		//long caretPos = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
		//::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
		// ::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0 , (LPARAM)list_data[0].filename);
	}else
	{
		PositionSetting = false;
	}
	
	return true;
}
void ClearLocationList()
{
	LocationList.clear();
	LocationPos = -1;
}


ToolBarButtonUnit ToolBarIconList[] = {
	{IDM_EX_OPTIONS, -1, -1, -1, IDB_EX_OPTIONS }, 
	{IDM_EX_UP, -1, -1, -1, IDB_EX_UP }, 
	{IDM_EX_DOWN, -1, -1, -1, IDB_EX_DOWN }, 
	{IDM_EX_BREFNAME, -1, -1, -1, IDB_EX_BREFNAME }, 
	{IDM_EX_DELETE_ALL, -1, -1, -1, IDB_EX_DELETE_ALL }, 
	{IDM_EX_DELETE, -1, -1, -1, IDB_EX_DELETE }, 
};

#define ListBoxToolBarSize sizeof(ToolBarIconList)/sizeof(ToolBarButtonUnit)

int toolbarHeight=28;

int BufferIdBeforeClick=0;

TCHAR strHint[500]={0};

INT_PTR CALLBACK LocationNavigateDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_COMMAND : 
		{
			if ( (HWND)lParam == toolBar.getHSelf() )
			{
				OnToolBarCommand( LOWORD(wParam) );
				return 0;
			}
			switch (wParam)
			{
				case MAKELONG(IDC_BUTTON_CLEAR,BN_CLICKED):
				{
					ClearLocationList();
					refreshDlg(1);
					::SendMessage(curScintilla, SCI_MARKERDELETEALL, _MARK_INDEX,0);
					::SendMessage(curScintilla, SCI_MARKERDELETEALL, _SAVE_INDEX,0);
					MarkHistory.clear();
				}
				break;
				case MAKELONG(IDC_CHECK_MARK,BN_CLICKED):
					{
						NeedMark  = (::SendMessage(_hMark, BM_GETCHECK,0,0))==1;
						InitBookmark();
						// 刷新菜单
						::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuNeedMark]._cmdID, MF_BYCOMMAND | (NeedMark?MF_CHECKED:MF_UNCHECKED));
					}
				break;
				case MAKELONG(IDC_CHECK_INCURR,BN_CLICKED):
				{
					InCurr  = (::SendMessage(_hInCurr, BM_GETCHECK,0,0))==1;
					// 刷新菜单
					::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuInCurr]._cmdID, MF_BYCOMMAND | (InCurr?MF_CHECKED:MF_UNCHECKED));
					refreshDlg(0);
				}
				break;
				case MAKELONG(IDC_CHECK_SAVERECORD,BN_CLICKED):
				case MAKELONG(IDC_CHECK_AUTO,BN_CLICKED):
				case MAKELONG(IDC_CHECK_ALWAYS,BN_CLICKED):
				case MAKELONG(IDC_CHECK_BOOKMARK,CBN_SELCHANGE):

				case MAKELONG(ID_LNHISTORY_EDIT,EN_KILLFOCUS):
				case MAKELONG(ID_LNHISTORY_EDIT2,EN_KILLFOCUS):

				{
					BOOL isSuccessful;
					int val = ::GetDlgItemInt(_hSelf, ID_LNHISTORY_EDIT, &isSuccessful, FALSE);
					if ( val >=2 && val <=5000 )
					{
						MaxOffset = val;
					}
					val = ::GetDlgItemInt(_hSelf, ID_LNHISTORY_EDIT2, &isSuccessful, FALSE);
					if ( val < 5) {
						val=5;
					} else if(val > 65536) {
						val=65536;
					}
					MaxList = val;
					//sync here
					AutoClean = (::SendMessage(_hAuto, BM_GETCHECK,0,0))==1;
					AlwaysRecord = (::SendMessage(_hAlways, BM_GETCHECK,0,0))==1;
					SaveRecord  = (::SendMessage(_hSaveRecord, BM_GETCHECK,0,0))==1;
					bIsPaused  = MF_CHECKED&::GetMenuState( ::GetMenu( nppData._nppHandle ), funcItem[menuPause]._cmdID, MF_BYCOMMAND );

					int preByBookMark = ByBookMark;
					ByBookMark = (MarkType)::SendMessage(_hBookmark, CB_GETCURSEL,0,0);
					if ( ByBookMark != preByBookMark)
					{
						//需要改变主界面的样式
						if( MarkBookmark!= ByBookMark)
						{
							InitBookmark();
						}
					}
					refreshValue();
					//::MessageBox(this->getHSelf(),TEXT("Setting is Saved!") , TEXT("Info"), MB_OK);
				}
				break;
				// 响应双击和按钮事件 doubleclick
				case MAKELONG(IDC_LOC_LIST,LBN_DBLCLK):
				case IDOK :
				{
					//if ( LocationPos > -1 && LocationPos < LocationList.size()) {
					//	BufferIdBeforeClick = LocationList[LocationPos].bufferID;
					//} else {
					//	BufferIdBeforeClick = 0;
					//}
					BufferIdBeforeClick=1;
					long index = ::SendMessage( _hListBox, LB_GETCURSEL, 0, 0);
					if ( index > -1 && index < LocationList.size())
					{
						LocationPos = index;
						SetPosByIndex(0, false);
					}
					//::SendMessage(nppData._nppHandle, NPPM_GETPOSFROMBUFFERID, 0, (LPARAM)&which);
					refreshDlg(0);
					BufferIdBeforeClick=0;
					return TRUE;
				}
			break;
			}
			return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
		}
		break;
		case SELF_REFRESH://WM_TIMER
		{
			// 首先需要检查 结构是否改变过
			// 读取变量刷新到页面
			//LockWindowUpdate(_hListBox) ;
			bool skingNxt=true;
			bool skingPrev=true;
			bool skingNxtChng=true;
			bool skingPrevChng=true;
			if(!InCurr) {
				skingNxt=skingPrev=false;
			}
			int LocationSize=LocationList.size()-1;
			bool do_refresh_dlg = lParam;
			if(wParam||do_refresh_dlg) {
				if(do_refresh_dlg) {
					ZeroMemory(strHint, 500);
					::SendMessage( _hListBox, LB_RESETCONTENT, 0, 0 );
				}
				//::MessageBox(NULL,TEXT("111") , TEXT(""), MB_OK);
				short compared;
				for ( int i=0;i<=LocationSize;i++ )
				{
					auto & infoI = LocationList[i];
					compared=-1;
					if(InCurr && (skingNxt||skingPrev) && !(compared=lstrcmp(infoI.FilePath, currFile))) {
						if(skingNxt&&i>LocationPos) {
							skingNxt=false;
						}
						if(skingPrev&&i<LocationPos) {
							skingPrev=false;
						}
					}
					if(infoI.changed) {
						hasChanged=1;
						if((skingNxtChng||skingPrevChng) && (!InCurr||compared==0||compared==-1&&!lstrcmp(infoI.FilePath, currFile))) {
							if(skingNxtChng&&i>LocationPos) {
								skingNxtChng=false;
							}
							if(skingPrevChng&&i<LocationPos) {
								skingPrevChng=false;
							}
						}
					}
					if(do_refresh_dlg) {
						if(ShowFNOnly) {
							int len=lstrlen(infoI.FilePath);
							TCHAR *strAddr = infoI.FilePath;
							for(int j=len-1;j>=0;j--) {
								if(strAddr[j]==TCHAR('\\')) {
									strAddr+=j+1;
									break;
								}
							}
							wsprintf(strHint,infoI.changed?TEXT("%s * : (%d)"):TEXT("%s : (%d)"), strAddr, infoI.position+1);
							//wsprintf(strHint,TEXT("%s * : (%d) (%d)"), strAddr, infoI.FilePath, infoI.FilePath);
						} else {
							wsprintf(strHint,TEXT("%c,%d,%s"), infoI.changed?'!':'=', infoI.position+1, infoI.FilePath);
						}
						::SendMessage( _hListBox, LB_ADDSTRING, 0, (LPARAM)strHint );
					}
				}
				skFlags = skingNxt|skingPrev<<1|skingNxtChng<<2|skingPrevChng<<3;
			} 
			if(!do_refresh_dlg) {
				::SendMessage( _hListBox, LB_SETSEL, 0, -1);
			}
			// 设置当前点
			//::SendMessage( _hListBox, LB_SETCURSEL, LocationPos, 0);
			if(!bSupressSel) {
				::SendMessage( _hListBox, LB_SETSEL, 1, LocationPos);
			}
			//refreshValue();
		}
		break;
		case WM_INITDIALOG:
		{
			_hListBox =	GetDlgItem(_hSelf, IDC_LOC_LIST);

			_hUG_T =	GetDlgItem(_hSelf, ID_UGO_STATIC);
			_hUG_E =	GetDlgItem(_hSelf, ID_LNHISTORY_EDIT);

			_hUG_T2 =	GetDlgItem(_hSelf, ID_UGO_STATIC2);
			_hUG_E2 =	GetDlgItem(_hSelf, ID_LNHISTORY_EDIT2);

			_hUG_OK =	GetDlgItem(_hSelf, IDOK);
			_hMark =	GetDlgItem(_hSelf, IDC_CHECK_MARK);

			_hABOUT = GetDlgItem(_hSelf, IDC_STATIC_ABOUT);

			_hAuto = GetDlgItem(_hSelf, IDC_CHECK_AUTO);
			_hAlways = GetDlgItem(_hSelf, IDC_CHECK_ALWAYS);

			_hSaveRecord = GetDlgItem(_hSelf, IDC_CHECK_SAVERECORD);
			
			_hClear = GetDlgItem(_hSelf, IDC_BUTTON_CLEAR);

			_hInCurr = GetDlgItem(_hSelf, IDC_CHECK_INCURR);

			_hBookmark = GetDlgItem(_hSelf, IDC_CHECK_BOOKMARK);

			::SendMessage( _hBookmark, CB_ADDSTRING ,0,(LPARAM)TEXT("Mark with highlight"));
			::SendMessage( _hBookmark, CB_ADDSTRING ,0,(LPARAM)TEXT("Mark with underline"));
			::SendMessage( _hBookmark, CB_ADDSTRING ,0,(LPARAM)TEXT("Mark with rect"));
			::SendMessage( _hBookmark, CB_ADDSTRING ,0,(LPARAM)TEXT("Mark with roundrect"));
			::SendMessage( _hBookmark, CB_ADDSTRING ,0,(LPARAM)TEXT("Mark with circle"));
			::SendMessage( _hBookmark, CB_ADDSTRING ,0,(LPARAM)TEXT("Mark with triangle"));
			::SendMessage( _hBookmark, CB_ADDSTRING ,0,(LPARAM)TEXT("Mark with arrow"));
			::SendMessage( _hBookmark, CB_ADDSTRING ,0,(LPARAM)TEXT("Mark with bookmark"));

			_hColor = GetDlgItem(_hSelf, ID_STATIC_COLOR);
			_color.init(_hInst, _hSelf);
			// 0x86c2ff
			_color.create(_hColor,_MARK_INDEX,MarkColor);

			_hSaveColor = GetDlgItem(_hSelf, ID_STATIC_SAVECOLOR);

#if 1
			ListBoxPanel.init( _hInst, _hSelf );
			//ListBoxWrap.init(_hInst, ListBoxPanel.getHSelf());
			//ListBoxPanel.SetChildWindow( &ListBoxWrap );
			toolBar.init( _hInst, _hSelf, TB_STANDARD, ToolBarIconList, ListBoxToolBarSize );
			//toolBar.init( _hInst, ListBoxPanel.getHSelf(), TB_STANDARD, ListBoxToolBarButtons, ListBoxToolBarSize );
			toolBar.display();
			ListBoxPanel.SetToolbar( &toolBar );

			toolBar.setCheck(IDM_EX_BREFNAME, ShowFNOnly?1:0);
#if TBPIsWrapper
			ListBoxPanel.create(_hSaveColor);
#else
			_savecolor.init(_hInst, _hSelf);
			_savecolor.create(_hSaveColor,_SAVE_INDEX,SaveColor);
			//RECT rect{0,0,100,100};
			//ListBoxPanel.reSizeTo(rect);
#endif
#endif

			refreshValue();
			// 
			//SetTimer(_hSelf,1,500,NULL);
		} break;
		case WM_SIZE:
		case WM_MOVE:
		{
			RECT rc;
			getClientRect(rc);


			//rc.top+=100;
			//rc.bottom-=100;

			::MoveWindow(_hListBox,rc.left, rc.top+toolbarHeight, rc.right, rc.bottom-70-toolbarHeight,TRUE);

			::MoveWindow(_hUG_T, rc.left+60, rc.bottom-62, 70, 18, TRUE);
			::MoveWindow(_hUG_E, rc.left+135, rc.bottom-65, 40, 18, TRUE);

			::MoveWindow(_hUG_T2, rc.left+60, rc.bottom-42, 70, 18, TRUE);
			::MoveWindow(_hUG_E2, rc.left+135, rc.bottom-45, 40, 18, TRUE);

			::MoveWindow(_hInCurr, rc.left+10, rc.bottom-65, 40,18, TRUE);
			::MoveWindow(_hMark, rc.left+10, rc.bottom-45, 40,18, TRUE);

				
			::MoveWindow(_hABOUT, rc.left+185, rc.bottom-65, 500,36, TRUE);
			::MoveWindow(_hClear, rc.left+180, rc.bottom-38, 40,14, TRUE);

			::MoveWindow(_hColor,  rc.left+225, rc.bottom-38, 60,16, TRUE);
			::MoveWindow(_hSaveColor,  rc.left+285, rc.bottom-38, 60,16, TRUE);
			::MoveWindow( _hAuto ,  rc.left+350, rc.bottom-38, 130,16, TRUE);

			::MoveWindow(_hUG_OK, rc.left+10, rc.bottom-22, 40,18, TRUE);
			::MoveWindow(_hSaveRecord, rc.left+60, rc.bottom-22, 150,18, TRUE);
			::MoveWindow( _hBookmark,  rc.left+220, rc.bottom-22, 120,16, TRUE);
			::MoveWindow(_hAlways,  rc.left+350, rc.bottom-22, 90,16, TRUE);
				
			rc.bottom=toolbarHeight;
			ListBoxPanel.reSizeTo(rc);

			redraw();
		} break;
		case WM_NOTIFY: 
		{
			LPNMHDR	pnmh	= (LPNMHDR)lParam;
			if (pnmh->hwndFrom == _hParent)
			{
				switch (LOWORD(pnmh->code))
				{
				case DMN_CLOSE:
					{
						setClosed(1);
						break;
					}
				default:
					break;
				}
			}
			break;
		}
	}
	return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}

bool getMenuItemNeedsKeep(int mid) {
	switch(mid) {
		case menuNext:
		case menuPrevious:
		case menuChgNext:
		case menuChgPrevious:
		case menuAutoRecord:
		case menuOption:
		case menuInCurr:
		case menuNeedMark:
		case menuSkipClosed:
		case menuClearOnClose:
		case menuPinMenu:
		case menuPause:
		return true;
	}
	return false;
}

bool getMenuItemChecked(int mid) {
	switch(mid) {
		case menuAutoRecord:
			return bAutoRecord;
		case menuOption:
		{
			UINT state = ::GetMenuState( ::GetMenu( nppData._nppHandle ),
				funcItem[menuOption]._cmdID, MF_BYCOMMAND );
			return state & MF_CHECKED;
		}
		case menuInCurr:
			return InCurr;
		case menuNeedMark:
			return NeedMark;
		case menuSkipClosed:
			return skipClosed;
		case menuPinMenu:
			return pinMenu;
		case menuClearOnClose:
			return AutoClean;
		case menuPause:
			return bIsPaused;
	}
	return false;
}

void TrackPopup(HWND _hSelf);

void simulToolbarMenu(HMENU pluginMenu, RECT *rc, HWND _hSelf, bool recreate){
	int cmd = TrackPopupMenu(pluginMenu, TPM_RETURNCMD, rc->left,  rc->top+toolbarHeight, 0, _hSelf, NULL);

	if(cmd) {
		for(int idx=0;idx<nbFunc;idx++) {
			if(funcItem[idx]._cmdID==cmd) {
				funcItem[idx]._pFunc();
				if(pinMenu && getMenuItemNeedsKeep(idx) /*|| idx==nbFunc-2*/) {
					if(recreate) {
						TrackPopup(_hSelf);
					} else {
						simulToolbarMenu(pluginMenu, rc, _hSelf, recreate);
					}
				}
			}
		}
	}
}

extern HANDLE				g_hModule;

void TrackPopup(HWND _hSelf) {

	HMENU pluginMenu = (HMENU)::SendMessage(nppData._nppHandle, NPPM_GETPLUGINMENU, 0 , (LPARAM)g_hModule);

	bool recreate=false;

	if(!pluginMenu) {
		//pluginMenu = (HMENU)::GetMenu(nppData._nppHandle);
		if(!pluginMenu) {

			//::MessageBox(NULL, TEXT("111"), TEXT(""), MB_OK);
			pluginMenu = ::CreatePopupMenu();
			unsigned short j = 0;
			for ( ; j < nbFunc ; ++j)
			{
				if (funcItem[j]._pFunc == NULL)
				{
					::InsertMenu(pluginMenu, j, MF_BYPOSITION | MF_SEPARATOR, 0, TEXT(""));
					continue;
				}
				generic_string itemName = funcItem[j]._itemName;
				::InsertMenu(pluginMenu, j, MF_BYPOSITION, funcItem[j]._cmdID, itemName.c_str());
				if (getMenuItemChecked(j))
					::CheckMenuItem(pluginMenu, funcItem[j]._cmdID, MF_BYCOMMAND | MF_CHECKED);
			}
			recreate = true;
		}
	}
	RECT rc;
	GetWindowRect(_hSelf, &rc);
	simulToolbarMenu(pluginMenu, &rc, _hSelf, recreate);
}

void LocationNavigateDlg::OnToolBarCommand( UINT Cmd )
{ 
	switch ( Cmd ) {
		case IDM_EX_OPTIONS:
			TrackPopup(_hSelf);
		return;
		case IDM_EX_BREFNAME: {
			ShowFNOnly=!ShowFNOnly;
			toolBar.setCheck(IDM_EX_BREFNAME, ShowFNOnly?1:0);
			refreshDlg(1);
		} return;
		case IDM_EX_UP:
			PreviousLocation();
		return;
		case IDM_EX_DOWN:
			NextLocation();
		return;
		case IDM_EX_DELETE: {
			int sellen = ::SendMessage(_hListBox, LB_GETSELCOUNT , 0, 1);
			if(sellen) {
				int* data = new int[sellen];
				::SendMessage(_hListBox, LB_GETSELITEMS, sellen, (LPARAM)data);
				for(sellen--;sellen>=0;sellen--) {
					LocationList.erase(LocationList.begin()+data[sellen]);
				}
				int sel=data[0];
				bSupressSel=1;
				::SendMessage(_hSelf, SELF_REFRESH, 1, 1);
				bSupressSel=0;
				if(sel>=LocationList.size()) {
					sel=LocationList.size()-1;
				}
				::SendMessage( _hListBox, LB_SETSEL, 1, sel);
			}
		} return;
		case IDM_EX_DELETE_ALL:
			ClearAllRecords();
		return;
	}
}