#include "StdAfx.h"
#include <zmouse.h>

#include "UIDxAnimation.h"
#include "InsituDebug.h"



//#define QKUI_BENCHMARK

#ifdef QKUI_BENCHMARK
#include <chrono>
#endif QKUI_BENCHMARK

DWORD lastDrawTm;
int drawCnt = 0 ;

#undef m_bUpdateNeeded

namespace DuiLib {
	#define WM_EFFECTS	WM_USER+1680
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
#ifdef QKUI_BENCHMARK
	long bechmark_drawCount=0;
	double bechmark_drawThisTime=0;
	double bechmark_drawThisTimeMax=0;
	double bechmark_drawThisTimeMin=0xFFFFFF;
	double bechmark_drawTotalTime=0;
	LONG bechmark_drawTotalTimeV2Start=0;
	bool bechmark_registered = false;
	QkString bechmark_text;
	HWND hMainTestWnd;
	DWORD WINAPI BenchmarkTest(LPVOID p)
	{	
		while(::IsWindow(hMainTestWnd)) 
		{
			//PostMessage(Wnd, WM_COMMAND, 12323283, 0);
			InvalidateRect(hMainTestWnd, NULL, FALSE);
			//Sleep(8);
		}
		return 0;
	}

	class BenchLabelView : public CLabelUI
	{
	public:
		static CControlUI* CreateControl() 
		{
			return new BenchLabelView;
		}
		QkString & GetText()
		{
			return bechmark_text;
		}
	};
	class benchmeasuredraw
	{
	public:
		benchmeasuredraw()
		{
			drawSt = std::chrono::high_resolution_clock::now();
			if (bechmark_drawTotalTimeV2Start==0)
			{
				bechmark_drawTotalTimeV2Start=::GetTickCount();
			}
		}
		~benchmeasuredraw()
		{
			bechmark_drawCount ++ ;
			bechmark_drawThisTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - drawSt).count()*1000;
			bechmark_drawTotalTime += bechmark_drawThisTime;
			if (bechmark_drawThisTime>bechmark_drawThisTimeMax)
			{
				bechmark_drawThisTimeMax = bechmark_drawThisTime;
			}
			if (bechmark_drawThisTime<bechmark_drawThisTimeMin)
			{
				bechmark_drawThisTimeMin = bechmark_drawThisTime;
			}
			float fps2 = bechmark_drawCount*1000.f/(::GetTickCount()-bechmark_drawTotalTimeV2Start);
			float fps1 = bechmark_drawCount*1000/bechmark_drawTotalTime;
			float fps = 1000/bechmark_drawThisTime;
			bechmark_text.AsBuffer();
			bechmark_text.Format(L"%3.2fms %03.2f/%03.2f %03.2ffps %.2ffps~ %.2ffps~2", bechmark_drawThisTime, bechmark_drawThisTimeMin, bechmark_drawThisTimeMax, fps, fps1, fps2);
		}
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock>  drawSt;
	};
#endif

	static void GetChildWndRect(HWND hWnd, HWND hChildWnd, RECT& rcChildWnd)
	{
		::GetWindowRect(hChildWnd, &rcChildWnd);

		POINT pt;
		pt.x = rcChildWnd.left;
		pt.y = rcChildWnd.top;
		::ScreenToClient(hWnd, &pt);
		rcChildWnd.left = pt.x;
		rcChildWnd.top = pt.y;

		pt.x = rcChildWnd.right;
		pt.y = rcChildWnd.bottom;
		::ScreenToClient(hWnd, &pt);
		rcChildWnd.right = pt.x;
		rcChildWnd.bottom = pt.y;
	}

	static UINT MapKeyState()
	{
		UINT uState = 0;
		if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
		if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_LBUTTON;
		if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_RBUTTON;
		if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
		if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
		return uState;
	}

	typedef struct tagFINDTABINFO
	{
		CControlUI* pFocus;
		CControlUI* pLast;
		bool bForward;
		bool bNextIsIt;
	} FINDTABINFO;

	typedef struct tagFINDSHORTCUT
	{
		TCHAR ch;
		bool bPickNext;
	} FINDSHORTCUT;

	typedef struct tagTIMERINFO
	{
		CControlUI* pSender;
		UINT nLocalID;
		HWND hWnd;
		UINT uWinTimer;
		bool bKilled;
	} TIMERINFO;


	/////////////////////////////////////////////////////////////////////////////////////
    CAnimationSpooler m_anim;


	tagTDrawInfo::tagTDrawInfo()
	{
		Clear();
	}

	void tagTDrawInfo::Parse(LPCTSTR pStrImage,	CPaintManagerUI *pManager)
	{
		// 1、aaa.jpg
		// 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
		// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'
		// size='25,25' align='left' padding='0,0,0,0' by DuiLib_Ultimate
		//sDrawString = pStrImage;
		// sDrawModify = pStrModify;
		sName = pStrImage;
		//LogIs(L"Parse=%s", STR(sName));

		QkString sItem;
		QkString sValue;
		LPTSTR pstr = NULL;
		//for( int i = 0; i < 2; ++i ) 
		{
			//if( i == 1) pStrImage = pStrModify;
			if( pStrImage )
			while( *pStrImage != _T('\0') ) {
				sItem.Empty();
				sValue.Empty();
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
					LPTSTR pstrTemp = ::CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sItem += *pStrImage++;
					}
				}
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				if( *pStrImage++ != _T('=') ) break;
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				if( *pStrImage++ != _T('\'') ) break;
				while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
					LPTSTR pstrTemp = ::CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sValue += *pStrImage++;
					}
				}
				if( *pStrImage++ != _T('\'') ) break;
				//LogIs(L"sItem=%s sValue=%s %d", STR(sItem), STR(sValue), 0);
				if( !sValue.IsEmpty() ) {
					if( sItem == _T("file") || sItem == _T("res") ) {
						sName = sValue;
					}
					else if( sItem == _T("restype") ) {
						sResType = sValue;
					}
					else if( sItem == _T("dest") ) {
						int left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
						int top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
						int right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
						int bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);  
						szIcon.cx = right - left;
						szIcon.cy = bottom - top;
						iAlign = GRAVITY_LEFT|GRAVITY_VCENTER;
					}
					else if( sItem == _T("source") ) {
						rcSource.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
						rcSource.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
						rcSource.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
						rcSource.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
						//if(pManager != NULL) pManager->GetDPIObj()->Scale(&rcSource);
					}
					else if( sItem == _T("corner") ) {
						rcCorner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
						rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
						rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
						rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
						//if(pManager != NULL) pManager->GetDPIObj()->Scale(&rcCorner);
					}
					else if( sItem == _T("mask") ) {
						if( sValue[0] == _T('#')) dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
						else dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
					}
					else if( sItem == _T("fade") ) {
						uFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
					}
					else if( sItem == _T("hole") ) {
						bHole = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
					}
					else if( sItem == _T("xtiled") ) {
						bTiledX = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
					}
					else if( sItem == _T("ytiled") ) {
						bTiledY = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
					}
					else if( sItem == _T("hsl") ) {
						bHSL = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
					}
					else if( sItem == _T("size") ) {
						szIcon.cx = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
						szIcon.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					}
					else if( sItem == _T("align") ) {
						// todo optimize by separate by '|'
						//LogIs(2, L"%s", STR(sValue));
						if(sValue.Find(_T("center")) != -1) {
							iAlign = GRAVITY_CENTER;
						}
						else {
							if(sValue.Find(_T("hcenter")) != -1) {
								iAlign |= GRAVITY_HCENTER;
							}
							else if(sValue.Find(_T("left")) != -1) {
								iAlign |= GRAVITY_LEFT;
							}
							else if(sValue.Find(_T("right")) != -1) {
								iAlign |= GRAVITY_RIGHT;
							}
							if(sValue.Find(_T("vcenter")) != -1) {
								iAlign |= GRAVITY_VCENTER;
							}
							else if(sValue.Find(_T("top")) != -1) {
								iAlign |= GRAVITY_TOP;
							}
							else if(sValue.Find(_T("bottom")) != -1) {
								iAlign |= GRAVITY_BOTTOM;
							}
						}
					}
					else if( sItem == _T("padding") ) {
						rcPadding.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
						rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
						rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
						rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);  
						//if(pManager != NULL) pManager->GetDPIObj()->Scale(&rcPadding);
					}
				}
				if( *pStrImage++ != _T(' ') ) break;
			}
		}

		//TCHAR buffer[100]={0};
		//wsprintf(buffer,TEXT("position=%s"), sName);
		//::MessageBox(NULL, buffer, TEXT(""), MB_OK);

		//// 调整DPI资源
		//if (pManager != NULL && pManager->GetDPIObj()->GetScale() != 100) {
		//	QkString sScale;
		//	sScale.Format(_T("@%d."), pManager->GetDPIObj()->GetScale());
		//	sName.Replace(_T("."), sScale);
		//}
	}

	void tagTDrawInfo::Clear()
	{
		//sDrawString.Empty();
		//sDrawModify.Empty();
		sName.Empty();

		//memset(&rcDest, 0, sizeof(RECT));
		memset(&rcSource, 0, sizeof(RECT));
		memset(&rcCorner, 0, sizeof(RECT));
		dwMask = 0;
		uFade = 255;
		bHole = false;
		bTiledX = false;
		bTiledY = false;
		bHSL = false;

		szIcon.cx = szIcon.cy = 0;
		iAlign = 0;
		memset(&rcPadding, 0, sizeof(RECT));
	}

	/////////////////////////////////////////////////////////////////////////////////////
	typedef BOOL (__stdcall *PFUNCUPDATELAYEREDWINDOW)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);
	PFUNCUPDATELAYEREDWINDOW g_fUpdateLayeredWindow = NULL;

	HPEN m_hUpdateRectPen = NULL;

	HINSTANCE CPaintManagerUI::m_hResourceInstance = NULL;
	QkString CPaintManagerUI::m_pStrResourcePath;
	QkString CPaintManagerUI::m_pStrResourceZip;
	QkString CPaintManagerUI::m_pStrResourceZipPwd;  //Garfield 20160325 带密码zip包解密
	HANDLE CPaintManagerUI::m_hResourceZip = NULL;
	bool CPaintManagerUI::m_bCachedResourceZip = true;
	int CPaintManagerUI::m_nResType = UILIB_FILE;
	TResInfo CPaintManagerUI::m_SharedResInfo;
	HINSTANCE CPaintManagerUI::m_hInstance = NULL;
	bool CPaintManagerUI::m_bUseHSL = false;
	short CPaintManagerUI::m_H = 180;
	short CPaintManagerUI::m_S = 100;
	short CPaintManagerUI::m_L = 100;
	CStdPtrArray CPaintManagerUI::m_aPreMessages;
	CStdPtrArray CPaintManagerUI::m_aPlugins;

	HCURSOR CPaintManagerUI::hCursorArrow = 0;
	HCURSOR CPaintManagerUI::hCursorHand = 0;

	CPaintManagerUI::CPaintManagerUI() :
		m_hWndPaint(NULL),
		m_hDcPaint(NULL),
		m_hDcOffscreen(NULL),
		m_hDcBackground(NULL),
		m_bOffscreenPaint(true),
		m_hbmpOffscreen(NULL),
		m_pOffscreenBits(NULL),
		m_hbmpBackground(NULL),
		m_pBackgroundBits(NULL),
		m_hwndTooltip(NULL),
		m_uTimerID(0x1000),
		m_pRoot(NULL),
		m_pFocus(NULL),
		_wndFocus(NULL),
		m_pEventHover(NULL),
		m_pEventClick(NULL),
		m_pEventKey(NULL),
		m_bFirstLayout(true),
		m_bFocusNeeded(false),
		m_bUpdateNeeded(false),
		m_bMouseTracking(false),
		m_bMouseCapture(false),
		m_bAsyncNotifyPosted(false),
		m_bUsedVirtualWnd(false),
		m_bForceUseSharedRes(false),
		m_nOpacity(0xFF),
		m_bLayered(false),
		m_bLayeredChanged(false),
		m_bShowUpdateRect(false),
		m_bUseGdiplusText(false),
		m_trh(0),
		m_bDragDrop(false),
		m_bDragMode(false),
		m_hDragBitmap(NULL),
		m_pDPI(NULL)
		,m_iHoverTime(400UL)
#ifdef MODULE_SKIA_RENDERER
		,_skCanvas(nullptr)
