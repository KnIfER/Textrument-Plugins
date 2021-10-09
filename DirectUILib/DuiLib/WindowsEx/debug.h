#pragma once

#include "../Core/InsituDebug.h"
#include <string>

/***********************************************************************
* COMCTL32_IsReflectedMessage [internal]
*
* Some parents reflect notify messages - for some messages sent by the child,
* they send it back with the message code increased by OCM__BASE (0x2000).
* This allows better subclassing of controls. We don't need to handle such
* messages but we don't want to print ERRs for them, so this helper function
* identifies them.
*
* Some of the codes are in the CCM_FIRST..CCM_LAST range, but there is no
* collision with defined CCM_ codes.
*/

#ifndef OCM__BASE      /* avoid including olectl.h */
#define OCM__BASE (WM_USER+0x1c00)
#endif

static int tab = 1;

inline int TRACE_ON(int type)
{
    return 1;
}


inline BOOL COMCTL32_IsReflectedMessage(UINT uMsg)
{
    switch (uMsg)
    {
    case OCM__BASE + WM_COMMAND:
    case OCM__BASE + WM_CTLCOLORBTN:
    case OCM__BASE + WM_CTLCOLOREDIT:
    case OCM__BASE + WM_CTLCOLORDLG:
    case OCM__BASE + WM_CTLCOLORLISTBOX:
    case OCM__BASE + WM_CTLCOLORMSGBOX:
    case OCM__BASE + WM_CTLCOLORSCROLLBAR:
    case OCM__BASE + WM_CTLCOLORSTATIC:
    case OCM__BASE + WM_DRAWITEM:
    case OCM__BASE + WM_MEASUREITEM:
    case OCM__BASE + WM_DELETEITEM:
    case OCM__BASE + WM_VKEYTOITEM:
    case OCM__BASE + WM_CHARTOITEM:
    case OCM__BASE + WM_COMPAREITEM:
    case OCM__BASE + WM_HSCROLL:
    case OCM__BASE + WM_VSCROLL:
    case OCM__BASE + WM_PARENTNOTIFY:
    case OCM__BASE + WM_NOTIFY:
        return TRUE;
    default:
        return FALSE;
    }
}

#define debugstr_w  
#define debugstr_a  

inline const std::wstring debugstr_wn(const WCHAR * msg, UINT ln){
    return std::wstring(msg, ln);
}

inline void nothing(const CHAR* msg, ...){}

#define TRACE(MSG,...) LogIs(3,MSG,__VA_ARGS__) 
//#define TRACE nothing
//#define TRACE(MSG,...) /##/## MSG

#define FIXME TRACE
#define WARN TRACE
#define ERR TRACE

inline int Free(void* ptr) {
  //  free(ptr);
    return 0;
};

static CHAR buffer[512];


#define wine_dbgstr_rect(rect) _wine_dbgstr_rect(rect).c_str()

inline const std::string _wine_dbgstr_rect(RECT* rect) {
    if (rect)
    {
        sprintf_s(buffer, 255, "%ld %ld %ld %ld", rect->left, rect->top, rect->right, rect->bottom);
        return buffer;
    }
    return "[/]";
};

inline const CHAR* wine_dbgstr_point(POINT* pt) {
    //CHAR buffer[512];
    if (pt)
    {
        //CHAR* buffer = malloc(512);
        sprintf_s(buffer, 255, "%ld %ld", pt->x, pt->y);
        return buffer;
    }
    return "[/]";
};

inline void* Alloc(size_t sz) {
    return malloc(sz);
};