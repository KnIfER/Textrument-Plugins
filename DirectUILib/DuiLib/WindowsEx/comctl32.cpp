#include <windows.h>
#include "comctl32.h"

namespace WindowsEx
{
struct sysparam_rgb_entry
{
    //struct sysparam_entry hdr;
    COLORREF              val;
    HBRUSH                brush;
    HPEN                  pen;
};

static struct sysparam_rgb_entry system_colors[] =
{
#define RGB_ENTRY(name,val,reg) { (val) }
    RGB_ENTRY( COLOR_SCROLLBAR, RGB(212, 208, 200), L"Scrollbar" ),
    RGB_ENTRY( COLOR_BACKGROUND, RGB(58, 110, 165), L"Background" ),
    RGB_ENTRY( COLOR_ACTIVECAPTION, RGB(10, 36, 106), L"ActiveTitle" ),
    RGB_ENTRY( COLOR_INACTIVECAPTION, RGB(128, 128, 128), L"InactiveTitle" ),
    RGB_ENTRY( COLOR_MENU, RGB(212, 208, 200), L"Menu" ),
    RGB_ENTRY( COLOR_WINDOW, RGB(255, 255, 255), L"Window" ),
    RGB_ENTRY( COLOR_WINDOWFRAME, RGB(0, 0, 0), L"WindowFrame" ),
    RGB_ENTRY( COLOR_MENUTEXT, RGB(0, 0, 0), L"MenuText" ),
    RGB_ENTRY( COLOR_WINDOWTEXT, RGB(0, 0, 0), L"WindowText" ),
    RGB_ENTRY( COLOR_CAPTIONTEXT, RGB(255, 255, 255), L"TitleText" ),
    RGB_ENTRY( COLOR_ACTIVEBORDER, RGB(212, 208, 200), L"ActiveBorder" ),
    RGB_ENTRY( COLOR_INACTIVEBORDER, RGB(212, 208, 200), L"InactiveBorder" ),
    RGB_ENTRY( COLOR_APPWORKSPACE, RGB(128, 128, 128), L"AppWorkSpace" ),
    RGB_ENTRY( COLOR_HIGHLIGHT, RGB(10, 36, 106), L"Hilight" ),
    RGB_ENTRY( COLOR_HIGHLIGHTTEXT, RGB(255, 255, 255), L"HilightText" ),
    RGB_ENTRY( COLOR_BTNFACE, RGB(212, 208, 200), L"ButtonFace" ),
    RGB_ENTRY( COLOR_BTNSHADOW, RGB(128, 128, 128), L"ButtonShadow" ),
    RGB_ENTRY( COLOR_GRAYTEXT, RGB(128, 128, 128), L"GrayText" ),
    RGB_ENTRY( COLOR_BTNTEXT, RGB(0, 0, 0), L"ButtonText" ),
    RGB_ENTRY( COLOR_INACTIVECAPTIONTEXT, RGB(212, 208, 200), L"InactiveTitleText" ),
    RGB_ENTRY( COLOR_BTNHIGHLIGHT, RGB(255, 255, 255), L"ButtonHilight" ),
    RGB_ENTRY( COLOR_3DDKSHADOW, RGB(64, 64, 64), L"ButtonDkShadow" ),
    RGB_ENTRY( COLOR_3DLIGHT, RGB(212, 208, 200), L"ButtonLight" ),
    RGB_ENTRY( COLOR_INFOTEXT, RGB(0, 0, 0), L"InfoText" ),
    RGB_ENTRY( COLOR_INFOBK, RGB(255, 255, 225), L"InfoWindow" ),
    RGB_ENTRY( COLOR_ALTERNATEBTNFACE, RGB(181, 181, 181), L"ButtonAlternateFace" ),
    RGB_ENTRY( COLOR_HOTLIGHT, RGB(0, 0, 200), L"HotTrackingColor" ),
    RGB_ENTRY( COLOR_GRADIENTACTIVECAPTION, RGB(166, 202, 240), L"GradientActiveTitle" ),
    RGB_ENTRY( COLOR_GRADIENTINACTIVECAPTION, RGB(192, 192, 192), L"GradientInactiveTitle" ),
    RGB_ENTRY( COLOR_MENUHILIGHT, RGB(10, 36, 106), L"MenuHilight" ),
    RGB_ENTRY( COLOR_MENUBAR, RGB(212, 208, 200), L"MenuBar" )
#undef RGB_ENTRY
};

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

/***********************************************************************
*           DEFWND_ControlColor
*
* Default colors for control painting.
*/
HBRUSH DEFWND_ControlColor( HDC hDC, UINT ctlType )
{
	if( ctlType == CTLCOLOR_SCROLLBAR)
	{
		HBRUSH hb = GetSysColorBrush(COLOR_SCROLLBAR);
		COLORREF bk = GetSysColor(COLOR_3DHILIGHT);
		SetTextColor( hDC, GetSysColor(COLOR_3DFACE));
		SetBkColor( hDC, bk);

		/* if COLOR_WINDOW happens to be the same as COLOR_3DHILIGHT
		* we better use 0x55aa bitmap brush to make scrollbar's background
		* look different from the window background.
		*/
		if (bk == GetSysColor(COLOR_WINDOW))
		{
			//return SYSCOLOR_Get55AABrush();
			// https://reactos.org/archives/public/ros-dev/2011-May/014227.html
			static const WORD wPattern55AA[] =
			{
				0x5555, 0xaaaa, 0x5555, 0xaaaa,
				0x5555, 0xaaaa, 0x5555, 0xaaaa
			};
			static HBITMAP hPattern55AABitmap = NULL;
			static HBRUSH hPattern55AABrush = NULL;
			if (hPattern55AABrush == NULL)
			{
				hPattern55AABitmap = CreateBitmap(8, 8, 1, 1, wPattern55AA);
				hPattern55AABrush = CreatePatternBrush(hPattern55AABitmap);
			}
			return hPattern55AABrush;
		}

		UnrealizeObject( hb );
		return hb;
	}

	SetTextColor( hDC, GetSysColor(COLOR_WINDOWTEXT));

	if ((ctlType == CTLCOLOR_EDIT) || (ctlType == CTLCOLOR_LISTBOX))
		SetBkColor( hDC, GetSysColor(COLOR_WINDOW) );
	else {
		SetBkColor( hDC, GetSysColor(COLOR_3DFACE) );
		return GetSysColorBrush(COLOR_3DFACE);
	}
	return GetSysColorBrush(COLOR_WINDOW);
}

/***********************************************************************
*		SYSCOLOR_GetPen
*/
HPEN SYSCOLOR_GetPen( INT index )
{
	/* We can assert here, because this function is internal to Wine */
	//assert (0 <= index && index < ARRAY_SIZE( system_colors ));

	if (!system_colors[index].pen)
	{
		HPEN pen = CreatePen( PS_SOLID, 1, GetSysColor( index ));
		//__wine_make_gdi_object_system( pen, TRUE );
		if (InterlockedCompareExchangePointer( (void **)&system_colors[index].pen, pen, 0 ))
		{
			//__wine_make_gdi_object_system( pen, FALSE );
			DeleteObject( pen );
		}
	}
	return system_colors[index].pen;
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

HTHEME ThemeForDpi(HWND hWnd, LPCWSTR pszClassList) 
{
	if (fnOpenThemeDataForDpi) {
		return fnOpenThemeDataForDpi(hWnd, pszClassList, DpiForWindow(hWnd));
	}
	return OpenThemeData(NULL, pszClassList);
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

	HMODULE uxtheme = ::GetModuleHandleW(L"uxtheme.dll");
	fnOpenThemeDataForDpi = DLLFunction<OpenThemeDataForDpiSig>(uxtheme, "OpenThemeDataForDpi");
}


/***********************************************************************
*           GdiGetCharDimensions    (GDI32.@)
*
* Gets the average width of the characters in the English alphabet.
*
* PARAMS
*  hdc    [I] Handle to the device context to measure on.
*  lptm   [O] Pointer to memory to store the text metrics into.
*  height [O] On exit, the maximum height of characters in the English alphabet.
*
* RETURNS
*  The average width of characters in the English alphabet.
*
* NOTES
*  This function is used by the dialog manager to get the size of a dialog
*  unit. It should also be used by other pieces of code that need to know
*  the size of a dialog unit in logical units without having access to the
*  window handle of the dialog.
*  Windows caches the font metrics from this function, but we don't and
*  there doesn't appear to be an immediate advantage to do so.
*
* SEE ALSO
*  GetTextExtentPointW, GetTextMetricsW, MapDialogRect.
*/
LONG WINAPI GdiGetCharDimensions(HDC hdc, LPTEXTMETRICW lptm, LONG *height)
{
	SIZE sz;
	static const WCHAR alphabet[] = {
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q',
		'r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H',
		'I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',0};

	if(lptm && !GetTextMetricsW(hdc, lptm)) return 0;

	if(!GetTextExtentPointW(hdc, alphabet, 52, &sz)) return 0;

	if (height) *height = sz.cy;
	return (sz.cx / 26 + 1) / 2;
}

/***********************************************************************
*           GdiGetCodePage   (GDI32.@)
*/
DWORD WINAPI GdiGetCodePage( HDC hdc )
{
	UINT cp = CP_ACP;
	//DC *dc = get_dc_ptr( hdc );
	//
	//if (dc)
	//{
	//	cp = dc->font_code_page;
	//	release_dc_ptr( dc );
	//}
	return cp;
}

/*************************************************************
*           GetCharWidthInfo    (GDI32.@)
*
*/

BOOL WINAPI GetCharWidthInfo(HDC hdc, struct char_width_info *info)
{
	//PHYSDEV dev;
	//BOOL ret;
	//DC *dc;
	//
	//dc = get_dc_ptr(hdc);
	//if (!dc) return FALSE;
	//dev = GET_DC_PHYSDEV( dc, pGetCharWidthInfo );
	//ret = dev->funcs->pGetCharWidthInfo( dev, info );
	//
	//if (ret)
	//{
	//	//info->lsb = width_to_LP( dc, info->lsb );
	//	//info->rsb = width_to_LP( dc, info->rsb );
	//}
	//release_dc_ptr(dc);
	return false;
}

// https://github.com/wine-mirror/wine/blob/e909986e6ea5ecd49b2b847f321ad89b2ae4f6f1/dlls/gdi32/region.c
// https://github.com/wine-mirror/wine/blob/e909986e6ea5ecd49b2b847f321ad89b2ae4f6f1/dlls/gdi32/clipping.c
// CombineRgn & GetClipRgn
static HRGN _hrgnEraser = CreateRectRgn( 0, 0, 0, 0 );
static HRGN _hrgn = CreateRectRgn( 0, 0, 0, 0 );

HRGN set_control_clipping( HDC hdc, const RECT *rect )
{
	RECT rc = *rect;
	HRGN hrgn = _hrgn;
	if (GetClipRgn( hdc, hrgn ) != 1) hrgn = 0;
	DPtoLP( hdc, (POINT *)&rc, 2 );
	if (GetLayout( hdc ) & LAYOUT_RTL)  /* compensate for the shifting done by IntersectClipRect */
	{
		rc.left++;
		rc.right++;
	}
	IntersectClipRect( hdc, rc.left, rc.top, rc.right, rc.bottom );
	return hrgn;
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