#endif
		,_parent(nullptr)
	{
		if (m_SharedResInfo.m_DefaultFontInfo.sFontName.IsEmpty())
		{
			m_SharedResInfo.m_dwDefaultDisabledColor = 0xFFA7A6AA;
			m_SharedResInfo.m_dwDefaultFontColor = 0xFF000000;
			m_SharedResInfo.m_dwDefaultLinkFontColor = 0xFF0000FF;
			m_SharedResInfo.m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
			m_SharedResInfo.m_dwDefaultSelectedBkColor = 0xFFBAE4FF;

			LOGFONT lf = { 0 };
			::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
			lf.lfCharSet = DEFAULT_CHARSET;
			HFONT hDefaultFont = ::CreateFontIndirect(&lf);
			m_SharedResInfo.m_DefaultFontInfo.hFont = hDefaultFont;
			m_SharedResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
			m_SharedResInfo.m_DefaultFontInfo.iSize = -lf.lfHeight;
			m_SharedResInfo.m_DefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
			m_SharedResInfo.m_DefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
			m_SharedResInfo.m_DefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
			::ZeroMemory(&m_SharedResInfo.m_DefaultFontInfo.tm, sizeof(m_SharedResInfo.m_DefaultFontInfo.tm));
		}

		m_ResInfo.m_dwDefaultDisabledColor = m_SharedResInfo.m_dwDefaultDisabledColor;
		m_ResInfo.m_dwDefaultFontColor = m_SharedResInfo.m_dwDefaultFontColor;
		m_ResInfo.m_dwDefaultLinkFontColor = m_SharedResInfo.m_dwDefaultLinkFontColor;
		m_ResInfo.m_dwDefaultLinkHoverFontColor = m_SharedResInfo.m_dwDefaultLinkHoverFontColor;
		m_ResInfo.m_dwDefaultSelectedBkColor = m_SharedResInfo.m_dwDefaultSelectedBkColor;

		if( m_hUpdateRectPen == NULL ) {
			m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
			// Boot Windows Common Controls (for the ToolTip control)
			::InitCommonControls();
			::LoadLibrary(_T("msimg32.dll"));
		}

		m_szMinWindow.cx = 0;
		m_szMinWindow.cy = 0;
		m_szMaxWindow.cx = 0;
		m_szMaxWindow.cy = 0;
		m_szInitWindowSize.cx = 0;
		m_szInitWindowSize.cy = 0;
		m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
		::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
		::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
		::ZeroMemory(&m_rcLayeredInset, sizeof(m_rcLayeredInset));
		::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
		m_ptLastMousePos.x = m_ptLastMousePos.y = -1;

		m_pGdiplusStartupInput = new Gdiplus::GdiplusStartupInput;
		Gdiplus::GdiplusStartup( &m_gdiplusToken, m_pGdiplusStartupInput, NULL); // 加载GDI接口

		CShadowUI::Initialize(m_hInstance);

#ifdef QKUI_BENCHMARK
		if (!bechmark_registered)
		{
			CControlFactory::GetInstance()->RegistControl(L"BenchLabel", BenchLabelView::CreateControl);
		}
#endif
		LogIs("CPaintManagerUI::新建 %lxd", this);
	}

	// 释放缓存，准备更新预览
	void CPaintManagerUI::reInit()
	{
		RemoveAllFonts(0);
		RemoveAllFonts(1);
		RemoveAllImages(0);
		RemoveAllImages(1);
		//RemoveAllStyle(0);
		//RemoveAllStyle(1);
		RemoveAllDefaultAttributeList(0);
		RemoveAllDefaultAttributeList(1);
		RemoveAllWindowCustomAttribute();
		RemoveAllOptionGroups();
		RemoveAllTimers();
		//RemoveAllDrawInfos();

		//m_ResInfo = {};

		//m_ResInfo.m_DefaultFontInfo = {};
		//m_SharedResInfo.m_DefaultFontInfo = {};
		
		
		//m_ResInfo.m_CustomFonts = {};
		//m_ResInfo.m_ImageHash = {};
		//m_ResInfo.m_AttrHash = {};
		//m_ResInfo.m_StyleHash = {};
		//m_ResInfo.m_DrawInfoHash = {};
	}

	// Dangerouse
	void CPaintManagerUI::reInit(CPaintManagerUI* parent)
	{
		LogIs("CPaintManagerUI::再初始化 %lxd = %lxd", this, parent);
		if (parent)
		{
			parent = parent->GetRealManager();
			if(parent==this) return;
			if (_parent!=parent)
			{
				RemoveAllFonts(0);
				RemoveAllImages(0);
				RemoveAllDrawInfos();
				RemoveAllDefaultAttributeList(0);
				RemoveAllWindowCustomAttribute();
			}
			m_ResInfo = parent->m_ResInfo;
			_parent = parent;
		}
	}

	void CPaintManagerUI::SetParent(CPaintManagerUI* parent)
	{
		if(parent!=this)
			_parent = parent;
	}

	CPaintManagerUI* CPaintManagerUI::GetRealManager()
	{
		CPaintManagerUI* ret = this;
		if(_parent) {
			ret = _parent;
			while(ret->_parent) 
				ret = ret->_parent;
		}
		return ret;
	}

	CPaintManagerUI::~CPaintManagerUI()
	{
		LogIs("CPaintManagerUI::解构 %lxd", this);
		//if(1) return;
		// Delete the control-tree structures
		for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
		m_aDelayedCleanup.Resize(0);
		for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
		m_aAsyncNotify.Resize(0);

		m_mNameHash.Resize(0);
		if( m_pRoot ) 
		{
			delete m_pRoot;
			m_pRoot = NULL;
		}

		::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
		RemoveAllFonts();
		RemoveAllImages();
		RemoveAllDefaultAttributeList();
		RemoveAllWindowCustomAttribute();
		RemoveAllDrawInfos();
		RemoveAllOptionGroups();
		RemoveAllTimers();
		if (!m_aFonts.IsEmpty()) {
			for (int i = 0; i < m_aFonts.GetSize();++i)
			{
				::RemoveFontMemResourceEx(static_cast<HANDLE>(m_aFonts.GetAt(i)));
			}
		}

		if( m_hwndTooltip != NULL ) {
			::DestroyWindow(m_hwndTooltip);
			m_hwndTooltip = NULL;
		}
		if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
		if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
		if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
		if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
		if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
		m_aPreMessages.Remove(m_aPreMessages.Find(this));
		// 销毁拖拽图片
		if( m_hDragBitmap != NULL ) ::DeleteObject(m_hDragBitmap);
		//卸载GDIPlus
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
		delete m_pGdiplusStartupInput;
		// DPI管理对象
		if (m_pDPI != NULL) {
			delete m_pDPI;
			m_pDPI = NULL;
		}
	}

	void CPaintManagerUI::Init(HWND hWnd, LPCTSTR pstrName)
	{
		ASSERT(::IsWindow(hWnd));

		m_mNameHash.Resize();
		RemoveAllFonts();
		RemoveAllImages();
		RemoveAllDefaultAttributeList();
		RemoveAllWindowCustomAttribute();
		RemoveAllOptionGroups();
		RemoveAllTimers();

		m_sName.Empty();
		if( pstrName != NULL ) m_sName = pstrName;

		if( m_hWndPaint != hWnd ) {
			m_hWndPaint = hWnd;
			m_hDcPaint = ::GetDC(hWnd);
			m_aPreMessages.Add(this);
		}
	}

	void CPaintManagerUI::SetAllowAutoFocus(bool bAllowAutoFocus)
	{
		_bAllowAutoFocus = bAllowAutoFocus;
	}

	void CPaintManagerUI::DeletePtr(void* ptr)
	{
		if(ptr) {delete ptr; ptr = NULL;}
	}

	HINSTANCE CPaintManagerUI::GetInstance()
	{
		return m_hInstance;
	}

	QkString CPaintManagerUI::GetInstancePath()
	{
		if( m_hInstance == NULL ) return _T('\0');

		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
		QkString sInstancePath = tszModule;
		int pos = sInstancePath.ReverseFind(_T('\\'));
		if( pos >= 0 ) sInstancePath = sInstancePath.Left(pos + 1);
		return sInstancePath;
	}

	QkString CPaintManagerUI::GetCurrentPath()
	{
		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetCurrentDirectory(MAX_PATH, tszModule);
		return tszModule;
	}

	HINSTANCE CPaintManagerUI::GetResourceDll()
	{
		if( m_hResourceInstance == NULL ) return m_hInstance;
		return m_hResourceInstance;
	}

	const QkString& CPaintManagerUI::GetResourcePath()
	{
		return m_pStrResourcePath;
	}

	const QkString& CPaintManagerUI::GetResourceZip()
	{
		return m_pStrResourceZip;
	}

	const QkString& CPaintManagerUI::GetResourceZipPwd()
	{
		return m_pStrResourceZipPwd;
	}

	bool CPaintManagerUI::IsCachedResourceZip()
	{
		return m_bCachedResourceZip;
	}

	bool CPaintManagerUI::ExtractItem(const TCHAR* name, CHAR** outData, DWORD & dataLen)
	{

		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		QkString sFile = CPaintManagerUI::GetResourcePath();
		if( CPaintManagerUI::GetResourceZip().IsEmpty() ) {
			sFile += name;
			HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) return false;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) { ::CloseHandle( hFile ); return false; }

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
				return false;
			}
			*outData = (CHAR*)pData;
			dataLen = dwSize;
		}
		else {
			sFile += CPaintManagerUI::GetResourceZip();
			QkString sFilePwd = CPaintManagerUI::GetResourceZipPwd();
			HZIP hz = NULL;
			if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
			else
			{
#ifdef UNICODE
				char* pwd = w2a((wchar_t*)sFilePwd.GetData());
				hz = OpenZip(sFile.GetData(), pwd);
				if(pwd) delete[] pwd;
#else
				hz = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
			}
			if( hz == NULL ) return false;
			ZIPENTRY ze; 
			int i = 0; 
			QkString key = name;
			key.Replace(_T("\\"), _T("/"));
			if( FindZipItem(hz, key, true, &i, &ze) != 0 ) return false;
			dwSize = ze.unc_size;
			if( dwSize == 0 ) { if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz); return false; }
			pData = new BYTE[ dwSize ];
			int res = UnzipItem(hz, i, pData, dwSize);
			if( res != 0x00000000 && res != 0x00000600) {
				delete[] pData;
				pData = NULL;
				if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
				return false;
			}
			*outData = (CHAR*)pData;
			dataLen = dwSize;
			if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
		}

		return true;
	}

	HANDLE CPaintManagerUI::GetResourceZipHandle()
	{
		return m_hResourceZip;
	}

	void CPaintManagerUI::SetInstance(HINSTANCE hInst)
	{
		m_hInstance = hInst;
		hCursorArrow = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
		hCursorHand = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
	}

	void CPaintManagerUI::SetCurrentPath(LPCTSTR pStrPath)
	{
		::SetCurrentDirectory(pStrPath);
	}

	void CPaintManagerUI::SetResourceDll(HINSTANCE hInst)
	{
		m_hResourceInstance = hInst;
	}

	void CPaintManagerUI::SetResourcePath(LPCTSTR pStrPath)
	{
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
		m_pStrResourceZip = L"";
		m_bCachedResourceZip = false;
		m_pStrResourcePath = pStrPath;
		if( m_pStrResourcePath.IsEmpty() ) return;
		TCHAR cEnd = m_pStrResourcePath.GetAt(m_pStrResourcePath.GetLength() - 1);
		if( cEnd != _T('\\') && cEnd != _T('/') ) m_pStrResourcePath += _T('\\');
	}

	void CPaintManagerUI::SetResourceZip(LPVOID pVoid, unsigned int len, LPCTSTR password)
	{
		if( m_pStrResourceZip == _T("membuffer") ) return;
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
		m_pStrResourceZip = _T("membuffer");
		m_bCachedResourceZip = true;
		m_pStrResourceZipPwd = password;  //Garfield 20160325 带密码zip包解密
		if( m_bCachedResourceZip ) 
		{
#ifdef UNICODE
			char* pwd = w2a((wchar_t*)password);
			m_hResourceZip = (HANDLE)OpenZip(pVoid, len, pwd);
			if(pwd) {
				delete[] pwd;
				pwd = NULL;
			}
#else
			m_hResourceZip = (HANDLE)OpenZip(pVoid, len, password);
#endif
		}
	}

	void CPaintManagerUI::SetResourceZip(LPCTSTR pStrPath, bool bCachedResourceZip, LPCTSTR password)
	{
		if( m_pStrResourceZip == pStrPath && m_bCachedResourceZip == bCachedResourceZip ) return;
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
		m_pStrResourceZip = pStrPath;
		m_bCachedResourceZip = bCachedResourceZip;
		m_pStrResourceZipPwd = password;
		if( m_bCachedResourceZip ) {
			QkString sFile = CPaintManagerUI::GetResourcePath();
			sFile += CPaintManagerUI::GetResourceZip();
#ifdef UNICODE
			char* pwd = w2a((wchar_t*)password);
			m_hResourceZip = (HANDLE)OpenZip(sFile.GetData(), pwd);
			if(pwd) {
				delete[] pwd;
				pwd = NULL;
			}
#else
			m_hResourceZip = (HANDLE)OpenZip(sFile.GetData(), password);
#endif
		}
	}

	void CPaintManagerUI::SetResourceType(int nType)
	{
		m_nResType = nType;
	}

	int CPaintManagerUI::GetResourceType()
	{
		return m_nResType;
	}

	bool CPaintManagerUI::GetHSL(short* H, short* S, short* L)
	{
		*H = m_H;
		*S = m_S;
		*L = m_L;
		return m_bUseHSL;
	}

	void CPaintManagerUI::SetHSL(bool bUseHSL, short H, short S, short L)
	{
		if( m_bUseHSL || m_bUseHSL != bUseHSL ) {
			m_bUseHSL = bUseHSL;
			if( H == m_H && S == m_S && L == m_L ) return;
			m_H = CLAMP(H, 0, 360);
			m_S = CLAMP(S, 0, 200);
			m_L = CLAMP(L, 0, 200);
			AdjustSharedImagesHSL();
			for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
				CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
				if( pManager != NULL ) pManager->AdjustImagesHSL();
			}
		}
	}

	void CPaintManagerUI::ReloadSkin()
	{
		ReloadSharedImages();
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
			CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			pManager->ReloadImages();
		}
	}

	CPaintManagerUI* CPaintManagerUI::GetPaintManager(LPCTSTR pstrName)
	{
		if( pstrName == NULL ) return NULL;
		QkString sName = pstrName;
		if( sName.IsEmpty() ) return NULL;
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
			CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			if( pManager != NULL && sName == pManager->GetName() ) return pManager;
		}
		return NULL;
	}

	CStdPtrArray* CPaintManagerUI::GetPaintManagers()
	{
		return &m_aPreMessages;
	}

	bool CPaintManagerUI::LoadPlugin(LPCTSTR pstrModuleName)
	{
		ASSERT( !::IsBadStringPtr(pstrModuleName,-1) || pstrModuleName == NULL );
		if( pstrModuleName == NULL ) return false;
		HMODULE hModule = ::LoadLibrary(pstrModuleName);
		if( hModule != NULL ) {
			LPCREATECONTROL lpCreateControl = (LPCREATECONTROL)::GetProcAddress(hModule, "CreateControl");
			if( lpCreateControl != NULL ) {
				if( m_aPlugins.Find(lpCreateControl) >= 0 ) return true;
				m_aPlugins.Add(lpCreateControl);
				return true;
			}
		}
		return false;
	}

	CStdPtrArray* CPaintManagerUI::GetPlugins()
	{
		return &m_aPlugins;
	}

	HWND CPaintManagerUI::GetPaintWindow() const
	{
		return m_hWndPaint;
	}

	HWND CPaintManagerUI::GetTooltipWindow() const
	{
		return m_hwndTooltip;
	}
	int CPaintManagerUI::GetHoverTime() const
	{
		return m_iHoverTime;
	}

	void CPaintManagerUI::SetHoverTime(int iTime)
	{
		m_iHoverTime = iTime;
	}

	LPCTSTR CPaintManagerUI::GetName() const
	{
		return m_sName;
	}

	HDC CPaintManagerUI::GetPaintDC() const
	{
		return m_hDcPaint;
	}

	POINT CPaintManagerUI::GetMousePos() const
	{
		return m_ptLastMousePos;
	}

	SIZE CPaintManagerUI::GetClientSize() const
	{
		RECT rcClient = { 0 };
		::GetClientRect(m_hWndPaint, &rcClient);
		return CDuiSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	}

	SIZE CPaintManagerUI::GetInitSize()
	{
		return m_szInitWindowSize;
	}

	void CPaintManagerUI::SetInitSize(int cx, int cy)
	{
		m_szInitWindowSize.cx = cx;
		m_szInitWindowSize.cy = cy;
		if( m_pRoot == NULL && m_hWndPaint != NULL ) {
			::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}

	RECT& CPaintManagerUI::GetSizeBox()
	{
		return m_rcSizeBox;
	}

	void CPaintManagerUI::SetSizeBox(RECT& rcSizeBox)
	{
		m_rcSizeBox = rcSizeBox;
	}

	RECT& CPaintManagerUI::GetCaptionRect()
	{
		return m_rcCaption;
	}

	void CPaintManagerUI::SetCaptionRect(RECT& rcCaption)
	{
		m_rcCaption = rcCaption;
	}

	SIZE CPaintManagerUI::GetRoundCorner() const
	{
		return m_szRoundCorner;
	}

	void CPaintManagerUI::SetRoundCorner(int cx, int cy)
	{
		m_szRoundCorner.cx = cx;
		m_szRoundCorner.cy = cy;
	}

	SIZE CPaintManagerUI::GetMinInfo() const
	{
		return m_szMinWindow;
	}

	void CPaintManagerUI::SetMinInfo(int cx, int cy)
	{
		ASSERT(cx>=0 && cy>=0);
		m_szMinWindow.cx = cx;
		m_szMinWindow.cy = cy;
	}

	SIZE CPaintManagerUI::GetMaxInfo() const
	{
		return m_szMaxWindow;
	}

	void CPaintManagerUI::SetMaxInfo(int cx, int cy)
	{
		ASSERT(cx>=0 && cy>=0);
		m_szMaxWindow.cx = cx;
		m_szMaxWindow.cy = cy;
	}

	bool CPaintManagerUI::IsShowUpdateRect() const
	{
		return m_bShowUpdateRect;
	}

	void CPaintManagerUI::SetShowUpdateRect(bool show)
	{
		m_bShowUpdateRect = show;
	}

	bool CPaintManagerUI::IsNoActivate()
	{
		return m_bNoActivate;
	}

	void CPaintManagerUI::SetNoActivate(bool bNoActivate)
	{
		m_bNoActivate = bNoActivate;
	}

	BYTE CPaintManagerUI::GetOpacity() const
	{
		return m_nOpacity;
	}

	void CPaintManagerUI::SetOpacity(BYTE nOpacity)
	{
		m_nOpacity = nOpacity;
		if( m_hWndPaint != NULL ) {
			typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
			PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes = NULL;

			HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
			if (hUser32)
			{
				fSetLayeredWindowAttributes = 
					(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
				if( fSetLayeredWindowAttributes == NULL ) return;
			}

			DWORD dwStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
			DWORD dwNewStyle = dwStyle;
			if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
			else dwNewStyle &= ~WS_EX_LAYERED;
			if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
			fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);
		}
	}

	bool CPaintManagerUI::IsLayered()
	{
		return m_bLayered;
	}

	void CPaintManagerUI::SetLayered(bool bLayered)
	{
		if( m_hWndPaint != NULL && bLayered != m_bLayered ) {
			UINT uStyle = GetWindowStyle(m_hWndPaint);
			if( (uStyle & WS_CHILD) != 0 ) return;
			if( g_fUpdateLayeredWindow == NULL ) {
				HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
				if (hUser32) {
					g_fUpdateLayeredWindow = 
						(PFUNCUPDATELAYEREDWINDOW)::GetProcAddress(hUser32, "UpdateLayeredWindow");
					if( g_fUpdateLayeredWindow == NULL ) return;
				}
			}
			m_bLayered = bLayered;
			if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
			Invalidate();
		}
	}

	RECT& CPaintManagerUI::GetLayeredInset()
	{
		return m_rcLayeredInset;
	}

	void CPaintManagerUI::SetLayeredInset(RECT& rcLayeredInset)
	{
		m_rcLayeredInset = rcLayeredInset;
		m_bLayeredChanged = true;
		Invalidate();
	}

	BYTE CPaintManagerUI::GetLayeredOpacity()
	{
		return m_nOpacity;
	}

	void CPaintManagerUI::SetLayeredOpacity(BYTE nOpacity)
	{
		m_nOpacity = nOpacity;
		m_bLayeredChanged = true;
		Invalidate();
	}

	LPCTSTR CPaintManagerUI::GetLayeredImage()
	{
		return m_diLayered.sName;
	}

	void CPaintManagerUI::SetLayeredImage(LPCTSTR pstrImage)
	{
		//m_diLayered.sDrawString = pstrImage;
		m_diLayered.Parse(pstrImage, this);
		RECT rcNull = {0};
		CRenderEngine::DrawImageInfo(NULL, this, rcNull, rcNull, &m_diLayered);
		m_bLayeredChanged = true;
		Invalidate();
	}

	CShadowUI* CPaintManagerUI::GetShadow()
	{
		return &m_shadow;
	}

	void CPaintManagerUI::SetUseGdiplusText(bool bUse)
	{
		m_bUseGdiplusText = bUse;
	}

	bool CPaintManagerUI::IsUseGdiplusText() const
	{
		return m_bUseGdiplusText;
	}

	void CPaintManagerUI::SetGdiplusTextRenderingHint(int trh)
	{
		m_trh = trh;
	}

	int CPaintManagerUI::GetGdiplusTextRenderingHint() const
	{
		return m_trh;
	}

	bool CPaintManagerUI::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
	{
		for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if( bHandled ) {
				return true;
			}
		}
		switch( uMsg ) {
		case WM_KEYDOWN:
			{
				// Tabbing between controls
				if( wParam == VK_TAB ) {
#ifdef QkUIRichEdit
					if( m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != NULL ) {
						if( static_cast<CRichEditUI*>(m_pFocus)->IsWantTab() ) return false;
					}
#endif
					if( m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("WkeWebkitUI")) != NULL ) {
						return false;
					}
					SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
					return true;
				}
			}
			break;
		case WM_SYSCHAR:
			{
				// Handle ALT-shortcut key-combinations
				FINDSHORTCUT fs = { 0 };
				fs.ch = toupper((int)wParam);
				CControlUI* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
				if( pControl != NULL ) {
					pControl->SetFocus();
					pControl->Activate();
					return true;
				}
			}
			break;
		case WM_SYSKEYDOWN:
			{
				if( m_pFocus != NULL ) {
					TEventUI event = { 0 };
					event.Type = UIEVENT_SYSKEY;
					event.chKey = (TCHAR)wParam;
					event.ptMouse = m_ptLastMousePos;
					event.wKeyState = MapKeyState();
					event.dwTimestamp = ::GetTickCount();
					m_pFocus->Event(event);
				}
			}
			break;
		}
		return false;
	}

	bool CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		if( m_hWndPaint == NULL ) return false;
		// Cycle through listeners
		for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if( bHandled ) {
				lRes = lResult;
				switch( uMsg ) {
				case WM_MOUSEMOVE:
				case WM_LBUTTONDOWN:
				case WM_LBUTTONDBLCLK:
				case WM_LBUTTONUP:
					{
						POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
						m_ptLastMousePos = pt;
					}
					break;
				case WM_CONTEXTMENU:
				case WM_MOUSEWHEEL:
					{
						POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
						::ScreenToClient(m_hWndPaint, &pt);
						m_ptLastMousePos = pt;
					}
					break;
				}
				return true;
			}
		}

		if( m_bLayered ) {
			switch( uMsg ) {
			case WM_NCACTIVATE:
				if( !::IsIconic(m_hWndPaint) ) {
					lRes = (wParam == 0) ? TRUE : FALSE;
					return true;
				}
				break;
			case WM_NCCALCSIZE:
			case WM_NCPAINT:
				lRes = 0;
				return true;
			}
		}
		// Custom handling of events
		switch( uMsg ) {
		case WM_APP + 1:
			{
				for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
					delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
				m_aDelayedCleanup.Empty();
				
				m_bAsyncNotifyPosted = false;

				TNotifyUI* pMsg = NULL;
				while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
					m_aAsyncNotify.Remove(0);
					if( pMsg->pSender != NULL ) {
						if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
					}
					for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
						static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
					}
					delete pMsg;
				}
			}
			break;
		case WM_CLOSE:
			{
				// Make sure all matching "closing" events are sent
				TEventUI event = { 0 };
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				if( m_pEventHover != NULL ) {
					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventHover;
					m_pEventHover->Event(event);
				}
				if( m_pEventClick != NULL ) {
					event.Type = UIEVENT_BUTTONUP;
					event.pSender = m_pEventClick;
					m_pEventClick->Event(event);
				}

				SetFocus(NULL);

				if( ::GetActiveWindow() == m_hWndPaint ) {
					HWND hwndParent = GetWindowOwner(m_hWndPaint);
					if( hwndParent != NULL ) ::SetFocus(hwndParent);
				}

				if (m_hwndTooltip != NULL) {
					::DestroyWindow(m_hwndTooltip);
					m_hwndTooltip = NULL;
				}
			}
			break;
		case WM_ERASEBKGND:
			{
				// We'll do the painting here...
				lRes = 1;
				if(0) {
					HBRUSH hbrWhite, hbrGray;

					hbrWhite = (HBRUSH)GetStockObject(WHITE_BRUSH); 
					hbrGray  = (HBRUSH)GetStockObject(GRAY_BRUSH); 

					HDC hdc = (HDC) wParam; 
					hdc = GetDC(m_pRoot->GetHWND()); 
					RECT rc = m_pRoot->GetPos();
					//GetClientRect(m_pRoot->, &rc); 
					//SetMapMode(hdc, MM_ANISOTROPIC); 
					//SetWindowExtEx(hdc, 100, 100, NULL); 
					//SetViewportExtEx(hdc, rc.right, rc.bottom, NULL); 
					FillRect(hdc, &rc, hbrWhite); 
				}
			}
			return true;
		case WM_EFFECTS:
		{
			if( m_anim.IsAnimating() )
			{
				// 3D animation in progress
				//   3D动画  
				int ret = m_anim.Render();

				// Do a minimum paint loop  做一个最小的绘制循环
				// Keep the client area invalid so we generate lots of
				// WM_PAINT messages. Cross fingers that Windows doesn't
				// batch these somehow in the future.
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWndPaint, &ps);
				::EndPaint(m_hWndPaint, &ps);
				::InvalidateRect(m_hWndPaint, NULL, FALSE);
			}
			else if( m_anim.IsJobScheduled() ) 
			{
				// Animation system needs to be initialized
				//	动画系统需要初始化
				m_anim.Init(m_hWndPaint);
				// A 3D animation was scheduled; allow the render engine to
				// capture the window content and repaint some other time
				//翻译(by 金山词霸)一个3d动画被准备;允许渲染引擎捕获窗口内容，并且适时重画

				if( !m_anim.PrepareAnimation(m_hWndPaint) ) m_anim.CancelJobs();
				::InvalidateRect(m_hWndPaint, NULL, TRUE);
			} 
		}
		case WM_PAINT:
		{
#ifdef QKUI_BENCHMARK
				if (!bechmark_registered)
				{
					HANDLE hThread;
					DWORD  threadId;
					hMainTestWnd = m_hWndPaint;
					hThread = CreateThread(NULL, 0,	BenchmarkTest, 0, 0, &threadId);
					bechmark_registered = true;
				}
#endif
				if(::IsIconic(m_hWndPaint) ) return false;
				if( m_pRoot == NULL ) 
				{
					PAINTSTRUCT ps = { 0 };
					::BeginPaint(m_hWndPaint, &ps);
					CRenderEngine::DrawColor(m_hDcPaint, ps.rcPaint, 0xFF000000);
					::EndPaint(m_hWndPaint, &ps);
					return true;
				}
				
				// Should we paint?
				RECT rcPaint = { 0 };
				if( !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE) ) return true;
				// todo why layered?
				//m_bLayered = false; // 设置 Layered 后导致 dx 动画无法显示。
				//m_bOffscreenPaint = false;

				// Set focus to first control?
				if( m_bFocusNeeded) {
					SetNextTabControl();
				}

				bool bNeedSizeMsg = false;

				RECT rcClient = { 0 };
				::GetClientRect(m_hWndPaint, &rcClient);

				//rcPaint.right/=2;
				//rcClient.right/=2;

				DWORD dwWidth = rcClient.right;
				DWORD dwHeight = rcClient.bottom;


				SetPainting(true);

				// Do we need to resize anything?
				// This is the time where we layout the controls on the form. | 此为布局之良机也
				// We delay this even from the WM_SIZE messages since resizing can be
				// a very expensize operation.
				if( m_bUpdateNeeded ) {
					m_bUpdateNeeded = false;
					if( !::IsRectEmpty(&rcClient))  //  && !::IsIconic(m_hWndPaint) 
					{
						if( m_pRoot->IsUpdateNeeded() ) 
						{
							RECT rcRoot = rcClient;
							// Reset offscreen device
							if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
							if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
							if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
							if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
							m_hDcOffscreen = NULL;
							m_hDcBackground = NULL;
							m_hbmpOffscreen = NULL;
							m_hbmpBackground = NULL;
							if( m_bLayered ) {
								rcRoot.left += m_rcLayeredInset.left;
								rcRoot.top += m_rcLayeredInset.top;
								rcRoot.right -= m_rcLayeredInset.right;
								rcRoot.bottom -= m_rcLayeredInset.bottom;
							}
							m_pRoot->SetPos(rcRoot, true);
							bNeedSizeMsg = true;
							if (_UpdateList.size())
							{
								_UpdateList.clear();
							}
						}
						else if(true) // WTF??
						{
							CControlUI* pControl = NULL;
							m_aFoundControls.Empty();
							m_pRoot->FindControl(__FindControlsFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST | UIFIND_UPDATETEST);
							for( int it = 0; it < m_aFoundControls.GetSize(); it++ ) {
								pControl = static_cast<CControlUI*>(m_aFoundControls[it]);
								if( !pControl->IsFloat() ) pControl->SetPos(pControl->GetPos(), true);
								else pControl->SetPos(pControl->GetRelativePos(), true);
							}
							bNeedSizeMsg = true;
						}
						else
						{
							CControlUI* pControl = NULL;
							CContainerUI* root = dynamic_cast<CContainerUI*>(m_pRoot);
							if (root)
							{
								auto & list = _UpdateList;
								while(list.size() && (pControl = list.back())) {
									if (pControl->IsUpdateNeeded())
									{ // sanity check
										if( !pControl->IsFloat() ) pControl->SetPos(pControl->GetPos(), true);
										else pControl->SetPos(pControl->GetRelativePos(), true);
									}
									list.pop_back();
								}
							}
							bNeedSizeMsg = true;
						}
						// We'll want to notify the window when it is first initialized
						// with the correct layout. The window form would take the time
						// to submit swipes/animations.
						if( m_bFirstLayout ) 
						{
							m_bFirstLayout = false;
							SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWINIT,  0, 0, false);
							if( m_bLayered && m_bLayeredChanged ) 
							{
								Invalidate();
								SetPainting(false);
								return true;
							}
							// 更新阴影窗口显示
							m_shadow.Update(m_hWndPaint);
						}
					}
				}
				else if( m_bLayered && m_bLayeredChanged ) 
				{
					RECT rcRoot = rcClient;
					if( m_pOffscreenBits ) ::ZeroMemory(m_pOffscreenBits, (rcRoot.right - rcRoot.left) 
						* (rcRoot.bottom - rcRoot.top) * 4);
					rcRoot.left += m_rcLayeredInset.left;
					rcRoot.top += m_rcLayeredInset.top;
					rcRoot.right -= m_rcLayeredInset.right;
					rcRoot.bottom -= m_rcLayeredInset.bottom;
					m_pRoot->SetPos(rcRoot, true);
				}

				if( m_bLayered ) 
				{
					DWORD dwExStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
					DWORD dwNewExStyle = dwExStyle | WS_EX_LAYERED;
					if(dwExStyle != dwNewExStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewExStyle);
					m_bOffscreenPaint = true;
					UnionRect(&rcPaint, &rcPaint, &m_rcLayeredUpdate);
					if( rcPaint.right > rcClient.right ) rcPaint.right = rcClient.right;
					if( rcPaint.bottom > rcClient.bottom ) rcPaint.bottom = rcClient.bottom;
					::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
				}

				//
				// Render screen // dx 动画
				//
