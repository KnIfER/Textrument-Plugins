#pragma once

#include "PluginInterface.h"
#include "NppDarkMode.h"
#include "InsituDebug.h"


#define nppData g_NppData


_declspec(selectany)  NppData g_NppData;

_declspec(selectany)  bool legacy;


_declspec(selectany)  HINSTANCE			g_hInstance = NULL;
CONST INT	nbFunc	= 4;
_declspec(selectany)  FuncItem			funcItem[nbFunc];
_declspec(selectany)  WNDPROC				g_NppWndProc;




