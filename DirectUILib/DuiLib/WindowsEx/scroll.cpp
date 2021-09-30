/*
 * Scrollbar control
 *
 * Copyright 1993 Martin Ayotte
 * Copyright 1994, 1996 Alexandre Julliard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#include "windows.h"
#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
//#include "controls.h"
//#include "win.h"
#include "debug.h"
#include "comctl32.h"
#include "scrollbar.h"
//#include "user_private.h"

//WINE_DEFAULT_DEBUG_CHANNEL(scroll);

/* data for window that has (one or two) scroll bars */
typedef struct
{
    SCROLLBAR_INFO horz;
    SCROLLBAR_INFO vert;
} WINSCROLLBAR_INFO, *LPWINSCROLLBAR_INFO;

typedef struct
{
    DWORD magic;
    SCROLLBAR_INFO info;
} SCROLLBAR_WNDDATA;

#define SCROLLBAR_MAGIC 0x5c6011ba

  /* Minimum size of the rectangle between the arrows */
#define SCROLL_MIN_RECT  4

  /* Minimum size of the thumb in pixels */
#define SCROLL_MIN_THUMB 8

  /* Overlap between arrows and thumb */
#define SCROLL_ARROW_THUMB_OVERLAP 0

  /* Delay (in ms) before first repetition when holding the button down */
#define SCROLL_FIRST_DELAY   200

  /* Delay (in ms) between scroll repetitions */
#define SCROLL_REPEAT_DELAY  50

  /* Scroll timer id */
#define SCROLL_TIMER   0

 /* What to do after SCROLL_SetScrollInfo() */
#define SA_SSI_HIDE		0x0001
#define SA_SSI_SHOW		0x0002
#define SA_SSI_REFRESH		0x0004
#define SA_SSI_REPAINT_ARROWS	0x0008


/***********************************************************************
 *           SCROLL_DrawArrows
 *
 * Draw the scroll bar arrows.
 */
static void SCROLL_DrawArrows( HDC hdc, UINT  infoPtr_flags,
                               RECT *rect, INT arrowSize, BOOL vertical,
                               BOOL top_pressed, BOOL bottom_pressed )
{
  RECT r;

  r = *rect;
  if( vertical )
    r.bottom = r.top + arrowSize;
  else
    r.right = r.left + arrowSize;

  DrawFrameControl( hdc, &r, DFC_SCROLL,
		    (vertical ? DFCS_SCROLLUP : DFCS_SCROLLLEFT)
		    | (top_pressed ? (DFCS_PUSHED | DFCS_FLAT) : 0 )
		    | (infoPtr_flags&ESB_DISABLE_LTUP ? DFCS_INACTIVE : 0 ) );

  r = *rect;
  if( vertical )
    r.top = r.bottom-arrowSize;
  else
    r.left = r.right-arrowSize;

  DrawFrameControl( hdc, &r, DFC_SCROLL,
		    (vertical ? DFCS_SCROLLDOWN : DFCS_SCROLLRIGHT)
		    | (bottom_pressed ? (DFCS_PUSHED | DFCS_FLAT) : 0 )
		    | (infoPtr_flags&ESB_DISABLE_RTDN ? DFCS_INACTIVE : 0) );
}

/***********************************************************************
 *           SCROLL_DrawInterior
 *
 * Draw the scroll bar interior (everything except the arrows).
 */