//#define 调试DX特效数组越界
#ifdef 调试DX特效数组越界 
				// debug 模式下（可观察未优化掉的变量），用 PostMessage 用连续快速点击触发特效时
				//	，导致 UIDxAnimation.cpp 中发生数组越界BUG。  m_p3DVertices[m_nBuffers]
				//  release 不用 PostMessage 也会越界。
				// PostMessage 是由 clrhcp 分支上第三个提交引进的，似乎是用于修复此BUG，但非正解。
				if( m_anim.IsAnimating() || m_anim.IsJobScheduled())
					::PostMessage(m_hWndPaint,WM_EFFECTS,NULL,NULL);
#else
				if(false) { SetPainting(false); return false; } 
				else if( m_anim.IsAnimating() )
				{
					//// 3D animation in progress
					////   3D动画  
					int ret = m_anim.Render();
					//
					//// Do a minimum paint loop  做一个最小的绘制循环
					//// Keep the client area invalid so we generate lots of
					//// WM_PAINT messages. Cross fingers that Windows doesn't
					//// batch these somehow in the future.
					PAINTSTRUCT ps = { 0 };
					::BeginPaint(m_hWndPaint, &ps);
					::EndPaint(m_hWndPaint, &ps);
				}
				else if( m_anim.IsJobScheduled() ) 
				{
					// Animation system needs to be initialized
					//	动画系统需要初始化
					m_anim.Init(m_hWndPaint);
					// A 3D animation was scheduled; allow the render engine to
					// capture the window content and repaint some other time
					
					if( !m_anim.PrepareAnimation(m_hWndPaint) ) m_anim.CancelJobs();
					::SetTimer(m_hWndPaint, 0x190, 15, NULL);
				} 
