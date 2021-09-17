// SciTE - Scintilla based Text Editor
/** @file SciTEWin__NOPE__.cxx
 ** Main code for the Windows version of the editor.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <ctime>

#include "WinSciTE.h"
#include "DLLFunction.h"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#ifndef NO_EXTENSIONS
#include "MultiplexExtension.h"

#ifndef NO_FILER
#include "DirectorExtension.h"
#endif

#ifndef NO_LUA
#include "LuaExtension.h"
#endif

#endif

#ifdef STATIC_BUILD
const GUI::gui_char appName[] = GUI_TEXT("Sc1");
#else
const GUI::gui_char appName[] = GUI_TEXT("SciTE");
static const GUI::gui_char scintillaName[] = GUI_TEXT("Scintilla.DLL");
#endif

static GUI::gui_string GetErrorMessage(DWORD nRet) {
	LPWSTR lpMsgBuf = nullptr;
	if (::FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				nRet,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   // Default language
				reinterpret_cast<LPWSTR>(&lpMsgBuf),
				0,
				nullptr
			) != 0) {
		GUI::gui_string s= lpMsgBuf;
		::LocalFree(lpMsgBuf);
		return s;
	} else {
		return TEXT("");
	}
}

long SciTEKeys::ParseKeyCode(const char *mnemonic) {
	SA::KeyMod modsInKey = static_cast<SA::KeyMod>(0);
	int keyval = -1;

	if (mnemonic && *mnemonic) {
		std::string sKey = mnemonic;

		if (RemoveStringOnce(sKey, "Ctrl+"))
			modsInKey = modsInKey | SA::KeyMod::Ctrl;
		if (RemoveStringOnce(sKey, "Shift+"))
			modsInKey = modsInKey | SA::KeyMod::Shift;
		if (RemoveStringOnce(sKey, "Alt+"))
			modsInKey = modsInKey | SA::KeyMod::Alt;

		if (sKey.length() == 1) {
			keyval = VkKeyScan(sKey.at(0)) & 0xFF;
		} else if (sKey.length() > 1) {
			if ((sKey.at(0) == 'F') && (IsADigit(sKey.at(1)))) {
				sKey.erase(0, 1);
				const int fkeyNum = atoi(sKey.c_str());
				if (fkeyNum >= 1 && fkeyNum <= 12)
					keyval = fkeyNum - 1 + VK_F1;
			} else if ((sKey.at(0) == 'V') && (IsADigit(sKey.at(1)))) {
				sKey.erase(0, 1);
				const int vkey = atoi(sKey.c_str());
				if (vkey > 0 && vkey <= 0x7FFF)
					keyval = vkey;
			} else if (StartsWith(sKey, "Keypad")) {
				sKey.erase(0, strlen("Keypad"));
				if ((sKey.length() > 0) && IsADigit(sKey.at(0))) {
					const int keyNum = atoi(sKey.c_str());
					if (keyNum >= 0 && keyNum <= 9)
						keyval = keyNum + VK_NUMPAD0;
				} else if (sKey == "Plus") {
					keyval = VK_ADD;
				} else if (sKey == "Minus") {
					keyval = VK_SUBTRACT;
				} else if (sKey == "Decimal") {
					keyval = VK_DECIMAL;
				} else if (sKey == "Divide") {
					keyval = VK_DIVIDE;
				} else if (sKey == "Multiply") {
					keyval = VK_MULTIPLY;
				}
			} else if (sKey == "Left") {
				keyval = VK_LEFT;
			} else if (sKey == "Right") {
				keyval = VK_RIGHT;
			} else if (sKey == "Up") {
				keyval = VK_UP;
			} else if (sKey == "Down") {
				keyval = VK_DOWN;
			} else if (sKey == "Insert") {
				keyval = VK_INSERT;
			} else if (sKey == "End") {
				keyval = VK_END;
			} else if (sKey == "Home") {
				keyval = VK_HOME;
			} else if (sKey == "Enter") {
				keyval = VK_RETURN;
			} else if (sKey == "Space") {
				keyval = VK_SPACE;
			} else if (sKey == "Tab") {
				keyval = VK_TAB;
			} else if (sKey == "Escape") {
				keyval = VK_ESCAPE;
			} else if (sKey == "Delete") {
				keyval = VK_DELETE;
			} else if (sKey == "PageUp") {
				keyval = VK_PRIOR;
			} else if (sKey == "PageDown") {
				keyval = VK_NEXT;
			} else if (sKey == "Win") {
				keyval = VK_LWIN;
			} else if (sKey == "Menu") {
				keyval = VK_APPS;
			} else if (sKey == "Backward") {
				keyval = VK_BROWSER_BACK;
			} else if (sKey == "Forward") {
				keyval = VK_BROWSER_FORWARD;
			}
		}
	}

	return (keyval > 0) ? (keyval | (static_cast<int>(modsInKey)<<16)) : 0;
}

bool SciTEKeys::MatchKeyCode(long parsedKeyCode, int keyval, int modifiers) noexcept {
	return parsedKeyCode && !(0xFFFF0000 & (keyval | modifiers)) && (parsedKeyCode == (keyval | (modifiers<<16)));
}

namespace {

using SystemParametersInfoForDpiSig = BOOL (WINAPI *)(
	UINT  uiAction,
	UINT  uiParam,
	PVOID pvParam,
	UINT  fWinIni,
	UINT  dpi
);

SystemParametersInfoForDpiSig fnSystemParametersInfoForDpi;

// Using VerifyVersionInfo on Windows 10 will pretend its Windows 8
// but that is good enough for switching UI elements to flatter.
// The VersionHelpers.h functions can't be used as they aren't supported by GCC.

bool UIShouldBeFlat() noexcept {
	OSVERSIONINFOEX osvi = OSVERSIONINFOEX();
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 2;

	constexpr BYTE op = VER_GREATER_EQUAL;
	DWORDLONG dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);

	return VerifyVersionInfo(
		       &osvi,
		       VER_MAJORVERSION | VER_MINORVERSION |
		       VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		       dwlConditionMask);
}

}

static int CodePageFromName(const std::string &encodingName) {
	struct Encoding {
		const char *name;
		int codePage;
	} knownEncodings[] = {
		{ "ascii", CP_UTF8 },
		{ "utf-8", CP_UTF8 },
		{ "latin1", 1252 },
		{ "latin2", 28592 },
		{ "big5", 950 },
		{ "gbk", 936 },
		{ "shift_jis", 932 },
		{ "euc-kr", 949 },
		{ "cyrillic", 1251 },
		{ "iso-8859-5", 28595 },
		{ "iso8859-11", 874 },
		{ "1250", 1250 },
		{ "windows-1251", 1251 },
	};
	for (const Encoding &enc : knownEncodings) {
		if (encodingName == enc.name) {
			return enc.codePage;
		}
	}
	return CP_UTF8;
}

static std::string StringEncode(std::wstring s, int codePage) {
	if (s.length()) {
		const int sLength = static_cast<int>(s.length());
		const int cchMulti = ::WideCharToMultiByte(codePage, 0, s.c_str(), sLength, nullptr, 0, nullptr, nullptr);
		std::string sMulti(cchMulti, 0);
		::WideCharToMultiByte(codePage, 0, s.c_str(), sLength, &sMulti[0], cchMulti, nullptr, nullptr);
		return sMulti;
	} else {
		return std::string();
	}
}

static std::wstring StringDecode(std::string s, int codePage) {
	if (s.length()) {
		const int sLength = static_cast<int>(s.length());
		const int cchWide = ::MultiByteToWideChar(codePage, 0, s.c_str(), sLength, nullptr, 0);
		std::wstring sWide(cchWide, 0);
		::MultiByteToWideChar(codePage, 0, s.c_str(), sLength, &sWide[0], cchWide);
		return sWide;
	} else {
		return std::wstring();
	}
}

// Convert to UTF-8
static std::string ConvertEncoding(const char *original, int codePage) {
	if (codePage == CP_UTF8) {
		return original;
	} else {
		GUI::gui_string sWide = StringDecode(std::string(original), codePage);
		return GUI::UTF8FromString(sWide);
	}
}


// Allow UTF-8 file names and command lines to be used in calls to io.open and io.popen in Lua scripts.
// The scite_lua_win.h header redirects fopen and _popen to these functions.

extern "C" {

	FILE *scite_lua_fopen(const char *filename, const char *mode) {
		GUI::gui_string sFilename = GUI::StringFromUTF8(filename);
		GUI::gui_string sMode = GUI::StringFromUTF8(mode);
		FILE *f = _wfopen(sFilename.c_str(), sMode.c_str());
		if (!f)
			// Fallback on narrow string in case already in CP_ACP
			f = fopen(filename, mode);
		return f;
	}

	FILE *scite_lua_popen(const char *filename, const char *mode) {
		GUI::gui_string sFilename = GUI::StringFromUTF8(filename);
		GUI::gui_string sMode = GUI::StringFromUTF8(mode);
		FILE *f = _wpopen(sFilename.c_str(), sMode.c_str());
		if (!f)
			// Fallback on narrow string in case already in CP_ACP
			f = _popen(filename, mode);
		return f;
	}

}

static FilePath GetSciTEPath(const FilePath &home) {
	if (home.IsSet()) {
		return FilePath(home);
	} else {
		GUI::gui_char path[MAX_PATH];
		if (::GetModuleFileNameW(0, path, static_cast<DWORD>(std::size(path))) == 0)
			return FilePath();
		// Remove the SciTE.exe
		GUI::gui_char *lastSlash = wcsrchr(path, pathSepChar);
		if (lastSlash)
			*lastSlash = '\0';
		return FilePath(path);
	}
}

// HH_AKLINK not in mingw headers
struct XHH_AKLINK {
	long cbStruct;
	BOOL fReserved;
	const wchar_t *pszKeywords;
	wchar_t *pszUrl;
	wchar_t *pszMsgText;
	wchar_t *pszMsgTitle;
	wchar_t *pszWindow;
	BOOL fIndexOnFail;
};

// from ScintillaWin.cxx
static UINT CodePageFromCharSet(SA::CharacterSet characterSet, UINT documentCodePage) noexcept {
	CHARSETINFO ci {};
	const BOOL bci = ::TranslateCharsetInfo(reinterpret_cast<DWORD *>(static_cast<uintptr_t>(characterSet)),
						&ci, TCI_SRCCHARSET);

	UINT cp = (bci) ? ci.ciACP : documentCodePage;

	CPINFO cpi {};
	if (!::IsValidCodePage(cp) && !::GetCPInfo(cp, &cpi))
		cp = CP_ACP;

	return cp;
}


CommandWorker::CommandWorker() noexcept : pSciTE(nullptr) {
	Initialise(true);
}

void CommandWorker::Initialise(bool resetToStart) noexcept {
	if (resetToStart)
		icmd = 0;
	originalEnd = 0;
	exitStatus = 0;
	flags = 0;
	seenOutput = false;
	outputScroll = 1;
}

void CommandWorker::Execute() {
	//pSciTE->ProcessExecute();
}

static constexpr bool IsSpaceOrTab(GUI::gui_char ch) noexcept {
	return (ch == ' ') || (ch == '\t');
}

/**
 * Draw the split bar.
 */
