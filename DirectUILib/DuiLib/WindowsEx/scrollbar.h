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


void WINAPI UXTHEME_ScrollBarDraw(HWND hwnd, HDC dc, INT bar, enum SCROLL_HITTEST hit_test,
    BOOL hit_is_push, 
    //const struct SCROLL_TRACKING_INFO *tracking_info,
    BOOL draw_arrows, BOOL draw_interior, RECT *rect, INT arrowsize,
    INT thumbpos, INT thumbsize, BOOL vertical);