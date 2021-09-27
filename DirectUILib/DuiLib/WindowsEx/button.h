#pragma once

void BUTTON_Register();

#ifndef BST_DROPDOWNPUSHED

// BUTTON STATE FLAGS
#define BST_DROPDOWNPUSHED      0x0400

// begin_r_commctrl

// BUTTON STYLES
#define BS_SPLITBUTTON          0x0000000CL
#define BS_DEFSPLITBUTTON       0x0000000DL
#define BS_COMMANDLINK          0x0000000EL
#define BS_DEFCOMMANDLINK       0x0000000FL

// SPLIT BUTTON INFO mask flags
#define BCSIF_GLYPH             0x0001
#define BCSIF_IMAGE             0x0002
#define BCSIF_STYLE             0x0004
#define BCSIF_SIZE              0x0008

// SPLIT BUTTON STYLE flags
#define BCSS_NOSPLIT            0x0001
#define BCSS_STRETCH            0x0002
#define BCSS_ALIGNLEFT          0x0004
#define BCSS_IMAGE              0x0008

// end_r_commctrl

// BUTTON STRUCTURES
typedef struct tagBUTTON_SPLITINFO
{
    UINT        mask;
    HIMAGELIST  himlGlyph;         // interpreted as WCHAR if BCSIF_GLYPH is set
    UINT        uSplitStyle;
    SIZE        size;
} BUTTON_SPLITINFO, * PBUTTON_SPLITINFO;

// BUTTON MESSAGES
#define BCM_SETDROPDOWNSTATE     (BCM_FIRST + 0x0006)
#define Button_SetDropDownState(hwnd, fDropDown) \
    (BOOL)SNDMSG((hwnd), BCM_SETDROPDOWNSTATE, (WPARAM)(fDropDown), 0)

#define BCM_SETSPLITINFO         (BCM_FIRST + 0x0007)
#define Button_SetSplitInfo(hwnd, pInfo) \
    (BOOL)SNDMSG((hwnd), BCM_SETSPLITINFO, 0, (LPARAM)(pInfo))

#define BCM_GETSPLITINFO         (BCM_FIRST + 0x0008)
#define Button_GetSplitInfo(hwnd, pInfo) \
    (BOOL)SNDMSG((hwnd), BCM_GETSPLITINFO, 0, (LPARAM)(pInfo))

#define BCM_SETNOTE              (BCM_FIRST + 0x0009)
#define Button_SetNote(hwnd, psz) \
    (BOOL)SNDMSG((hwnd), BCM_SETNOTE, 0, (LPARAM)(psz))

#define BCM_GETNOTE              (BCM_FIRST + 0x000A)
#define Button_GetNote(hwnd, psz, pcc) \
    (BOOL)SNDMSG((hwnd), BCM_GETNOTE, (WPARAM)pcc, (LPARAM)psz)

#define BCM_GETNOTELENGTH        (BCM_FIRST + 0x000B)
#define Button_GetNoteLength(hwnd) \
    (LRESULT)SNDMSG((hwnd), BCM_GETNOTELENGTH, 0, 0)


#if (NTDDI_VERSION >= NTDDI_VISTA)
// Macro to use on a button or command link to display an elevated icon
#define BCM_SETSHIELD            (BCM_FIRST + 0x000C)
#define Button_SetElevationRequiredState(hwnd, fRequired) \
    (LRESULT)SNDMSG((hwnd), BCM_SETSHIELD, 0, (LPARAM)fRequired)
#endif // (NTDDI_VERSION >= NTDDI_VISTA)

// Value to pass to BCM_SETIMAGELIST to indicate that no glyph should be
// displayed
#define BCCL_NOGLYPH  (HIMAGELIST)(-1)

// NOTIFICATION MESSAGES
typedef struct tagNMBCDROPDOWN
{
    NMHDR   hdr;
    RECT    rcButton;
} NMBCDROPDOWN, * LPNMBCDROPDOWN;

#define BCN_DROPDOWN            (BCN_FIRST + 0x0002)

#endif // (NTDDI_VERSION >= NTDDI_VISTA)

typedef struct _BUTTON_INFO
{
    HWND             hwnd;
    HWND             parent;
    LONG             dwStyle;
    LONG             dtStyle;
    LONG             exStyle;
    LONG             state;
    HFONT            font;
    WCHAR           *note;
    INT              note_length;
    DWORD            image_type; /* IMAGE_BITMAP or IMAGE_ICON */
    BUTTON_IMAGELIST imagelist;
    UINT             split_style;
    HIMAGELIST       glyph;      /* this is a font character code when split_style doesn't have BCSS_IMAGE */
    SIZE             glyph_size;
    RECT             text_margin;
    HANDLE           image; /* Original handle set with BM_SETIMAGE and returned with BM_GETIMAGE. */
    union
    {
        HICON   icon;
        HBITMAP bitmap;
        HANDLE  image; /* Duplicated handle used for drawing. */
    } u;
    BOOL is_delegate;
    BOOL enabled;
    RECT* rcDraw;
    RECT* rcPadding;
    WCHAR* delegated_Text;
    LONG textAutoWidth;
    LONG textAutoHeight;
} BUTTON_INFO;

namespace Button
{
    LRESULT _Create(HWND hWnd, WPARAM wParam, LPARAM lParam);

    LRESULT _Paint(BUTTON_INFO* infoPtr, WPARAM wParam, HWND hWnd=0);

    void BUTTON_QueryPreempterSize(BUTTON_INFO* infoPtr, WPARAM _preSizeX);
}