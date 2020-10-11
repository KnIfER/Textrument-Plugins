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
#include "MDTextDlg.h"
#include "Scintilla.h"


MarkDownTextDlg _MDText;

extern int BufferIdBeforeClick;

extern bool WindowOpaqueMsg;

bool pinMenu = false;

long MarkColor = DefaultColor;
long SaveColor = DefaultSaveColor;

#ifdef UNICODE 
	#define generic_itoa _itow
#else
	#define generic_itoa itoa
#endif

void pluginCleanUp()
{
	_MDText.saveParameters();
}


void commandMenuCleanUp()
{
	for(FuncItem fI:funcItems){
		delete fI._pShKey;
	}
	funcItems.clear();
}

//----------------------------------------------//
//-- ASSOCIATED FUNCTIONS START------------------//
//----------------------------------------------//

void PreviewCurrentFile() {
}

void ToggleMDPanel()
{
	_MDText.setParent( nppData._nppHandle );
	if ( !_MDText.isCreated() )
	{
		tTbData data = {0};
		WindowOpaqueMsg = 0;
		_MDText.create( &data );
		WindowOpaqueMsg = 1;
		// define the default docking behaviour
		data.uMask          = DWS_DF_CONT_RIGHT | DWS_ICONTAB;
		data.pszModuleName = _MDText.getPluginFileName();
		// the dlgDlg should be the index of funcItem where the current function pointer is
		data.dlgID = menuOption;
		data.hIconTab       = ( HICON )::LoadImage( _MDText.getHinst(),
			MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 14, 14,
			LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT );
		::SendMessage( nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, ( LPARAM )&data );
		_MDText.display();
	}
	else {
		bool NeedShowDlg = !_MDText.isVisible();
		_MDText.display(NeedShowDlg);
	}
	if(!_MDText.isClosed()) {
		//_MDText.RefreshWebview();
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

	lstrcpy(funcItems[index]._itemName, cmdName);
	funcItems[index]._pFunc = pFunc;
	funcItems[index]._init2Check = check0nInit;
	funcItems[index]._pShKey = sk;

	return true;
}

void BoldenText()
{
	int padLen=2;
	char* padStart="**";
	char* padEnd="**";
	int currentEdit=0;
	SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
	auto currentSci = currentEdit?nppData._scintillaSecondHandle:nppData._scintillaMainHandle;

	unsigned p1_ = SendMessage(currentSci, SCI_GETSELECTIONSTART, 0, 0);
	unsigned p2_ = SendMessage(currentSci, SCI_GETSELECTIONEND, 0, 0);
	bool hasSel=p2_>p1_;
	if(!hasSel && !legacy)
	{
		SendMessage(nppData._nppHandle, NPPM_SELCARET, 0, 0);
		p1_ = SendMessage(currentSci, SCI_GETSELECTIONSTART, 0, 0);
		p2_ = SendMessage(currentSci, SCI_GETSELECTIONEND, 0, 0);
		hasSel=p2_>p1_;
	}
	if(hasSel)
	{
		unsigned sellen = SendMessage(currentSci, SCI_GETSELTEXT, 0, 0)-1;
		unsigned textLen = SendMessage(currentSci, SCI_GETTEXTLENGTH, 0, 0);
		unsigned idealSTL = sellen;

		int p1 = p1_-padLen;
		unsigned p2 = p2_+padLen;
		unsigned bufferLen=sellen+padLen*2+2;
		char *txUCS2 = new char[bufferLen];
		if(txUCS2)
		{
			memset(txUCS2, 0, bufferLen);
			auto writeAddr=txUCS2;
			if(p1<0) 
			{
				writeAddr-=p1;
				idealSTL+=p1;
				p1=0;
			}
			if(p2>=textLen) 
			{
				idealSTL+=textLen-1-p2;
				p2=textLen-1;
			}
			//size_t len = SendMessage(currentSci, SCI_GETSELTEXT, 0, (LPARAM)(txUCS2+padLen))-1;
			Sci_TextRange tr{0};
			tr.chrg.cpMin = p1;
			tr.chrg.cpMax = p2;
			tr.lpstrText = writeAddr;

			size_t len = SendMessage(currentSci, SCI_GETTEXTRANGE, 0, (LPARAM)&tr)-1;

			if(len<=sellen+padLen*2)
			{
				memcpy(txUCS2, padStart, padLen);
				len=padLen+sellen;
				memcpy(txUCS2+len, padEnd, padLen);

				len+=padLen;
				SendMessage(currentSci, SCI_SETTARGETSTART, p1_, 0);
				SendMessage(currentSci, SCI_SETTARGETEND, p2_, 0);
				SendMessage(currentSci, SCI_REPLACETARGET, len, (LPARAM)txUCS2);

			}
		}
	}
}

void commandMenuInit()
{
	// Initialization of your plugin commands
	// Firstly we get the parameters from your plugin config file (if any)
	// get path of plugin configuration
	_MDText.readParameters();
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
	ShortcutKey *AutoKey = new ShortcutKey{1,0,1,VK_F9};
	ShortcutKey *ManualKey = new ShortcutKey{0,0,0,VK_F9};
	ShortcutKey *ClearRecordsKey = new ShortcutKey{1,1,1,VK_F9};
	ShortcutKey *incurrKey = new ShortcutKey{0,1,0,VK_OEM_MINUS};
	ShortcutKey *markKey = new ShortcutKey{1,1,0,0x4D};// VK_M

	funcItems.resize(nbFunc);

	ShortcutKey* shortKey = new ShortcutKey{0,0,0,NULL};
	funcItems[menuPreviewCurr]={TEXT("Preview Current File"), PreviewCurrentFile, menuPreviewCurr, false, shortKey};

	shortKey = new ShortcutKey{1,0,1,0x4D}; // VK_M
	funcItems[menuOption]={TEXT("Markdown Text Panel"), ToggleMDPanel, menuOption, false, shortKey};

	setCommand(menuSeparator0, TEXT("-SEPARATOR-"),NULL, NULL, false);

	shortKey = new ShortcutKey{0,0,0,NULL};
	funcItems[menuBolden]={TEXT("Bolden"), BoldenText, menuBolden, false, shortKey};

	shortKey = new ShortcutKey{0,0,0,NULL};
	funcItems[menuItalic]={TEXT("Italic"), ToggleMDPanel, menuItalic, false, shortKey};

	shortKey = new ShortcutKey{0,0,0,NULL};
	funcItems[menuUnderLine]={TEXT("Underline"), ToggleMDPanel, menuUnderLine, false, shortKey};
	

	setCommand(menuSeparator2, TEXT("-SEPARATOR-"),NULL, NULL, false);

	shortKey = new ShortcutKey{0,0,0,NULL};
	funcItems[menuPause]={TEXT("Pause Update"), ToggleMDPanel, menuPause, false, shortKey};

	shortKey = new ShortcutKey{0,0,0,NULL};
	funcItems[menuSettings]={TEXT("Settings…"), ToggleMDPanel, menuSettings, false, shortKey};

	// pause update
	// pause update

}