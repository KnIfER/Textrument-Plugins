#pragma once

#define LB_CARETON             0x01a3
#define LB_CARETOFF            0x01a4

void COMBO_Register();

namespace Combo
{

enum combobox_state_flags
{
    CBF_DROPPED      = 0x0001,
    CBF_BUTTONDOWN   = 0x0002,
    CBF_NOROLLUP     = 0x0004,
    CBF_MEASUREITEM  = 0x0008,
    CBF_FOCUSED      = 0x0010,
    CBF_CAPTURE      = 0x0020,
    CBF_EDIT         = 0x0040,
    CBF_NORESIZE     = 0x0080,
    CBF_NOTIFY       = 0x0100,
    CBF_NOREDRAW     = 0x0200,
    CBF_SELCHANGE    = 0x0400,
    CBF_HOT          = 0x0800,
    CBF_NOEDITNOTIFY = 0x1000,
    CBF_NOLBSELECT   = 0x2000, /* do not change current selection */
    CBF_BEENFOCUSED  = 0x4000, /* has it ever had focus           */
    CBF_EUI          = 0x8000,
};

typedef struct
{
	HWND           self;
	HWND           owner;
	UINT           dwStyle;
	HWND           hWndEdit;
	HWND           hWndLBox;
	UINT           wState;
	HFONT          hFont;
	RECT           textRect;
	RECT           buttonRect;
	RECT           droppedRect;
	INT            droppedIndex;
	INT            fixedOwnerDrawHeight;
	INT            droppedWidth;   /* not used unless set explicitly */
	INT            item_height;
	INT            visibleItems;
} HEADCOMBO, *LPHEADCOMBO;




}

using namespace Combo;