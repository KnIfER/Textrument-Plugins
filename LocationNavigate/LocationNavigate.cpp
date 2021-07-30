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
#include <shlwapi.h>
#include <tchar.h>
#include "LNhistoryDlg.h"
#include <list>
#include <WinBase.h>
#include <set>  
#include <map>  
#include "Scintilla.h"

#define UseThread 0

struct ptrCmp
{
	bool operator()( const TCHAR * s1, const TCHAR * s2 ) const
	{
		return lstrcmp( s1, s2 ) < 0;
	}
};
set<TCHAR *, ptrCmp> filename_table;

////////////////SELF DATA BEGIN///////////
TCHAR currFile[MAX_PATH]={0};
static int currBufferID=-1,PrePositionNumber=-1;
static bool ready = false;
static bool haveDeleted = false;
static bool PageActive = false;
static bool isOpenFile = false;
static bool isAutoModify = false;
vector<MarkData> MarkHistory;
// 需要和 MarkType 统一
int MarkTypeArr[] ={
	SC_MARK_BACKGROUND,
	SC_MARK_UNDERLINE ,
	SC_MARK_FULLRECT,
	SC_MARK_ROUNDRECT,
	SC_MARK_CIRCLE,
	SC_MARK_ARROW,
	SC_MARK_SHORTARROW};
///////DEBUG//////////////////////////////////////
//TCHAR buffer[800]={0};
//wsprintf(buffer,TEXT("%d %d %d %d-%d %d %d %d-%d %d %d %d-%d %d %d %d-%d %d %d"),	notifyCode->position,	
//		 notifyCode->ch,		
//		 notifyCode->modifiers,	
//		 ModifyType,	
//		 notifyCode->length,		
//		 notifyCode->linesAdded,	
//		 notifyCode->message,	
//		 notifyCode->line,		
//		 notifyCode->foldLevelNow,	
//		 notifyCode->foldLevelPrev,	
//		 notifyCode->margin,		
//		 notifyCode->listType,	
//		 notifyCode->x,			
//		 notifyCode->y,		
//		 notifyCode->token,		
//		 notifyCode->annotationLinesAdded,
//		 notifyCode->nmhdr.code,
//		 notifyCode->nmhdr.hwndFrom,
//		 notifyCode->nmhdr.idFrom);
//::MessageBox(NULL,buffer , TEXT(""), MB_OK);
////////////////SELF DATA END///////////

extern FuncItem funcItem[nbFunc];

extern LocationNavigateDlg _LNhistory;


extern int BufferIdBeforeClick;

int PrevSelStart=-1, PrevSelEnd=-1;

void getTextSelection(int& start, int& end) {
	// Get active scintilla
	int currentEdit;
	::SendMessage( nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM) &currentEdit );
	auto activeVanilla = currentEdit?nppData._scintillaSecondHandle:nppData._scintillaMainHandle;
	start = SendMessage(activeVanilla, SCI_GETSELECTIONSTART, 0, 0);
	end = SendMessage(activeVanilla, SCI_GETSELECTIONEND, 0, 0);
}

bool IsSelectionOverlapping( const int CurrSelStart, const int CurrSelEnd )
{
	//if(1) return 0;

	if ( CurrSelStart == CurrSelEnd )
	{
		// No text selected now, must be no overlapping
		return false;
	}

	if ( PrevSelStart == PrevSelEnd )
	{
		// Previously no text selected, no overlapping possible
		return false;
	}

	//if ( PrevSelStart == CurrSelStart && PrevSelEnd == CurrSelEnd )
	//{
	//	// Optimisation: if same region selected, then don't copy text as it has already been copied
	//	return false;
	//}
	//
	//// This test catches "underlaps" as well as overlaps
	//if ( CurrSelStart <= PrevSelEnd && CurrSelEnd >= PrevSelStart )
	//{
	//	return true;
	//}

	// Finally, there's no overlapping
	return CurrSelStart==PrevSelStart||CurrSelEnd==PrevSelStart||CurrSelStart==PrevSelEnd||CurrSelEnd==PrevSelEnd;
}

bool IsSelectionOverlapping() {
	int CurrSelStart = -1, CurrSelEnd = -1;
	getTextSelection(CurrSelStart, CurrSelEnd);
	bool ret = IsSelectionOverlapping(CurrSelStart, CurrSelEnd);
	PrevSelStart = CurrSelStart;
	PrevSelEnd   = CurrSelEnd;
	return ret;
}