static void SCROLL_DrawInterior( HWND hwnd, HDC hdc, INT nBar,
                                 RECT *rect, INT arrowSize,
                                 INT thumbSize, INT thumbPos,
                                 UINT flags, BOOL vertical,
                                 BOOL top_selected, BOOL bottom_selected )
{
    RECT r;
    HPEN hSavePen;
    HBRUSH hSaveBrush,hBrush;

      /* Select the correct brush and pen */

    /* Only scrollbar controls send WM_CTLCOLORSCROLLBAR.
     * The window-owned scrollbars need to call DEFWND_ControlColor
     * to correctly setup default scrollbar colors
     */
    //if (nBar == SB_CTL) {
    //    hBrush = (HBRUSH)SendMessageW( GetParent(hwnd), WM_CTLCOLORSCROLLBAR,
    //                                   (WPARAM)hdc,(LPARAM)hwnd);
    //} else {
        hBrush = DEFWND_ControlColor( hdc, CTLCOLOR_SCROLLBAR );
    //}
    hSavePen = (HPEN)SelectObject( hdc, SYSCOLOR_GetPen(COLOR_WINDOWFRAME) );
    hSaveBrush = (HBRUSH)SelectObject( hdc, hBrush );

      /* Calculate the scroll rectangle */

    r = *rect;
    if (vertical)
    {
        r.top    += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
        r.bottom -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    }
    else
    {
        r.left  += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
        r.right -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    }

      /* Draw the scroll bar frame */

      /* Draw the scroll rectangles and thumb */

    if (!thumbPos)  /* No thumb to draw */
    {
        PatBlt( hdc, r.left, r.top, r.right - r.left, r.bottom - r.top, PATCOPY );

        /* cleanup and return */
        SelectObject( hdc, hSavePen );
        SelectObject( hdc, hSaveBrush );
        return;
    }

    if (vertical)
    {
        PatBlt( hdc, r.left, r.top, r.right - r.left,
                thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
                top_selected ? 0x0f0000 : PATCOPY );
        r.top += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
        PatBlt( hdc, r.left, r.top + thumbSize, r.right - r.left,
                r.bottom - r.top - thumbSize,
                bottom_selected ? 0x0f0000 : PATCOPY );
        r.bottom = r.top + thumbSize;
    }
    else  /* horizontal */
    {
        PatBlt( hdc, r.left, r.top,
                thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
                r.bottom - r.top, top_selected ? 0x0f0000 : PATCOPY );
        r.left += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
        PatBlt( hdc, r.left + thumbSize, r.top, r.right - r.left - thumbSize,
                r.bottom - r.top, bottom_selected ? 0x0f0000 : PATCOPY );
        r.right = r.left + thumbSize;
    }

      /* Draw the thumb */

    SelectObject( hdc, GetSysColorBrush(COLOR_BTNFACE) );
    Rectangle( hdc, r.left+1, r.top+1, r.right-1, r.bottom-1 );
    DrawEdge( hdc, &r, EDGE_RAISED, BF_RECT );

    /* cleanup */
    SelectObject( hdc, hSavePen );
    SelectObject( hdc, hSaveBrush );
}

void WINAPI USER_ScrollBarDraw( UINT  infoPtr_flags, HWND hwnd, HDC hdc, INT nBar
                                , enum SCROLL_HITTEST hit_test, BOOL hit_is_push, 
                                //const struct SCROLL_TRACKING_INFO *tracking_info, 
                                BOOL arrows,
                                BOOL interior, RECT *rect, INT arrowSize, INT thumbPos,
                                INT thumbSize, BOOL vertical )
{
    if (nBar == SB_CTL)
    {
        DWORD style = 0;//GetWindowLongW( hwnd, GWL_STYLE );

        if (style & SBS_SIZEGRIP)
        {
            RECT rc = *rect;

            FillRect( hdc, &rc, GetSysColorBrush( COLOR_SCROLLBAR ) );
            rc.left = max( rc.left, rc.right - GetSystemMetrics( SM_CXVSCROLL ) - 1 );
            rc.top = max( rc.top, rc.bottom - GetSystemMetrics( SM_CYHSCROLL ) - 1 );
            DrawFrameControl( hdc, &rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP );
            return;
        }

        if (style & SBS_SIZEBOX)
        {
            FillRect( hdc, rect, GetSysColorBrush( COLOR_SCROLLBAR ) );
            return;
        }
    }

    //if (!(infoPtr = SCROLL_GetInternalInfo( hwnd, nBar, TRUE )))
    //    return;

      /* Draw the arrows */

    if (arrows && arrowSize)
    {
        if (hit_is_push)
            SCROLL_DrawArrows( hdc, infoPtr_flags, rect, arrowSize, vertical,
                               hit_test == SCROLL_TOP_ARROW,
                               hit_test == SCROLL_BOTTOM_ARROW );
	    else
            SCROLL_DrawArrows( hdc, infoPtr_flags, rect, arrowSize, vertical, FALSE, FALSE );
    }

    if (interior)
    {
        if (hit_is_push)
        {
            SCROLL_DrawInterior( hwnd, hdc, nBar, rect, arrowSize, thumbSize, thumbPos,
                                 infoPtr_flags, vertical,
                                 hit_test == SCROLL_TOP_RECT,
                                 hit_test == SCROLL_BOTTOM_RECT );
        }
        else
        {
            SCROLL_DrawInterior( hwnd, hdc, nBar, rect, arrowSize, thumbSize, thumbPos,
                                 infoPtr_flags, vertical, FALSE, FALSE );
        }
    }

    /* if scroll bar has focus, reposition the caret */
    //if(hwnd==GetFocus() && (nBar==SB_CTL))
    //{
    //    if (!vertical)
    //    {
    //        SetCaretPos(thumbPos + 1, rect->top + 1);
    //    }
    //    else
    //    {
    //        SetCaretPos(rect->top + 1, thumbPos + 1);
    //    }
    //}
}
