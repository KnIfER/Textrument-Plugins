#pragma once

#include "usp10.h"

#pragma comment( lib, "Usp10.lib" )

#define BUFLIMIT_INITIAL    30000   /* initial buffer size */
#define GROWLENGTH		32	/* buffers granularity in bytes: must be power of 2 */
#define ROUND_TO_GROW(size)	(((size) + (GROWLENGTH - 1)) & ~(GROWLENGTH - 1))
#define HSCROLL_FRACTION	3	/* scroll window by 1/3 width */

/*
*	extra flags for EDITSTATE.flags field
*/
#define EF_MODIFIED		0x0001	/* text has been modified */
#define EF_FOCUSED		0x0002	/* we have input focus */
#define EF_UPDATE		0x0004	/* notify parent of changed state */
#define EF_VSCROLL_TRACK	0x0008	/* don't SetScrollPos() since we are tracking the thumb */
#define EF_HSCROLL_TRACK	0x0010	/* don't SetScrollPos() since we are tracking the thumb */
#define EF_AFTER_WRAP		0x0080	/* the caret is displayed after the last character of a
wrapped line, instead of in front of the next character */
#define EF_USE_SOFTBRK		0x0100	/* Enable soft breaks in text. */
#define EF_DIALOGMODE           0x0200  /* Indicates that we are inside a dialog window */

#define EF_SHOWLNRET           0x1000  /* 显示换行符（单行模式） */

#define ID_CB_LISTBOX 1000


void EDIT_Register();

typedef enum
{
	END_0 = 0,			/* line ends with terminating '\0' character */
	END_WRAP,			/* line is wrapped */
	END_HARD,			/* line ends with a hard return '\r\n' */
	END_SOFT,       		/* line ends with a soft return '\r\r\n' */
	END_RICH        		/* line ends with a single '\n' */
} LINE_END;

typedef struct tagLINEDEF {
	INT length;			/* bruto length of a line in bytes */
	INT net_length;			/* netto length of a line in visible characters */
	LINE_END ending;
	INT width;			/* width of the line in pixels */
	INT index; 			/* line index into the buffer */
	SCRIPT_STRING_ANALYSIS ssa;	/* Uniscribe Data */
	struct tagLINEDEF *next;
} LINEDEF;

typedef struct _EDIT_STATE
{
	LPWSTR text;			/* the actual contents of the control */
	UINT text_length;               /* cached length of text buffer (in WCHARs) - use get_text_length() to retrieve */
	UINT buffer_size;		/* the size of the buffer in characters */
	UINT buffer_limit;		/* the maximum size to which the buffer may grow in characters */
	HFONT font;			/* NULL means standard system font */
	INT x_offset;			/* scroll offset	for multi lines this is in pixels
							for single lines it's in characters */
	INT line_height;		/* height of a screen line in pixels */
	INT char_width;			/* average character width in pixels */
	DWORD style;			/* sane version of wnd->dwStyle */
	WORD flags;			/* flags that are not in es->style or wnd->flags (EF_XXX) */
	INT undo_insert_count;		/* number of characters inserted in sequence */
	UINT undo_position;		/* character index of the insertion and deletion */
	LPWSTR undo_text;		/* deleted text */
	UINT undo_buffer_size;		/* size of the deleted text buffer */
	INT selection_start;		/* == selection_end if no selection */
	INT selection_end;		/* == current caret position */
	WCHAR password_char;		/* == 0 if no password char, and for multi line controls */
	INT left_margin;		/* in pixels */
	INT right_margin;		/* in pixels */
	RECT format_rect;
	INT text_width;			/* width of the widest line in pixels for multi line controls
							and just line width for single line controls	*/
	INT region_posx;		/* Position of cursor relative to region: */
	INT region_posy;		/* -1: to left, 0: within, 1: to right */
	EDITWORDBREAKPROCW word_break_proc;
	INT line_count;			/* number of lines */
	INT y_offset;			/* scroll offset in number of lines */
	BOOL bCaptureState; 		/* flag indicating whether mouse was captured */
	BOOL bEnableState;		/* flag keeping the enable state */
	HWND hwndSelf;			/* the our window handle */
	HWND hwndParent;		/* Handle of parent for sending EN_* messages.
							Even if parent will change, EN_* messages
							should be sent to the first parent. */
	HWND hwndListBox;		/* handle of ComboBox's listbox or NULL */
	INT wheelDeltaRemainder;        /* scroll wheel delta left over after scrolling whole lines */
	WCHAR *cue_banner_text;
	BOOL cue_banner_draw_focused;

	/*
	*	only for multi line controls
	*/
	INT lock_count;			/* amount of re-entries in the EditWndProc */
	INT tabs_count;
	LPINT tabs;
	LINEDEF *first_line_def;	/* linked list of (soft) linebreaks */
	HLOCAL hloc32W;			/* our unicode local memory block */
	HLOCAL hlocapp;                 /* The text buffer handle belongs to the app */
									/*
									* IME Data
									*/
	UINT composition_len;   /* length of composition, 0 == no composition */
	int composition_start;  /* the character position for the composition */
							/*
							* Uniscribe Data
							*/
	SCRIPT_LOGATTR *logAttr;
	SCRIPT_STRING_ANALYSIS ssa; /* Uniscribe Data for single line controls */

	BOOL is_delegate;
	BOOL useTextObj;
	LONG_PTR textObj;
	RECT* rcDraw;
	RECT* rcDrawMax;
	RECT* rcPadding;

	COLORREF         bgrTextColor;

	std::set<SCRIPT_STRING_ANALYSIS> validPtrs;

	int prex;
	int prey;

	const bool * clip_texts;
	Listener _listener;

	INT inflatedSt=-1;
	INT inflatedEd=-1;
} EDITSTATE;