#endif
				else {
#ifdef QKUI_BENCHMARK
				benchmeasuredraw measuredraw;
				GetClientRect(m_hWndPaint, &rcPaint);
#endif
				//_rcPaint = rcPaint;
				//
				// Render screen //渲染屏幕
				//
				// Prepare offscreen bitmap
				if( m_bOffscreenPaint && m_hbmpOffscreen == NULL ) {
					m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
					m_hbmpOffscreen = CRenderEngine::CreateARGB32Bitmap(m_hDcPaint, dwWidth, dwHeight, (LPBYTE*)&m_pOffscreenBits); 
					ASSERT(m_hDcOffscreen);
					ASSERT(m_hbmpOffscreen);
#ifdef MODULE_SKIA_RENDERER
					// BGRA
					SkImageInfo info = SkImageInfo::Make(dwWidth, dwHeight, kBGRA_8888_SkColorType,kPremul_SkAlphaType); 
					_skSurf = SkSurface::MakeRasterDirect(info
						, m_pOffscreenBits
						, dwWidth * sizeof(uint32_t));
					_skCanvas = _skSurf->getCanvas();
					if (_skCanvas)
					{
						_skCanvas->translate(0, dwHeight);
						_skCanvas->scale(1, -1);
					}
#endif
				}
				// Begin Windows paint
				if(_bIsWinFrame) {
					rcPaint.left = rcClient.right-16; // DEFAULT_SCROLLBAR_SIZE
				}
				PAINTSTRUCT ps = { 0 };
				HDC hdc_ = ::BeginPaint(m_hWndPaint, &ps);
				if( m_bOffscreenPaint )  {
					HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
					int iSaveDC = ::SaveDC(m_hDcOffscreen);
					if (m_bLayered) {
						for( LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y ) {
							for( LONG x = rcPaint.left; x < rcPaint.right; ++x ) {
								int i = (y * dwWidth + x) * 4;
								*(DWORD*)(&m_pOffscreenBits[i]) = 0;
							}
						}
					}
					m_pRoot->Paint(m_hDcOffscreen, rcPaint, NULL);

					if( m_bLayered ) {
						for( int i = 0; i < m_aNativeWindow.GetSize(); ) {
							HWND hChildWnd = static_cast<HWND>(m_aNativeWindow[i]);
							if (!::IsWindow(hChildWnd)) {
								m_aNativeWindow.Remove(i);
								m_aNativeWindowControl.Remove(i);
								continue;
							}
							++i;
							if (!::IsWindowVisible(hChildWnd)) continue;
							RECT rcChildWnd = GetNativeWindowRect(hChildWnd);
							RECT rcTemp = { 0 };
							if( !::IntersectRect(&rcTemp, &rcPaint, &rcChildWnd) ) continue;

							COLORREF* pChildBitmapBits = NULL;
							HDC hChildMemDC = ::CreateCompatibleDC(m_hDcOffscreen);
							HBITMAP hChildBitmap = CRenderEngine::CreateARGB32Bitmap(hChildMemDC, rcChildWnd.right-rcChildWnd.left, rcChildWnd.bottom-rcChildWnd.top, (BYTE**)&pChildBitmapBits); 
							::ZeroMemory(pChildBitmapBits, (rcChildWnd.right - rcChildWnd.left)*(rcChildWnd.bottom - rcChildWnd.top)*4);
							HBITMAP hOldChildBitmap = (HBITMAP) ::SelectObject(hChildMemDC, hChildBitmap);
							::SendMessage(hChildWnd, WM_PRINT, (WPARAM)hChildMemDC,(LPARAM)(PRF_CHECKVISIBLE|PRF_CHILDREN|PRF_CLIENT|PRF_OWNED));
							COLORREF* pChildBitmapBit;
							for( LONG y = 0; y < rcChildWnd.bottom-rcChildWnd.top; y++ ) {
								for( LONG x = 0; x < rcChildWnd.right-rcChildWnd.left; x++ ) {
									pChildBitmapBit = pChildBitmapBits+y*(rcChildWnd.right-rcChildWnd.left) + x;
									if (*pChildBitmapBit != 0x00000000) *pChildBitmapBit |= 0xff000000;
								}
							}
							::BitBlt(m_hDcOffscreen, rcChildWnd.left, rcChildWnd.top, rcChildWnd.right - rcChildWnd.left,
								rcChildWnd.bottom - rcChildWnd.top, hChildMemDC, 0, 0, SRCCOPY);
							::SelectObject(hChildMemDC, hOldChildBitmap);
							::DeleteObject(hChildBitmap);
							::DeleteDC(hChildMemDC);
						}
					}

					for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
						CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
						pPostPaintControl->DoPostPaint(m_hDcOffscreen, rcPaint);
					}

#ifdef MODULE_SKIA_RENDERER
					//if (_skCanvas) SKDraw(_skCanvas, dwWidth, dwHeight);
#endif

					::RestoreDC(m_hDcOffscreen, iSaveDC);

					if( m_bLayered ) {
						RECT rcWnd = { 0 };
						::GetWindowRect(m_hWndPaint, &rcWnd);
						if(!m_diLayered.sName.IsEmpty()) {
							DWORD dwWidth = rcClient.right - rcClient.left;
							DWORD dwHeight = rcClient.bottom - rcClient.top;
							RECT rcLayeredClient = rcClient;
							rcLayeredClient.left += m_rcLayeredInset.left;
							rcLayeredClient.top += m_rcLayeredInset.top;
							rcLayeredClient.right -= m_rcLayeredInset.right;
							rcLayeredClient.bottom -= m_rcLayeredInset.bottom;

							COLORREF* pOffscreenBits = (COLORREF*)m_pOffscreenBits;
							COLORREF* pBackgroundBits = m_pBackgroundBits;
							BYTE A = 0;
							BYTE R = 0;
							BYTE G = 0;
							BYTE B = 0;
							if (!m_diLayered.sName.IsEmpty()) {
								if( m_hbmpBackground == NULL) {
									m_hDcBackground = ::CreateCompatibleDC(m_hDcPaint);
									m_hbmpBackground = CRenderEngine::CreateARGB32Bitmap(m_hDcPaint, dwWidth, dwHeight, (BYTE**)&m_pBackgroundBits); 
									::ZeroMemory(m_pBackgroundBits, dwWidth * dwHeight * 4);
									::SelectObject(m_hDcBackground, m_hbmpBackground);
									CRenderClip clip;
									CRenderClip::GenerateClip(m_hDcBackground, rcLayeredClient, clip);
									CRenderEngine::DrawImageInfo(m_hDcBackground, this, rcLayeredClient, rcLayeredClient, &m_diLayered);
								}
								else if( m_bLayeredChanged ) {
									::ZeroMemory(m_pBackgroundBits, dwWidth * dwHeight * 4);
									CRenderClip clip;
									CRenderClip::GenerateClip(m_hDcBackground, rcLayeredClient, clip);
									CRenderEngine::DrawImageInfo(m_hDcBackground, this, rcLayeredClient, rcLayeredClient, &m_diLayered);
								}
								for( LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y ) {
									for( LONG x = rcPaint.left; x < rcPaint.right; ++x ) {
										pOffscreenBits = (COLORREF*)(m_pOffscreenBits + y * dwWidth + x);
										pBackgroundBits = m_pBackgroundBits + y * dwWidth + x;
										A = (BYTE)((*pBackgroundBits) >> 24);
										R = (BYTE)((*pOffscreenBits) >> 16) * A / 255;
										G = (BYTE)((*pOffscreenBits) >> 8) * A / 255;
										B = (BYTE)(*pOffscreenBits) * A / 255;
										*pOffscreenBits = RGB(B, G, R) + ((DWORD)A << 24);
									}
								}
							}
						}
						else {
							for( LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y ) {
								for( LONG x = rcPaint.left; x < rcPaint.right; ++x ) {
									int i = (y * dwWidth + x) * 4;
									if((m_pOffscreenBits[i + 3] == 0)&& (m_pOffscreenBits[i + 0] != 0 || m_pOffscreenBits[i + 1] != 0|| m_pOffscreenBits[i + 2] != 0))
										m_pOffscreenBits[i + 3] = 255;
								}
							}
						}

						BLENDFUNCTION bf = { AC_SRC_OVER, 0, m_nOpacity, AC_SRC_ALPHA };
						POINT ptPos   = { rcWnd.left, rcWnd.top };
						SIZE sizeWnd  = { dwWidth, dwHeight };
						POINT ptSrc   = { 0, 0 };
						g_fUpdateLayeredWindow(m_hWndPaint, m_hDcPaint, &ptPos, &sizeWnd, m_hDcOffscreen, &ptSrc, 0, &bf, ULW_ALPHA);
					}
					else {
						::BitBlt(m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top, m_hDcOffscreen, rcPaint.left, rcPaint.top, SRCCOPY);
					}
					::SelectObject(m_hDcOffscreen, hOldBitmap);

					if( m_bShowUpdateRect && !m_bLayered ) {
						HPEN hOldPen = (HPEN)::SelectObject(m_hDcPaint, m_hUpdateRectPen);
						::SelectObject(m_hDcPaint, ::GetStockObject(HOLLOW_BRUSH));
						::Rectangle(m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
						::SelectObject(m_hDcPaint, hOldPen);
					}
				}
				else {
					// A standard paint job
					int iSaveDC = ::SaveDC(m_hDcPaint);
					m_pRoot->Paint(m_hDcPaint, rcPaint, NULL);
					for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
						CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
						pPostPaintControl->DoPostPaint(m_hDcPaint, rcPaint);
					}
					::RestoreDC(m_hDcPaint, iSaveDC);
				}
				// All Done!
				::EndPaint(m_hWndPaint, &ps);

				// 绘制结束

				}

				// If any of the painting requested a resize again, we'll need
				// to invalidate the entire window once more.
				SetPainting(false);
				m_bLayeredChanged = false;
				//if( m_bUpdateNeeded ) Invalidate();

				// 发送窗口大小改变消息
				if(bNeedSizeMsg) {
					//this->SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWSIZE, 0, 0, true);
				}
			return true;
		}
		case WM_PRINTCLIENT:
			{
				if( m_pRoot == NULL ) break;
				RECT rcClient;
				::GetClientRect(m_hWndPaint, &rcClient);
				HDC hDC = (HDC) wParam;
				int save = ::SaveDC(hDC);
				m_pRoot->Paint(hDC, rcClient, NULL);
				if( (lParam & PRF_CHILDREN) != 0 ) {
					HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
					while( hWndChild != NULL ) {
						RECT rcPos = { 0 };
						::GetWindowRect(hWndChild, &rcPos);
						::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
						::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
						::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
						hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
					}
				}
				::RestoreDC(hDC, save);
			}
			break;
		case WM_GETMINMAXINFO:
			{
				MONITORINFO Monitor = {};
				Monitor.cbSize = sizeof(Monitor);
				::GetMonitorInfo(::MonitorFromWindow(m_hWndPaint, MONITOR_DEFAULTTOPRIMARY), &Monitor);
				RECT rcWork = Monitor.rcWork;
				if( Monitor.dwFlags != MONITORINFOF_PRIMARY ) {
					::OffsetRect(&rcWork, -rcWork.left, -rcWork.top);
				}

				LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
				if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
				if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
				if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
				if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
				if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxSize.x = m_szMaxWindow.cx;
				if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxSize.y = m_szMaxWindow.cy;
			}
			break;
		case WM_SIZE:
			{
				if( m_pFocus != NULL ) {
					TEventUI event = { 0 };
					event.Type = UIEVENT_WINDOWSIZE;
					event.pSender = m_pFocus;
					event.dwTimestamp = ::GetTickCount();
					m_pFocus->Event(event);
				}
				//当对话框尺寸变化时 删除动画 job
				if( m_anim.IsAnimating() ) m_anim.CancelJobs();
				//m_bUpdateNeeded = true;
				if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
			}
			return true;
		case WM_TIMER:
			{
				if (wParam==0x190)
				{
					if (m_anim.IsAnimating())
					{
						::InvalidateRect(m_hWndPaint, NULL, FALSE);
					}
					else
					{
						::KillTimer(m_hWndPaint, 0x190);
					}
					return true;
				}
				for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
					const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
					if(pTimer->hWnd == m_hWndPaint && 
						pTimer->uWinTimer == LOWORD(wParam) && 
						pTimer->bKilled == false)
					{
						TEventUI event = { 0 };
						event.Type = UIEVENT_TIMER;
						event.pSender = pTimer->pSender;
						event.dwTimestamp = ::GetTickCount();
						event.ptMouse = m_ptLastMousePos;
						event.wKeyState = MapKeyState();
						event.wParam = pTimer->nLocalID;
						event.lParam = lParam;
						pTimer->pSender->Event(event);
						break;
					}
				}
			}
			break;
		case WM_MOUSEHOVER:
			{
				m_bMouseTracking = false;
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				CControlUI* pHover = FindControl(pt);
				if( pHover == NULL ) break;
				// Generate mouse hover event
				if( m_pEventHover != NULL ) {
					TEventUI event = { 0 };
					event.Type = UIEVENT_MOUSEHOVER;
					event.pSender = m_pEventHover;
					event.wParam = wParam;
					event.lParam = lParam;
					event.dwTimestamp = ::GetTickCount();
					event.ptMouse = pt;
					event.wKeyState = MapKeyState();
					m_pEventHover->Event(event);
				}
				// Create tooltip information
				QkString sToolTip = pHover->GetToolTip();
				if( sToolTip.IsEmpty() ) return true;
				::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
				m_ToolTip.cbSize = sizeof(TOOLINFO);
				m_ToolTip.uFlags = TTF_IDISHWND;
				m_ToolTip.hwnd = m_hWndPaint;
				m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
				m_ToolTip.hinst = m_hInstance;
				m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
				m_ToolTip.rect = pHover->GetPos();
				if( m_hwndTooltip == NULL ) {
					m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
					::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
					::SendMessage(m_hwndTooltip,TTM_SETMAXTIPWIDTH,0, pHover->GetToolTipWidth());
				}
				if(!::IsWindowVisible(m_hwndTooltip))
				{
					::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTip);
					::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_ToolTip);
				}
			}
			return true;
		case WM_MOUSELEAVE:
			{
				if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
				if( m_bMouseTracking ) {
					POINT pt = { 0 };
					RECT rcWnd = { 0 };
					::GetCursorPos(&pt);
					::GetWindowRect(m_hWndPaint, &rcWnd);
					if( !::IsIconic(m_hWndPaint) && ::GetActiveWindow() == m_hWndPaint && ::PtInRect(&rcWnd, pt) ) {
						if( ::SendMessage(m_hWndPaint, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y)) == HTCLIENT ) {
							::ScreenToClient(m_hWndPaint, &pt);
							::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
						}
						else 
							::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
					}
					else 
						::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
				}
				m_bMouseTracking = false;
			}
			break;
		case WM_MOUSEMOVE:
			{
				// Start tracking this entire window again...
				if( !m_bMouseTracking ) {
					TRACKMOUSEEVENT tme = { 0 };
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_HOVER | TME_LEAVE;
					tme.hwndTrack = m_hWndPaint;
					tme.dwHoverTime = m_hwndTooltip == NULL ? m_iHoverTime : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
					_TrackMouseEvent(&tme);
					m_bMouseTracking = true;
				}
				// 是否移动
				bool bNeedDrag = true;

				// Generate the appropriate mouse messages
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				if(m_ptLastMousePos.x == pt.x && m_ptLastMousePos.y == pt.y) {
					bNeedDrag = false;
				}
				// 记录鼠标位置
				m_ptLastMousePos = pt;
				CControlUI* pNewHover = FindControl(pt);
				if( pNewHover != NULL && pNewHover->GetManager() != this ) break;

				// 拖拽事件
				if(bNeedDrag && m_bDragMode && wParam == MK_LBUTTON)
				{
					::ReleaseCapture();
					CIDropSource* pdsrc = new CIDropSource;
					if(pdsrc == NULL) return 0;
					pdsrc->AddRef();

					CIDataObject* pdobj = new CIDataObject(pdsrc);
					if(pdobj == NULL) return 0;
					pdobj->AddRef();

					FORMATETC fmtetc = {0};
					STGMEDIUM medium = {0};
					fmtetc.dwAspect = DVASPECT_CONTENT;
					fmtetc.lindex = -1;
					//////////////////////////////////////
					fmtetc.cfFormat = CF_BITMAP;
					fmtetc.tymed = TYMED_GDI;			
					medium.tymed = TYMED_GDI;
					HBITMAP hBitmap = (HBITMAP)OleDuplicateData(m_hDragBitmap, fmtetc.cfFormat, NULL);
					medium.hBitmap = hBitmap;
					pdobj->SetData(&fmtetc,&medium,FALSE);
					//////////////////////////////////////
					BITMAP bmap;
					GetObject(hBitmap, sizeof(BITMAP), &bmap);
					RECT rc={0, 0, bmap.bmWidth, bmap.bmHeight};
					fmtetc.cfFormat = CF_ENHMETAFILE;
					fmtetc.tymed = TYMED_ENHMF;
					HDC hMetaDC = CreateEnhMetaFile(m_hDcPaint, NULL, NULL, NULL);
					HDC hdcMem = CreateCompatibleDC(m_hDcPaint);
					HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBitmap);
					::BitBlt(hMetaDC, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
					::SelectObject(hdcMem, hOldBmp);
					medium.hEnhMetaFile = CloseEnhMetaFile(hMetaDC);
					DeleteDC(hdcMem);
					medium.tymed = TYMED_ENHMF;
					pdobj->SetData(&fmtetc, &medium, TRUE);
					//////////////////////////////////////
					CDragSourceHelper dragSrcHelper;
					POINT ptDrag = {0};
					ptDrag.x = bmap.bmWidth / 2;
					ptDrag.y = bmap.bmHeight / 2;
					dragSrcHelper.InitializeFromBitmap(hBitmap, ptDrag, rc, pdobj); //will own the bmp
					DWORD dwEffect;
					HRESULT hr = ::DoDragDrop(pdobj, pdsrc, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
					if(dwEffect ) pdsrc->Release();
					else delete pdsrc;
					pdobj->Release();
					m_bDragMode = false;
					break;
				}
				TEventUI event = { 0 };
				event.ptMouse = pt;
				event.wParam = wParam;
				event.lParam = lParam;
				event.dwTimestamp = ::GetTickCount();
				event.wKeyState = MapKeyState();
				if( !IsCaptured() ) 
				{
					pNewHover = FindControl(pt);
					if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
					if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
						event.Type = UIEVENT_MOUSELEAVE;
						event.pSender = m_pEventHover;

						CStdPtrArray aNeedMouseLeaveNeeded(m_aNeedMouseLeaveNeeded.GetSize());
						aNeedMouseLeaveNeeded.Resize(m_aNeedMouseLeaveNeeded.GetSize());
						::CopyMemory(aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetSize() * sizeof(LPVOID));
						for( int i = 0; i < aNeedMouseLeaveNeeded.GetSize(); i++ ) {
							static_cast<CControlUI*>(aNeedMouseLeaveNeeded[i])->Event(event);
						}

						m_pEventHover->Event(event);
						m_pEventHover = NULL;
						if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
					}
					if( pNewHover != m_pEventHover && pNewHover != NULL ) {
						event.Type = UIEVENT_MOUSEENTER;
						event.pSender = pNewHover;
						pNewHover->Event(event);
						m_pEventHover = pNewHover;
					}
				}
				if( m_pEventClick != NULL ) {
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = m_pEventClick;
					m_pEventClick->Event(event);
				}
				else if( pNewHover != NULL ) {
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = pNewHover;
					pNewHover->Event(event);
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				// We alway set focus back to our app (this helps
				// when Win32 child windows are placed on the dialog
				// and we need to remove them on focus change).
				if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
				if( m_pRoot == NULL ) break;
				// 查找控件
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControlEx(pt, UIFIND_FOCUSABLE);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;

				// 准备拖拽
				if(m_bDragDrop && pControl->IsDragEnabled()) {
					m_bDragMode = true;
					if( m_hDragBitmap != NULL ) {
						::DeleteObject(m_hDragBitmap);
						m_hDragBitmap = NULL;
					}
					m_hDragBitmap = CRenderEngine::GenerateBitmap(this, pControl, pControl->GetPos());
				}

				// 开启捕获
				SetCapture();
				// 事件处理
				m_pEventClick = pControl;
				pControl->SetFocus();

				if(pControl->m_bIsStatic)
				{
					ReleaseCapture();
					::SendMessage(GetRealManager()->GetPaintWindow(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
					::SetFocus(GetRealManager()->GetPaintWindow());
				}

				TEventUI event = { 0 };
				event.Type = UIEVENT_BUTTONDOWN;
				event.pSender = pControl;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);

				// No need to burden user with 3D animations
				m_anim.CancelJobs();
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				if (!m_bNoActivate) ::SetFocus(m_hWndPaint);

				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_DBLCLICK;
				event.pSender = pControl;
				event.ptMouse = pt;
				event.wParam = wParam;
				event.lParam = lParam;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;
		case WM_LBUTTONUP:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				if( m_pEventClick == NULL ) break;
				ReleaseCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_BUTTONUP;
				event.pSender = m_pEventClick;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();

				CControlUI* pClick = m_pEventClick;
				m_pEventClick = NULL;
				pClick->Event(event);
			}
			break;
		case WM_RBUTTONDOWN:
			{
				if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				if(false) pControl->SetFocus();
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_RBUTTONDOWN;
				event.pSender = pControl;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;
		case WM_RBUTTONUP:
			{
				if(m_bMouseCapture) ReleaseCapture();

				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				m_pEventClick = FindControl(pt);
				if(m_pEventClick == NULL) break;

				TEventUI event = { 0 };
				event.Type = UIEVENT_RBUTTONUP;
				event.pSender = m_pEventClick;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				m_pEventClick->Event(event);
			}
			break;
		case WM_MBUTTONDOWN:
			{
				if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				if(false) pControl->SetFocus();
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_MBUTTONDOWN;
				event.pSender = pControl;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;
		case WM_MBUTTONUP:
			{
				if(m_bMouseCapture) ReleaseCapture();
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				m_pEventClick = FindControl(pt);
				if(m_pEventClick == NULL) break;

				TEventUI event = { 0 };
				event.Type = UIEVENT_MBUTTONUP;
				event.pSender = m_pEventClick;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				m_pEventClick->Event(event);
			}
			break;
		case WM_CONTEXTMENU:
			{
				if( m_pRoot == NULL ) break;
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(m_hWndPaint, &pt);
				m_ptLastMousePos = pt;
				if( m_pEventClick == NULL ) break;
				ReleaseCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_CONTEXTMENU;
				event.pSender = m_pEventClick;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				//event.lParam = (LPARAM)m_pEventClick;
				event.dwTimestamp = ::GetTickCount();
				m_pEventClick->Event(event);
				m_pEventClick = NULL;
			}
			break;
		case WM_MOUSEWHEEL:
			{
				if( m_pRoot == NULL ) break;
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(m_hWndPaint, &pt);
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				int zDelta = (int) (short) HIWORD(wParam);
				TEventUI event = { 0 };
				event.Type = UIEVENT_SCROLLWHEEL;
				event.pSender = pControl;
				//event.wParam = MAKEWPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, zDelta);
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);

				// Let's make sure that the scroll item below the cursor is the same as before...
				::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
			}
			break;
		case WM_CHAR:
			{
				if( m_pRoot == NULL ) break;
				if( m_pFocus == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_CHAR;
				event.pSender = m_pFocus;
				event.wParam = wParam;
				event.lParam = lParam;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
			break;
		case WM_IME_REQUEST:
			{
				if( m_pFocus == NULL ) break;
				if( m_pFocus->GetInterface(L"Editor") == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_IME_REQUEST;
				event.pSender = m_pFocus;
				event.wParam = wParam;
				event.lParam = lParam;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
			break;
		case WM_KEYDOWN:
			{
				if( m_pRoot == NULL ) break;
				if( m_pFocus == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_KEYDOWN;
				event.pSender = m_pFocus;
				event.wParam = wParam;
				event.lParam = lParam;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
				m_pEventKey = m_pFocus;
			}
			break;
		case WM_KEYUP:
			{
				if( m_pRoot == NULL ) break;
				if( m_pEventKey == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_KEYUP;
				event.pSender = m_pEventKey;
				event.wParam = wParam;
				event.lParam = lParam;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pEventKey->Event(event);
				m_pEventKey = NULL;
			}
			break;
		case WM_SETCURSOR:
			{
				if( m_pRoot == NULL ) break;
				if( LOWORD(lParam) != HTCLIENT ) break;
				if( m_bMouseCapture ) return true;

				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(m_hWndPaint, &pt);
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_SETCURSOR;
				event.pSender = pControl;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
			}
			return true;
		case WM_SETFOCUS:
			{
				//LogIs(L"WM_SETFOCUS");
				if( _wndFocus && ::IsWindow(_wndFocus) ) {
					::SetFocus(_wndFocus);
				}
				else if( m_pFocus != NULL ) 
				{
					TEventUI event = { 0 };
					event.Type = UIEVENT_SETFOCUS;
					event.wParam = wParam;
					event.lParam = lParam;
					event.pSender = m_pFocus;
					event.dwTimestamp = ::GetTickCount();
					m_pFocus->Event(event);
					SendNotify(m_pFocus, DUI_MSGTYPE_SETFOCUS);
				}
				break;
			}
		case WM_KILLFOCUS:
			{
				//LogIs(L"WM_KILLFOCUS %s %ld", m_pFocus?m_pFocus->GetClass():L"0", ::GetFocus());
				if(IsCaptured()) ReleaseCapture();
				SetFocus(NULL);
				_wndFocus = ::GetFocus();
				if(!::IsChild(m_hWndPaint, _wndFocus))
					_wndFocus = nullptr;
				//m_pFocus = NULL; // 如此当窗口重获焦点时，不恢复焦点所在。
				//if( m_pFocus != NULL ) {
				//	TEventUI event = { 0 };
				//	event.Type = UIEVENT_KILLFOCUS;
				//	event.wParam = wParam;
				//	event.lParam = lParam;
				//	event.pSender = m_pFocus;
				//	event.dwTimestamp = ::GetTickCount();
				//	m_pFocus->Event(event);
				//	SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
				//}
				break;
			}
		case WM_NOTIFY:
			{
				if( lParam == 0 ) break;
				LPNMHDR lpNMHDR = (LPNMHDR) lParam;
				if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
				return true;
			}
			break;
		case WM_COMMAND:
			{
				if( lParam == 0 ) break;
				HWND hWndChild = (HWND) lParam;
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
				if(lRes != 0) return true;
			}
			break;
		case WM_SIZING:
			{
				if(!_SIZING) _SIZING = true;
			}
			break;
		case WM_EXITSIZEMOVE:
			{
				_SIZING = false;
			}
			break;
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
			{
				// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
				// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
				if( lParam == 0 ) break;
				HWND hWndChild = (HWND) lParam;
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
				if(lRes != 0) return true;
			}
			break;
		default:
			break;
		}
		return false;
	}

	bool CPaintManagerUI::IsUpdateNeeded() const
	{
		return m_bUpdateNeeded;
	}

	void CPaintManagerUI::NeedUpdate()
	{
		m_bUpdateNeeded = true;
	}

	void CPaintManagerUI::Invalidate()
	{
		RECT rcClient = { 0 };
		::GetClientRect(m_hWndPaint, &rcClient);
		::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcClient);
		::InvalidateRect(m_hWndPaint, NULL, FALSE);
	}

	void CPaintManagerUI::Invalidate(RECT& rcItem)
	{
		if( rcItem.left < 0 ) rcItem.left = 0;
		if( rcItem .top < 0 ) rcItem.top = 0;
		if( rcItem.right < rcItem.left ) rcItem.right = rcItem.left;
		if( rcItem.bottom < rcItem.top ) rcItem.bottom = rcItem.top;
		::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcItem);
		::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
	}

	bool CPaintManagerUI::AttachDialog(CControlUI* pControl, bool bNeedFocus)
	{
		ASSERT(::IsWindow(m_hWndPaint));
		// 创建阴影窗口
		m_shadow.Create(this);

		// Reset any previous attachment
		if (_bAllowAutoFocus)
		{
			SetFocus(NULL);
		}
		else
		{
			m_pFocus = NULL;
		}
		m_pEventKey = NULL;
		m_pEventHover = NULL;
		m_pEventClick = NULL;
		// Remove the existing control-tree. We might have gotten inside this function as
		// a result of an event fired or similar, so we cannot just delete the objects and
		// pull the internal memory of the calling code. We'll delay the cleanup.
		if( m_pRoot != NULL ) {
			m_aPostPaintControls.Empty();
			AddDelayedCleanup(m_pRoot);
		}
		// Set the dialog root element
		m_pRoot = pControl;
		// Go ahead...
		m_bUpdateNeeded = true;
		m_bFirstLayout = true;
		m_bFocusNeeded = bNeedFocus;
		// Initiate all control
		return InitControls(pControl);
	}

	bool CPaintManagerUI::InitControls(CControlUI* pControl, CControlUI* pParent /*= NULL*/)
	{
		ASSERT(pControl);
		if( pControl == NULL ) return false;
		pControl->SetManager(this, pParent ? pParent : pControl->GetParent(), true);
		pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
		return true;
	}

	void CPaintManagerUI::ReapObjects(CControlUI* pControl)
	{
		if( pControl == m_pEventKey ) m_pEventKey = NULL;
		if( pControl == m_pEventHover ) m_pEventHover = NULL;
		if( pControl == m_pEventClick ) m_pEventClick = NULL;
		if( pControl == m_pFocus ) m_pFocus = NULL;
		KillTimer(pControl);
		const QkString& sName = pControl->GetName();
		if( !sName.IsEmpty() ) {
			if( pControl == FindControl(sName) ) m_mNameHash.Remove(sName);
		}
		for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
			TNotifyUI* pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
			if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
		}    
		if (pControl->IsUpdateNeeded())
		{
			_UpdateList.remove(pControl);
		}
		//if(pControl->GetParent())pControl->GetParent()->Remove(pControl);
	}

	bool CPaintManagerUI::AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) {
			CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
					return false;
				}
			}
			aOptionGroup->Add(pControl);
		}
		else {
			CStdPtrArray* aOptionGroup = new CStdPtrArray(6);
			aOptionGroup->Add(pControl);
			m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
		}
		return true;
	}

	CStdPtrArray* CPaintManagerUI::GetOptionGroup(LPCTSTR pStrGroupName)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) return static_cast<CStdPtrArray*>(lp);
		return NULL;
	}

	void CPaintManagerUI::RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
	{
		LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
		if( lp ) {
			CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
			if( aOptionGroup == NULL ) return;
			for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
				if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
					aOptionGroup->Remove(i);
					break;
				}
			}
			if( aOptionGroup->IsEmpty() ) {
				delete aOptionGroup;
				m_mOptionGroup.Remove(pStrGroupName);
			}
		}
	}

	void CPaintManagerUI::RemoveAllOptionGroups()
	{
		CStdPtrArray* aOptionGroup;
		for( int i = 0; i< m_mOptionGroup.GetSize(); i++ ) {
			aOptionGroup = static_cast<CStdPtrArray*>(m_mOptionGroup.GetValueAt(i));
			if(aOptionGroup) delete aOptionGroup;
		}
		m_mOptionGroup.RemoveAll();
	}

	void CPaintManagerUI::MessageLoop()
	{
		MSG msg = { 0 };
		while( ::GetMessage(&msg, NULL, 0, 0) ) {
			if( !CPaintManagerUI::TranslateMessage(&msg) ) {
				::TranslateMessage(&msg);
				try{
					::DispatchMessage(&msg);
				} catch(...) {
					DUITRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);
#ifdef _DEBUG
					throw "CPaintManagerUI::MessageLoop";
#endif
				}
			}
		}
	}

	void CPaintManagerUI::Term()
	{
		// 销毁资源管理器
		CResourceManager::GetInstance()->Release();
		CControlFactory::GetInstance()->Release();

		// 清理共享资源
		// 图片
		TImageInfo* data;
		for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
			data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.GetValueAt(i));
			if (data) {
				CRenderEngine::FreeImage(data);
				data = NULL;
			}
		}
		m_SharedResInfo.m_ImageHash.RemoveAll();
		// 字体
		TFontInfo* pFontInfo;
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.GetValueAt(i));
			if (pFontInfo) {
				::DeleteObject(pFontInfo->hFont);
				delete pFontInfo;
				pFontInfo = NULL;
			}
		}
		m_SharedResInfo.m_CustomFonts.RemoveAll();
		// 默认字体
		if(m_SharedResInfo.m_DefaultFontInfo.hFont != NULL) {
			::DeleteObject(m_SharedResInfo.m_DefaultFontInfo.hFont);
		}

		// 样式
		Style* pAttr;
		for( int i = 0; i< m_SharedResInfo.m_StyleIdHash.GetSize(); i++ ) {
			pAttr = static_cast<Style*>(m_SharedResInfo.m_StyleIdHash.GetValueAt(i));
			if(pAttr->name) m_SharedResInfo.m_StyleControlHash.Remove(pAttr->name);
			if(pAttr) delete pAttr;
		}
		for( int i = 0; i< m_SharedResInfo.m_StyleControlHash.GetSize(); i++ ) {
			pAttr = static_cast<Style*>(m_SharedResInfo.m_StyleControlHash.GetValueAt(i));
			if(pAttr) delete pAttr;
		}
		m_SharedResInfo.m_StyleIdHash.RemoveAll();
		m_SharedResInfo.m_StyleControlHash.RemoveAll();

		// 关闭ZIP
		if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
			CloseZip((HZIP)m_hResourceZip);
			m_hResourceZip = NULL;
		}
	}

	CDPI * DuiLib::CPaintManagerUI::GetDPIObj()
	{
		if (m_pDPI == NULL) {
			m_pDPI = new CDPI;
		}
		return m_pDPI;
	}

	void DuiLib::CPaintManagerUI::SetDPI(int iDPI)
	{
		int scale1 = GetDPIObj()->GetScale();
		GetDPIObj()->SetScale(iDPI);
		int scale2 = GetDPIObj()->GetScale();
		if (scale1!=scale2)
		{
			ResetDPIAssets();
			RECT rcWnd = {0};
			::GetWindowRect(GetPaintWindow(), &rcWnd);
			RECT*  prcNewWindow = &rcWnd;
			if (!::IsZoomed(GetPaintWindow())) {
				RECT rc = rcWnd;
				rc.right = rcWnd.left + (rcWnd.right - rcWnd.left) * scale2 / scale1;
				rc.bottom = rcWnd.top + (rcWnd.bottom - rcWnd.top) * scale2 / scale1;
				prcNewWindow = &rc;
			}
			SetWindowPos(GetPaintWindow(), NULL, prcNewWindow->left, prcNewWindow->top, prcNewWindow->right - prcNewWindow->left, prcNewWindow->bottom - prcNewWindow->top, SWP_NOZORDER | SWP_NOACTIVATE);
			if (GetRoot() != NULL) GetRoot()->NeedUpdate();
			::PostMessage(GetPaintWindow(), UIMSG_SET_DPI, 0, 0);
		}
	}

	void DuiLib::CPaintManagerUI::SetAllDPI(int iDPI)
	{
		for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
			CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			pManager->SetDPI(iDPI);
		}
	}

	void DuiLib::CPaintManagerUI::ResetDPIAssets()
	{
		if(_parent)
		{
			return _parent->ResetDPIAssets();
		}
		//RemoveAllDrawInfos();
		//RemoveAllImages();

		for (int it = 0; it < m_ResInfo.m_CustomFonts.GetSize(); it++) {
			TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(m_ResInfo.m_CustomFonts[it]));
			RebuildFont(pFontInfo);
		}
		RebuildFont(&m_ResInfo.m_DefaultFontInfo);

		for (int it = 0; it < m_SharedResInfo.m_CustomFonts.GetSize(); it++) {
			TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(m_SharedResInfo.m_CustomFonts[it]));
			RebuildFont(pFontInfo);
		}
		RebuildFont(&m_SharedResInfo.m_DefaultFontInfo);
