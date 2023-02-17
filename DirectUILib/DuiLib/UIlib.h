#pragma once
#ifdef UILIB_STATIC
#define UILIB_API 
#else
#if defined(UILIB_EXPORTS)
#	if defined(_MSC_VER)
#		define UILIB_API __declspec(dllexport)
#	else
#		define UILIB_API
#	endif
#else
#	if defined(_MSC_VER)
#		define UILIB_API __declspec(dllimport)
#	else
#		define UILIB_API 
#	endif
#endif
#endif
#define UILIB_COMDAT __declspec(selectany)

#pragma warning(disable:4505)
#pragma warning(disable:4251)
#pragma warning(disable:4189)
#pragma warning(disable:4121)
#pragma warning(disable:4100)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

//#define MODULE_SKIA_RENDERER
#ifdef MODULE_SKIA_RENDERER
#include "include/utils/SkRandom.h"
#include "include/utils/SkRandom.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkPath.h"
#include "include/core/SkImage.h"
#include "include/core/SKImageInfo.h"
#include "include/core/SkImageGenerator.h"

#include "include/codec/SkCodec.h"


#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#include "include/core/SkFont.h"
#include "include/core/SkCanvas.h"
#endif

#include "Core/UICore.h"

#include "Core/XMLMarkup.h"

#include "Core/UIDlgBuilder.h"
#include "Core/UIRender.h"

#include "Utils/TrayIcon.h"
#include "Utils/WinImplBase.h"


#include "Utils/TrayIcon.h"

#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"


#include <functional>
typedef std::function<int(void*, UINT, WPARAM, LPARAM)> Listener;

#include "Control/ListHeader.h"
#include "Control/ListHeaderItem.h"
#include "Control/ListBasicItem.h"

#include "Control/ListView.h"

#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"

#include "Control/UILabel.h"
#include "Control/UIText.h"
#include "Control/UIEdit.h"
#include "Control/InputBox.h"
#include "Control/UIGifAnim.h"

#include "Control/UIAnimation.h"
#include "Control/UIButton.h"
#include "Control/UIOption.h"

#include "Control/UIProgress.h"
#include "Control/UISlider.h"

#include "Control/UIComboBox.h"
#include "Control/UIRichEdit.h"
#include "Control/UIDateTime.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"

#include "Control/UIMenu.h"
#include "Control/UICalendar.h"

#include "core/WinFrame.h"
#include "Control/WinButton.h"
#include "Control/WinTabbar.h"
#ifdef SCI_EDITTEXT
#include "Control/SciEditText.h"
#endif
#include "Control/WinSplitter.h"

#include "ControlEx/ImageView.h"

#include "Utils/DemoUitls.h"

#pragma comment( lib, "Shlwapi.lib" )
#pragma comment( lib, "comctl32.lib" )
#pragma comment( lib, "GdiPlus.lib" )
#pragma comment( lib, "Imm32.lib" )
#pragma comment( lib, "winmm.lib" )

#define CDuiString QkString
#define CButtonUI Button
#define COptionUI OptionBtn

#define REGIST_DUICONTROL REGIST_QKCONTROL
#define DECLARE_DUICONTROL DECLARE_QKCONTROL
#define IMPLEMENT_DUICONTROL IMPLEMENT_QKCONTROL

#define FX_MIN 1
#define FX_MOU 2
#define FX_CLK 3

const int VIEWSTATEMASK_Visibility       =   0x1;
const int VIEWSTATEMASK_Enabled          =   0x1<<3; 
const int VIEWSTATEMASK_Focused          =   0x1<<4;
const int VIEWSTATEMASK_Focusable        =   0x1<<5;
const int VIEWSTATEMASK_HotTracked       =   0x1<<6;
const int VIEWSTATEMASK_MouseEnabled     =   0x1<<7;
const int VIEWSTATEMASK_KeyboardEnabled  =   0x1<<8;
const int VIEWSTATEMASK_MenuUsed         =   0x1<<9;
const int VIEWSTATEMASK_DragEnabled      =   0x1<<10;
const int VIEWSTATEMASK_DropEnabled      =   0x1<<11;
const int VIEWSTATEMASK_RichEvent        =   0x1<<12;
const int VIEWSTATEMASK_SettingPos       =   0x1<<13;
const int VIEWSTATEMASK_NeedEstimateSize =   0x1<<14;
const int VIEWSTATEMASK_IsDirectUI       =   0x1<<15;
const int VIEWSTATEMASK_IsViewGroup      =   0x1<<16;
const int VIEWSTATEMASK_AutoDestroy      =   0x1<<17;
const int VIEWSTATEMASK_DelayedDestroy   =   0x1<<18;
const int VIEWSTATEMASK_UpdateNeeded     =   0x1<<19;
const int VIEWSTATEMASK_EnabledEffect    =   0x1<<20;
const int VIEWSTATEMASK_Float            =   0x1<<21;
const int VIEWSTATEMASK_ResourceText     =   0x1<<22;
const int VIEWSTATEMASK_ColorHSL         =   0x1<<23;
const int VIEWSTATEMASK_RoundClip        =   0x1<<24;
const int VIEW_INFO_DIRTY_COLORS         =   0x1<<25;
const int VIEWSTATEMASK_BKColorNonRound     =   0x1<<26;
const int VIEWSTATEMASK_BorderRoundByArc    =   0x3<<27;
const int VIEWSTATEMASK_BorderEnhanced      =   0x1<<29;
const int VIEWSTATEMASK_BorderAutoEnhance   =   0x1<<30;
const LONG_FLAG VIEWSTATEMASK_RedrawOnFocusChanged    =   0x1LL<<31;
const LONG_FLAG VIEWSTATEMASK_CustomWidget            =   0x1LL<<32L;
const LONG_FLAG VIEWSTATEMASK_PlainCustomWidget       =   0x1LL<<33;
const LONG_FLAG VIEWSTATEMASK_SPREADX       =   0x1LL<<34;
const LONG_FLAG VIEWSTATEMASK_SPREADY       =   0x1LL<<35;
const LONG_FLAG VIEWSTATEMASK_IsStatic      =   0x1LL<<36;