namespace Edit{

LRESULT _Create(HWND hwnd, WPARAM wParam, LPARAM lParam);

void _Paint(EDITSTATE *es, HDC hdc, const RECT* rcPaint=0);

void _SetText(EDITSTATE *es, LPCWSTR text);

void EDIT_LockBuffer(EDITSTATE *es);

void EDIT_UnlockBuffer(EDITSTATE *es, BOOL force);

void _SetFont(EDITSTATE *es, HFONT font, BOOL redraw);

LRESULT _Char(EDITSTATE *es, WCHAR c);

void _SetLimitText(EDITSTATE *es, UINT limit);

LRESULT _LButtonDown(EDITSTATE *es, DWORD keys, INT x, INT y);

LRESULT _LButtonUp(EDITSTATE *es);

void EDIT_SetCaretPos(EDITSTATE *es, INT pos, BOOL after_wrap);

LRESULT _MouseMove(EDITSTATE *es, INT x, INT y);

LRESULT EDIT_EM_PosFromChar(EDITSTATE *es, INT index, BOOL after_wrap);

LRESULT _MouseWheel(EDITSTATE *es, WPARAM wParam, LPARAM lParam);

LRESULT _KeyDown(EDITSTATE *es, INT key);

LRESULT _SelectWholeLine(EDITSTATE *es);

LRESULT _LButtonDblClk(EDITSTATE *es);

void EDIT_WM_ContextMenu(EDITSTATE *es, INT x, INT y);

void EDIT_MoveBackward(EDITSTATE *es, BOOL extend, BOOL fromTimer=FALSE);

void EDIT_MoveForward(EDITSTATE *es, BOOL extend, BOOL fromTimer=FALSE);

inline void EDIT_InvalidateUniscribeData(EDITSTATE *es);

void EDIT_ResetText(EDITSTATE *es);

BOOL EDIT_EM_LineScroll(EDITSTATE *es, INT dx, INT dy);

BOOL EDIT_EM_LineScroll_internal(EDITSTATE *es, INT dx, INT dy);

using EDITOBJ_MakeFitSig = BOOL(*)(EDITSTATE *es, UINT size);
__declspec(selectany) EDITOBJ_MakeFitSig fnEDITOBJ_MakeFit = nullptr;

using EDITOBJ_SyncTextSig = void(*)(EDITSTATE *es);
__declspec(selectany) EDITOBJ_SyncTextSig fnEDITOBJ_SyncText = nullptr;

using EDITOBJ_RecalcSzSig = void(*)(EDITSTATE *es);
__declspec(selectany) EDITOBJ_RecalcSzSig fnEDITOBJ_RecalcSz = nullptr;

}

using namespace Edit;