#ifdef QkUIRichEdit
		CStdPtrArray *richEditList = FindSubControlsByClass(GetRoot(), _T("RichEditUI"));
		for (int i = 0; i < richEditList->GetSize(); i++)
		{
			CRichEditUI* pT = static_cast<CRichEditUI*>((*richEditList)[i]);
			pT->SetFont(0, pT->GetFont());
		}
#endif
	}

	void DuiLib::CPaintManagerUI::RebuildFont(TFontInfo * pFontInfo)
	{
		::DeleteObject(pFontInfo->hFont);
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		_tcsncpy(lf.lfFaceName, pFontInfo->sFontName, LF_FACESIZE);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -GetDPIObj()->Scale(pFontInfo->iSize); // here
		lf.lfQuality = CLEARTYPE_QUALITY;
		if (pFontInfo->bBold) lf.lfWeight += FW_BOLD;
		if (pFontInfo->bUnderline) lf.lfUnderline = TRUE;
		if (pFontInfo->bItalic) lf.lfItalic = TRUE;
		HFONT hFont = ::CreateFontIndirect(&lf);
		pFontInfo->hFont = hFont;
		::ZeroMemory(&(pFontInfo->tm), sizeof(pFontInfo->tm));
		if (m_hDcPaint) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
	}

	CControlUI* CPaintManagerUI::GetFocus() const
	{
		return m_pFocus;
	}

	void CPaintManagerUI::SetFocus(CControlUI* pControl)
	{
		//LogIs(4, L"SetFocus %s %s", pControl?pControl->GetClass():L"0", m_pFocus?m_pFocus->GetClass():L"0");
		// Paint manager window has focus?
		if(pControl) {
			if(_wndFocus) _wndFocus=0; // 证悟圆通大自在
			HWND hFocusWnd = ::GetFocus();
			HWND ctrlWnd = pControl->GetManager()->GetPaintWindow();
			if(hFocusWnd!=ctrlWnd) {
				if( hFocusWnd != ctrlWnd ) ::SetFocus(ctrlWnd);
				if(pControl==m_pFocus)
					m_pFocus = NULL;
			}
		} 
		//else if(hFocusWnd != m_hWndPaint && pControl != m_pFocus)
		//{
		//	::SetFocus(m_hWndPaint); // WTF ???
		//}
		// Already has focus?
		if( pControl == m_pFocus ) return;
		// Remove focus from old control
		if( m_pFocus != NULL ) 
		{
			TEventUI event = { 0 };
			event.Type = UIEVENT_KILLFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
			//m_pFocus = NULL;
		}
		if( pControl == NULL ) return;
		// Set focus to new control
		if( pControl != NULL 
			&& pControl->GetManager() == this 
			&& pControl->IsVisible() 
			&& pControl->IsEnabled() ) 
		{
			m_pFocus = pControl;
			TEventUI event = { 0 };
			event.Type = UIEVENT_SETFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, DUI_MSGTYPE_SETFOCUS);
		}
	}

	void CPaintManagerUI::SetFocusNeeded(CControlUI* pControl)
	{
		if( pControl == NULL ) {
			m_bFocusNeeded = false;
			return;
		}
		::SetFocus(m_hWndPaint);
		if( m_pFocus != NULL ) {
			TEventUI event = { 0 };
			event.Type = UIEVENT_KILLFOCUS;
			event.pSender = pControl;
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
			m_pFocus = NULL;
		}
		FINDTABINFO info = { 0 };
		info.pFocus = pControl;
		info.bForward = false;
		m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		m_bFocusNeeded = true;
		if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
	}

	bool CPaintManagerUI::SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse, bool bRestart)
	{
		ASSERT(pControl!=NULL);
		ASSERT(uElapse>0);
		for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == m_hWndPaint
				&& pTimer->nLocalID == nTimerID ) 
			{
					if (bRestart && !pTimer->bKilled)
					{
						::KillTimer(m_hWndPaint, pTimer->uWinTimer);
						pTimer->bKilled = true;
					}
					if( pTimer->bKilled) 
					{
						if( ::SetTimer(m_hWndPaint, pTimer->uWinTimer, uElapse, NULL) ) 
						{
							pTimer->bKilled = false;
							return true;
						}
						return false;
					}
					return false;
			}
		}

		m_uTimerID = (++m_uTimerID) % 0xF0; //0xf1-0xfe特殊用途
		if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
		TIMERINFO* pTimer = new TIMERINFO;
		if( pTimer == NULL ) return FALSE;
		pTimer->hWnd = m_hWndPaint;
		pTimer->pSender = pControl;
		pTimer->nLocalID = nTimerID;
		pTimer->uWinTimer = m_uTimerID;
		pTimer->bKilled = false;
		return m_aTimers.Add(pTimer);
	}

	bool CPaintManagerUI::KillTimer(CControlUI* pControl, UINT nTimerID)
	{
		ASSERT(pControl!=NULL);
		for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->pSender == pControl
				&& pTimer->hWnd == m_hWndPaint
				&& pTimer->nLocalID == nTimerID )
			{
				if( pTimer->bKilled == false ) {
					if( ::IsWindow(m_hWndPaint) ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
					pTimer->bKilled = true;
					return true;
				}
			}
		}
		return false;
	}

	void CPaintManagerUI::KillTimer(CControlUI* pControl)
	{
		ASSERT(pControl!=NULL);
		int count = m_aTimers.GetSize();
		for( int i = 0, j = 0; i < count; i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i - j]);
			if( pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint ) {
				if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
				delete pTimer;
				m_aTimers.Remove(i - j);
				j++;
			}
		}
	}

	void CPaintManagerUI::RemoveAllTimers()
	{
		for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
			TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			if( pTimer->hWnd == m_hWndPaint ) {
				if( pTimer->bKilled == false ) {
					if( ::IsWindow(m_hWndPaint) ) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
				}
				delete pTimer;
			}
		}

		m_aTimers.Empty();
	}

	void CPaintManagerUI::SetCapture()
	{
		::SetCapture(m_hWndPaint);
		m_bMouseCapture = true;
	}

	void CPaintManagerUI::ReleaseCapture()
	{
		::ReleaseCapture();
		m_bMouseCapture = false;
		m_bDragMode = false;
	}

	bool CPaintManagerUI::IsCaptured()
	{
		return m_bMouseCapture;
	}

	bool CPaintManagerUI::IsPainting()
	{
		return m_bIsPainting;
	}

	void CPaintManagerUI::SetPainting(bool bIsPainting)
	{
		m_bIsPainting = bIsPainting;
	}

	bool CPaintManagerUI::SetNextTabControl(bool bForward)
	{
		if (!_bAllowAutoFocus)
		{
			return false;
		}
		// If we're in the process of restructuring the layout we can delay the
		// focus calulation until the next repaint.
		if( m_bUpdateNeeded && bForward ) {
			m_bFocusNeeded = true;
			::InvalidateRect(m_hWndPaint, NULL, FALSE);
			return true;
		}
		m_bFocusNeeded = false;
		// Find next/previous tabbable control
		FINDTABINFO info1 = { 0 };
		info1.pFocus = m_pFocus;
		info1.bForward = bForward;
		CControlUI* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		if( pControl == NULL ) {  
			if( bForward ) {
				// Wrap around
				FINDTABINFO info2 = { 0 };
				info2.pFocus = bForward ? NULL : info1.pLast;
				info2.bForward = bForward;
				pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
			}
			else {
				pControl = info1.pLast;
			}
		}
		if( pControl != NULL ) SetFocus(pControl);
		return true;
	}

	bool CPaintManagerUI::AddNotifier(INotifyUI* pNotifier)
	{
		ASSERT(m_aNotifiers.Find(pNotifier)<0);
		return m_aNotifiers.Add(pNotifier);
	}

	bool CPaintManagerUI::RemoveNotifier(INotifyUI* pNotifier)
	{
		for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
			if( static_cast<INotifyUI*>(m_aNotifiers[i]) == pNotifier ) {
				return m_aNotifiers.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::AddPreMessageFilter(IMessageFilterUI* pFilter)
	{
		ASSERT(m_aPreMessageFilters.Find(pFilter)<0);
		return m_aPreMessageFilters.Add(pFilter);
	}

	bool CPaintManagerUI::RemovePreMessageFilter(IMessageFilterUI* pFilter)
	{
		for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) {
			if( static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i]) == pFilter ) {
				return m_aPreMessageFilters.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::AddMessageFilter(IMessageFilterUI* pFilter)
	{
		if (m_aMessageFilters.Find(pFilter)<0)
		{
			return m_aMessageFilters.Add(pFilter);
		}
		return false;
	}

	bool CPaintManagerUI::RemoveMessageFilter(IMessageFilterUI* pFilter)
	{
		for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
			if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
				return m_aMessageFilters.Remove(i);
			}
		}
		return false;
	}

	int CPaintManagerUI::GetPostPaintCount() const
	{
		return m_aPostPaintControls.GetSize();
	}

	bool CPaintManagerUI::IsPostPaint(CControlUI* pControl)
	{
		return m_aPostPaintControls.Find(pControl) >= 0;
	}

	bool CPaintManagerUI::AddPostPaint(CControlUI* pControl)
	{
		ASSERT(m_aPostPaintControls.Find(pControl) < 0);
		return m_aPostPaintControls.Add(pControl);
	}

	bool CPaintManagerUI::RemovePostPaint(CControlUI* pControl)
	{
		for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
			if( static_cast<CControlUI*>(m_aPostPaintControls[i]) == pControl ) {
				return m_aPostPaintControls.Remove(i);
			}
		}
		return false;
	}

	bool CPaintManagerUI::SetPostPaintIndex(CControlUI* pControl, int iIndex)
	{
		RemovePostPaint(pControl);
		return m_aPostPaintControls.InsertAt(iIndex, pControl);
	}

	int CPaintManagerUI::GetNativeWindowCount() const
	{
		return m_aNativeWindow.GetSize();
	}

	bool CPaintManagerUI::AddNativeWindow(CControlUI* pControl, HWND hChildWnd)
	{
		if (pControl == NULL || hChildWnd == NULL) return false;

		RECT rcChildWnd = GetNativeWindowRect(hChildWnd);
		Invalidate(rcChildWnd);

		if (m_aNativeWindow.Find(hChildWnd) >= 0) return false;
		if (m_aNativeWindow.Add(hChildWnd)) {
			m_aNativeWindowControl.Add(pControl);
			return true;
		}
		return false;
	}

	bool CPaintManagerUI::RemoveNativeWindow(HWND hChildWnd)
	{
		for( int i = 0; i < m_aNativeWindow.GetSize(); i++ ) {
			if( static_cast<HWND>(m_aNativeWindow[i]) == hChildWnd ) {
				if( m_aNativeWindow.Remove(i) ) {
					m_aNativeWindowControl.Remove(i);
					return true;
				}
				return false;
			}
		}
		return false;
	}

	RECT CPaintManagerUI::GetNativeWindowRect(HWND hChildWnd)
	{
		RECT rcChildWnd;
		::GetWindowRect(hChildWnd, &rcChildWnd);
		::ScreenToClient(m_hWndPaint, (LPPOINT)(&rcChildWnd));
		::ScreenToClient(m_hWndPaint, (LPPOINT)(&rcChildWnd)+1);
		return rcChildWnd;
	}

	void CPaintManagerUI::AddDelayedCleanup(CControlUI* pControl)
	{
		if (pControl == NULL) return;
		pControl->SetManager(this, NULL, false);
		m_aDelayedCleanup.Add(pControl);
		PostAsyncNotify();
	}

	void CPaintManagerUI::AddMouseLeaveNeeded(CControlUI* pControl)
	{
		if (pControl == NULL) return;
		for( int i = 0; i < m_aNeedMouseLeaveNeeded.GetSize(); i++ ) {
			if( static_cast<CControlUI*>(m_aNeedMouseLeaveNeeded[i]) == pControl ) {
				return;
			}
		}
		m_aNeedMouseLeaveNeeded.Add(pControl);
	}

	bool CPaintManagerUI::RemoveMouseLeaveNeeded(CControlUI* pControl)
	{
		if (pControl == NULL) return false;
		for( int i = 0; i < m_aNeedMouseLeaveNeeded.GetSize(); i++ ) {
			if( static_cast<CControlUI*>(m_aNeedMouseLeaveNeeded[i]) == pControl ) {
				return m_aNeedMouseLeaveNeeded.Remove(i);
			}
		}
		return false;
	}

	void CPaintManagerUI::SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
	{
		TNotifyUI Msg;
		Msg.pSender = pControl;
		Msg.sType = pstrMessage;
		Msg.wParam = wParam;
		Msg.lParam = lParam;
		SendNotify(Msg, bAsync);
	}

	void CPaintManagerUI::SendNotify(TNotifyUI& Msg, bool bAsync /*= false*/)
	{
		Msg.ptMouse = m_ptLastMousePos;
		Msg.dwTimestamp = ::GetTickCount();
		if( m_bUsedVirtualWnd )
		{
			Msg.sVirtualWnd = Msg.pSender->GetVirtualWnd();
		}

		if( !bAsync ) {
			// Send to all listeners
			if( Msg.pSender != NULL ) {
				if( Msg.pSender->OnNotify ) Msg.pSender->OnNotify(&Msg);
			}
			for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
				static_cast<INotifyUI*>(m_aNotifiers[i])->Notify(Msg);
			}
		}
		else {
			TNotifyUI *pMsg = new TNotifyUI;
			pMsg->pSender = Msg.pSender;
			pMsg->sType = Msg.sType;
			pMsg->wParam = Msg.wParam;
			pMsg->lParam = Msg.lParam;
			pMsg->ptMouse = Msg.ptMouse;
			pMsg->dwTimestamp = Msg.dwTimestamp;
			m_aAsyncNotify.Add(pMsg);

			PostAsyncNotify();
		}
	}

	bool CPaintManagerUI::IsForceUseSharedRes() const
	{
		return m_bForceUseSharedRes;
	}

	void CPaintManagerUI::SetForceUseSharedRes(bool bForce)
	{
		m_bForceUseSharedRes = bForce;
	}

	DWORD CPaintManagerUI::GetDefaultDisabledColor() const
	{
		return _parent?_parent->GetDefaultDisabledColor():m_ResInfo.m_dwDefaultDisabledColor;
	}

	void CPaintManagerUI::SetDefaultDisabledColor(DWORD dwColor, bool bShared)
	{
		if (bShared)
		{
			if (m_ResInfo.m_dwDefaultDisabledColor == m_SharedResInfo.m_dwDefaultDisabledColor)
				m_ResInfo.m_dwDefaultDisabledColor = dwColor;
			m_SharedResInfo.m_dwDefaultDisabledColor = dwColor;
		}
		else if(_parent)
		{
			_parent->SetDefaultDisabledColor(dwColor);
		}
		else
		{
			m_ResInfo.m_dwDefaultDisabledColor = dwColor;
		}
	}

	DWORD CPaintManagerUI::GetDefaultFontColor() const
	{
		return _parent?_parent->GetDefaultFontColor():m_ResInfo.m_dwDefaultFontColor;
	}

	void CPaintManagerUI::SetDefaultFontColor(DWORD dwColor, bool bShared)
	{
		if (bShared)
		{
			if (m_ResInfo.m_dwDefaultFontColor == m_SharedResInfo.m_dwDefaultFontColor)
				m_ResInfo.m_dwDefaultFontColor = dwColor;
			m_SharedResInfo.m_dwDefaultFontColor = dwColor;
		}
		else if(_parent)
		{
			_parent->SetDefaultFontColor(dwColor);
		}
		else
		{
			m_ResInfo.m_dwDefaultFontColor = dwColor;
		}
	}

	DWORD CPaintManagerUI::GetDefaultLinkFontColor() const
	{
		return _parent?_parent->GetDefaultLinkFontColor():m_ResInfo.m_dwDefaultLinkFontColor;
	}

	void CPaintManagerUI::SetDefaultLinkFontColor(DWORD dwColor, bool bShared)
	{
		if (bShared)
		{
			if (m_ResInfo.m_dwDefaultLinkFontColor == m_SharedResInfo.m_dwDefaultLinkFontColor)
				m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
			m_SharedResInfo.m_dwDefaultLinkFontColor = dwColor;
		}
		else if(_parent)
		{
			_parent->SetDefaultLinkFontColor(dwColor);
		}
		else
		{
			m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
		}
	}

	DWORD CPaintManagerUI::GetDefaultLinkHoverFontColor() const
	{
		return _parent?_parent->GetDefaultLinkHoverFontColor():m_ResInfo.m_dwDefaultLinkHoverFontColor;
	}

	void CPaintManagerUI::SetDefaultLinkHoverFontColor(DWORD dwColor, bool bShared)
	{
		if (bShared)
		{
			if (m_ResInfo.m_dwDefaultLinkHoverFontColor == m_SharedResInfo.m_dwDefaultLinkHoverFontColor)
				m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
			m_SharedResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
		}
		else if(_parent)
		{
			_parent->SetDefaultLinkHoverFontColor(dwColor);
		}
		else
		{
			m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
		}
	}

	DWORD CPaintManagerUI::GetDefaultSelectedBkColor() const
	{
		return _parent?_parent->GetDefaultSelectedBkColor():m_ResInfo.m_dwDefaultSelectedBkColor;
	}

	void CPaintManagerUI::SetDefaultSelectedBkColor(DWORD dwColor, bool bShared)
	{
		if (bShared)
		{
			if (m_ResInfo.m_dwDefaultSelectedBkColor == m_SharedResInfo.m_dwDefaultSelectedBkColor)
				m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
			m_SharedResInfo.m_dwDefaultSelectedBkColor = dwColor;
		}
		else if(_parent)
		{
			_parent->SetDefaultSelectedBkColor(dwColor);
		}
		else
		{
			m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
		}
	}

	TFontInfo* CPaintManagerUI::GetDefaultFontInfo()
	{
		if(_parent)
		{
			return _parent->GetDefaultFontInfo();
		}
		if (m_ResInfo.m_DefaultFontInfo.sFontName.IsEmpty())
		{
			if( m_SharedResInfo.m_DefaultFontInfo.tm.tmHeight == 0 ) 
			{
				HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_SharedResInfo.m_DefaultFontInfo.hFont);
				::GetTextMetrics(m_hDcPaint, &m_SharedResInfo.m_DefaultFontInfo.tm);
				::SelectObject(m_hDcPaint, hOldFont);
			}
			return &m_SharedResInfo.m_DefaultFontInfo;
		}
		else
		{
			if( m_ResInfo.m_DefaultFontInfo.tm.tmHeight == 0 ) 
			{
				HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_ResInfo.m_DefaultFontInfo.hFont);
				::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
				::SelectObject(m_hDcPaint, hOldFont);
			}
			return &m_ResInfo.m_DefaultFontInfo;
		}
	}

	void CPaintManagerUI::SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
	{
		if(_parent)
		{
			return _parent->SetDefaultFont(pStrFontName, nSize, bBold, bUnderline, bItalic, bShared);
		}
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		if(lstrlen(pStrFontName) > 0) {
			TCHAR szFaceName[32] = {0};//_T("@");
			_tcsncat(szFaceName, pStrFontName, LF_FACESIZE);
			_tcsncpy(lf.lfFaceName, szFaceName, LF_FACESIZE);
		}
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -GetDPIObj()->Scale(nSize);;
		if( bBold ) lf.lfWeight += FW_BOLD;
		if( bUnderline ) lf.lfUnderline = TRUE;
		if( bItalic ) lf.lfItalic = TRUE;

		HFONT hFont = ::CreateFontIndirect(&lf);
		if( hFont == NULL ) return;

		if (bShared)
		{
			::DeleteObject(m_SharedResInfo.m_DefaultFontInfo.hFont);
			m_SharedResInfo.m_DefaultFontInfo.hFont = hFont;
			m_SharedResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
			m_SharedResInfo.m_DefaultFontInfo.iSize =nSize;
			m_SharedResInfo.m_DefaultFontInfo.bBold = bBold;
			m_SharedResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
			m_SharedResInfo.m_DefaultFontInfo.bItalic = bItalic;
			::ZeroMemory(&m_SharedResInfo.m_DefaultFontInfo.tm, sizeof(m_SharedResInfo.m_DefaultFontInfo.tm));
			if( m_hDcPaint ) {
				HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
				::GetTextMetrics(m_hDcPaint, &m_SharedResInfo.m_DefaultFontInfo.tm);
				::SelectObject(m_hDcPaint, hOldFont);
			}
		}
		else
		{
			::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
			m_ResInfo.m_DefaultFontInfo.hFont = hFont;
			m_ResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
			m_ResInfo.m_DefaultFontInfo.iSize = nSize;
			m_ResInfo.m_DefaultFontInfo.bBold = bBold;
			m_ResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
			m_ResInfo.m_DefaultFontInfo.bItalic = bItalic;
			::ZeroMemory(&m_ResInfo.m_DefaultFontInfo.tm, sizeof(m_ResInfo.m_DefaultFontInfo.tm));
			if( m_hDcPaint ) {
				HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
				::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
				::SelectObject(m_hDcPaint, hOldFont);
			}
		}
	}

	DWORD CPaintManagerUI::GetCustomFontCount(bool bShared) const
	{
		if (bShared)
			return m_SharedResInfo.m_CustomFonts.GetSize();
		else if(_parent)
		{
			return _parent->GetCustomFontCount(bShared);
		}
		else
			return m_ResInfo.m_CustomFonts.GetSize();
	}

	HFONT CPaintManagerUI::AddFont(LPCTSTR pStrFontId, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
	{
		if(_parent)
		{
			return _parent->AddFont(pStrFontId, pStrFontName, nSize, bBold, bUnderline, bItalic, bShared);
		}
		LOGFONT lf = { 0 };
		BOOL failed = 0;
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		if(lstrlen(pStrFontName) > 0) {
			TCHAR szFaceName[32] = {0};//_T("@");
			_tcsncat(szFaceName, pStrFontName, LF_FACESIZE);
			_tcsncpy(lf.lfFaceName, szFaceName, LF_FACESIZE);
		}
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -GetDPIObj()->Scale(nSize);
		if( bBold ) lf.lfWeight = FW_BOLD;
		if( bUnderline ) lf.lfUnderline = TRUE;
		if( bItalic ) lf.lfItalic = TRUE;
		HFONT hFont = ::CreateFontIndirect(&lf);
		if( hFont == NULL ) return NULL;

		TFontInfo* pFontInfo = new TFontInfo;
		if( !pFontInfo ) return false;
		::ZeroMemory(pFontInfo, sizeof(TFontInfo));
		pFontInfo->hFont = hFont;
		pFontInfo->sFontName = lf.lfFaceName;
		pFontInfo->iSize = nSize;
		pFontInfo->bBold = bBold;
		pFontInfo->bUnderline = bUnderline;
		pFontInfo->bItalic = bItalic;
		if( m_hDcPaint ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		bool tmp = pStrFontId==g_tmpFontId; // isTmpId for html
		//LogIs(L"id=str=%s", pStrFontId);
		if(tmp || *pStrFontId>='0' && *pStrFontId<='9') {
			TResInfo & resInfo = bShared || m_bForceUseSharedRes?m_SharedResInfo:m_ResInfo;
			TCHAR idBuffer[16];
			::ZeroMemory(idBuffer, sizeof(idBuffer));
			LPTSTR pstr = NULL;
			int id = tmp?(int)pStrFontId:_tcstol(pStrFontId, &pstr, 10);
			_itot(id, idBuffer, 10);

			TFontInfo* pOldFontInfo = static_cast<TFontInfo*>(resInfo.m_CustomFonts.Find(idBuffer));
			if (pOldFontInfo)
			{
				::DeleteObject(pOldFontInfo->hFont);
				delete pOldFontInfo;
				resInfo.m_CustomFonts.Remove(idBuffer);
			}
			if( !resInfo.m_CustomFonts.Insert(idBuffer, pFontInfo) ) 
			{
				failed = 1;
			}
		} 
		else {
			tmp = bShared || m_bForceUseSharedRes; // is shared named font
			TResInfo & resInfo = tmp?m_SharedResInfo:m_ResInfo;
			int slot = tmp?MAX_UNSHAREDFONT_ID:MAX_UNNAMEDFONT_ID;
			int id = (int)resInfo._namedFontsMap.Find(pStrFontId);
			if (!id)
			{
				if(resInfo._namedFontsMap.Insert(pStrFontId, (LPVOID)(resInfo._namedFonts.size()+slot))) {
					resInfo._namedFonts.push_back(pFontInfo);
				}
				else failed = 1;
			} else {
				id = id-slot;
				if(id>=0 && id<resInfo._namedFonts.size()) { // sanity check
					TFontInfo* pOldFontInfo = resInfo._namedFonts[id];
					if (pOldFontInfo)
					{
						::DeleteObject(pOldFontInfo->hFont);
						delete pOldFontInfo;
					}
					resInfo._namedFonts[id] = pFontInfo;
				}
				else failed = 1;
			}
		}
		if(failed) {
			::DeleteObject(hFont);
			delete pFontInfo;
			return NULL;
		}
		return hFont;
	}

	void CPaintManagerUI::AddFontArray(LPCTSTR pstrPath) 
	{
		if(_parent)
		{
			return _parent->AddFontArray(pstrPath);
		}
		LPBYTE pData = NULL;
		DWORD dwSize = 0;
		do
		{
			QkString sFile = CPaintManagerUI::GetResourcePath();
			if (CPaintManagerUI::GetResourceZip().IsEmpty()) {
				sFile += pstrPath;
				HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
					FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE) break;
				dwSize = ::GetFileSize(hFile, NULL);
				if (dwSize == 0) break;

				DWORD dwRead = 0;
				pData = new BYTE[dwSize];
				::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
				::CloseHandle(hFile);

				if (dwRead != dwSize) {
					delete[] pData;
					pData = NULL;
					break;
				}
			}
			else {
				sFile += CPaintManagerUI::GetResourceZip();
				HZIP hz = NULL;
				if (CPaintManagerUI::IsCachedResourceZip()) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
				else {
					QkString sFilePwd = CPaintManagerUI::GetResourceZipPwd();
#ifdef UNICODE
					char* pwd = w2a((wchar_t*)sFilePwd.GetData());
					hz = OpenZip(sFile.GetData(), pwd);
					if (pwd) delete[] pwd;
#else
					hz = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
				}
				if (hz == NULL) break;
				ZIPENTRY ze;
				int i = 0;
				QkString key = pstrPath;
				key.Replace(_T("\\"), _T("/"));
				if (FindZipItem(hz, key, true, &i, &ze) != 0) break;
				dwSize = ze.unc_size;
				if (dwSize == 0) break;
				pData = new BYTE[dwSize];
				int res = UnzipItem(hz, i, pData, dwSize);
				if (res != 0x00000000 && res != 0x00000600) {
					delete[] pData;
					pData = NULL;
					if (!CPaintManagerUI::IsCachedResourceZip()) CloseZip(hz);
					break;
				}
				if (!CPaintManagerUI::IsCachedResourceZip()) CloseZip(hz);
			}

		} while (0);

		while (!pData)
		{
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile(pstrPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if (dwSize == 0) break;

			DWORD dwRead = 0;
			pData = new BYTE[dwSize];
			::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
			::CloseHandle(hFile);

			if (dwRead != dwSize) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}
		DWORD nFonts;
		HANDLE hFont = ::AddFontMemResourceEx(pData, dwSize, NULL, &nFonts);
		delete[] pData;
		pData = NULL;
		m_aFonts.Add(hFont);
	}
	HFONT CPaintManagerUI::GetFont(int id)
	{
		if (id < 0) return GetDefaultFontInfo()->hFont;
		TFontInfo* pFontInfo = 0;
		if(id < MAX_UNNAMEDFONT_ID) {
			TCHAR idBuffer[16];
			::ZeroMemory(idBuffer, sizeof(idBuffer));
			_itot(id, idBuffer, 10);
			pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
			if( !pFontInfo ) pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
		}
		else if(id < MAX_UNSHAREDFONT_ID) {
			id -= MAX_UNNAMEDFONT_ID;
			if(id>=0 && id < m_ResInfo._namedFonts.size()) {
				pFontInfo = m_ResInfo._namedFonts[id];
			}
		}
		else {
			id -= MAX_UNSHAREDFONT_ID;
			if(id>=0 && id < m_SharedResInfo._namedFonts.size()) {
				pFontInfo = m_SharedResInfo._namedFonts[id];
			}
		}
		if (!pFontInfo) return GetDefaultFontInfo()->hFont;
		return pFontInfo->hFont;
	}

	HFONT CPaintManagerUI::GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		if(_parent)
		{
			return _parent->GetFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
		}
		TFontInfo* pFontInfo = NULL;
		for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.GetValueAt(i));
			if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return pFontInfo->hFont;
		}
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
			pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.GetValueAt(i));
			if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return pFontInfo->hFont;
		}

		return NULL;
	}

	int CPaintManagerUI::GetFontIndex(HFONT hFont, bool bShared)
	{
		if(_parent)
		{
			return _parent->GetFontIndex(hFont, bShared);
		}
		LPVOID pFontInfo = NULL;
		LPCTSTR key;
		TResInfo & resInfo = (bShared)?m_SharedResInfo:m_ResInfo; //  || m_bForceUseSharedRes
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(m_SharedResInfo.m_CustomFonts.GetKeyValueAt(i, key, pFontInfo)) 
			{
				if (pFontInfo && static_cast<TFontInfo*>(pFontInfo)->hFont == hFont) 
					return _ttoi(key);
			}
		}
		return -1;
	}

	int CPaintManagerUI::GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
	{
		if(_parent)
		{
			return _parent->GetFontIndex(pStrFontName, nSize, bBold, bUnderline, bItalic);
		}
		TFontInfo* pFontInfo = NULL;
		LPVOID pRet = NULL;
		LPCTSTR key;
		TResInfo & resInfo = (bShared)?m_SharedResInfo:m_ResInfo; //  || m_bForceUseSharedRes
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(m_SharedResInfo.m_CustomFonts.GetKeyValueAt(i, key, pRet)) 
			{
				pFontInfo = static_cast<TFontInfo*>(pFontInfo);
				if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
					pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
					return _ttoi(key);
			}
		}
		return -1;
	}

	void CPaintManagerUI::RemoveFont(HFONT hFont, bool bShared)
	{
		if(_parent)
		{
			return _parent->RemoveFont(hFont, bShared);
		}
		TFontInfo* pFontInfo = NULL;
		LPVOID pRet = NULL;
		LPCTSTR key;
		TResInfo & resInfo = (bShared)?m_SharedResInfo:m_ResInfo; //  || m_bForceUseSharedRes
		for( int i = 0; i < resInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(resInfo.m_CustomFonts.GetKeyValueAt(i, key, pRet)) 
			{
				pFontInfo = static_cast<TFontInfo*>(pRet);
				if (pFontInfo && pFontInfo->hFont == hFont) 
				{
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
					resInfo.m_CustomFonts.Remove(key);
					return;
				}
			}
		}
		// todo ... 
	}

	void CPaintManagerUI::RemoveFont(int id, bool bShared)
	{
		TResInfo & resInfo = (bShared)?m_SharedResInfo:m_ResInfo; //  || m_bForceUseSharedRes
		TFontInfo* pFontInfo;
		if(_parent)
		{
			return _parent->RemoveFont(id, bShared);
		}
		if(id < MAX_UNNAMEDFONT_ID) {
			TCHAR idBuffer[16];
			::ZeroMemory(idBuffer, sizeof(idBuffer));
			_itot(id, idBuffer, 10);

			pFontInfo = static_cast<TFontInfo*>(resInfo.m_CustomFonts.Find(idBuffer));
			if (pFontInfo)
			{
				::DeleteObject(pFontInfo->hFont);
				delete pFontInfo;
				resInfo.m_CustomFonts.Remove(idBuffer);
			}
		}
		else if(id < MAX_UNSHAREDFONT_ID) {
			id -= MAX_UNNAMEDFONT_ID;
			if(id>=0 && id < m_ResInfo._namedFonts.size()) {
				pFontInfo = m_ResInfo._namedFonts[id];
				m_ResInfo._namedFonts[id] = nullptr;
				delete pFontInfo;
			}
		}
		else {
			id -= MAX_UNSHAREDFONT_ID;
			if(id>=0 && id < m_SharedResInfo._namedFonts.size()) {
				pFontInfo = m_SharedResInfo._namedFonts[id];
				m_SharedResInfo._namedFonts[id] = nullptr;
				delete pFontInfo;
			}
		}
	}

	void CPaintManagerUI::RemoveAllFonts(bool bShared)
	{
		//if(_parent) return _parent->RemoveAllFonts(bShared);
		TFontInfo* pFontInfo;
		LPVOID pRet;
		TResInfo & resInfo = (bShared)?m_SharedResInfo:m_ResInfo; //  || m_bForceUseSharedRes
		for( int i = 0; i< resInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			pFontInfo = static_cast<TFontInfo*>(resInfo.m_CustomFonts.GetValueAt(i));
			if (pFontInfo) {
				::DeleteObject(pFontInfo->hFont);
				delete pFontInfo;
			}
		}
		resInfo._namedFonts.clear();
		resInfo._namedFontsMap.RemoveAll();
		resInfo.m_CustomFonts.RemoveAll();
	}

	//todo check efficient
	TFontInfo* CPaintManagerUI::GetFontInfo(int id)
	{
		if(_parent)
		{
			return _parent->GetFontInfo(id);
		}
		if (id < 0) return GetDefaultFontInfo();

		TCHAR idBuffer[16];
		::ZeroMemory(idBuffer, sizeof(idBuffer));
		_itot(id, idBuffer, 10);
		TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
		if (!pFontInfo) pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
		if (!pFontInfo) pFontInfo = GetDefaultFontInfo();
		if (pFontInfo->tm.tmHeight == 0) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return pFontInfo;
	}

	TFontInfo* CPaintManagerUI::GetFontInfo(HFONT hFont)
	{
		if(_parent)
		{
			return _parent->GetFontInfo(hFont);
		}
		TFontInfo* pFontInfo = NULL;
		for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.GetValueAt(i));
			if (pFontInfo && pFontInfo->hFont == hFont) break;
		}
		if (!pFontInfo)
		{
			for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) 
			{
				pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.GetValueAt(i));
				if (pFontInfo && pFontInfo->hFont == hFont) break;
			}
		}
		if (!pFontInfo) pFontInfo = GetDefaultFontInfo();
		if( pFontInfo->tm.tmHeight == 0 ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return pFontInfo;
	}

	const TImageInfo* CPaintManagerUI::GetImage(LPCTSTR bitmap)
	{
		if(_parent)
		{
			return _parent->GetImage(bitmap);
		}
		TImageInfo* data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
		if( !data ) data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
		return data;
	}

	const TImageInfo* CPaintManagerUI::GetImageEx(LPCTSTR bitmap, LPCTSTR type, DWORD mask, bool bUseHSL, HINSTANCE instance)
	{
		if(_parent)
		{
			return _parent->GetImageEx(bitmap, type, mask, bUseHSL, instance);
		}
		const TImageInfo* data = GetImage(bitmap);
		if( !data ) {
			if( AddImage(bitmap, type, mask, bUseHSL, false, instance) ) {
				if (m_bForceUseSharedRes) data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
				else data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap)); 
			}
		}

		return data;
	}

	const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap, LPCTSTR type, DWORD mask, bool bUseHSL, bool bShared, HINSTANCE instance)
	{
		LogIs(L"AddImage:: %s %s", bitmap?bitmap:L"",type?type:L"" );
		if(_parent)
		{
			return _parent->AddImage(bitmap, type, mask, bUseHSL, bShared, instance);
		}
		if( bitmap == NULL || bitmap[0] == _T('\0') ) return NULL;

		TImageInfo* data = NULL;
		if( type != NULL && lstrlen(type) > 0) {
			if( isdigit(*bitmap) ) {
				LPTSTR pstr = NULL;
				int iIndex = _tcstol(bitmap, &pstr, 10);
				data = CRenderEngine::LoadImageStr(iIndex, type, mask, instance);
			}
		}
		else {
			data = CRenderEngine::LoadImageStr(bitmap, NULL, mask, instance);
		}

		if( data == NULL ) {
			return NULL;
		}
		data->bUseHSL = bUseHSL;
		if( type != NULL ) data->sResType = type;
		data->dwMask = mask;
		if( data->bUseHSL ) {
			data->pSrcBits = new BYTE[data->nX * data->nY * 4];
			::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
		}
		else data->pSrcBits = NULL;
		if( m_bUseHSL ) CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);
		if (data)
		{
			if (bShared || m_bForceUseSharedRes)
			{
				TImageInfo* pOldImageInfo = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
				if (pOldImageInfo)
				{
					CRenderEngine::FreeImage(pOldImageInfo);
					m_SharedResInfo.m_ImageHash.Remove(bitmap);
				}

				if( !m_SharedResInfo.m_ImageHash.Insert(bitmap, data) ) {
					CRenderEngine::FreeImage(data);
					data = NULL;
				}
			}
			else
			{
				TImageInfo* pOldImageInfo = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
				if (pOldImageInfo)
				{
					CRenderEngine::FreeImage(pOldImageInfo);
					m_ResInfo.m_ImageHash.Remove(bitmap);
				}

				if( !m_ResInfo.m_ImageHash.Insert(bitmap, data) ) {
					CRenderEngine::FreeImage(data);
					data = NULL;
				}
			}
		}

		return data;
	}

	const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared)
	{
		if(_parent)
		{
			return _parent->AddImage(bitmap, hBitmap, iWidth, iHeight, bAlpha, bShared);
		}
		// 因无法确定外部HBITMAP格式，不能使用hsl调整
		if( bitmap == NULL || bitmap[0] == _T('\0') ) return NULL;
		if( hBitmap == NULL || iWidth <= 0 || iHeight <= 0 ) return NULL;

		TImageInfo* data = new TImageInfo;
		data->pBits = NULL;
		data->pSrcBits = NULL;
		data->hBitmap = hBitmap;
		data->pBits = NULL;
		data->nX = iWidth;
		data->nY = iHeight;
		data->bAlpha = bAlpha;
		data->bUseHSL = false;
		data->pSrcBits = NULL;
		data->dwMask = 0;

		if (bShared || m_bForceUseSharedRes)
		{
			if( !m_SharedResInfo.m_ImageHash.Insert(bitmap, data) ) {
				CRenderEngine::FreeImage(data);
				data = NULL;
			}
		}
		else
		{
			if( !m_ResInfo.m_ImageHash.Insert(bitmap, data) ) {
				CRenderEngine::FreeImage(data);
				data = NULL;
			}
		}

		return data;
	}

	void CPaintManagerUI::RemoveImage(LPCTSTR bitmap, bool bShared)
	{
		if(_parent)
		{
			return _parent->RemoveImage(bitmap, bShared);
		}
		TImageInfo* data = NULL;
		if (bShared) 
		{
			data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
			if (data)
			{
				CRenderEngine::FreeImage(data) ;
				m_SharedResInfo.m_ImageHash.Remove(bitmap);
			}
		}
		else
		{
			data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
			if (data)
			{
				CRenderEngine::FreeImage(data) ;
				m_ResInfo.m_ImageHash.Remove(bitmap);
			}
		}
	}

	void CPaintManagerUI::RemoveAllImages(bool bShared)
	{
		//if(_parent) return _parent->RemoveAllImages(bShared);
		TResInfo & resInfo = (bShared)?m_SharedResInfo:m_ResInfo; //  || m_bForceUseSharedRes
		TImageInfo* data;
		for( int i = 0; i< resInfo.m_ImageHash.GetSize(); i++ ) {
			data = static_cast<TImageInfo*>(resInfo.m_ImageHash.GetValueAt(i));
			if (data) CRenderEngine::FreeImage(data);
		}
		resInfo.m_ImageHash.RemoveAll();
	}

	void CPaintManagerUI::AdjustSharedImagesHSL()
	{
		TImageInfo* data;
		for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
			data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.GetValueAt(i));
			if( data && data->bUseHSL ) {
				CRenderEngine::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
			}
		}
	}

	void CPaintManagerUI::AdjustImagesHSL()
	{
		if(_parent)
		{
			return _parent->AdjustImagesHSL();
		}
		TImageInfo* data;
		for( int i = 0; i< m_ResInfo.m_ImageHash.GetSize(); i++ ) {
			data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.GetValueAt(i));
			if( data && data->bUseHSL ) {
				CRenderEngine::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
			}
		}
		Invalidate();
	}

	void CPaintManagerUI::PostAsyncNotify()
	{
		if (!m_bAsyncNotifyPosted) {
			::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
			m_bAsyncNotifyPosted = true;
		}
	}
	void CPaintManagerUI::ReloadSharedImages()
	{
		LPCTSTR bitmap;
		TImageInfo* data = NULL;
		LPVOID pRet;
		TImageInfo* pNewData = NULL;
		for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
			if(m_SharedResInfo.m_ImageHash.GetKeyValueAt(i, bitmap, pRet)) 
			{
				data = static_cast<TImageInfo*>(pRet);
				if( data != NULL ) {
					if( !data->sResType.IsEmpty() ) {
						if( isdigit(*bitmap) ) {
							LPTSTR pstr = NULL;
							int iIndex = _tcstol(bitmap, &pstr, 10);
							pNewData = CRenderEngine::LoadImageStr(iIndex, data->sResType.GetData(), data->dwMask);
						}
					}
					else {
						pNewData = CRenderEngine::LoadImageStr(bitmap, NULL, data->dwMask);
					}
					if( pNewData == NULL ) continue;

					CRenderEngine::FreeImage(data, false);
					data->hBitmap = pNewData->hBitmap;
					data->pBits = pNewData->pBits;
					data->nX = pNewData->nX;
					data->nY = pNewData->nY;
					data->bAlpha = pNewData->bAlpha;
					data->pSrcBits = NULL;
					if( data->bUseHSL ) {
						data->pSrcBits = new BYTE[data->nX * data->nY * 4];
						::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
					}
					else data->pSrcBits = NULL;
					if( m_bUseHSL ) CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);

					delete pNewData;
				}
			}
		}
	}

	void CPaintManagerUI::ReloadImages()
	{
		if(_parent) return _parent->ReloadImages();
		RemoveAllDrawInfos();

		LPCTSTR bitmap;
		TImageInfo* data = NULL;
		LPVOID pRet;
		TImageInfo* pNewData = NULL;
		for( int i = 0; i< m_ResInfo.m_ImageHash.GetSize(); i++ ) {
			if(m_ResInfo.m_ImageHash.GetKeyValueAt(i, bitmap, pRet)) 
			{
				data = static_cast<TImageInfo*>(pRet);
				if( data != NULL ) {
					if( !data->sResType.IsEmpty() ) {
						if( isdigit(*bitmap) ) {
							LPTSTR pstr = NULL;
							int iIndex = _tcstol(bitmap, &pstr, 10);
							pNewData = CRenderEngine::LoadImageStr(iIndex, data->sResType.GetData(), data->dwMask);
						}
					}
					else {
						pNewData = CRenderEngine::LoadImageStr(bitmap, NULL, data->dwMask);
					}

					CRenderEngine::FreeImage(data, false);
					if( pNewData == NULL ) {
						m_ResInfo.m_ImageHash.Remove(bitmap);
						continue;
					}
					data->hBitmap = pNewData->hBitmap;
					data->pBits = pNewData->pBits;
					data->nX = pNewData->nX;
					data->nY = pNewData->nY;
					data->bAlpha = pNewData->bAlpha;
					data->pSrcBits = NULL;
					if( data->bUseHSL ) {
						data->pSrcBits = new BYTE[data->nX * data->nY * 4];
						::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
					}
					else data->pSrcBits = NULL;
					if( m_bUseHSL ) CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);

					delete pNewData;
				}
			}
		}

		if( m_pRoot ) m_pRoot->Invalidate();
	}

	const TDrawInfo* CPaintManagerUI::GetDrawInfo(LPCTSTR pStrImage)
	{
		if(_parent)
		{
			return _parent->GetDrawInfo(pStrImage);
		}
		TDrawInfo* pDrawInfo = static_cast<TDrawInfo*>(m_ResInfo.m_DrawInfoHash.Find(pStrImage));
		if(pDrawInfo == NULL) { //  && !sKey.IsEmpty()
			pDrawInfo = new TDrawInfo();
			pDrawInfo->Parse(pStrImage, this);
			m_ResInfo.m_DrawInfoHash.Insert(pStrImage, pDrawInfo);
		}
		return pDrawInfo;
	}

	void CPaintManagerUI::RemoveDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify)
	{
		if(_parent) return _parent->RemoveDrawInfo(pStrImage, pStrModify);
		QkString sStrImage = pStrImage;
		QkString sStrModify = pStrModify;
		QkString sKey = sStrImage + sStrModify;
		TDrawInfo* pDrawInfo = static_cast<TDrawInfo*>(m_ResInfo.m_DrawInfoHash.Find(sKey));
		if(pDrawInfo != NULL) {
			m_ResInfo.m_DrawInfoHash.Remove(sKey);
			delete pDrawInfo;
			pDrawInfo = NULL;
		}
	}

	void CPaintManagerUI::RemoveAllDrawInfos()
	{
		//if(_parent) return _parent->RemoveAllDrawInfos();
		TDrawInfo* pDrawInfo = NULL;
		for( int i = 0; i< m_ResInfo.m_DrawInfoHash.GetSize(); i++ ) {
			pDrawInfo = static_cast<TDrawInfo*>(m_ResInfo.m_DrawInfoHash.GetValueAt(i));
			if (pDrawInfo) delete pDrawInfo;
		}
		m_ResInfo.m_DrawInfoHash.RemoveAll();
	}

	Style* CPaintManagerUI::GetStyleForId(LPCTSTR pName) const
	{
		if(_parent)
		{
			return _parent->GetStyleForId(pName);
		}
		Style* pStyle = static_cast<Style*>(m_ResInfo.m_StyleIdHash.Find(pName));
		if( !pStyle ) pStyle = static_cast<Style*>(m_SharedResInfo.m_StyleIdHash.Find(pName));
		return pStyle;
	}

	void CPaintManagerUI::AddStyle(Style* style, bool bShared)
	{
		if(_parent)
		{
			return _parent->AddStyle(style, bShared);
		}
		Style* pTmpDefAttr;
		TResInfo & resInfo = (bShared || m_bForceUseSharedRes)?m_SharedResInfo:m_ResInfo;
		bool added = false;
		if (!style->id.IsEmpty())
		{
			pTmpDefAttr = static_cast<Style*>(resInfo.m_StyleIdHash.Set(style->id, (LPVOID)style));
			if (pTmpDefAttr && (pTmpDefAttr->name.IsEmpty() || !resInfo.m_StyleControlHash.Find(pTmpDefAttr->name)))
				delete pTmpDefAttr;
			added = true;
		}
		if (!style->name.IsEmpty())
		{
			pTmpDefAttr = static_cast<Style*>(resInfo.m_StyleControlHash.Set(style->name, (LPVOID)style));
			if (pTmpDefAttr && (pTmpDefAttr->id.IsEmpty() || !resInfo.m_StyleIdHash.Find(pTmpDefAttr->id)))
				delete pTmpDefAttr;
			added = true;
		}
		if (!added)
		{
			delete style;
		}
	}

	void CPaintManagerUI::AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStyleId, LPCTSTR pStrControlAttrList, bool bShared)
	{
		if(_parent)
		{
			return _parent->AddDefaultAttributeList(pStrControlName, pStyleId, pStrControlAttrList, bShared);
		}
		Style* style = new Style{};
		style->name = pStrControlName;
		style->id = pStyleId;
		AddStyle(style, bShared);

		int index=0;
		QkString sXmlData = pStrControlAttrList;
		sXmlData.Replace(_T("&quot;"), _T("\""));
		LPCTSTR pstrList = sXmlData.GetData();
		// 解析样式属性
		QkString sItem;
		QkString sValue;
		sItem.AsBuffer();
		sValue.AsBuffer();
		style->styles.resize(64);
		//style->styles.resize(1);
		while( *pstrList != _T('\0') ) {
			sItem.Empty();
			sValue.Empty();
			while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
				LPTSTR pstrTemp = ::CharNext(pstrList);
				while( pstrList < pstrTemp) {
					sItem += *pstrList++;
				}
			}
			ASSERT( *pstrList == _T('=') );
			if( *pstrList++ != _T('=') ) break;
			ASSERT( *pstrList == _T('\"') );
			if( *pstrList++ != _T('\"') ) break;
			while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
				LPTSTR pstrTemp = ::CharNext(pstrList);
				while( pstrList < pstrTemp) {
					sValue += *pstrList++;
				}
			}
			ASSERT( *pstrList == _T('\"') );
			if( *pstrList++ != _T('\"') ) break;

			++index;
			//style->styles.resize(index);
			StyleDefine & styleDef = style->styles[index-1];
			styleDef.name = sItem;
			styleDef.value = sValue;

			if( *pstrList++ != _T(' ') && *pstrList++ != _T(',') ) break;
		}
		//style->styles.resize(index);
		//style->styles.resize(0);
	}

	Style* CPaintManagerUI::GetDefaultAttributeList(LPCTSTR pStrControlName) const
	{
		if(_parent)
		{
			return _parent->GetDefaultAttributeList(pStrControlName);
		}
		Style* pDefaultAttr = static_cast<Style*>(m_ResInfo.m_StyleControlHash.Find(pStrControlName));
		if( !pDefaultAttr ) pDefaultAttr = static_cast<Style*>(m_SharedResInfo.m_StyleControlHash.Find(pStrControlName));
		return pDefaultAttr;
	}

	bool CPaintManagerUI::RemoveDefaultAttributeList(LPCTSTR pStrControlName, bool bShared)
	{
		if(_parent)
		{
			return _parent->RemoveDefaultAttributeList(pStrControlName, bShared);
		}
		TResInfo & resInfo = (bShared)?m_SharedResInfo:m_ResInfo; //  || m_bForceUseSharedRes
		Style* pDefaultAttr = static_cast<Style*>(resInfo.m_StyleControlHash.Find(pStrControlName));
		if( !pDefaultAttr ) return false;
		if (!resInfo.m_StyleIdHash.Find(pStrControlName))
		{
			delete pDefaultAttr;
		}
		return resInfo.m_StyleControlHash.Remove(pStrControlName);
	}

	void CPaintManagerUI::RemoveAllDefaultAttributeList(bool bShared)
	{
		//if(_parent) return _parent->RemoveAllDefaultAttributeList(bShared);
		TResInfo & resInfo = (bShared)?m_SharedResInfo:m_ResInfo; //  || m_bForceUseSharedRes
		Style* pAttr;
		for( int i = 0; i< resInfo.m_StyleIdHash.GetSize(); i++ ) {
			pAttr = static_cast<Style*>(resInfo.m_StyleIdHash.GetValueAt(i));
			if(pAttr->name) resInfo.m_StyleControlHash.Remove(pAttr->name);
			if(pAttr) delete pAttr;
		}
		for( int i = 0; i< resInfo.m_StyleControlHash.GetSize(); i++ ) {
			pAttr = static_cast<Style*>(resInfo.m_StyleControlHash.GetValueAt(i));
			if(pAttr) delete pAttr;
		}
		resInfo.m_StyleIdHash.RemoveAll();
		resInfo.m_StyleControlHash.RemoveAll();
	}

	void CPaintManagerUI::AddWindowCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr)
	{
		if(_parent)
		{
			return _parent->AddWindowCustomAttribute(pstrName, pstrAttr);
		}
		if( pstrName == NULL || pstrName[0] == _T('\0') || pstrAttr == NULL || pstrAttr[0] == _T('\0') ) return;
		QkString* pCostomAttr = new QkString(pstrAttr);
		if (pCostomAttr != NULL) {
			if (m_mWindowCustomAttrHash.Find(pstrName) == NULL)
				m_mWindowCustomAttrHash.Set(pstrName, (LPVOID)pCostomAttr);
			else
				delete pCostomAttr;
		}
	}

	LPCTSTR CPaintManagerUI::GetWindowCustomAttribute(LPCTSTR pstrName) const
	{
		if(_parent)
		{
			return _parent->GetWindowCustomAttribute(pstrName);
		}
		if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
		QkString* pCostomAttr = static_cast<QkString*>(m_mWindowCustomAttrHash.Find(pstrName));
		if( pCostomAttr ) return pCostomAttr->GetData();
		return NULL;
	}

	bool CPaintManagerUI::RemoveWindowCustomAttribute(LPCTSTR pstrName)
	{
		if(_parent)
		{
			return _parent->RemoveWindowCustomAttribute(pstrName);
		}
		if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
		QkString* pCostomAttr = static_cast<QkString*>(m_mWindowCustomAttrHash.Find(pstrName));
		if( !pCostomAttr ) return false;

		delete pCostomAttr;
		return m_mWindowCustomAttrHash.Remove(pstrName);
	}

	void CPaintManagerUI::RemoveAllWindowCustomAttribute()
	{
		//if(_parent) return _parent->RemoveAllWindowCustomAttribute();
		QkString* pCostomAttr;
		for( int i = 0; i< m_mWindowCustomAttrHash.GetSize(); i++ ) {
			pCostomAttr = static_cast<QkString*>(m_mWindowCustomAttrHash.GetValueAt(i));
			if(pCostomAttr) delete pCostomAttr;
		}
		m_mWindowCustomAttrHash.Resize();
	}

	CControlUI* CPaintManagerUI::GetRoot() const
	{
		ASSERT(m_pRoot);
		return m_pRoot;
	}

	CControlUI* CPaintManagerUI::FindControl(POINT pt) const
	{
		ASSERT(m_pRoot);
		return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST | UIFIND_FOCUSABLE);
	}
	
	CControlUI* CPaintManagerUI::FindControlEx(POINT pt, UINT flag) const
	{
		ASSERT(m_pRoot);
		return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST | flag);
	}

	CControlUI* CPaintManagerUI::FindControl(LPCTSTR pstrName) const
	{
		ASSERT(m_pRoot);
		return static_cast<CControlUI*>(m_mNameHash.Find(pstrName));
	}

	CControlUI* CPaintManagerUI::FindSubControlByPoint(CControlUI* pParent, POINT pt) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	}

	CControlUI* CPaintManagerUI::FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);
	}

	CControlUI* CPaintManagerUI::FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex)
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		m_aFoundControls.Resize(iIndex + 1);
		return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
	}

	CStdPtrArray* CPaintManagerUI::FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass)
	{
		if( pParent == NULL ) pParent = GetRoot();
		ASSERT(pParent);
		m_aFoundControls.Empty();
		pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
		return &m_aFoundControls;
	}

	CStdPtrArray* CPaintManagerUI::GetFoundControls()
	{
		return &m_aFoundControls;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromNameHash(CControlUI* pThis, LPVOID pData)
	{
		CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(pData);
		const QkString& sName = pThis->GetName();
		if( sName.IsEmpty() ) return NULL;
		// Add this control to the hash list
		pManager->m_mNameHash.Set(sName, pThis);
		return NULL; // Attempt to add all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromCount(CControlUI* /*pThis*/, LPVOID pData)
	{
		int* pnCount = static_cast<int*>(pData);
		(*pnCount)++;
		return NULL;  // Count all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromPoint(CControlUI* pThis, LPVOID pData)
	{
		LPPOINT pPoint = static_cast<LPPOINT>(pData);
		return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromTab(CControlUI* pThis, LPVOID pData)
	{
		FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
		if( pInfo->pFocus == pThis ) {
			if( pInfo->bForward ) pInfo->bNextIsIt = true;
			return pInfo->bForward ? NULL : pInfo->pLast;
		}
		if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
		pInfo->pLast = pThis;
		if( pInfo->bNextIsIt ) return pThis;
		if( pInfo->pFocus == NULL ) return pThis;
		return NULL;  // Examine all controls
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromShortcut(CControlUI* pThis, LPVOID pData)
	{
		if( !pThis->IsVisible() ) return NULL; 
		FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
		if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = true;
		if( _tcsstr(pThis->GetClass(), _T("LabelUI")) != NULL ) return NULL;   // Labels never get focus!
		return pFS->bPickNext ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromName(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
		const QkString& sName = pThis->GetName();
		if( sName.IsEmpty() ) return NULL;
		return (_tcsicmp(sName, pstrName) == 0) ? pThis : NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlFromClass(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
		LPCTSTR pType = pThis->GetClass();
		CStdPtrArray* pFoundControls = pThis->GetManager()->GetFoundControls();
		if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) {
			int iIndex = -1;
			while( pFoundControls->GetAt(++iIndex) != NULL ) ;
			if( iIndex < pFoundControls->GetSize() ) pFoundControls->SetAt(iIndex, pThis);
		}
		if( pFoundControls->GetAt(pFoundControls->GetSize() - 1) != NULL ) return pThis; 
		return NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlsFromClass(CControlUI* pThis, LPVOID pData)
	{
		LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
		LPCTSTR pType = pThis->GetClass();
		if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) 
			pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
		return NULL;
	}

	CControlUI* CALLBACK CPaintManagerUI::__FindControlsFromUpdate(CControlUI* pThis, LPVOID pData)
	{
		if( pThis->IsUpdateNeeded() ) {
			pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
			return pThis;
		}
		return NULL;
	}

	bool CPaintManagerUI::TranslateAccelerator(LPMSG pMsg)
	{
		for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
		{
			LRESULT lResult = static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i])->TranslateAccelerator(pMsg);
			if( lResult == S_OK ) return true;
		}
		return false;
	}

	bool CPaintManagerUI::TranslateMessage(const LPMSG pMsg)
	{
		// Pretranslate Message takes care of system-wide messages, such as
		// tabbing and shortcut key-combos. We'll look for all messages for
		// each window and any child control attached.
		UINT uStyle = GetWindowStyle(pMsg->hwnd);
		UINT uChildRes = uStyle & WS_CHILD;	
		LRESULT lRes = 0;
		if (uChildRes != 0)
		{
			HWND hWndParent = ::GetParent(pMsg->hwnd);

			for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
			{
				CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);        
				HWND hTempParent = hWndParent;
				while(hTempParent)
				{
					if(pMsg->hwnd == pT->GetPaintWindow() || hTempParent == pT->GetPaintWindow())
					{
						if (pT->TranslateAccelerator(pMsg))
							return true;

						pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes);
					}
					hTempParent = ::GetParent(hTempParent);
				}
			}
		}
		else
		{
			for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
			{
				CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
				if(pMsg->hwnd == pT->GetPaintWindow())
				{
					if (pT->TranslateAccelerator(pMsg))
						return true;

					if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
						return true;

					return false;
				}
			}
		}
		return false;
	}

	bool CPaintManagerUI::AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
	{
		ASSERT(m_aTranslateAccelerator.Find(pTranslateAccelerator) < 0);
		return m_aTranslateAccelerator.Add(pTranslateAccelerator);
	}

	bool CPaintManagerUI::RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
	{
		for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
		{
			if (static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i]) == pTranslateAccelerator)
			{
				return m_aTranslateAccelerator.Remove(i);
			}
		}
		return false;
	}

	void CPaintManagerUI::UsedVirtualWnd(bool bUsed)
	{
		m_bUsedVirtualWnd = bUsed;
	}

	const TImageInfo* CPaintManagerUI::GetImageString(LPCTSTR pStrImage, LPCTSTR pStrModify)
	{
		QkString sName = pStrImage;
		QkString sImageResType = _T("");
		DWORD dwMask = 0;
		QkString sItem;
		QkString sValue;
		LPCTSTR pstr = NULL;

		for( int i = 0; i < 2; ++i) {
			if( i == 1)
				pStrImage = pStrModify;

			if( !pStrImage ) continue;

			while( *pStrImage != _T('\0') ) {
				sItem.Empty();
				sValue.Empty();
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
					LPTSTR pstrTemp = ::CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sItem += *pStrImage++;
					}
				}
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				if( *pStrImage++ != _T('=') ) break;
				while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
				if( *pStrImage++ != _T('\'') ) break;
				while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
					LPTSTR pstrTemp = ::CharNext(pStrImage);
					while( pStrImage < pstrTemp) {
						sValue += *pStrImage++;
					}
				}
				if( *pStrImage++ != _T('\'') ) break;
				if( !sValue.IsEmpty() ) {
					if( sItem == _T("file") || sItem == _T("res") ) {
						sName = sValue;
					}
					else if( sItem == _T("restype") ) {
						sImageResType = sValue;
					}
					else if( sItem == _T("mask") ) 
					{
						pstr = STR2ARGB(sValue.GetData(), dwMask);
					}

				}
				if( *pStrImage++ != _T(' ') ) break;
			}
		}
		return GetImageEx(sName, sImageResType, dwMask);
	}

	bool CPaintManagerUI::EnableDragDrop(bool bEnable)
	{
		if(m_bDragDrop == bEnable) return false;
		m_bDragDrop = bEnable;

		if(bEnable) {
			AddRef();

			if(FAILED(RegisterDragDrop(m_hWndPaint, this))) {
				return false;
			}

			SetTargetWnd(m_hWndPaint);

			FORMATETC ftetc={0};
			ftetc.cfFormat = CF_BITMAP;
			ftetc.dwAspect = DVASPECT_CONTENT;
			ftetc.lindex = -1;
			ftetc.tymed = TYMED_GDI;
			AddSuportedFormat(ftetc);
			ftetc.cfFormat = CF_DIB;
			ftetc.tymed = TYMED_HGLOBAL;
			AddSuportedFormat(ftetc);
			ftetc.cfFormat = CF_HDROP;
			ftetc.tymed = TYMED_HGLOBAL;
			AddSuportedFormat(ftetc);
			ftetc.cfFormat = CF_ENHMETAFILE;
			ftetc.tymed = TYMED_ENHMF;
			AddSuportedFormat(ftetc);
		}
		else{
			Release();
			if(FAILED(RevokeDragDrop(m_hWndPaint))) {
				return false;
			}
		}
		return true;
	}

	static WORD DIBNumColors(void* pv) 
	{     
		int bits;     
		LPBITMAPINFOHEADER  lpbi;     
		LPBITMAPCOREHEADER  lpbc;      
		lpbi = ((LPBITMAPINFOHEADER)pv);     
		lpbc = ((LPBITMAPCOREHEADER)pv);      
		/*  With the BITMAPINFO format headers, the size of the palette 
		*  is in biClrUsed, whereas in the BITMAPCORE - style headers, it      
		*  is dependent on the bits per pixel ( = 2 raised to the power of      
		*  bits/pixel).
		*/     
		if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
		{         
			if (lpbi->biClrUsed != 0)
				return (WORD)lpbi->biClrUsed;         
			bits = lpbi->biBitCount;     
		}     
		else         
			bits = lpbc->bcBitCount;
		switch (bits)
		{         
		case 1:                 
			return 2;         
		case 4:                 
			return 16;         
		case 8:       
			return 256;
		default:
			/* A 24 bitcount DIB has no color table */                 
			return 0;
		} 
	} 
	//code taken from SEEDIB MSDN sample
	static WORD ColorTableSize(LPVOID lpv)
	{
		LPBITMAPINFOHEADER lpbih = (LPBITMAPINFOHEADER)lpv;

		if (lpbih->biSize != sizeof(BITMAPCOREHEADER))
		{
			if (((LPBITMAPINFOHEADER)(lpbih))->biCompression == BI_BITFIELDS)
				/* Remember that 16/32bpp dibs can still have a color table */
				return (sizeof(DWORD) * 3) + (DIBNumColors (lpbih) * sizeof (RGBQUAD));
			else
				return (WORD)(DIBNumColors (lpbih) * sizeof (RGBQUAD));
		}
		else
			return (WORD)(DIBNumColors (lpbih) * sizeof (RGBTRIPLE));
	}

	bool CPaintManagerUI::OnDrop(FORMATETC* pFmtEtc, STGMEDIUM& medium,DWORD *pdwEffect)
	{
		POINT ptMouse = {0};
		GetCursorPos(&ptMouse);
		::SendMessage(m_hTargetWnd, WM_LBUTTONUP, NULL, MAKELPARAM(ptMouse.x, ptMouse.y));

		if(pFmtEtc->cfFormat == CF_DIB && medium.tymed == TYMED_HGLOBAL)
		{
			if(medium.hGlobal != NULL)
			{
				LPBITMAPINFOHEADER  lpbi = (BITMAPINFOHEADER*)GlobalLock(medium.hGlobal);
				if(lpbi != NULL)
				{
					HBITMAP hbm = NULL;
					HDC hdc = GetDC(NULL);
					if(hdc != NULL)
					{
						int i = ((BITMAPFILEHEADER *)lpbi)->bfOffBits;
						hbm = CreateDIBitmap(hdc,(LPBITMAPINFOHEADER)lpbi,
							(LONG)CBM_INIT,
							(LPSTR)lpbi + lpbi->biSize + ColorTableSize(lpbi),
							(LPBITMAPINFO)lpbi,DIB_RGB_COLORS);

						::ReleaseDC(NULL,hdc);
					}
					GlobalUnlock(medium.hGlobal);
					if(hbm != NULL)
						hbm = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm);
					if(hbm != NULL)
						DeleteObject(hbm);
					return true; //release the medium
				}
			}
		}
		if(pFmtEtc->cfFormat == CF_BITMAP && medium.tymed == TYMED_GDI)
		{
			if(medium.hBitmap != NULL)
			{
				HBITMAP hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)medium.hBitmap);
				if(hBmp != NULL)
					DeleteObject(hBmp);
				return false; //don't free the bitmap
			}
		}
		if(pFmtEtc->cfFormat == CF_ENHMETAFILE && medium.tymed == TYMED_ENHMF)
		{
			ENHMETAHEADER emh;
			GetEnhMetaFileHeader(medium.hEnhMetaFile, sizeof(ENHMETAHEADER),&emh);
			RECT rc;//={0,0,EnhMetaHdr.rclBounds.right-EnhMetaHdr.rclBounds.left, EnhMetaHdr.rclBounds.bottom-EnhMetaHdr.rclBounds.top};
			HDC hDC= GetDC(m_hTargetWnd);
			//start code: taken from ENHMETA.EXE MSDN Sample
			//*ALSO NEED to GET the pallete (select and RealizePalette it, but i was too lazy*
			// Get the characteristics of the output device
			float PixelsX = (float)GetDeviceCaps( hDC, HORZRES );
			float PixelsY = (float)GetDeviceCaps( hDC, VERTRES );
			float MMX = (float)GetDeviceCaps( hDC, HORZSIZE );
			float MMY = (float)GetDeviceCaps( hDC, VERTSIZE );
			// Calculate the rect in which to draw the metafile based on the
			// intended size and the current output device resolution
			// Remember that the intended size is given in 0.01mm units, so
			// convert those to device units on the target device
			rc.top = (int)((float)(emh.rclFrame.top) * PixelsY / (MMY*100.0f));
			rc.left = (int)((float)(emh.rclFrame.left) * PixelsX / (MMX*100.0f));
			rc.right = (int)((float)(emh.rclFrame.right) * PixelsX / (MMX*100.0f));
			rc.bottom = (int)((float)(emh.rclFrame.bottom) * PixelsY / (MMY*100.0f));
			//end code: taken from ENHMETA.EXE MSDN Sample

			HDC hdcMem = CreateCompatibleDC(hDC);
			HGDIOBJ hBmpMem = CreateCompatibleBitmap(hDC, emh.rclBounds.right, emh.rclBounds.bottom);
			HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBmpMem);
			PlayEnhMetaFile(hdcMem,medium.hEnhMetaFile,&rc);
			HBITMAP hBmp = (HBITMAP)::SelectObject(hdcMem, hOldBmp);
			DeleteDC(hdcMem);
			ReleaseDC(m_hTargetWnd,hDC);
			hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
			if(hBmp != NULL)
				DeleteObject(hBmp);
			return true;
		}
		if(pFmtEtc->cfFormat == CF_HDROP && medium.tymed == TYMED_HGLOBAL)
		{
			HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
			if(hDrop != NULL)
			{
				TCHAR szFileName[MAX_PATH];
				UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); 
				if(cFiles > 0)
				{
					DragQueryFile(hDrop, 0, szFileName, sizeof(szFileName)); 
					HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, szFileName,IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);
					if(hBitmap)
					{
						HBITMAP hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
						if(hBmp != NULL)
							DeleteObject(hBmp);
					}
				}
			}
			GlobalUnlock(medium.hGlobal);
		}
		return true; //let base free the medium
	}
} // namespace DuiLib