void ContentWin::Paint(HDC hDC, GUI::Rectangle) {
	const GUI::Rectangle rcInternal = GetClientPosition();

	const int heightClient = rcInternal.Height();
	const int widthClient = rcInternal.Width();

	const int heightEditor = heightClient - 100 - 10;
	const int yBorder = heightEditor;
	const int xBorder = widthClient - 100 - 10;
	for (int i = 0; i < 10; i++) {
		int colourIndex = COLOR_3DFACE;
		if (0) {
			if (i == 0 || i == 10 - 1)
				colourIndex = COLOR_3DFACE;
			else
				colourIndex = COLOR_WINDOW;
		} else {
			if (i == 1)
				colourIndex = COLOR_3DHIGHLIGHT;
			else if (i == 10 - 2)
				colourIndex = COLOR_3DSHADOW;
			else if (i == 10 - 1)
				colourIndex = COLOR_3DDKSHADOW;
			else
				colourIndex = COLOR_3DFACE;
		}
		HPEN pen = ::CreatePen(0, 1, ::GetSysColor(colourIndex));
		HPEN penOld = SelectPen(hDC, pen);
		if (1) {
			::MoveToEx(hDC, xBorder + i, 0, nullptr);
			::LineTo(hDC, xBorder + i, heightClient);
		} else {
			::MoveToEx(hDC, 0, yBorder + i, nullptr);
			::LineTo(hDC, widthClient, yBorder + i);
		}
		SelectPen(hDC, penOld);
		DeletePen(pen);
	}
}

