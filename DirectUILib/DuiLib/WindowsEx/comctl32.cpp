#include <windows.h>
#include "comctl32.h"

namespace WindowsEx
{
void ReadColors()
{
	comctl32_color.clrBtnHighlight     =  GetSysColor(COLOR_BTNHIGHLIGHT);
	comctl32_color.clrBtnShadow        =  GetSysColor(COLOR_BTNSHADOW);
	comctl32_color.clrBtnText          =  GetSysColor(COLOR_BTNTEXT);
	comctl32_color.clrBtnFace          =  GetSysColor(COLOR_BTNFACE);
	comctl32_color.clrHighlight        =  GetSysColor(COLOR_HIGHLIGHT);
	comctl32_color.clrHighlightText    =  GetSysColor(COLOR_HIGHLIGHTTEXT);
	//comctl32_color.clrHotTrackingColor =  GetSysColor(COLOR_BTNFACE);
	comctl32_color.clr3dHilight        =  GetSysColor(COLOR_3DHILIGHT);
	comctl32_color.clr3dShadow         =  GetSysColor(COLOR_3DSHADOW);
	comctl32_color.clr3dDkShadow       =  GetSysColor(COLOR_3DDKSHADOW);
	comctl32_color.clr3dFace           =  GetSysColor(COLOR_3DFACE);
	comctl32_color.clrWindow           =  GetSysColor(COLOR_WINDOW);
	comctl32_color.clrWindowText       =  GetSysColor(COLOR_WINDOWTEXT);
	comctl32_color.clrGrayText         =  GetSysColor(COLOR_GRAYTEXT);
	comctl32_color.clrActiveCaption    =  GetSysColor(COLOR_ACTIVECAPTION);
	comctl32_color.clrInfoBk           =  GetSysColor(COLOR_INFOBK);
	comctl32_color.clrInfoText         =  GetSysColor(COLOR_INFOTEXT);

	if (true)
	{
		comctl32_color.hBrushBtnFace=GetSysColorBrush(COLOR_BTNFACE);
	}
}

UINT DpiForWindow(HWND hWnd) 
{
	if (fnGetDpiForWindow) {
		return fnGetDpiForWindow(hWnd);
	}
	if (fnGetDpiForMonitor) {
		HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		UINT dpiX = 0;
		UINT dpiY = 0;
		if (fnGetDpiForMonitor(hMonitor, 0 /*MDT_EFFECTIVE_DPI*/, &dpiX, &dpiY) == S_OK) {
			return dpiY;
		}
	}
	return uSystemDPI;
}

/// Find a function in a DLL and convert to a function pointer.
/// This avoids undefined and conditionally defined behaviour.
template<typename T>
T DLLFunction(HMODULE hModule, LPCSTR lpProcName) noexcept {
	if (!hModule) {
		return nullptr;
	}
	FARPROC function = ::GetProcAddress(hModule, lpProcName);
	static_assert(sizeof(T) == sizeof(function));
	T fp {};
	memcpy(&fp, &function, sizeof(T));
	return fp;
}

//  ..\Scintilla\scintilla\win32\PlatWin.cxx
void LoadDpiForWindow() noexcept {
	HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
	fnGetDpiForWindow = DLLFunction<GetDpiForWindowSig>(user32, "GetDpiForWindow");
	fnGetSystemMetricsForDpi = DLLFunction<GetSystemMetricsForDpiSig>(user32, "GetSystemMetricsForDpi");
	fnAdjustWindowRectExForDpi = DLLFunction<AdjustWindowRectExForDpiSig>(user32, "AdjustWindowRectExForDpi");

	using GetDpiForSystemSig = UINT(WINAPI *)(void);
	GetDpiForSystemSig fnGetDpiForSystem = DLLFunction<GetDpiForSystemSig>(user32, "GetDpiForSystem");
	if (fnGetDpiForSystem) {
		uSystemDPI = fnGetDpiForSystem();
	} else {
		HDC hdcMeasure = ::CreateCompatibleDC({});
		uSystemDPI = ::GetDeviceCaps(hdcMeasure, LOGPIXELSY);
		::DeleteDC(hdcMeasure);
	}

	if (!fnGetDpiForWindow) {
		hDLLShcore = ::LoadLibraryExW(L"shcore.dll", {}, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hDLLShcore) {
			fnGetDpiForMonitor = DLLFunction<GetDpiForMonitorSig>(hDLLShcore, "GetDpiForMonitor");
		}
	}
}
}

void InitWindowsEx(HINSTANCE hInst)
{
	if (!WindowsExInitialized)
	{
		if (!hInst) hInst = GetModuleHandle(NULL);
		COMCTL32_hModule = hInst;
		WindowsExInitialized = true;
		LoadDpiForWindow();
		ReadColors();
	}
}

