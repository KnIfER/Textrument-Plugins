// SciTE - Scintilla based Text Editor
/** @file SciTEWin__NOPE__.h
 ** Header of main code for the Windows version of the editor.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SCITEWIN_H
#define SCITEWIN_H

#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include <tuple>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <mutex>

#include <fcntl.h>

#include <sys/stat.h>

#ifdef __MINGW_H
#define _WIN32_IE	0x0400
#endif

#undef _WIN32_WINNT
#undef WINVER
#ifdef WIN_TARGET
#define _WIN32_WINNT WIN_TARGET
#define WINVER WIN_TARGET
#else
#define _WIN32_WINNT  0x0501
#define WINVER 0x0501
#endif

//#undef NOMINMAX
//#define NOMINMAX 1

//#include <GdiPlus.h>
#include "..\DuiLib\UIlib.h"
using namespace DuiLib;

#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <windowsx.h>
#if defined(DISABLE_THEMES)
// Old compilers do not have Uxtheme.h
typedef void *HTHEME;
#else
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#define THEME_AVAILABLE
#endif
#include <shlwapi.h>
// need this header for SHBrowseForFolder
#include <shlobj.h>


#include "ILoader.h"
#include "ILexer.h"

#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ScintillaCall.h"

#include "Scintilla.h"
#include "Lexilla.h"
#include "LexillaAccess.h"

#include "GUI.h"
//#include "ScintillaWindow.h"

#include "StringList.h"
#include "StringHelpers.h"
#include "FilePath.h"
#include "StyleDefinition.h"
#include "PropSetFile.h"
#include "StyleWriter.h"
#include "Extender.h"
#include "SciTE.h"
#include "JobQueue.h"
#include "Cookie.h"
#include "Worker.h"
#include "FileWorker.h"
//#include "MatchMarker.h"
#include "SciTEBase.h"
//#include "UniqueInstance.h"
#include "StripDefinition.h"
#include "Strips.h"
#include "SciTEKeys.h"

constexpr int SCITE_TRAY = WM_APP + 0;
constexpr int SCITE_DROP = WM_APP + 1;
constexpr int SCITE_WORKER = WM_APP + 2;
constexpr int SCITE_SHOWOUTPUT = WM_APP + 3;

enum {
	WORK_EXECUTE = WORK_PLATFORM + 1
};

//class SciEditText;

class CommandWorker : public Worker {
public:
	SciEditText *pSciTE;
	size_t icmd;
	Scintilla::Position originalEnd;
	int exitStatus;
	GUI::ElapsedTime commandTime;
	std::string output;
	int flags;
	bool seenOutput;
	int outputScroll;

	CommandWorker() noexcept;
	void Initialise(bool resetToStart) noexcept;
	void Execute() override;
};

class Dialog;

class ContentWin : public BaseWin {
	SciEditText *pSciTEWin;
	bool capturedMouse;
public:
	ContentWin() noexcept : pSciTEWin(nullptr), capturedMouse(false) {
	}
	void SetSciEdit(SciEditText *pSciTEWin_) noexcept {
		pSciTEWin = pSciTEWin_;
	}
	void Paint(HDC hDC, GUI::Rectangle rcPaint);
	LRESULT WndProc(UINT iMessage, WPARAM wParam, LPARAM lParam) override;
};

struct Band {
	bool visible;
	int height;
	bool expands;
	GUI::Window win;
	Band(bool visible_, int height_, bool expands_, GUI::Window win_) noexcept :
		visible(visible_),
		height(height_),
		expands(expands_),
		win(win_) {
	}
};

inline bool IsKeyDown(int key) noexcept {
	return (::GetKeyState(key) & 0x80000000) != 0;
}

GUI::Point PointOfCursor() noexcept;
GUI::Point ClientFromScreen(HWND hWnd, GUI::Point ptScreen) noexcept;

// Common minor conversions

constexpr GUI::Point PointFromLong(LPARAM lPoint) noexcept {
	// static_cast<short> needed for negative coordinates
	return GUI::Point(static_cast<short>(LOWORD(lPoint)), static_cast<short>(HIWORD(lPoint)));
}

constexpr int ControlIDOfWParam(WPARAM wParam) noexcept {
	return wParam & 0xffff;
}

inline HWND HwndOf(GUI::Window w) noexcept {
	return static_cast<HWND>(w.GetID());
}

inline HMENU HmenuID(size_t id) noexcept {
	return reinterpret_cast<HMENU>(id);
}

inline POINT *PointPointer(GUI::Point *pt) noexcept {
	return reinterpret_cast<POINT *>(pt);
}

#endif