#define m_bEnabled                 (_view_states&VIEWSTATEMASK_Enabled)
#define m_bVisible                 (_view_states&VIEWSTATEMASK_Visibility)
#define m_bFocused                 (_view_states&VIEWSTATEMASK_Focused)
#define m_bFocusable               (_view_states&VIEWSTATEMASK_Focusable)
#define m_bMouseEnabled            (_view_states&VIEWSTATEMASK_MouseEnabled)
#define m_bKeyboardEnabled         (_view_states&VIEWSTATEMASK_KeyboardEnabled)
#define m_bMenuUsed                (_view_states&VIEWSTATEMASK_MenuUsed)
#define m_bDragEnabled             (_view_states&VIEWSTATEMASK_DragEnabled)
#define m_bDropEnabled             (_view_states&VIEWSTATEMASK_DropEnabled)
#define m_bRichEvent               (_view_states&VIEWSTATEMASK_RichEvent)
// 防止SetPos循环调用
#define m_bSettingPos              (_view_states&VIEWSTATEMASK_SettingPos)
#define m_bNeedEstimateSize        (_view_states&VIEWSTATEMASK_NeedEstimateSize)
#define m_bIsDirectUI              (_view_states&VIEWSTATEMASK_IsDirectUI)
#define m_bIsViewGroup             (_view_states&VIEWSTATEMASK_IsViewGroup)
#define m_bAutoDestroy             (_view_states&VIEWSTATEMASK_AutoDestroy)
#define m_bDelayedDestroy          (_view_states&VIEWSTATEMASK_DelayedDestroy)
#define m_bUpdateNeeded            (_view_states&VIEWSTATEMASK_UpdateNeeded)
#define m_bEnabledEffect           (_view_states&VIEWSTATEMASK_EnabledEffect)
#define m_bFloat                   (_view_states&VIEWSTATEMASK_Float)
#define m_bResourceText            (_view_states&VIEWSTATEMASK_ResourceText)
#define m_bColorHSL                (_view_states&VIEWSTATEMASK_ColorHSL)
#define m_bRoundClip               (_view_states&VIEWSTATEMASK_RoundClip)
#define m_bInfoDirtyColors         (_view_states&VIEW_INFO_DIRTY_COLORS)
#define m_bBkRound                 ((_view_states&VIEWSTATEMASK_BKColorNonRound)==0)
// 0=自动；1=路径; 2=矩形
#define m_iBorderRoundByArc        ((_view_states&VIEWSTATEMASK_BorderRoundByArc)>>27)
#define m_bBorderEnhanced		   (_view_states&VIEWSTATEMASK_BorderEnhanced)
#define m_bAutoEnhanceFocus        (_view_states&VIEWSTATEMASK_BorderAutoEnhance)
#define m_bRedrawOnFocusChanged    (_view_states&VIEWSTATEMASK_RedrawOnFocusChanged)
#define m_bCustomWidget            _view_states&VIEWSTATEMASK_CustomWidget
#define m_bPlainCustomWidget       _view_states&VIEWSTATEMASK_PlainCustomWidget
#define m_bIsStatic  			   _view_states&VIEWSTATEMASK_IsStatic

#define m_bFocused_YES _view_states|=VIEWSTATEMASK_Focused
#define m_bFocused_NO _view_states&=~VIEWSTATEMASK_Focused

#define VIEWSTATEMASK_APPLY(mask, value) if(value) _view_states |= mask; \
			else _view_states &= ~mask;

#define VIEWSTATEMASK_APPLY(mask, value) if(value) _view_states |= mask; \
			else _view_states &= ~mask;

#define VIEWSTATEMASK_APPLY_INT(mask, value, flagPos) _view_states = (_view_states&~mask)|(value<<flagPos);

#define VIEWSTATE_MARK_DIRTY(mask) _view_states |= mask;

#define VIEWSTATE_MARK_SYNCED(mask) _view_states &= ~mask;

using namespace DuiLib;

