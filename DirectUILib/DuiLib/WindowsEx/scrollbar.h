#pragma once

/* Minimum size of the thumb in pixels */
#define SCROLL_MIN_THUMB 6

/* Minimum size of the rectangle between the arrows */
#define SCROLL_MIN_RECT  4

enum SCROLL_HITTEST
{
    SCROLL_NOWHERE,      /* Outside the scroll bar */
    SCROLL_TOP_ARROW,    /* Top or left arrow */
    SCROLL_TOP_RECT,     /* Rectangle between the top arrow and the thumb */
    SCROLL_THUMB,        /* Thumb rectangle */
    SCROLL_BOTTOM_RECT,  /* Rectangle between the thumb and the bottom arrow */
    SCROLL_BOTTOM_ARROW  /* Bottom or right arrow */
};


/* data for a single scroll bar */
typedef struct
{
    INT   curVal;   /* Current scroll-bar value */
    INT   minVal;   /* Minimum scroll-bar value */
    INT   maxVal;   /* Maximum scroll-bar value */
    INT   page;     /* Page size of scroll bar (Win32) */
    UINT  flags;    /* EnableScrollBar flags */
} SCROLLBAR_INFO, *LPSCROLLBAR_INFO;


void WINAPI UXTHEME_ScrollBarDraw(HWND hwnd, HDC dc, INT bar, enum SCROLL_HITTEST hit_test,
    BOOL hit_is_push, 
    //const struct SCROLL_TRACKING_INFO *tracking_info,
    BOOL draw_arrows, BOOL draw_interior, RECT *rect, INT arrowsize,
    INT thumbpos, INT thumbsize, BOOL vertical);

void WINAPI USER_ScrollBarDraw( UINT  infoPtr_flags, HWND hwnd, HDC hdc, INT nBar
    , enum SCROLL_HITTEST hit_test, BOOL hit_is_push, 
    //const struct SCROLL_TRACKING_INFO *tracking_info, 
    BOOL arrows,
    BOOL interior, RECT *rect, INT arrowSize, INT thumbPos,
    INT thumbSize, BOOL vertical );