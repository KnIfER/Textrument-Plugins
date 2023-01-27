#pragma once

#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#include "UIDxAnimation.h"

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CControlUI;
	class CRichEditUI;
	class CIDropTarget;

	/////////////////////////////////////////////////////////////////////////////////////
	//
	enum UILIB_RESTYPE
	{
		UILIB_FILE=1,		// 来自磁盘文件
		UILIB_ZIP,			// 来自磁盘zip压缩包
		UILIB_RESOURCE,		// 来自资源
		UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
	};
	/////////////////////////////////////////////////////////////////////////////////////
	//

	enum EVENTTYPE_UI
	{
		UIEVENT__FIRST = 1,
		UIEVENT__KEYBEGIN,
		UIEVENT_KEYDOWN,
		UIEVENT_KEYUP,
		UIEVENT_CHAR,
		UIEVENT_SYSKEY,
		UIEVENT__KEYEND,
		UIEVENT__MOUSEBEGIN,
		UIEVENT_MOUSEMOVE,
		UIEVENT_MOUSELEAVE,
		UIEVENT_MOUSEENTER,
		UIEVENT_MOUSEHOVER,
		UIEVENT_BUTTONDOWN,
		UIEVENT_BUTTONUP,
		UIEVENT_RBUTTONDOWN,
		UIEVENT_RBUTTONUP,
		UIEVENT_MBUTTONDOWN,
		UIEVENT_MBUTTONUP,
		UIEVENT_DBLCLICK,
		UIEVENT_CONTEXTMENU,
		UIEVENT_SCROLLWHEEL,
		UIEVENT__MOUSEEND,
		UIEVENT_KILLFOCUS,
		UIEVENT_SETFOCUS,
		UIEVENT_WINDOWSIZE,
		UIEVENT_SETCURSOR,
		UIEVENT_TIMER,
		UIEVENT_IME_REQUEST,
		UIEVENT__LAST,
	};

	typedef enum
	{
		UIFONT__FIRST = 0,
		UIFONT_NORMAL,
		UIFONT_BOLD,
		UIFONT_CAPTION,
		UIFONT_MENU,
		UIFONT_LINK,
		UIFONT_TITLE,
		UIFONT_HEADLINE,
		UIFONT_SUBSCRIPT,
		UIFONT__LAST,
	} UITYPE_FONT;

	typedef enum
	{
		UICOLOR__FIRST = 0,
		UICOLOR_WINDOW_BACKGROUND,
		UICOLOR_WINDOW_TEXT,
		UICOLOR_DIALOG_BACKGROUND,
		UICOLOR_DIALOG_TEXT_NORMAL,
		UICOLOR_DIALOG_TEXT_DARK,
		UICOLOR_MENU_BACKGROUND,
		UICOLOR_MENU_TEXT_NORMAL,
		UICOLOR_MENU_TEXT_HOVER,
		UICOLOR_MENU_TEXT_SELECTED,
		UICOLOR_TEXTCOLOR_NORMAL,
		UICOLOR_TEXTCOLOR_HOVER,
		UICOLOR_TAB_BACKGROUND_NORMAL,
		UICOLOR_TAB_BACKGROUND_SELECTED,
		UICOLOR_TAB_FOLDER_NORMAL,
		UICOLOR_TAB_FOLDER_SELECTED,
		UICOLOR_TAB_BORDER,
		UICOLOR_TAB_TEXT_NORMAL,
		UICOLOR_TAB_TEXT_SELECTED,
		UICOLOR_TAB_TEXT_DISABLED,
		UICOLOR_NAVIGATOR_BACKGROUND,
		UICOLOR_NAVIGATOR_BUTTON_HOVER,
		UICOLOR_NAVIGATOR_BUTTON_PUSHED,
		UICOLOR_NAVIGATOR_BUTTON_SELECTED,
		UICOLOR_NAVIGATOR_BORDER_NORMAL,
		UICOLOR_NAVIGATOR_BORDER_SELECTED,
		UICOLOR_NAVIGATOR_TEXT_NORMAL,
		UICOLOR_NAVIGATOR_TEXT_SELECTED,
		UICOLOR_NAVIGATOR_TEXT_PUSHED,
		UICOLOR_BUTTON_BACKGROUND_NORMAL,
		UICOLOR_BUTTON_BACKGROUND_DISABLED,
		UICOLOR_BUTTON_BACKGROUND_PUSHED,
		UICOLOR_BUTTON_TEXT_NORMAL,
		UICOLOR_BUTTON_TEXT_PUSHED,
		UICOLOR_BUTTON_TEXT_DISABLED,
		UICOLOR_BUTTON_BORDER_LIGHT,
		UICOLOR_BUTTON_BORDER_DARK,
		UICOLOR_BUTTON_BORDER_DISABLED,
		UICOLOR_BUTTON_BORDER_FOCUS,
		UICOLOR_CONTROL_BACKGROUND_NORMAL,
		UICOLOR_CONTROL_BACKGROUND_SELECTED,
		UICOLOR_CONTROL_BACKGROUND_DISABLED,
		UICOLOR_CONTROL_BACKGROUND_READONLY,
		UICOLOR_CONTROL_BACKGROUND_HOVER,
		UICOLOR_CONTROL_BACKGROUND_SORTED,
		UICOLOR_CONTROL_BACKGROUND_EXPANDED,
		UICOLOR_CONTROL_BORDER_NORMAL,
		UICOLOR_CONTROL_BORDER_SELECTED,
		UICOLOR_CONTROL_BORDER_DISABLED,
		UICOLOR_CONTROL_TEXT_NORMAL,
		UICOLOR_CONTROL_TEXT_SELECTED,
		UICOLOR_CONTROL_TEXT_DISABLED,
		UICOLOR_CONTROL_TEXT_READONLY,
		UICOLOR_TOOL_BACKGROUND_NORMAL,
		UICOLOR_TOOL_BACKGROUND_DISABLED,
		UICOLOR_TOOL_BACKGROUND_HOVER,
		UICOLOR_TOOL_BACKGROUND_PUSHED,
		UICOLOR_TOOL_BORDER_NORMAL,
		UICOLOR_TOOL_BORDER_DISABLED,
		UICOLOR_TOOL_BORDER_HOVER,
		UICOLOR_TOOL_BORDER_PUSHED,
		UICOLOR_EDIT_BACKGROUND_NORMAL,
		UICOLOR_EDIT_BACKGROUND_HOVER,
		UICOLOR_EDIT_BACKGROUND_DISABLED,
		UICOLOR_EDIT_BACKGROUND_READONLY,
		UICOLOR_EDIT_TEXT_NORMAL,
		UICOLOR_EDIT_TEXT_DISABLED,
		UICOLOR_EDIT_TEXT_READONLY,
		UICOLOR_TITLE_BACKGROUND,
		UICOLOR_TITLE_TEXT,
		UICOLOR_TITLE_BORDER_LIGHT,
		UICOLOR_TITLE_BORDER_DARK,
		UICOLOR_HEADER_BACKGROUND,
		UICOLOR_HEADER_BORDER,
		UICOLOR_HEADER_SEPARATOR,
		UICOLOR_HEADER_TEXT,
		UICOLOR_TASK_BACKGROUND,
		UICOLOR_TASK_CAPTION,
		UICOLOR_TASK_BORDER,
		UICOLOR_TASK_TEXT,
		UICOLOR_LINK_TEXT_HOVER,
		UICOLOR_LINK_TEXT_NORMAL,
		UICOLOR_STANDARD_BLACK,
		UICOLOR_STANDARD_YELLOW,
		UICOLOR_STANDARD_RED,
		UICOLOR_STANDARD_GREY,
		UICOLOR_STANDARD_LIGHTGREY,
		UICOLOR_STANDARD_WHITE,
		UICOLOR__LAST,
		UICOLOR__INVALID,
	} UITYPE_COLOR;

	/////////////////////////////////////////////////////////////////////////////////////
	//
	// 内部保留的消息
	enum MSGTYPE_UI
	{
		UIMSG_TRAYICON = WM_USER + 1,// 托盘消息
		UIMSG_SET_DPI,				 // DPI
		WM_MENUCLICK,				 // 菜单消息
		UIMSG_USER = WM_USER + 100,	 // 程序自定义消息
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

	// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_UPDATETEST    0x00000008
#define UIFIND_TOP_FIRST     0x00000010
#define UIFIND_ME_FIRST      0x80000000

	// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040

	typedef struct _StyleDefine
	{
		QkString name;
		QkString value;
	} StyleDefine;

	typedef struct _Style
	{
		QkString id; // 样式名
		QkString name; // 控件名
		CControlUI* control;
		std::vector<StyleDefine> styles;
	} Style;

	/////////////////////////////////////////////////////////////////////////////////////
	//
	typedef struct tagEffectAge
	{
		bool		m_bEnableEffect;
		int			m_iZoom;
		DWORD		m_dFillingBK;
		int			m_iOffectX;
		int			m_iOffectY;
		int			m_iAlpha;
		float		m_fRotation;
		int			m_iNeedTimer;
	}TEffectAge;

	typedef struct UILIB_API tagTFontInfo
	{
		HFONT hFont;
		QkString sFontName;
		int iSize;
		bool bBold;
		bool bUnderline;
		bool bItalic;
		TEXTMETRIC tm;
	} TFontInfo;

	typedef struct UILIB_API tagTImageInfo
	{
		HBITMAP hBitmap;
		LPBYTE pBits;
		LPBYTE pSrcBits;
		int nX;
		int nY;
		bool bAlpha;
		bool bUseHSL;
		QkString sResType;
		DWORD dwMask;
		LPBYTE pBmBits;

	} TImageInfo;

	// Structure for relative position to the parent
	typedef struct tagTRelativePosUI
	{
		bool bRelative;
		SIZE szParent;
		int nMoveXPercent;
		int nMoveYPercent;
		int nZoomXPercent;
		int nZoomYPercent;
	}TRelativePosUI;

	typedef struct UILIB_API tagTDrawInfo
	{
		tagTDrawInfo();
		void Parse(LPCTSTR pStrImage, LPCTSTR pStrModify, CPaintManagerUI *pManager);
		void Clear();

		QkString sDrawString;
		QkString sDrawModify;
		QkString sImageName;
		QkString sResType;
		RECT rcDest;
		RECT rcSource;
		RECT rcCorner;
		DWORD dwMask;
		BYTE uFade;
		bool bHole;
		bool bTiledX;
		bool bTiledY;
		bool bHSL;

		CDuiSize szImage;
		RECT rcPadding;
		QkString sAlign;
	} TDrawInfo;

	typedef struct UILIB_API tagTPercentInfo
	{
		double left;
		double top;
		double right;
		double bottom;
	} TPercentInfo;

	typedef struct UILIB_API tagTResInfo
	{
		DWORD m_dwDefaultDisabledColor;
		DWORD m_dwDefaultFontColor;
		DWORD m_dwDefaultLinkFontColor;
		DWORD m_dwDefaultLinkHoverFontColor;
		DWORD m_dwDefaultSelectedBkColor;
		TFontInfo m_DefaultFontInfo;
		QkStringPtrMap m_CustomFonts; // todo use sparse array
		QkStringPtrMap m_ImageHash;
		//QkStringPtrMap m_AttrHash;
		//QkStringPtrMap m_StyleHash;
		QkStringPtrMap m_DrawInfoHash;
		QkStringPtrMap m_StyleIdHash; // 样式名 - 样式 映射
		QkStringPtrMap m_StyleControlHash; // 控件名 - 样式 映射
		QkStringPtrMap _namedFontsMap; // 具名字体 - 虚拟id 映射
		std::vector<TFontInfo*> _namedFonts; // 具名字体
	} TResInfo;

	// Structure for notifications from the system
	// to the control implementation.
	typedef struct UILIB_API tagTEventUI
	{
		int Type;
		CControlUI* pSender;
		DWORD dwTimestamp;
		POINT ptMouse;
		TCHAR chKey;
		WORD wKeyState;
		WPARAM wParam;
		LPARAM lParam;
	} TEventUI;

	// Drag&Drop control
	const TCHAR* const CF_MOVECONTROL = _T("CF_MOVECONTROL");

	typedef struct UILIB_API tagTCFMoveUI
	{
		CControlUI* pControl;
	} TCFMoveUI;

	// Listener interface
	class INotifyUI
	{
	public:
		virtual void Notify(TNotifyUI& msg) = 0;
	};

	// MessageFilter interface
	class IMessageFilterUI
	{
	public:
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
	};

	class ITranslateAccelerator
	{
	public:
		virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//
	typedef CControlUI* (*LPCREATECONTROL)(LPCTSTR pstrType);

	class UILIB_API CPaintManagerUI : public CIDropTarget
	{
	public:
		CPaintManagerUI();
		~CPaintManagerUI();

		void reInit();

		void reInit(CPaintManagerUI* as);

		void SetParent(CPaintManagerUI* parent);
	public:
		void Init(HWND hWnd, LPCTSTR pstrName = NULL);
		bool IsUpdateNeeded() const;
		void NeedUpdate();
		void Invalidate();
		void Invalidate(RECT& rcItem);

		LPCTSTR GetName() const;
		HDC GetPaintDC() const;
		HWND GetPaintWindow() const;
		HWND GetTooltipWindow() const;
		int GetHoverTime() const;
		void SetHoverTime(int iTime);

		POINT GetMousePos() const;
		SIZE GetClientSize() const;
		SIZE GetInitSize();
		void SetInitSize(int cx, int cy);
		RECT& GetSizeBox();
		void SetSizeBox(RECT& rcSizeBox);
		RECT& GetCaptionRect();
		void SetCaptionRect(RECT& rcCaption);
		SIZE GetRoundCorner() const;
		void SetRoundCorner(int cx, int cy);
		SIZE GetMinInfo() const;
		void SetMinInfo(int cx, int cy);
		SIZE GetMaxInfo() const;
		void SetMaxInfo(int cx, int cy);
		bool IsShowUpdateRect() const;
		void SetShowUpdateRect(bool show);
		bool IsNoActivate();
		void SetNoActivate(bool bNoActivate);

		BYTE GetOpacity() const;
		void SetOpacity(BYTE nOpacity);

		bool IsLayered();
		void SetLayered(bool bLayered);
		RECT& GetLayeredInset();
		void SetLayeredInset(RECT& rcLayeredInset);
		BYTE GetLayeredOpacity();
		void SetLayeredOpacity(BYTE nOpacity);
		LPCTSTR GetLayeredImage();
		void SetLayeredImage(LPCTSTR pstrImage);

		CShadowUI* GetShadow();

		void SetUseGdiplusText(bool bUse);
		bool IsUseGdiplusText() const;
		void SetGdiplusTextRenderingHint(int trh);
		int GetGdiplusTextRenderingHint() const;

		static HINSTANCE GetInstance();
		static QkString GetInstancePath();
		static QkString GetCurrentPath();
		static HINSTANCE GetResourceDll();
		static const QkString& GetResourcePath();
		static const QkString& GetResourceZip();
		static const QkString& GetResourceZipPwd();
		static bool IsCachedResourceZip();
		static bool ExtractItem(const TCHAR* name, CHAR** outData, DWORD & dataLen);
		static HANDLE GetResourceZipHandle();
		static void SetInstance(HINSTANCE hInst);
		static void SetCurrentPath(LPCTSTR pStrPath);
		static void SetResourceDll(HINSTANCE hInst);
		static void SetResourcePath(LPCTSTR pStrPath);
		static void SetResourceZip(LPVOID pVoid, unsigned int len, LPCTSTR password = NULL);
		static void SetResourceZip(LPCTSTR pstrZip, bool bCachedResourceZip = false, LPCTSTR password = NULL);
		static void SetResourceType(int nType);
		static int GetResourceType();
		static bool GetHSL(short* H, short* S, short* L);
		static void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
		static void ReloadSkin();
		static CPaintManagerUI* GetPaintManager(LPCTSTR pstrName);
		static CStdPtrArray* GetPaintManagers();
		static bool LoadPlugin(LPCTSTR pstrModuleName);
		static CStdPtrArray* GetPlugins();

		void SetAllowAutoFocus(bool bNoAutoFocus);

		bool IsForceUseSharedRes() const;
		void SetForceUseSharedRes(bool bForce);
		// 注意：只支持简单类型指针，因为只释放内存，不会调用类对象的析构函数
		void DeletePtr(void* ptr);

		DWORD GetDefaultDisabledColor() const;
		void SetDefaultDisabledColor(DWORD dwColor, bool bShared = false);
		DWORD GetDefaultFontColor() const;
		void SetDefaultFontColor(DWORD dwColor, bool bShared = false);
		DWORD GetDefaultLinkFontColor() const;
		void SetDefaultLinkFontColor(DWORD dwColor, bool bShared = false);
		DWORD GetDefaultLinkHoverFontColor() const;
		void SetDefaultLinkHoverFontColor(DWORD dwColor, bool bShared = false);
		DWORD GetDefaultSelectedBkColor() const;
		void SetDefaultSelectedBkColor(DWORD dwColor, bool bShared = false);
		TFontInfo* GetDefaultFontInfo();
		void SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
		DWORD GetCustomFontCount(bool bShared = false) const;
		void AddFontArray(LPCTSTR pstrPath);
		HFONT AddFont(LPCTSTR pStrFontId, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
		HFONT GetFont(int id);
		HFONT GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
		int GetFontIndex(HFONT hFont, bool bShared = false);
		int GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
		void RemoveFont(HFONT hFont, bool bShared = false);
		void RemoveFont(int id, bool bShared = false);
		void RemoveAllFonts(bool bShared = false);
		TFontInfo* GetFontInfo(int id);
		TFontInfo* GetFontInfo(HFONT hFont);

		const TImageInfo* GetImage(LPCTSTR bitmap);
		const TImageInfo* GetImageEx(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0, bool bUseHSL = false, HINSTANCE instance = NULL);
		const TImageInfo* AddImage(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0, bool bUseHSL = false, bool bShared = false, HINSTANCE instance = NULL);
		const TImageInfo* AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared = false);
		void RemoveImage(LPCTSTR bitmap, bool bShared = false);
		void RemoveAllImages(bool bShared = false);
		static void ReloadSharedImages();
		void ReloadImages();

		const TDrawInfo* GetDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify);
		void RemoveDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify);
		void RemoveAllDrawInfos();

		void AddStyle(Style* style, bool bShared);
		Style* GetStyleForId(LPCTSTR pName) const;

		void AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStyleId, LPCTSTR pStrControlAttrList, bool bShared = false);
		Style* GetDefaultAttributeList(LPCTSTR pStrControlName) const;
		bool RemoveDefaultAttributeList(LPCTSTR pStrControlName, bool bShared = false);
		void RemoveAllDefaultAttributeList(bool bShared = false);

		void AddWindowCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
		LPCTSTR GetWindowCustomAttribute(LPCTSTR pstrName) const;
		bool RemoveWindowCustomAttribute(LPCTSTR pstrName);
		void RemoveAllWindowCustomAttribute();

		const TImageInfo* GetImageString(LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

		// 初始化拖拽
		bool EnableDragDrop(bool bEnable);
		virtual bool OnDrop(FORMATETC* pFmtEtc, STGMEDIUM& medium,DWORD *pdwEffect);

		bool AttachDialog(CControlUI* pControl, bool bNeedFocus=true);
		bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
		void ReapObjects(CControlUI* pControl);

		bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
		CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
		void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
		void RemoveAllOptionGroups();

		CControlUI* GetFocus() const;
		void SetFocus(CControlUI* pControl);
		void SetFocusNeeded(CControlUI* pControl);

		bool SetNextTabControl(bool bForward = true);

		bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse, bool bRestart=false);
		bool KillTimer(CControlUI* pControl, UINT nTimerID);
		void KillTimer(CControlUI* pControl);
		void RemoveAllTimers();

		void SetCapture();
		void ReleaseCapture();
		bool IsCaptured();

		bool IsPainting();
		void SetPainting(bool bIsPainting);

		bool AddNotifier(INotifyUI* pControl);
		bool RemoveNotifier(INotifyUI* pControl);   
		void SendNotify(TNotifyUI& Msg, bool bAsync = false);
		void SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

		bool AddPreMessageFilter(IMessageFilterUI* pFilter);
		bool RemovePreMessageFilter(IMessageFilterUI* pFilter);

		bool AddMessageFilter(IMessageFilterUI* pFilter);
		bool RemoveMessageFilter(IMessageFilterUI* pFilter);

		int GetPostPaintCount() const;
		bool IsPostPaint(CControlUI* pControl);
		bool AddPostPaint(CControlUI* pControl);
		bool RemovePostPaint(CControlUI* pControl);
		bool SetPostPaintIndex(CControlUI* pControl, int iIndex);

		int GetNativeWindowCount() const;
		RECT GetNativeWindowRect(HWND hChildWnd);
		bool AddNativeWindow(CControlUI* pControl, HWND hChildWnd);
		bool RemoveNativeWindow(HWND hChildWnd);

		void AddDelayedCleanup(CControlUI* pControl);
		void AddMouseLeaveNeeded(CControlUI* pControl);
		bool RemoveMouseLeaveNeeded(CControlUI* pControl);

		bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
		bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
		bool TranslateAccelerator(LPMSG pMsg);

		CControlUI* GetRoot() const;
		CControlUI* FindControl(POINT pt) const;
		CControlUI* FindControl(LPCTSTR pstrName) const;
		CControlUI* FindSubControlByPoint(CControlUI* pParent, POINT pt) const;
		CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;
		CControlUI* FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex = 0);
		CStdPtrArray* FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass);

		static void MessageLoop();
		static bool TranslateMessage(const LPMSG pMsg);
		static void Term();

		CDPI* GetDPIObj();
		void ResetDPIAssets();
		void RebuildFont(TFontInfo* pFontInfo);
		void SetDPI(int iDPI);
		static void SetAllDPI(int iDPI);

		bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
		void UsedVirtualWnd(bool bUsed);

		bool AddAnimationJob(const CDxAnimationUI& _DxAnimationUI);
		HPEN GetThemePen(UITYPE_COLOR Index) const;
		HFONT GetThemeFont(UITYPE_FONT Index) const;
		HBRUSH GetThemeBrush(UITYPE_COLOR Index) const;
		COLORREF GetThemeColor(UITYPE_COLOR Index) const;
		HICON GetThemeIcon(int Index, int cxySize) const;
		const TEXTMETRIC& GetThemeFontInfo(UITYPE_FONT Index) const;
		bool GetThemeColorPair(UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2) const;

		bool AddEffectsStyle(LPCTSTR pStrStyleName,LPCTSTR pStrStyleValue);
		QkString GetEffectsStyle(LPCTSTR pStrStyleName);
		const QkStringPtrMap& GetEffectsStyles() const;
		bool RemoveEffectStyle(LPCTSTR pStrStyleName);
		void RemoveAllEffectStyle();
		//const RECT & GetPaintRect() {return _rcPaint;};