inline bool KeyMatch(const std::string &sKey, int keyval, int modifiers) {
	return SciTEKeys::MatchKeyCode(
		       SciTEKeys::ParseKeyCode(sKey.c_str()), keyval, modifiers);
}

static void RestrictDLLPath() noexcept {
	// Try to limit the locations where DLLs will be loaded from to prevent binary planting.
	// That is where a bad DLL is placed in the current directory or in the PATH.
	typedef BOOL(WINAPI *SetDefaultDllDirectoriesSig)(DWORD DirectoryFlags);
	typedef BOOL(WINAPI *SetDllDirectorySig)(LPCTSTR lpPathName);
	HMODULE kernel32 = ::GetModuleHandle(TEXT("kernel32.dll"));
	if (kernel32) {
		// SetDefaultDllDirectories is stronger, limiting search path to just the application and
		// system directories but is only available on Windows 8+
		SetDefaultDllDirectoriesSig SetDefaultDllDirectoriesFn =
			reinterpret_cast<SetDefaultDllDirectoriesSig>(::GetProcAddress(
						kernel32, "SetDefaultDllDirectories"));
		if (SetDefaultDllDirectoriesFn) {
			SetDefaultDllDirectoriesFn(LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
		} else {
			SetDllDirectorySig SetDllDirectoryFn =
				reinterpret_cast<SetDllDirectorySig>(::GetProcAddress(
							kernel32, "SetDllDirectoryW"));
			if (SetDllDirectoryFn) {
				// For security, remove current directory from the DLL search path
				SetDllDirectoryFn(TEXT(""));
			}
		}
	}
}

#ifdef STATIC_BUILD
extern "C" Scintilla::ILexer5 * __stdcall CreateLexer(const char *name);
#endif

#if defined(_MSC_VER) && defined(_PREFAST_)
// Stop warning for WinMain. Microsoft headers have annotations and MinGW don't.
#pragma warning(disable: 28251)
#endif

int WINAPI WinMainX(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	RestrictDLLPath();

#ifndef NO_EXTENSIONS
	MultiplexExtension multiExtender;

#ifndef NO_LUA
	multiExtender.RegisterExtension(LuaExtension::Instance());
#endif

#ifndef NO_FILER
	multiExtender.RegisterExtension(DirectorExtension::Instance());
#endif
#endif

	//SciTEWin__NOPE__::Register(hInstance);
	Lexilla::SetDefaultDirectory(GetSciTEPath(FilePath()).AsUTF8());
#ifdef STATIC_BUILD
	Scintilla_RegisterClasses(hInstance);
	Lexilla::SetDefault(CreateLexer);
#else

	HMODULE hmod = ::LoadLibrary(scintillaName);
	if (!hmod) {
		GUI::gui_string explanation = scintillaName;
		explanation += TEXT(" could not be loaded.  SciTE will now close");
		::MessageBox(NULL, explanation.c_str(),
			     TEXT("Error loading Scintilla"), MB_OK | MB_ICONERROR);
	}
#endif

	uintptr_t result = 0;
	try {
#ifdef NO_EXTENSIONS
		Extension *extender = 0;
#else
		Extension *extender = &multiExtender;
#endif
		//SciTEWin__NOPE__ MainWind(extender);
		LPTSTR lptszCmdLine = GetCommandLine();
		if (*lptszCmdLine == '\"') {
			lptszCmdLine++;
			while (*lptszCmdLine && (*lptszCmdLine != '\"'))
				lptszCmdLine++;
			if (*lptszCmdLine == '\"')
				lptszCmdLine++;
		} else {
			while (*lptszCmdLine && (*lptszCmdLine != ' '))
				lptszCmdLine++;
		}
		//while (*lptszCmdLine == ' ')
		//	lptszCmdLine++;
		//try {
		//	MainWind.Run(lptszCmdLine);
		//	// Set the current directory always to the program directory
		//	// to ensure a opened file directory is never locked.
		//	GetSciTEPath(FilePath()).SetWorkingDirectory();
		//	result = MainWind.EventLoop();
		//} catch (const SA::Failure &sf) {
		//	MainWind.CheckForScintillaFailure(sf.status);
		//} catch (const std::bad_alloc &) {
		//	::MessageBox({}, TEXT("Allocation failure"), TEXT("Failure in SciTE"), MB_OK | MB_ICONERROR | MB_APPLMODAL);
		//}
		//MainWind.Finalise();
	} catch (std::bad_array_new_length &) {
		::MessageBox({}, TEXT("Allocation failure"), TEXT("Failure to allocate SciTE at start up"), MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}

#ifdef STATIC_BUILD
	Scintilla_ReleaseResources();
#else

	::FreeLibrary(hmod);
#endif

	return static_cast<int>(result);
}