#define NEWFILE TEXT("new  1")
/////////// SELF FUNCTION BEGIN //////
static bool AllCloseFlag = false;
TCHAR currTmpFile[MAX_PATH];// 临时存储当前文件名
int currTmpBufferID; // 临时存储当前文件ID
enum ActionType
{
	ActionModify =0,
	ActionActive,
	ActionClosed,
	ActionLocation,
};
struct ActionData 
{
	ActionType type;
	int position;
	int length;
	bool changed;
};
list<ActionData> ActionDataList;
void DoFilesCheck()
{
	// 查看此文件是否已经多个窗口都关闭，首先获得全部窗口，然后判断文件路径
	TCHAR  **buffer;
	long filecount = ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, (LPARAM)ALL_OPEN_FILES); 
	//TCHAR TMP[200];
	//wsprintf(TMP,TEXT("%d,%d,%d"),filecount);
	//::MessageBox(NULL,TMP , TEXT(""), MB_OK);
	buffer = new TCHAR*[filecount];
	for (int i=0;i<filecount;i++)
	{
		buffer[i] = new TCHAR[MAX_PATH];
	}
	//todo msg(NPPM_GETOPENFILENAMES) return closed tmp files
	::SendMessage(nppData._nppHandle, NPPM_GETOPENFILENAMES, (WPARAM)buffer, (LPARAM)filecount);
	
	//for (int i=0;i<filecount;i++) ::MessageBox(NULL, buffer[i], TEXT(""), MB_OK);
	
	// 将所有列表中不存在文件的都删除掉
	long PreBufferID= -1;
	bool haveFiles = false;
	//EnterCriticalSection(&criCounter); 

	for (int listIndex=0;listIndex<LocationList.size();listIndex++)
	{
		if ( LocationList[listIndex].bufferID != PreBufferID )
		{
			// 不一样需要检查
			int len = lstrlen(LocationList[listIndex].FilePath);
			haveFiles = false;
			PreBufferID = LocationList[listIndex].bufferID;

			for ( int ifile=0;ifile<filecount;ifile++)
			{
				if( lstrcmp( buffer[ifile], LocationList[listIndex].FilePath ) ==0 )
				{
					haveFiles = true;
					break;
				}
			}
		}
		// 找不到文件需要删除
		if( !haveFiles )
		{
			LocationList.erase(LocationList.begin()+listIndex);
			if(LocationPos>=listIndex)LocationPos--;
			if (LocationPos<0)
			{
				LocationPos =0;
			}
			//删除后此位置需要重新计算
			listIndex--;
		}
	}
	for (int i=0;i<filecount;i++)
	{
		delete []buffer[i];
	}
	delete []buffer;

	//_LNhistory.refreshDlg();
}
void DoModify(int len,int pos)
{
	if(bIsPaused) return;
	// SEARCH in files WILL CHANGE IT
	//TCHAR buffer[10000];
	//wsprintf(buffer,TEXT("position=%d,length=%d,%d=%d,%d"),pos,len,notifyCode->nmhdr.idFrom,currBufferID,curScintilla);
	//::MessageBox(NULL,buffer, TEXT(""), MB_OK);
	bool needRemove = false;
	for (int i=0;i<LocationList.size();i++)
	{
		if ( LocationList[i].bufferID == currBufferID )
		{
			if (len>0)
			{
				if (LocationList[i].position>pos )
				{
					LocationList[i].position+=len;
				}
			}else
			{
				long lastpos=pos-len;
				if ( LocationList[i].position>pos && LocationList[i].position<lastpos)
				{
					// 减的时候可能导致空的,无效值
					//移走
					LocationList[i].position = -1;
					needRemove = true;
				}else if ( LocationList[i].position>= lastpos)							{
					LocationList[i].position+=len;
				}
			}
		}
	}
	if( needRemove )
	{
		for (int i=0;i<LocationList.size();i++)
		{
			if ( -1 == LocationList[i].position)
			{
				LocationList.erase(LocationList.begin()+i);
				// LocationPos 改变
				if(LocationPos>=i)LocationPos--;
				if (LocationPos<0)
				{
					LocationPos =0;
				}
				//删除后此位置需要重新计算
				i--;
			}
		}
		haveDeleted = true;
	}
	// 不是删除的，最后一个为修改的
	if ( !haveDeleted)
	{
		int lastPos =  LocationList.size()-1;
		if ( lastPos >=0)
		{
			LocationList[lastPos].changed = true;
		}
	}
}

static bool ThreadNeedRefresh = false;

