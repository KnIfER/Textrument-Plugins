// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#include "resource.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>
#include <limits>

#define _CRT_SECURE_NO_DEPRECATE
// TODO: 在此处引用程序需要的其他头文件

#include "..\DuiLib\UIlib.h"

#ifdef _VC2008
#ifdef _DEBUG
#ifdef _LIB
#pragma comment(lib, "../../Common/Static_Debug_2008/UiLib_d.lib")
#else
#pragma comment(lib, "../../Common/DLL_Debug_2008/UiLib_d.lib")
#endif
#else
#ifdef _LIB
#pragma comment(lib, "../../Common/Static_Release_2008/UiLib.lib")
#else
#pragma comment(lib, "../../Common/DLL_Release_2008/UiLib.lib")
#endif
#endif
#endif

#ifdef _VC201X
#ifdef _DEBUG
#ifdef _LIB
#pragma comment(lib, "../../Common/Static_Debug_201X/UiLib_d.lib")
#else
#pragma comment(lib, "../../Common/DLL_Debug_201X/UiLib_d.lib")
#endif
#else
#ifdef _LIB
#pragma comment(lib, "../../Common/Static_Release_201X/UiLib.lib")
#else
#pragma comment(lib, "../../Common/DLL_Release_201X/UiLib.lib")
#endif
#endif
#endif