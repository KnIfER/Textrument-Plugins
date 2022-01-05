//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
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
#include "utility.h"
#include "./DockingFeature/resource.h"

extern FuncItem funcItem[nbFunc];
extern NppData nppData;


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
		commandMenuCleanUp();
        pluginCleanUp();
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}


HWND mainAppWnd;

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	nppData = notpadPlusData;
	mainAppWnd = nppData._nppHandle;
	commandMenuInit();
	loadOption(nppData._nppHandle, struOptions);
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


extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	int code = notifyCode->nmhdr.code;
	if ((notifyCode->nmhdr.hwndFrom == nppData._nppHandle) && 
		(code == NPPN_TBMODIFICATION))
	{
		/* add toolbar icon */
		auto HRO = (HINSTANCE) _hInst;

		long filecount2 = ::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, (LPARAM)PRIMARY_VIEW);

		long version = ::SendMessage(nppData._nppHandle, NPPM_GETNOTMADVERSION, 0, 0);

		bool legacy = version<0x666;

		static toolbarIcons TBEXEC{0,0,0x666,HRO,IDI_ICON_EXEC,0,0,IDB_BITMAP_EXEC};

		if(legacy)TBEXEC.hToolbarBmp = (HBITMAP)::LoadImage(HRO, MAKEINTRESOURCE(IDB_BITMAP_EXEC), IMAGE_BITMAP, 0,0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));
		
		::SendMessage(nppData._nppHandle, NPPM_ADDTOOLBARICON, (WPARAM)funcItem[1]._cmdID, (LPARAM)&TBEXEC);
	}
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
#endif