void AddListData(LocationInfo *tmp)
{
	if(bIsPaused) return;

	long preLen = LocationList.size();

	if(preLen && IsSelectionOverlapping()) {
		if(LocationList[preLen-1].bufferID == (*tmp).bufferID) {
			if(LocationList[preLen-1].position!=(*tmp).position) {
				LocationList[preLen-1].position=(*tmp).position;
				ThreadNeedRefresh = true;
			}
			return;
		}
	}

	if ( LocationPos != preLen-1 && !AlwaysRecord && !PageActive)
	{

		// 不是最后一个，那么需要删除 LocationPos 之后的数据
		for (int i=LocationPos+1;i<preLen;i++)
		{
			// 移除最后的
			LocationList.pop_back();
		}
	}
	PageActive = false;
	//  相邻的不能重复 volatile
	long lastIndex = LocationList.size()-1;
	if ( lastIndex > 0 )
	{
		if ( LocationList[lastIndex].bufferID == (*tmp).bufferID &&
			//LocationList[lastIndex].position == (*tmp).position 
			abs(LocationList[lastIndex].position - (*tmp).position )< MaxOffset
			)
		{
			// 一样的不需要插入数据
			return;
		}
	}
	//TCHAR buffer[500]={0};
	//wsprintf(buffer,TEXT("%d"),lastIndex);
	//::MessageBox(nppData._nppHandle, buffer, TEXT(""), MB_OK);
	LocationList.push_back(*tmp);
	while ( LocationList.size()>MaxList )
	{
		LocationList.pop_front();
	}
	LocationPos = LocationList.size()-1;
}
void AddList(bool flag)
{
	if ( !bAutoRecord && !flag)
	{
		return;
	}
	long position=0;//,col=0;
	//line = ::SendMessage(handle, NPPM_GETCURRENTLINE, 0, 0);
	// Get the current scintilla
	//int which = -1;
	//::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	//if (which == -1)
	//	return;
	//HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

	position = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);

	// 如果页面没有变化，同时位置也没有变化，直接跳走 !bufferChanged &&
	if( PrePositionNumber == position )return;
	//col = ::SendMessage(handle, NPPM_GETCURRENTCOLUMN, 0, 0);	
	if(PositionSetting&&!flag){
		// 如果是设置的，会重复调用一次，设置位置为LocationPos中的位置
		PositionSetting = false;
		PrePositionNumber =  LocationList[LocationPos].position;
		return;
	}
	long len = LocationList.size();
	long prepos=0;
	if ( len==0 || LocationPos == -1|| LocationPos>len-1)
	{
		prepos = PrePositionNumber;
	}else{
		prepos = LocationList[LocationPos].position;
	}

	if ( abs(prepos-position) > MaxOffset || PrePositionNumber == -1 )
	{
		//int currentEdit;
		//TCHAR path[MAX_PATH];
		//::SendMessage(nppData._nppHandle, NPPM_GETFULLCURRENTPATH, 0, (LPARAM)path);
		//::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
		//EnterCriticalSection(&criCounter);
		//LocationInfo tmp;
		//tmp.position = position;
		//tmp.bufferID = currBufferID;
		bool tmpChanged;
		if ( haveDeleted )
		{
			//tmp.changed  = true;
			tmpChanged = true;
			haveDeleted = false;
		}else
		{
			//tmp.changed  = false;
			tmpChanged = false;
		}
		//lstrcpy(tmp.FilePath,currFile);
		ActionData actTmp;
		actTmp.type = ActionLocation;
		actTmp.position = position;
		actTmp.changed = tmpChanged;
		ActionDataList.push_back(actTmp);
		//AddListData(&tmp);
		//LeaveCriticalSection(&criCounter);
		//_LNhistory.refreshDlg();
	}
	PrePositionNumber=position;
	PositionSetting = false;
}
const int _MARK_BOOKMARK = 1 << 24;
void InitBookmark()
{
	if ( !NeedMark || isAutoModify )
	{
		return;
	}
	// 如果通过书签的，那么跳过
	if(ByBookMark == MarkBookmark)return;

	// 定义未保存的颜色标记
	HWND ScintillaArr[]={nppData._scintillaMainHandle,nppData._scintillaSecondHandle};
	// 需要将主副窗口改进
	for ( int i=0;i<2;i++)
	{
		HWND tmpScintilla = ScintillaArr[i];
		//nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
		::SendMessage(tmpScintilla,SCI_SETMARGINTYPEN,_MARK_INDEX,SC_MARGIN_SYMBOL);
		// 定义保存的颜色标记
		::SendMessage(tmpScintilla,SCI_SETMARGINTYPEN,_SAVE_INDEX,SC_MARGIN_SYMBOL);
		// 默认样式为高亮背景色，宽度为0
		int widthMargin = 0;
		int SC_MarkType = SC_MARK_BACKGROUND;
		if(   ByBookMark == MarkUnderLine )
		{
			widthMargin = 1; // 必须设置宽度，否则显示有BUG
		}else if(   ByBookMark == MarkRect )
		{
			widthMargin = 4;
		}else if(
			ByBookMark == MarkRoundrect
			||ByBookMark == MarkCircle
			||ByBookMark == MarkTriangle
			||ByBookMark == MarkArrow
			)
		{
			widthMargin = 12;
		}
		::SendMessage(tmpScintilla,SCI_SETMARGINWIDTHN,4, widthMargin);
		// 定义样式
		try
		{
			SC_MarkType = MarkTypeArr[ByBookMark];
		}
		catch (...)
		{
		}
		int OriMask = ::SendMessage( tmpScintilla, SCI_GETMARGINMASKN, 4, 0 );
		int markerMask1 = ( 1 << _MARK_INDEX );
		int markerMask2 = ( 1 << _SAVE_INDEX );
		int tmpMask = 0;
		tmpMask = OriMask|markerMask1|markerMask2;

		if ( ByBookMark > MarkUnderLine )
		{
			// 用的左边标记
			::SendMessage(tmpScintilla,SCI_MARKERSETFORE,_MARK_INDEX,MarkColor);// 标记的边框颜色和内容一致
			::SendMessage(tmpScintilla,SCI_MARKERSETFORE,_SAVE_INDEX,SaveColor);// 标记的边框颜色和内容一致
		}
		// set mask to exclude marker
		::SendMessage( tmpScintilla, SCI_SETMARGINMASKN, 4, tmpMask );

		::SendMessage(tmpScintilla,SCI_MARKERSETBACK,_MARK_INDEX,MarkColor);// 默认颜色DefaultColor
		::SendMessage(tmpScintilla,SCI_MARKERSETBACK,_SAVE_INDEX,SaveColor);// 默认颜色DefaultColor
		::SendMessage(tmpScintilla,SCI_MARKERDEFINE,_MARK_INDEX,SC_MarkType);
		::SendMessage(tmpScintilla,SCI_MARKERDEFINE,_SAVE_INDEX,SC_MarkType);

		// SC_MARK_FULLRECT,SC_MARK_UNDERLINE ,SC_MARK_CIRCLE,SC_MARK_ARROW, SC_MARK_SMALLRECT,
	}
}

