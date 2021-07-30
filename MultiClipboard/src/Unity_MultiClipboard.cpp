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

/*
Unity build notes:
This file #includes all header and cpp files together into one compilation unit
for faster compilation time. Note that while it improves compile time when compilation
is required for many .cpp files, it may actually worsen compile time when a single
small cpp file compile is needed, due to the fact that it always compile all cpp files

In this file, there is a #define option to include all headers in all source and
header files are only once in this file, but not in their respective header/source
file. This is actually a bit unnecessary because modern compilers should be able
to cache header files during compilation, and hence there should be no difference
in compilation time. Just trying it out. It does get rid of some compile warnings though
*/

#define UNITY_BUILD_MULTICLIPBOARD
#define UNITY_BUILD_SINGLE_INCLUDE

#ifdef UNITY_BUILD_MULTICLIPBOARD

#define WINVER 0x0500
#define _WIN32_IE 0x0501
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRA_LEAN

// Windows headers
#include <windows.h>
#include <ShellAPI.h>
#include <windowsx.h>


#ifdef UNITY_BUILD_SINGLE_INCLUDE

// C++ standard library headers
#include <algorithm>
#include <iterator>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>

// Tiny XML headers
#include "../TinyXML/tinyxml.h"

// McOptionsManager header
#include "McOptionsManager.h"

// Notepad++ plugin interface headers
#include "Scintilla.h"
#include "../../PowerEditor/src/menuCmdID.h"
#include "PluginInterface.h"

// NativeLang headers
#include "NativeLang_def.h"

// NppPlugins\Common headers
#include "DockingDlgInterface.h"
#include "SciSubClassWrp.h"
#include "StaticDialog.h"
#include "SysMsg.h"
	// Get rid of ugly compiler warning in SysMsg.h/cpp:
	//   "The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name"
	#ifdef generic_stricmp
		#undef generic_stricmp
		#ifdef UNICODE
			#define generic_stricmp _wcsicmp
		#else
			#define generic_stricmp _stricmp
		#endif
	#endif
#include "ToolBar.h"
#include "URLCtrl.h"
#include "Window.h"

// MultiClipboard plugin headers
#include "resource.h"
#include "stdafx.h"
#include "MultiClipboardProxy.h"
#include "MCSubClassWndProc.h"

// AboutDlg headers
#include "AboutDialog.h"

// ClipboardSettings headers
#include "McOptions.h"

// ClipboardControls headers
#include "CBContextMenu.h"
#include "CBEditbox.h"
#include "MultiClipboardListbox.h"
#include "SplitterPanel.h"
#include "ToolbarPanel.h"

// ClipboardMVC headers
#include "ModelViewController.h"
#include "ArraysOfClips.h"
#include "MultiClipCyclicPaste.h"
#include "MultiClipOLEDragDrop.h"
#include "MultiClipPasteMenu.h"
#include "MultiClipViewerDialog.h"
#include "OSClipboardController.h"
#include "SelectedTextAutoCopier.h"

// ClipboardSettings headers
#include "OptionsDlg.h"

#endif	// UNITY_BUILD_SINGLE_INCLUDE


// AboutDlg
#include "AboutDialog.cpp"

// ClipboardControls
#include "CBContextMenu.cpp"
#include "CBEditbox.cpp"
#include "MultiClipboardListbox.cpp"
#include "SplitterPanel.cpp"
#include "ToolbarPanel.cpp"

// ClipboardMVC
#include "ClipboardList.cpp"
#include "ModelViewController.cpp"
#include "MultiClipCyclicPaste.cpp"
#include "MultiClipOLEDragDrop.cpp"
#include "MultiClipPasteMenu.cpp"
#include "MultiClipViewerDialog.cpp"
#include "OSClipboardController.cpp"
#include "SelectedTextAutoCopier.cpp"

// ClipboardSettings
#include "McOptionsManager.cpp"
#include "OptionsDlg.cpp"

// Common
#include "ImageListSet.cpp"
#include "StaticDialog.cpp"
#include "stdafx.cpp"
#include "SysMsg.cpp"
#include "ToolBar.cpp"
#include "URLCtrl.cpp"

#include "MCSubClassWndProc.cpp"
#include "MultiClipboard.cpp"
#include "MultiClipboardProxy.cpp"

#endif	// UNITY_BUILD_MULTICLIPBOARD