#ifdef SKIA_RENDERER
		SkCanvas* GetSkiaCanvas(){ return _skCanvas; };
		SkPaint & GetSkiaFillPaint(){ return _fillpaint; };
#endif

		bool _SIZING;
		bool _inflaing;
		bool _bIsLayoutOnly = false;
		bool _bIsWinFrame = false;

		static HCURSOR hCursorArrow;
		static HCURSOR hCursorHand;

		std::list<CControlUI*> _UpdateList;
		std::list<CControlUI*> _WNDList;

		POINT m_ptLastMousePos;

		TResInfo m_ResInfo;
		static TResInfo m_SharedResInfo;
	private:
		CStdPtrArray* GetFoundControls();
		static CControlUI* CALLBACK __FindControlFromNameHash(CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromCount(CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromTab(CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromShortcut(CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlFromClass(CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlsFromClass(CControlUI* pThis, LPVOID pData);
		static CControlUI* CALLBACK __FindControlsFromUpdate(CControlUI* pThis, LPVOID pData);

		static void AdjustSharedImagesHSL();
		void AdjustImagesHSL();
		void PostAsyncNotify();
	private:
		QkString m_sName;
		HWND m_hWndPaint;	//所附加的窗体的句柄
		HDC m_hDcPaint;
		HDC m_hDcOffscreen;
		HDC m_hDcBackground;
		HBITMAP m_hbmpOffscreen;
		BYTE* m_pOffscreenBits;
		HBITMAP m_hbmpBackground;
		COLORREF* m_pBackgroundBits;

		// 提示信息
		HWND m_hwndTooltip;
		TOOLINFO m_ToolTip;
		int m_iHoverTime;
		bool m_bNoActivate;
		bool m_bShowUpdateRect;

		//
		CControlUI* m_pRoot;
		CControlUI* m_pFocus;
		CControlUI* m_pEventHover;
		CControlUI* m_pEventClick;
		CControlUI* m_pEventKey;
		CControlUI* m_pLastToolTip;
		//
		SIZE m_szMinWindow;
		SIZE m_szMaxWindow;
		SIZE m_szInitWindowSize;
		RECT m_rcSizeBox;
		SIZE m_szRoundCorner;
		RECT m_rcCaption;
		UINT m_uTimerID;
		bool m_bFirstLayout;
		bool m_bUpdateNeeded;
		bool m_bFocusNeeded;
		bool m_bOffscreenPaint;
		
		BYTE m_nOpacity;
		bool m_bLayered;
		RECT m_rcLayeredInset;
		bool m_bLayeredChanged;
		RECT m_rcLayeredUpdate;
		RECT _rcPaint;
		TDrawInfo m_diLayered;

		bool m_bMouseTracking;
		bool m_bMouseCapture;
		bool m_bIsPainting;
		bool m_bUsedVirtualWnd;
		bool m_bAsyncNotifyPosted;

		//
		CStdPtrArray m_aNotifiers;
		CStdPtrArray m_aTimers;
		CStdPtrArray m_aTranslateAccelerator;
		CStdPtrArray m_aPreMessageFilters;
		CStdPtrArray m_aMessageFilters;
		CStdPtrArray m_aPostPaintControls;
		CStdPtrArray m_aNativeWindow;
		CStdPtrArray m_aNativeWindowControl;
		CStdPtrArray m_aDelayedCleanup;
		CStdPtrArray m_aAsyncNotify;
		CStdPtrArray m_aFoundControls;
		CStdPtrArray m_aFonts;
		CStdPtrArray m_aNeedMouseLeaveNeeded;
		QkStringPtrMap m_mNameHash;
		QkStringPtrMap m_mWindowCustomAttrHash;
		QkStringPtrMap m_mOptionGroup;
		QkStringPtrMap m_mEffectsStyle;
		
		bool m_bForceUseSharedRes;
		CPaintManagerUI* _parent;
		
		// 窗口阴影
		CShadowUI m_shadow;
		
		// DPI管理器
		CDPI* m_pDPI;
		// 是否开启Gdiplus
		bool m_bUseGdiplusText;
		int m_trh;
		ULONG_PTR m_gdiplusToken;
		Gdiplus::GdiplusStartupInput *m_pGdiplusStartupInput;

		// 拖拽
		bool m_bDragDrop;
		bool m_bDragMode;
		HBITMAP m_hDragBitmap;

		bool _bAllowAutoFocus = false;
		
		//
		static HINSTANCE m_hInstance;
		static HINSTANCE m_hResourceInstance;
		static QkString m_pStrResourcePath;
		static QkString m_pStrResourceZip;
		static QkString m_pStrResourceZipPwd;
		static HANDLE m_hResourceZip;
		static bool m_bCachedResourceZip;
		static int m_nResType;
		static bool m_bUseHSL;
		static short m_H;
		static short m_S;
		static short m_L;
		static CStdPtrArray m_aPreMessages;
		static CStdPtrArray m_aPlugins;

#ifdef SKIA_RENDERER
		sk_sp<SkSurface> _skSurf;
		SkCanvas* _skCanvas;
		SkPaint _fillpaint;
#endif
	};

} // namespace DuiLib

#endif // __UIMANAGER_H__