void processNavActions() {
	while ( !ActionDataList.empty())
	{
		if ( !ThreadNeedRefresh )
		{
			ThreadNeedRefresh = true;
		}
		ActionData tmpAct = ActionDataList.front();
		switch( tmpAct.type ) {
			case ActionModify : {
				DoModify(tmpAct.length,tmpAct.position); // 修改
			} break;
			case ActionActive : {
				// 页面激活，将全局变量赋值
				currBufferID =  currTmpBufferID;
				lstrcpy(currFile,currTmpFile);
				// 需要更新全部 bufferID=0的值
				for (int i=0;i<LocationList.size();i++ ) {
					if ( LocationList[i].bufferID == 0 
						&& lstrcmp( LocationList[i].FilePath,currFile) == 0 ) {
						LocationList[i].bufferID = currBufferID;
					}
				}
				//::MessageBox(NULL, TEXT("ActionActive"), TEXT(""), MB_OK);

				InitBookmark();
			} break;
			case ActionClosed : {
				// 进行列表检查
				DoFilesCheck();
			} break;
			case ActionLocation : {
				// 进行定位
				LocationInfo tmp;
				tmp.position = tmpAct.position;
				tmp.bufferID = currBufferID;
				tmp.changed =  tmpAct.changed;
				//tmp.FilePath = new TCHAR[MAX_PATH];
				TCHAR * fileName;

				auto it = filename_table.find(currFile);
				if(it!=filename_table.end()) {
					fileName = *it;
				} else {
					fileName = new TCHAR[MAX_PATH];
					lstrcpy(fileName,currFile);
					filename_table.insert(fileName);
					//::MessageBox(NULL, TEXT("new data!"), TEXT(""), MB_OK);
				}
				tmp.FilePath = fileName;

				AddListData(&tmp);
			} break;
		}
		ActionDataList.pop_front();
	}
	if ( ThreadNeedRefresh)
	{
		_LNhistory.refreshDlg(1);
		ThreadNeedRefresh = false;
	}
}

DWORD   WINAPI   ThreadFunc(   LPVOID   lpParam   )  
{
	int cc=0;
	while (!AllCloseFlag)
	{
		cc++;
		processNavActions();


		bool val = (((int)(cc/10))%2)==0; //一闪一闪亮晶晶

		//EnableTBButton( menuNext,  val);

		//_LNhistory.setParent( nppData._nppHandle );

		//_LNhistory.display(val);
		//LocationNavigateHistoryDlg();
		//
		//
		//HWND hReBar;
		//
		//hReBar = FindWindowEx( nppData._nppHandle, hReBar, TEXT( "ReBarWindow32" ), NULL );
		//
		//hToolbar = FindWindowEx( hReBar, NULL, TEXT( "ToolbarWindow32" ), NULL );
		//
		//::SendMessage( hToolbar, TB_ENABLEBUTTON, ( WPARAM )funcItem[menuNext]._cmdID,  MAKELONG( val , 0 ) );

	}
	return   0;
}
void DoSavedColor()
{
	// 需要修改当前全部的Mark颜色
	if (NeedMark && !isOpenFile)
	{
		if ( ByBookMark==MarkBookmark )return;

		//::MessageBox(NULL, TEXT("DoSavedColor"), TEXT(""), MB_OK);

		for (int i=0;i< MarkHistory.size();i++)
		{
			if ( MarkHistory[i].BufferID == currTmpBufferID)
			{
				// 切换Handle
				int MarkLine = ::SendMessage(curScintilla, SCI_MARKERLINEFROMHANDLE, MarkHistory[i].markHandle, 0);
				MarkHistory[i].markHandle = ::SendMessage(curScintilla, SCI_MARKERADD, MarkLine, _SAVE_INDEX);
				// 移除Marker
				::SendMessage(curScintilla, SCI_MARKERDELETE, MarkLine, _MARK_INDEX);
				// 移走
				//MarkHistory.erase(MarkHistory.begin()+i);
				//i--;
			}
		}
		// 2011-12-08 add for 0.4.7.4
		int MarkLine = -1;
		int markerMask1 = ( 1 << _MARK_INDEX );
		// 有些marker可能未在队列中，统一更新
		while((MarkLine = ::SendMessage(curScintilla, SCI_MARKERNEXT, MarkLine+1, markerMask1)) !=-1)
		{
			//TCHAR tmp[200]={0};
			//wsprintf(tmp,TEXT("%d"),MarkLine);
			//MessageBox(NULL,tmp,NULL,MB_OK);
			::SendMessage(curScintilla, SCI_MARKERADD, MarkLine, _SAVE_INDEX);
		}
		::SendMessage(curScintilla, SCI_MARKERDELETEALL, _MARK_INDEX,0);
	}

}
int AddMarkFromLine(int line)
{
	int markHandle = -1;
	int state = (int)::SendMessage(curScintilla, SCI_MARKERGET, line, 0);
	if ( ByBookMark==MarkBookmark )
	{
		bool bookExist = (_MARK_BOOKMARK & state) != 0;
		if (bookExist)
			return markHandle;

		//int backLine = (int)::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0);
		//int pos = (int)::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
		::SendMessage(curScintilla, SCI_GOTOLINE, line, 0);
		::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_SEARCH_TOGGLE_BOOKMARK);
		markHandle = 0;
	}else{
		// 1<<_SAVE_INDEX 表明被标注为保存过
		if (state == STATE_SAVE )
		{
			::SendMessage(curScintilla, SCI_MARKERDELETE, line, _SAVE_INDEX);
		}else if (state !=0 )
			return markHandle;

		markHandle = ::SendMessage(curScintilla, SCI_MARKERADD, line, _MARK_INDEX);
	}
	return markHandle;
}
void SetBookmark(int lineNo,int pos,int linesAdded,int len)
{
	
	// 简单undo策略,将Line和Position放入列表，UNDO时去查找，如果curScintilla,Position和Line一致则删除
	int handle = AddMarkFromLine(lineNo);
	if ( handle == -1 && linesAdded == 0)
	{
		return;
	}
	if ( linesAdded > 0)
	{
		for ( int line = 1;line<=linesAdded;line++)
		{
			AddMarkFromLine(lineNo+line);
		}
	}
	// 获取本行的长度  SCI_GETlineLenITION
	int lineLen = -1;
	if ( linesAdded == 0 )
	{
		lineLen = (int)::SendMessage(curScintilla,SCI_LINELENGTH , lineNo, 0) - len;
	}
	// 插入数据
	MarkData tmp;
	tmp.position = pos;
	tmp.line = lineNo;
	tmp.lineAdd = linesAdded;
	tmp.BufferID = currTmpBufferID;
	tmp.markHandle = handle;
	tmp.lineLen = lineLen;
	MarkHistory.push_back(tmp);

}
bool RemoveMarkFromLine(int line)
{
	int state = (int)::SendMessage(curScintilla, SCI_MARKERGET, line, 0);
	if ( ByBookMark==MarkBookmark )
	{
		bool bookNotExist = (_MARK_BOOKMARK & state) == 0;
		if (bookNotExist)
		{
			return false;
		}
		//int backLine = (int)::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0);
		//int pos = (int)::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
		::SendMessage(curScintilla, SCI_GOTOLINE, line, 0);
		::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_SEARCH_TOGGLE_BOOKMARK);
	}else{
		// 必须要有Mark句柄
		if (state ==0 )
		{
			return false;
		}
		// 1<<_SAVE_INDEX
		if (state == STATE_SAVE )
		{
			::SendMessage(curScintilla, SCI_MARKERDELETE, line, _SAVE_INDEX);
		}else{
			::SendMessage(curScintilla, SCI_MARKERDELETE, line, _MARK_INDEX);
		}
	}
	return true;
}
void DelBookmark(int lineNo,int pos,int lineAdd)
{
	if ( ByBookMark!=MarkBookmark )
	{
		// 判断是不是结束了
		int canUndoFlag = ::SendMessage(curScintilla, SCI_CANUNDO, 0, 0);
		if ( 0 == canUndoFlag )
		{
			::SendMessage(curScintilla, SCI_MARKERDELETEALL, _MARK_INDEX,0);
			::SendMessage(curScintilla, SCI_MARKERDELETEALL, _SAVE_INDEX,0);
			MarkHistory.clear();
			return;
		}
	}
	int isMarkedIndex = -1;
	int lastIndex = MarkHistory.size()-1;
	int curSciCount = 0;
	int cur2Index[2]={-1,-1};
	for (int i=lastIndex;i>-1;i--)
	{
		if( MarkHistory[i].BufferID == currTmpBufferID )
		{
			curSciCount++;
			if (curSciCount ==1 // 必须是当前Scintilla最后一个
				)
			{
				cur2Index[0] = i;//  SCI_GETlineLenITION
				if (  MarkHistory[i].line == lineNo 
					&&MarkHistory[i].position == pos 
					)
				{
					// 	 如果是一行的改变，还需要加上本行的长度判断
					if( lineAdd == 0 )
					{
						int lineLen = (int)::SendMessage(curScintilla,SCI_LINELENGTH , lineNo, 0);
						if ( lineLen == MarkHistory[i].lineLen )
						{
							isMarkedIndex = i;
						}
					}else
					{
						isMarkedIndex = i;
					}
				}
			}
			if( lineAdd == 0 )break;
			// 如果改变多行，那么还需要再往前走一步
			if( curSciCount== 2 ){
				cur2Index[1] = i;
				break;
			}
		}
	}
	//可能需要修正前面的Mark 
	for (int i=0;i<2;i++ )
	{
		int tmpIndex = cur2Index[i];
		if ( -1 == tmpIndex)
		{
			break;
		}
		MarkData pretmp = MarkHistory[tmpIndex];
		int MarkLine = ::SendMessage(curScintilla, SCI_MARKERLINEFROMHANDLE, pretmp.markHandle, 0);
		if ( MarkLine != pretmp.line )
		{
			//去掉
			RemoveMarkFromLine(MarkLine);
			MarkHistory[tmpIndex].markHandle = AddMarkFromLine(pretmp.line);
		}
	}
	if ( -1 == isMarkedIndex)
	{
		// 没有找到标记的返回
		return;
	}
	int lineB=lineNo;
	int lineE=lineNo+lineAdd;
	if (lineAdd<0)
	{
		lineE = lineNo-lineAdd;
	}
	//bool hasRemoveMark = false;
	for (int line = lineB;line<=lineE;line++)
	{
		RemoveMarkFromLine(line);
	}
	MarkHistory.erase(MarkHistory.begin()+isMarkedIndex);
}

//////////  SELF FUNCTION END ////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reasonForCall, 
                       LPVOID lpReserved )
{
    switch (reasonForCall)
    {
      case DLL_PROCESS_ATTACH:
        pluginInit(hModule);
        break;

      case DLL_PROCESS_DETACH:
        pluginCleanUp();
        break;
      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}


extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	nppData = notpadPlusData;

	_LNhistory.init((HINSTANCE)_LNhistory.getHinst(), nppData._nppHandle);

	commandMenuInit();
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

typedef const TBBUTTON *LPCTBBUTTON;
static long preModifyPos = -1;//之前在的位置
static long preModifyLineAdd = -1;//之前添加的行数
extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	int ModifyType = notifyCode->modificationType;
	int code = notifyCode->nmhdr.code;

	switch (code) 
	{
		case NPPN_TBMODIFICATION:
			if(notifyCode->nmhdr.hwndFrom == nppData._nppHandle) {
				/* add toolbar icon */
				auto HRO = (HINSTANCE)g_hModule;

				long filecount2 = ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, (LPARAM)PRIMARY_VIEW);

				long version = ::SendMessage(nppData._nppHandle, NPPM_GETNOTMADVERSION, 0, 0);

				legacy = version<0x666;

				g_TBPreviousChg.HRO = HRO;
				if(legacy)g_TBPreviousChg.hToolbarBmp = (HBITMAP)::LoadImage(HRO, MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0,0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));
				::SendMessage(nppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[menuChgPrevious]._cmdID, (LPARAM)&g_TBPreviousChg);

				g_TBPrevious.HRO = HRO;
				if(legacy)g_TBPrevious.hToolbarBmp = (HBITMAP)::LoadImage(HRO, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0,0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));
				::SendMessage(nppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[menuPrevious]._cmdID, (LPARAM)&g_TBPrevious);

				g_TBNext.HRO = HRO;
				if(legacy)g_TBNext.hToolbarBmp = (HBITMAP)::LoadImage(HRO, MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));
				::SendMessage(nppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[menuNext]._cmdID, (LPARAM)&g_TBNext);

				g_TBNextChg.HRO = HRO;
				if(legacy)g_TBNextChg.hToolbarBmp = (HBITMAP)::LoadImage(HRO, MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 0, 0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));
				::SendMessage(nppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[menuChgNext]._cmdID, (LPARAM)&g_TBNextChg);

				/////初始化变量/////////////////////////////////////////////////////////////////////
				for (int i=0;i<menuCount;i++)
				{
					menuState[i] = true; // 默认都是有效的
					IconID[i] = -1;
				}
			}
		break;
		case NPPN_SHUTDOWN:
		{
			commandMenuCleanUp();
			//bIsPaused=1;
			//AllCloseFlag=1;
		}
		break;
		case NPPN_READY:
		{
			// 获取窗口 ReBarWindow32 ToolbarWindow32
			//nppData._nppHandle
			HWND hReBar = NULL;
			while(true){
				hReBar = FindWindowEx(nppData._nppHandle,hReBar,TEXT("ReBarWindow32"),NULL);
				if ( hReBar != NULL)
				{
					hToolbar = FindWindowEx(hReBar,NULL,TEXT("ToolbarWindow32"),NULL);
					if ( hToolbar != NULL)
					{
						//TB_SETIMAGELIST (LPARAM)&tButton
						TBBUTTON tButton;
						for ( int i=0;i<menuCount;i++)
						{
							// 根据按钮命令ID得到按钮的索引
							LRESULT lRetCode = SendMessage(hToolbar, TB_COMMANDTOINDEX, (WPARAM)funcItem[i]._cmdID,0 );
							if ( lRetCode !=NULL)
							{
								//SendMessage(hToolbar,WM_COMMAND,0,(LPARAM)lRetCode);
								// 根据按钮的索引获取按钮的内部ID值
								::SendMessage(hToolbar, TB_GETBUTTON, (WPARAM)lRetCode,(LPARAM) &tButton);
								IconID[i] = tButton.idCommand;
							}
						}
						break;
					}
				}else
				{
					break;
				}
			}
			// 先将按钮全部灰掉 默认都无效的
			for (int i=menuPrevious;i<=menuChgNext;i++)
			{
				EnableTBButton((menuList)i, false, true);
			}
			// 刷新菜单
			::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuInCurr]._cmdID, MF_BYCOMMAND | (InCurr?MF_CHECKED:MF_UNCHECKED));
			::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuSkipClosed]._cmdID, MF_BYCOMMAND | (skipClosed?MF_CHECKED:MF_UNCHECKED));
			::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuAlwaysRefresh]._cmdID, MF_BYCOMMAND | (AlwaysRefreshBtns?MF_CHECKED:MF_UNCHECKED));
			::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuPinMenu]._cmdID, MF_BYCOMMAND | (pinMenu?MF_CHECKED:MF_UNCHECKED));
			::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuAutoRecord]._cmdID, MF_BYCOMMAND | (bAutoRecord?MF_CHECKED:MF_UNCHECKED));
			::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuClearOnClose]._cmdID, MF_BYCOMMAND | (AutoClean?MF_CHECKED:MF_UNCHECKED));
			::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuPause]._cmdID, MF_BYCOMMAND | (bIsPaused?MF_CHECKED:MF_UNCHECKED));
			::EnableMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuManualRecord]._cmdID,MF_BYCOMMAND|(bAutoRecord?MF_GRAYED:MF_ENABLED ));
			

			::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuNeedMark]._cmdID, MF_BYCOMMAND | (NeedMark?MF_CHECKED:MF_UNCHECKED));
		////////////////////////////////
		// 判断 RecordContent 是否有值
			if( SaveRecord && 0)
			{
				bool haveErrorInIni= false;
				TCHAR iniContent[RecordConentMax]={0};
				::GetPrivateProfileString(sectionName, strRecordContent,TEXT(""), iniContent,RecordConentMax,iniFilePath);
				TCHAR str[300]={0};
				int len = sizeof(str);
				int j=0;
				LocationInfo tmp;
				//tmp.FilePath = new TCHAR[MAX_PATH];
				//::MessageBox(NULL,iniContent,TEXT(""),MB_OK);
				for(int i=0;i<RecordConentMax;i++)
				{
					if ( iniContent[i]== '\0')
					{
						break;
					}else if ( iniContent[i]== '<')
					{
						if ( str[0] == '1' )
						{
							tmp.changed = true;
						}else if ( str[0] == '0' )
						{
							tmp.changed = false;
						}else
						{
							// 异常错误
							haveErrorInIni = true;
							break;
						}
						memset(str,0,len);
						j=0;
						continue;
					}else if ( iniContent[i]== '>')
					{
						int pos = _tstoi(str);
						if ( pos == 0 )
						{
							if (  str[0]=='0'&& str[1]=='\0')
							{
								// 表明是 0 的值
							}else
							{
								// 异常错误
								haveErrorInIni = true;
								break;
							}
						}
						tmp.position = pos;
						memset(str,0,len);
						j=0;
						continue;
					}else if ( iniContent[i]== '|')
					{
						if ( str[0]== NEWFILE[0] 
						&& str[1]== NEWFILE[1] 
						&& str[2]== NEWFILE[2])
						{
							// 是以 new 开头的文件名
						}else if(GetFileAttributes(str)==0xFFFFFFFF ){
							// 判断文件是否存在，不存在不做任何操作
						}else
						{
							lstrcpy(tmp.FilePath,str);
							LocationList.push_back(tmp);
						}
						
						memset(str,0,len);
						j=0;
						continue;
					}else
					{
						str[j++]=iniContent[i];
					}
				}
				if ( haveErrorInIni )
				{
					ClearLocationList();
				}
				LocationPos = LocationList.size()-1;
			}
#if UseThread
			// 不能启用线程，会引起关闭时崩溃
			DWORD   dwThreadId,   dwThrdParam   =   1;
			HANDLE   hThread = ::CreateThread(
				NULL, //   no   security   attributes  
				0,//   use   default   stack   size    
				ThreadFunc,//   thread   function  
				&dwThrdParam,//   argument   to   thread   function  
				0,//   use   default   creation   flags  
				&dwThreadId);//   returns   the   thread   identifier
			CloseHandle(hThread);
#endif
			//InitBookmark();
			ready = true;
		}
		break;
		// mark setting remove to SCN_SAVEPOINTREACHED
		case NPPN_FILESAVED:
		{
			if(bIsPaused) break;
			int which = -1;
			::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
			if (which != -1){
				curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
			}
			currTmpBufferID = notifyCode->nmhdr.idFrom;
			::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, (WPARAM)notifyCode->nmhdr.idFrom, (LPARAM)currTmpFile);
			lstrcpy(currFile,currTmpFile);

			DoSavedColor();
		}
		break;
		case NPPN_FILEBEFORECLOSE:
		{
			if(bIsPaused) break;
			// 如果是关闭的，那么要进行一个数据备份，用于进行日志的保存
			// 由于无法判断是否是最终的全部关闭，之前采用比较丑陋的办法，需要设置一个时间，如果超过一定时间无效,启动一个线程
			// 通过分析，可以通过 modifiers	4924 判断
			if (  !AllCloseFlag && notifyCode->modifiers == 4924 )
			{
				AllCloseFlag = true;
				if( SaveRecord )
				{
					LocationSave.assign(LocationList.begin(),LocationList.end());
				}
			}
		}
		break;
		case NPPN_FILEBEFOREOPEN:
		case NPPN_FILEBEFORELOAD:
		{
			isOpenFile = true;
		}
		break;
		case NPPN_FILEOPENED:
		case NPPN_FILECLOSED:
		{
			if(bIsPaused) break;
			isOpenFile = false;
		// 页面打开或关闭
			if ( AutoClean )
			{
				if(!bIsPaused && NPPN_FILECLOSED == code) {
					// 只需要告知需要进行文件列表的检查
					ActionData tmp;
					tmp.type = ActionClosed;
					ActionDataList.push_back(tmp);
				}
				
				// 第一次默认文件打开时会在原文件基础上打开
				if ( code == NPPN_FILEOPENED && LocationList.size()>0)
				{
					if ( LocationList[0].position == 0 && lstrcmp(LocationList[0].FilePath,NEWFILE)==0 )
					{
						TCHAR  **buffer;
						long filecount2 = ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, (LPARAM)PRIMARY_VIEW);
						buffer = new TCHAR*[filecount2];
						for (int i=0;i<filecount2;i++)
						{
							buffer[i] = new TCHAR[MAX_PATH];
						}
						::SendMessage(nppData._nppHandle, NPPM_GETOPENFILENAMESPRIMARY , (WPARAM)buffer, (LPARAM)filecount2);
						// 如果 new  1 不在第一个视图中,那么就清除
						bool haveNew= false;
						for ( int ifile=0;ifile<filecount2;ifile++)
						{
							if( lstrcmp( buffer[ifile], NEWFILE) ==0 )
							{
								haveNew = true;
								break;
							}
						}
						if ( !haveNew)
						{
							LocationList.pop_front();
						}
						for (int i=0;i<filecount2;i++)
						{
							delete []buffer[i];
						}
						delete []buffer;
					}
				}
				//LeaveCriticalSection(&criCounter);
			}
		}
		break;
		// 页面切换
		case NPPN_BUFFERACTIVATED:
		{
			PrevSelStart=PrevSelStart=-1;
			//if(bIsPaused) break;
			//scnNotification->nmhdr.code = NPPN_BUFFERACTIVATED;
			//scnNotification->nmhdr.hwndFrom = hwndNpp;
			//scnNotification->nmhdr.idFrom = activatedBufferID;
			//wsprintf(buffer,TEXT("%d,"),notifyCode->nmhdr.idFrom);
			// Get the current scintilla
			int which = -1;
			::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
			if (which != -1){
				curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
			}


			bool NewDocAct = currTmpBufferID!=notifyCode->nmhdr.idFrom;
			
			if(!BufferIdBeforeClick) {
				currTmpBufferID = notifyCode->nmhdr.idFrom;
				::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, (WPARAM)notifyCode->nmhdr.idFrom, (LPARAM)currTmpFile);
				ActionData tmp;
				tmp.type = ActionActive;
				ActionDataList.push_back(tmp);
			}

			// 最终将修改 currBufferID currFile
			if(ready){
				// 最后被触发的
				//bufferChanged = true;
				if(!PositionSetting){
					PageActive = true;
					PrePositionNumber = -1;
					AddList();
				}else
				{
					// 是跳转时设置的
					PageActive = false;
				}
			}
		}
		break;
		case SCN_UPDATEUI:
		{
			if (ready)
			{
				if ( notifyCode->nmhdr.hwndFrom != nppData._scintillaMainHandle &&
					notifyCode->nmhdr.hwndFrom != nppData._scintillaSecondHandle)break;
				AddList();
			}
		}
		break;
		case SCN_MODIFIED:
		{
			//if(bIsPaused) break;
			// 以下事件是多行执行时的信息，如果多行改动会触发，但是效率较低
			//if(ModifyType == 0x11 || ModifyType == 0x12	// 多行改动如多行缩进等
			//|| ModifyType == 0xA1	|| ModifyType == 0xA2 // 多行撤销
			//|| ModifyType == 0xC1	|| ModifyType == 0xC2 // 多行重复
			//)
			//{
			////	break;
			//}
			// 正在打开文档
			//TCHAR tmp[100]={0};
			//wsprintf(tmp, TEXT("0x%X\n"),ModifyType);
			//OutputDebugString(tmp);

			if ( isOpenFile )
			{
				break;
			}
			if ( notifyCode->nmhdr.hwndFrom != nppData._scintillaMainHandle &&
				notifyCode->nmhdr.hwndFrom != nppData._scintillaSecondHandle) 
			{
				break;
			}
			if(LocationList.size()==0)
			{
				// 没有数据
				break;
			}
			long pos,len;
			bool flag=false;
			// SC_MOD_BEFOREINSERT SC_MOD_INSERTTEXT SC_MOD_BEFOREDELETE SC_MOD_DELETETEXT 
			if(ModifyType & SC_MOD_INSERTTEXT)
			{
				pos = notifyCode->position;
				len = notifyCode->length;
				flag = true;
			//}else if( ModifyType &  ){
			}else if(ModifyType & SC_MOD_DELETETEXT)
			{
				pos = notifyCode->position;
				len = -notifyCode->length;
				flag = true;
			}
			
			if(flag)
			{
				if( notifyCode->text == NULL && ( ModifyType == 0x12 || ModifyType == 0x11 ))
				{
					// 此处为不记录内容的，或为自动执行
					break;
				}
				if ( 0 == pos )
				{
					// maybe search in files, checked it
					// Get the current scintilla
					HWND tmpcurScintilla=(HWND)-1;
					int which = -1;
					::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
					if (which != -1){
						tmpcurScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
					}
					if ( -1 == (int)tmpcurScintilla)
					{
						// search in files
						break;
					}
					//  SC_PERFORMED_USER| SC_MOD_DELETETEXT
					// 简单的0x12和0x11一般是系统自动执行或替换
					// && (ModifyType == 0x12 || ModifyType == 0x11)
					if ( notifyCode->text == NULL && 1024 == notifyCode->length )
					{
						// Open New file // 新打开文件触发 || ModifyType == 0x2011
						//isOpenFile = true;
						break;
					}
				}
				// 提交到队列
				ActionData tmp;
				tmp.type = ActionModify;
				tmp.position = pos;
				tmp.length = len;
				ActionDataList.push_back(tmp);
				if (NeedMark)
				{
					// 需要判断是否是系统自动执行的，如果是系统自动执行，那么将不进行Mark
					isAutoModify = (::SendMessage(curScintilla,SCI_GETUNDOCOLLECTION, 0, 0)==0);
					if (!isAutoModify)
					{
						// 需要自动打上书签，根据POS找到line调用
						//int line = ::SendMessage(nppData._nppHandle,NPPM_GETCURRENTLINE, 0, 0);
						// 如果是多个执行的，如果位置一样，只要执行最后一个
						if ( ModifyType &  SC_PERFORMED_UNDO )
						{
							int line = -1;
							// 同一位置出现重复设置时，如果影响的行数为0，那么采用上一次的设置
							if( notifyCode->linesAdded==0 && preModifyPos != pos && preModifyPos !=-1)
							{
								line = ::SendMessage(curScintilla,SCI_LINEFROMPOSITION, preModifyPos, 0);
								DelBookmark(line,preModifyPos,preModifyLineAdd);
							}
							// 如果影响的行数不为0，或为最后一次，无论如何均执行
							if ( notifyCode->linesAdded!=0||(ModifyType & SC_LASTSTEPINUNDOREDO))
							{
								line = ::SendMessage(curScintilla,SCI_LINEFROMPOSITION, pos, 0);
								DelBookmark(line,pos,notifyCode->linesAdded);
								preModifyPos = -1;
							}else{
								preModifyPos = pos;
								preModifyLineAdd = notifyCode->linesAdded;
							}
						}else
						{
							// SC_PERFORMED_REDO
							int line = ::SendMessage(curScintilla,SCI_LINEFROMPOSITION, notifyCode->position , 0);
							SetBookmark(line,notifyCode->position,notifyCode->linesAdded,len);
						}
					}
				}
			}

#if 0
			TCHAR buffer[128];
			wsprintf(buffer,TEXT("ModifyType:%d ; Pointer:%d"), ModifyType, notifyCode);
			::MessageBox(NULL, buffer, TEXT(""), MB_OK);
#endif

		}
		break;
		case SCN_SAVEPOINTREACHED:
		{
			//DoSavedColor();
		}
		break;
		default:
		break;
	}
#if not UseThread
	processNavActions();
#endif
}


// Here you can process the Npp Messages 
// I will make the messages accessible little by little, according to the need of plugin development.
// Please let me know if you need to access to some messages :
// http://sourceforge.net/forum/forum.php?forum_id=482781
//

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	// only support WM_SIZE and  WM_MOVE
	return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
#endif //UNICODE
