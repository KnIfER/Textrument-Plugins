#include "StdAfx.h"

using namespace Gdiplus;
int tickSetAttr=0;
namespace DuiLib {
	IMPLEMENT_QKCONTROL(CControlUI)

	CControlUI::CControlUI()
		:_manager(NULL)
		,_hWnd(NULL) 
		,_parent(NULL) 
		,_instance(NULL)
		,m_uFloatAlign(0)
		,m_chShortcut('\0')
		,m_pTag(NULL)
		,m_dwBackColor(0)
		,m_dwBackColor2(0)
		,m_dwBackColor3(0)
		,m_dwForeColor(0)
		,_borderColor(0)
		,_focusedBorderColor(0)
		,_borderSizeType(0)
		,m_nBorderStyle(PS_SOLID)
		,m_nTooltipWidth(300)
		,m_wCursor((WORD)IDC_ARROW)
		,_hCursor(CPaintManagerUI::hCursorArrow)

		,_statusDrawable(NULL)

		,_LastScaleProfile(-1)
		,_view_states(0)
		,m_uButtonState(0)
	{
		m_cXY.cx = m_cXY.cy = 0;
		m_cxyFixed.cx = m_cxyFixed.cy = -1;
		m_cxyMin.cx = m_cxyMin.cy = 0;
		m_cxyMax.cx = m_cxyMax.cy = -1;
		m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

		_view_states |= VIEWSTATEMASK_Enabled;
		_view_states |= VIEWSTATEMASK_Focusable;
		_view_states |= VIEWSTATEMASK_Visibility;
		_view_states |= VIEWSTATEMASK_MouseEnabled;
		_view_states |= VIEWSTATEMASK_KeyboardEnabled;

		_view_states |= VIEWSTATEMASK_IsDirectUI;
		_view_states |= VIEWSTATEMASK_AutoDestroy;
		_view_states |= VIEWSTATEMASK_DelayedDestroy;
		_view_states |= VIEWSTATEMASK_KeyboardEnabled;
		_view_states |= VIEWSTATEMASK_UpdateNeeded;

		::ZeroMemory(&m_rcPadding, sizeof(RECT));
		::ZeroMemory(&m_rcItem, sizeof(RECT));
		::ZeroMemory(&m_rcPaint, sizeof(RECT));
		::ZeroMemory(&m_rcBorderSize,sizeof(RECT));
		::ZeroMemory(&m_tRelativePos, sizeof(TRelativePosUI));

		::ZeroMemory(&m_rcInset, sizeof(RECT));
		::ZeroMemory(&m_rcInsetScaled, sizeof(RECT));
		::ZeroMemory(&_borderInset, sizeof(RECT));

		m_piFloatPercent.left = m_piFloatPercent.top = m_piFloatPercent.right = m_piFloatPercent.bottom = 0.0f;
	}

	CControlUI::~CControlUI()
	{
		if( OnDestroy ) OnDestroy(this);
		_view_states &= ~VIEWSTATEMASK_DelayedDestroy;
		RemoveAllCustomAttribute();	
		RemoveAll();
		if( _manager != NULL ) _manager->ReapObjects(this);
		if( _statusDrawable ) delete _statusDrawable;
	}
	
	void CControlUI::Free()
	{
		if( OnDestroy ) OnDestroy(this);
		_view_states &= ~VIEWSTATEMASK_DelayedDestroy;
		RemoveAllCustomAttribute();	
		RemoveAll();
		if( _manager != NULL ) _manager->ReapObjects(this);
	}

	QkString CControlUI::GetName() const
	{
		return m_sName;
	}

	void CControlUI::SetName(LPCTSTR pstrName)
	{
		m_sName = pstrName;
	}

	UINT CControlUI::GetID() const
	{
		return _id;
	}

	void CControlUI::SetID(UINT id)
	{
		_id = id;
	}

	LPVOID CControlUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_CONTROL) == 0 ) return this;
		return NULL;
	}

	LPCTSTR CControlUI::GetClass() const
	{
		return _T("ControlUI");
	}

	UINT CControlUI::GetControlFlags() const
	{
		return 0;
	}

	bool CControlUI::Activate()
	{
		if( !IsVisible() ) return false;
		if( !IsEnabled() ) return false;
		return true;
	}

	CPaintManagerUI* CControlUI::GetManager() const
	{
		return _manager;
	}

	void CControlUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit, bool setChild)
	{
		/* DEBUG_PARSE_LAYOUT */
		//LogIs(4, L"CControlUI::SetManager, tagName=%s, id=%s, text=%s, bInit=%d, _parent=%ld", (LPCWSTR)GetClass(), (LPCWSTR)GetName(), (LPCWSTR)GetText(), bInit, _parent);

		_manager = pManager;
		_parent = pParent;
		if(setChild)
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			static_cast<CControlUI*>(m_items[it])->SetManager(pManager, this, bInit);
		}

		if (pParent)
		{
			_hParent = _parent->GetHWND();
		}
		if (pManager && m_bIsDirectUI)
		{
			_hWnd = pManager->GetPaintWindow();
		}
		if( bInit && _parent ) Init();
	}

	CControlUI* CControlUI::GetParent() const
	{
		return _parent;
	}

	void CControlUI::setInstance(HINSTANCE instance)
	{
		_instance = instance;
	};

	HWND CControlUI::GetHWND() 
	{
		return _hWnd;
	}

	void CControlUI::ApplyInsetToRect(RECT & rc) const {
		rc.left +=   m_rcInsetScaled.left;
		rc.top +=    m_rcInsetScaled.top;
		rc.right -=  m_rcInsetScaled.right;
		rc.bottom -= m_rcInsetScaled.bottom;
		if (_borderSizeType)
		{
			if (_borderSizeType==1)
			{
				int border = _LastScaleProfile?MulDiv(m_rcBorderSize.left, _LastScaleProfile, 100):m_rcBorderSize.left;
				rc.left +=   border;
				rc.top +=    border;
				rc.right -=  border;
				rc.bottom -= border;
			}
			else
			{
				rc.left +=   _rcBorderSizeScaled.left;
				rc.top +=    _rcBorderSizeScaled.top;
				rc.right -=  _rcBorderSizeScaled.right;
				rc.bottom -= _rcBorderSizeScaled.bottom;
			}
		}
	}

	void CControlUI::ReAddInsetToRect(RECT & rc) const {
		rc.left -=   m_rcInsetScaled.left;
		rc.top  -=    m_rcInsetScaled.top;
		rc.right  +=  m_rcInsetScaled.right;
		rc.bottom += m_rcInsetScaled.bottom;
	}

	void CControlUI::SetInset(const RECT& rcInset, LPCTSTR handyStr)
	{
		if (handyStr)
		{
			if(_tcschr(handyStr, ',')) {
				STR2Rect(handyStr, m_rcInset);
			}
			else {
				SetRectInt(m_rcInset, _ttoi(handyStr));
			}    
		}
		else
		{
			m_rcInset = rcInset;
		}
		if (_manager)
			m_rcInsetScaled = _manager->GetDPIObj()->ScaleInset(m_rcInset);
		else
			_LastScaleProfile=-1;
		if (!handyStr)
			NeedUpdate();
		else
			_view_states |= VIEWSTATEMASK_NeedEstimateSize;
	}

	bool CControlUI::SetTimer(UINT nTimerID, UINT nElapse, bool restart)
	{
		if(_manager == NULL) return false;

		return _manager->SetTimer(this, nTimerID, nElapse, restart);
	}

	void CControlUI::KillTimer(UINT nTimerID)
	{
		if(_manager == NULL) return;

		_manager->KillTimer(this, nTimerID);
	}

	QkString & CControlUI::GetText()
	{
		if (!IsResourceText()) return m_sText;
		return CResourceManager::GetInstance()->GetText(m_sText);
	}

	void CControlUI::SetText(LPCTSTR pstrText)
	{
		if( m_sText == pstrText ) return;

		m_sText = pstrText;
		// 解析xml换行符
		m_sText.Replace(_T("{\\n}"), _T("\n"));
		Invalidate();
	}

	void CControlUI::SetFont(LPCTSTR pStrFontId, int parsedId) 
	{
		//LogIs("SetFont %ld", _manager);
		if(parsedId>=0 || !pStrFontId) 
			_font = parsedId;
		else if(*pStrFontId>='0' && *pStrFontId<='9')
			_font = _ttoi(pStrFontId);
		else {
			TResInfo* resInfo;
			int parsed = 0;
			if(_manager) {
				resInfo = &_manager->m_ResInfo;
				parsed = (int)resInfo->_namedFontsMap.Find(pStrFontId);
			}
			if(!parsed) {
				resInfo = &CPaintManagerUI::m_SharedResInfo;
				parsed = (int)resInfo->_namedFontsMap.Find(pStrFontId);
				if(!parsed) {
					parsed = resInfo->_namedFonts.size()+MAX_UNSHAREDFONT_ID;
					if(resInfo->_namedFontsMap.Insert(pStrFontId, (LPVOID)parsed)) {
						resInfo->_namedFonts.push_back(nullptr);
					}
				}
			}
			_font = parsed;
		}
		_view_states |= VIEWSTATEMASK_NeedEstimateSize;
		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		Invalidate();
	}

	bool CControlUI::IsResourceText() const
	{
		return m_bResourceText;
	}

	void CControlUI::SetResourceText(bool bResource)
	{
		if( m_bResourceText == bResource ) return;
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_ResourceText, bResource);
		Invalidate();
	}

	bool CControlUI::IsDragEnabled() const
	{
		return m_bDragEnabled;
	}

	void CControlUI::SetDragEnable(bool bDrag)
	{
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_DragEnabled, bDrag);
	}

	bool CControlUI::IsDropEnabled() const
	{
		return m_bDropEnabled;
	}

	void CControlUI::SetDropEnable(bool bDrop)
	{
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_DropEnabled, bDrop);
	}


	bool CControlUI::IsRichEvent() const
	{
		return m_bRichEvent;
	}

	void CControlUI::SetHot(bool val)
	{
		if((m_uButtonState&UISTATE_HOT) != val)
		{
			m_uButtonState &= ~UISTATE_HOT;
			if(val) m_uButtonState |= UISTATE_HOT;
		}
	}

	void CControlUI::SetRichEvent(bool bEnable)
	{
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_RichEvent, bEnable);
	}

	LPCTSTR CControlUI::GetGradient()
	{
		return m_sGradient;
	}

	void CControlUI::SetGradient(LPCTSTR pStrImage)
	{
		if( m_sGradient == pStrImage ) return;

		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		m_sGradient = pStrImage;
		Invalidate();
	}

	DWORD CControlUI::GetBkColor() const
	{
		return m_dwBackColor;
	}

	void CControlUI::SetBkColor(DWORD dwBackColor)
	{
		if( m_dwBackColor == dwBackColor ) return;

		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		m_dwBackColor = dwBackColor;
		Invalidate();
	}

	DWORD CControlUI::GetBkColor2() const
	{
		return m_dwBackColor2;
	}

	void CControlUI::SetBkColor2(DWORD dwBackColor)
	{
		if( m_dwBackColor2 == dwBackColor ) return;

		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		m_dwBackColor2 = dwBackColor;
		Invalidate();
	}

	DWORD CControlUI::GetBkColor3() const
	{
		return m_dwBackColor3;
	}

	void CControlUI::SetBkColor3(DWORD dwBackColor)
	{
		if( m_dwBackColor3 == dwBackColor ) return;

		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		m_dwBackColor3 = dwBackColor;
		Invalidate();
	}

	DWORD CControlUI::GetForeColor() const
	{
		return m_dwForeColor;
	}

	void CControlUI::SetForeColor(DWORD dwForeColor)
	{
		if( m_dwForeColor == dwForeColor ) return;

		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		m_dwForeColor = dwForeColor;
		Invalidate();
	}


	void CControlUI::SetBkImage(LPCTSTR pStrImage)
	{
		//if( m_sBkImage == pStrImage ) return;
		m_tBkImage.Parse(pStrImage, _manager);
		Invalidate();
	}

	void CControlUI::SetForeImage(LPCTSTR pStrImage)
	{
		//if( m_sForeImage == pStrImage ) return;
		m_tForeImage.Parse(pStrImage, _manager);
		Invalidate();
	}

	DWORD CControlUI::GetBorderColor() const
	{
		if(m_bFocused) if(_focusedBorderColor) return _focusedBorderColor;
		if(m_uButtonState) 
		{
			if(m_uButtonState & UISTATE_PUSHED) {if(_pushedBorderColor) return _pushedBorderColor;}
			else if(m_uButtonState & UISTATE_HOT) {if(_hotBorderColor) return _hotBorderColor;}
		}
		return _borderColor;
	}

	void CControlUI::SetBorderColor(DWORD dwBorderColor)
	{
		if( _borderColor == dwBorderColor ) return;

		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		_borderColor = dwBorderColor;
		Invalidate();
	}

	DWORD CControlUI::GetFocusBorderColor() const
	{
		return _focusedBorderColor;
	}

	void CControlUI::SetFocusBorderColor(DWORD dwBorderColor)
	{
		if( _focusedBorderColor == dwBorderColor ) return;

		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		_focusedBorderColor = dwBorderColor;
		Invalidate();
	}

	bool CControlUI::IsColorHSL() const
	{
		return m_bColorHSL;
	}

	void CControlUI::SetColorHSL(bool bColorHSL)
	{
		if( m_bColorHSL == bColorHSL ) return;

		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_ColorHSL, bColorHSL);
		Invalidate();
	}

	void CControlUI::SetRoundClip(bool bClip)
	{
		if( m_bRoundClip == bClip ) return;

		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_RoundClip, bClip);
		Invalidate();
	}
	
	void CControlUI::SetBkRound(bool val)
	{
		if(m_bBkRound == val ) return;

		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_BKColorNonRound, !val);
		Invalidate();
	}

	// todo remove ???
	int CControlUI::GetBorderSize() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_rcBorderSize.left);
		return m_rcBorderSize.left;
	}

	void CControlUI::SetBorderSize(int nSize)
	{
		if( _borderSizeType!=2 && m_rcBorderSize.left == nSize ) return;

		m_rcBorderSize.left = nSize;
		m_rcBorderSize.top = nSize;
		m_rcBorderSize.right = nSize;
		m_rcBorderSize.bottom = nSize;

		if (_manager)
			_rcBorderSizeScaled = _manager->GetDPIObj()->Scale(m_rcBorderSize);
		else 
			_LastScaleProfile = -1;

		_borderSizeType = nSize?1:0;

		Invalidate();
	}

	void CControlUI::SetBorderSize(const RECT & rc, LPCTSTR handyStr)
	{
		if (handyStr)
		{
			if(_tcschr(handyStr, ',')) {
				STR2Rect(handyStr, m_rcBorderSize);
			}
			else {
				SetRectInt(m_rcBorderSize, _ttoi(handyStr));
			}  
		}
		else
		{
			m_rcBorderSize = rc;
		}

		if (m_rcBorderSize.left==m_rcBorderSize.right
			&& m_rcBorderSize.bottom==m_rcBorderSize.top && m_rcBorderSize.right==m_rcBorderSize.bottom
			)
		{
			_borderSizeType = rc.left?1:0;
		}
		else
		{
			_borderSizeType = 2;
		}
		if (_manager)
			_rcBorderSizeScaled = _manager->GetDPIObj()->Scale(m_rcBorderSize);
		else 
			_LastScaleProfile = -1;
		if (!handyStr)
			Invalidate();
	}

	SIZE CControlUI::GetBorderRound() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_cxyBorderRound);
		return m_cxyBorderRound;
	}

	void CControlUI::SetBorderRound(SIZE cxyRound, LPCTSTR handyStr)
	{
		if (handyStr)
		{
			STR2Size(handyStr, m_cxyBorderRound);
		}
		else
		{
			m_cxyBorderRound = cxyRound;
		}
		if (_manager)
			_sizeBorderRoundScaled = _manager->GetDPIObj()->Scale(cxyRound);
		else 
			_LastScaleProfile = -1;
		if (!handyStr)
			Invalidate();
	}

	void CControlUI::SetBorderArc(int val)
	{
		if(m_iBorderRoundByArc != val) {
			VIEWSTATEMASK_APPLY_INT(VIEWSTATEMASK_BorderRoundByArc, val, 27);
			Invalidate();
		}
	}

	void CControlUI::SetBorderEnhanced(bool val)
	{
		if(m_bBorderEnhanced != val) {
			VIEWSTATEMASK_APPLY(VIEWSTATEMASK_BorderEnhanced, val);
			Invalidate();
		}
	}

	bool CControlUI::GetBorderEnhanced()
	{
		return m_bBorderEnhanced;
	}

	bool CControlUI::DrawImage(HDC hDC, LPCTSTR pStrImage, RECT* rcDest)
	{
		return CRenderEngine::DrawImageString(hDC, _manager, m_rcItem, m_rcPaint, pStrImage, rcDest, _instance);
	}

	bool CControlUI::DrawImage(HDC hDC, const TDrawInfo & info, RECT* rcDest)
	{
		return CRenderEngine::DrawImageInfo(hDC, _manager, m_rcItem, m_rcPaint, &info, rcDest, _instance);
	}
	

	const RECT& CControlUI::GetPos() const
	{
		return m_rcItem;
	}

	RECT CControlUI::GetRelativePos() const
	{
		CControlUI* pParent = GetParent();
		if( pParent != NULL ) {
			RECT rcParentPos = pParent->GetPos();
			CDuiRect rcRelativePos(m_rcItem);
			rcRelativePos.Offset(-rcParentPos.left, -rcParentPos.top);
			return rcRelativePos;
		}
		else {
			return CDuiRect(0, 0, 0, 0);
		}
	}

	RECT CControlUI::GetAbsolutePos()
	{
		CDuiRect ret(m_rcItem);
		//::GetClientRect(_hWnd, &ret);
		POINT pt{ret.left, ret.top};
		::ClientToScreen(_hWnd, &pt);
		ret.right += pt.x - ret.left;
		ret.bottom += pt.y - ret.top;
		ret.left = pt.x;
		ret.top = pt.y;
		return ret;
	}

	RECT CControlUI::GetClientPos() const 
	{
		return m_rcItem;
	}

	void CControlUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		if( rc.right < rc.left ) rc.right = rc.left;
		if( rc.bottom < rc.top ) rc.bottom = rc.top;

		CDuiRect invalidateRc = m_rcItem;
		if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

		m_rcItem = rc;
		if( _manager == NULL ) return;

		if (!m_bIsViewGroup && m_items.GetSize())
		{
			ApplyInsetToRect(rc);
			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				pControl->SetPos(rc, false);
			}
		}

		if( OnSize && !m_bSettingPos ) {
			_view_states |= VIEWSTATEMASK_SettingPos;
			OnSize(this);
			_view_states &= ~VIEWSTATEMASK_SettingPos;
		}

		_view_states &= ~VIEWSTATEMASK_UpdateNeeded;

		if( bNeedInvalidate && IsVisible() ) {
			Invalidate(); // todo check history
		}
	}

	void CControlUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		m_cXY.cx += szOffset.cx;
		m_cXY.cy += szOffset.cy;
		NeedParentUpdate();
	}

	int CControlUI::GetWidth() const
	{
		return m_rcItem.right - m_rcItem.left;
	}

	int CControlUI::GetHeight() const
	{
		return m_rcItem.bottom - m_rcItem.top;
	}

	int CControlUI::GetX() const
	{
		return m_rcItem.left;
	}

	int CControlUI::GetY() const
	{
		return m_rcItem.top;
	}

	RECT CControlUI::GetPadding() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_rcPadding);
		return m_rcPadding;
	}

	void CControlUI::SetPadding(RECT rcPadding, LPCTSTR handyStr)
	{
		if (handyStr)
		{
			if(_tcschr(handyStr, ',')) {
				STR2Rect(handyStr, m_rcPadding);
			}
			else {
				SetRectInt(m_rcPadding, _ttoi(handyStr));
			}  
		}
		else
		{
			m_rcPadding = rcPadding;
		}
		if (!handyStr)
			NeedParentUpdate();
		//else ...
	}

	SIZE CControlUI::GetFixedXY() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_cXY);
		return m_cXY;
	}

	void CControlUI::SetFixedXY(SIZE szXY)
	{
		m_cXY.cx = szXY.cx;
		m_cXY.cy = szXY.cy;
		NeedParentUpdate();
	}

	SIZE CControlUI::GetFixedSize() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_cxyFixed);
		return m_cxyFixed;
	}
	int CControlUI::GetFixedWidth() const
	{
		if (_manager != NULL) {
			return m_cxyFixed.cx<=0?m_cxyFixed.cx:_manager->GetDPIObj()->Scale(m_cxyFixed.cx);
		}

		return m_cxyFixed.cx;
	}

	void CControlUI::SetFixedWidth(int cx)
	{
		//if( cx < -2 ) return; 
		m_cxyFixed.cx = cx;
		_LastScaleProfile = -1;
		NeedParentUpdate();
	}

	int CControlUI::GetFixedHeight() const
	{
		if (_manager != NULL) {
			return m_cxyFixed.cy<=0?m_cxyFixed.cy:_manager->GetDPIObj()->Scale(m_cxyFixed.cy);
		}
		
		return m_cxyFixed.cy;
	}

	void CControlUI::SetFixedHeight(int cy)
	{
		//if( cy < -2 ) return; 
		m_cxyFixed.cy = cy;
		_LastScaleProfile = -1;
		NeedParentUpdate();
	}

	int CControlUI::GetMinWidth() const
	{
		if (_manager != NULL) {
			return _manager->GetDPIObj()->Scale(m_cxyMin.cx);
		}
		return m_cxyMin.cx;
	}

	void CControlUI::SetMinWidth(int cx)
	{
		if( m_cxyMin.cx == cx ) return;

		if( cx < 0 ) return; 
		m_cxyMin.cx = cx;
		NeedParentUpdate();
	}

	int CControlUI::GetMaxWidth() const
	{
		if (_manager != NULL && m_cxyMax.cx>0) {
			return _manager->GetDPIObj()->Scale(m_cxyMax.cx);
		}
		return m_cxyMax.cx<0?LONG_MAX:m_cxyMax.cx;
	}

	int CControlUI::GetMaxHeight() const
	{
		if (_manager != NULL && m_cxyMax.cx>0) {
			return _manager->GetDPIObj()->Scale(m_cxyMax.cy);
		}

		return m_cxyMax.cy<0?LONG_MAX:m_cxyMax.cy;
	}


	int CControlUI::GetMaxAvailWidth() const
	{
		int ret = max(m_cxyFixed.cx, m_cxyMax.cx);
		if (_manager != NULL && ret>0) {
			return _manager->GetDPIObj()->Scale(ret);
		}
		return ret<0?LONG_MAX:ret;
	}
	int CControlUI::GetMaxAvailHeight() const
	{
		int ret = max(m_cxyFixed.cy, m_cxyMax.cy);
		if (_manager != NULL && ret>0) {
			return _manager->GetDPIObj()->Scale(ret);
		}
		return ret<0?LONG_MAX:ret;
	}

	int CControlUI::GetMinHeight() const
	{
		if (_manager != NULL) {
			return _manager->GetDPIObj()->Scale(m_cxyMin.cy);
		}
		
		return m_cxyMin.cy;
	}

	void CControlUI::SetMinHeight(int cy)
	{
		if( m_cxyMin.cy == cy ) return;

		if( cy < 0 ) return; 
		m_cxyMin.cy = cy;
		NeedParentUpdate();
	}

	void CControlUI::SetMaxWidth(int cx)
	{
		if( m_cxyMax.cx == cx ) return;

		//if( cx < 0 ) return; 
		m_cxyMax.cx = cx;
		NeedParentUpdate();
	}

	void CControlUI::SetMaxHeight(int cy)
	{
		if( m_cxyMax.cy == cy ) return;

		//if( cy < 0 ) return; 
		m_cxyMax.cy = cy;
		NeedParentUpdate();
	}

	TPercentInfo CControlUI::GetFloatPercent() const
	{
		return m_piFloatPercent;
	}
	
	void CControlUI::SetFloatPercent(TPercentInfo piFloatPercent)
	{
		m_piFloatPercent = piFloatPercent;
		NeedParentUpdate();
	}

	void CControlUI::SetFloatAlign(UINT uAlign)
	{
		m_uFloatAlign = uAlign;
		NeedParentUpdate();
	}

	UINT CControlUI::GetFloatAlign() const
	{
		return m_uFloatAlign;
	}

	QkString CControlUI::GetToolTip() const
	{
		if (!IsResourceText()) return m_sToolTip;
		return CResourceManager::GetInstance()->GetText(m_sToolTip);
	}

	void CControlUI::SetToolTip(LPCTSTR pstrText)
	{
		QkString strTemp(pstrText);
		strTemp.Replace(_T("<n>"),_T("\r\n"));
		m_sToolTip = strTemp;
	}

	void CControlUI::SetToolTipWidth( int nWidth )
	{
		m_nTooltipWidth = nWidth;
	}

	int CControlUI::GetToolTipWidth( void )
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_nTooltipWidth);
		return m_nTooltipWidth;
	}
	
	WORD CControlUI::GetCursor()
	{
		return m_wCursor;
	}

	void CControlUI::SetCursor(WORD wCursor)
	{
		m_wCursor = wCursor;
		if (wCursor==(WORD)IDC_ARROW)
		{
			_hCursor = CPaintManagerUI::hCursorArrow;
		}
		else if (wCursor==(WORD)IDC_HAND)
		{
			_hCursor = CPaintManagerUI::hCursorHand;
		}
		Invalidate();
	}

	TCHAR CControlUI::GetShortcut() const
	{
		return m_chShortcut;
	}

	void CControlUI::SetShortcut(TCHAR ch)
	{
		m_chShortcut = ch;
	}

	bool CControlUI::IsContextMenuUsed() const
	{
		return m_bMenuUsed;
	}

	void CControlUI::SetContextMenuUsed(bool bMenuUsed)
	{
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_MenuUsed, bMenuUsed);
	}

	const QkString& CControlUI::GetUserData()
	{
		return m_sUserData;
	}

	void CControlUI::SetUserData(LPCTSTR pstrText)
	{
		m_sUserData = pstrText;
	}

	//todo ???
	void CControlUI::SetUserDataTranslator(LPCTSTR pstrText)
	{
		if (!m_sUserDataTally)
		{
			m_sUserDataTally = new char[128]{};
		}
		WideCharToMultiByte(CP_ACP, 0, pstrText, -1, m_sUserDataTally, 127, 0, 0);
	}

	UINT_PTR CControlUI::GetTag() const
	{
		return m_pTag;
	}

	void CControlUI::SetTag(LONG_PTR pTag)
	{
		m_pTag = pTag;
	}

	void CControlUI::SetDirectUI(bool value)
	{
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_IsDirectUI, value);
	};
	
	bool CControlUI::IsDirectUI() const {
		return m_bIsDirectUI;
	};

	bool CControlUI::IsVisible() const {
		return m_bVisible;
	};

	void CControlUI::SetVisible(bool bVisible)
	{
		if( bVisible != m_bVisible ) 
		{
			VIEWSTATEMASK_APPLY(VIEWSTATEMASK_Visibility, bVisible);

			if( m_bFocused ) m_bFocused_NO;
			if (!bVisible && _manager && _manager->GetFocus() == this) {
				_manager->SetFocus(NULL) ;
			}
			NeedParentUpdate();
		}
	}

	bool CControlUI::IsEnabled() const
	{
		return _view_states&VIEWSTATEMASK_Enabled;
	}

	void CControlUI::SetEnabled(bool bEnabled)
	{
		if( bEnabled != m_bEnabled  )
		{
			if(bEnabled) _view_states |= VIEWSTATEMASK_Enabled;
			else _view_states &= ~VIEWSTATEMASK_Enabled;
			Invalidate();
		}
	}

	bool CControlUI::IsMouseEnabled() const
	{
		return m_bMouseEnabled;
	}

	void CControlUI::SetMouseEnabled(bool bEnabled)
	{
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_MouseEnabled, bEnabled);
	}

	bool CControlUI::IsKeyboardEnabled() const
	{
		return m_bKeyboardEnabled ;
	}
	void CControlUI::SetKeyboardEnabled(bool bEnabled)
	{
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_KeyboardEnabled, bEnabled);
	}

	bool CControlUI::IsFocused() const
	{
		return m_bFocused;
	}

	void CControlUI::SetFocus()
	{
		if( _manager && m_bFocusable ) 
			_manager->SetFocus(this);
		//if(!m_bFocused)
		{
			m_bFocused_YES;
			StatFocus();
		}
	}
	
	void CControlUI::StatFocus()
	{
		if(m_bRedrawOnFocusChanged) Invalidate();
	}

	bool CControlUI::HasFocus() const
	{
		if (_manager)
		{
			if(m_bFocused) return true;
			CControlUI* focus = _manager->GetFocus();
			while (focus)
			{
				if(focus==this) return true;
				focus=focus->GetParent();
			}
		}
		return false;
	}

	bool CControlUI::IsFloat() const
	{
		return m_bFloat;
	}

	void CControlUI::SetFloat(bool bFloat)
	{
		if( m_bFloat == bFloat ) return;

		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_Float, bFloat);
		NeedParentUpdate();
	}

	CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
		if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
		
		if( !m_bIsViewGroup && (uFlags==0 || (uFlags & UIFIND_HITTEST)) ) {
			int length = m_items.GetSize() - 1;
			if (length>=0)
			{
				RECT rc = m_rcItem;
				ApplyInsetToRect(rc);
				CControlUI* pResult = NULL;
				bool topFirst = uFlags & UIFIND_TOP_FIRST;
				for( int it = length; it >= 0; it-- ) {
					pResult = static_cast<CControlUI*>(m_items[topFirst?it:(length-it)])->FindControl(Proc, pData, uFlags);
					if( pResult != NULL ) {
						if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
							continue;
						else 
							return pResult;
					}          
				}
			}
		}

		if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
		return Proc(this, pData);
	}

	void CControlUI::Invalidate()
	{
		if( !IsVisible() || !_manager || _manager->_inflaing) return;

		RECT invalidateRc = m_rcItem;

		CControlUI* pParent = this;
		//RECT rcTemp;
		while( pParent = pParent->GetParent() )
		{
			//rcTemp = invalidateRc;
			if( !::IntersectRect(&invalidateRc, &invalidateRc, &pParent->GetPos()) ) 
			{
				return;
			}
		}

		_manager->Invalidate(invalidateRc);
	}

	bool CControlUI::IsUpdateNeeded() const
	{
		return m_bUpdateNeeded;
	}

	CContainerUI* CControlUI::GetRoot()
	{
		CControlUI* vp = this;
		while(vp->_parent) {
			vp = vp->_parent;
		}
		return dynamic_cast<CContainerUI*>(vp);
	}

	void CControlUI::NeedUpdate()
	{
		if( !IsVisible() ) return;
		if( m_bUpdateNeeded && _manager && _manager->_UpdateList.Find(this)>=0 ) return;
		_view_states |= VIEWSTATEMASK_UpdateNeeded;
		Invalidate();
		// requestParentNeedUpd
		if( _manager) 
		{
			_manager->NeedUpdate();
			if (!_manager->IsPainting())
			{
				_manager->_UpdateList.Add(this);
			}
		}
	}

	void CControlUI::NeedParentUpdate()
	{
		if(!_manager || _manager->_inflaing) return;
		if( GetParent() ) {
			GetParent()->NeedUpdate();
			GetParent()->Invalidate();
		}
		else {
			NeedUpdate();
		}

		if( _manager != NULL ) _manager->NeedUpdate();
	}

	bool CControlUI::ParentNeedUpdate()
	{
		return _parent && _parent->m_bUpdateNeeded;
	}

	void CControlUI::NeedParentAutoUpdate()
	{
		CControlUI* parent = NULL;
		CControlUI* vp = this;
		while( (vp = vp->GetParent()) && vp->GetAutoMeasureDimensionMatch(this) ) 
		{
			parent = vp;
		}
		if( parent ) 
		{
			if (parent->GetParent())
			{
				parent = parent->GetParent();
			}
			parent->NeedUpdate();
			parent->Invalidate();
		}
		else 
		{
			NeedUpdate();
		}
		if( _manager != NULL ) _manager->NeedUpdate();
	}

	DWORD CControlUI::GetAdjustColor(DWORD dwColor)
	{
		if( !m_bColorHSL ) return dwColor;
		short H, S, L;
		CPaintManagerUI::GetHSL(&H, &S, &L);
		return CRenderEngine::AdjustColor(dwColor, H, S, L);
	}

	void CControlUI::Init()
	{
		DoInit();
		if( OnInit ) OnInit(this);
	}

	void CControlUI::DoInit()
	{

	}

	void CControlUI::Event(TEventUI& event)
	{
		if( OnEvent(&event) ) DoEvent(event);
	}

	void CControlUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_SETCURSOR ) {
			::SetCursor(_hCursor);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused_YES;
			StatFocus();
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused_NO;
			StatFocus();
			return;
		}
		if( event.Type == UIEVENT_TIMER )
		{
			if(event.wParam==0x80) {
				PostMessage(_manager->GetPaintWindow(), WM_MOUSEMOVE, 0, MAKELONG(_manager->m_ptLastMousePos.x, _manager->m_ptLastMousePos.y));
				KillTimer(event.wParam);
				return;
			}
			_manager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() ) {
				_manager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
				return;
			}
		}

		if( _parent != NULL ) _parent->DoEvent(event);
	}


	void CControlUI::SetVirtualWnd(LPCTSTR pstrValue)
	{
		m_sVirtualWnd = pstrValue;
		_manager->UsedVirtualWnd(true);
	}

	QkString CControlUI::GetVirtualWnd() const
	{
		QkString str;
		if( !m_sVirtualWnd.IsEmpty() ){
			str = m_sVirtualWnd;
		}
		else{
			CControlUI* pParent = GetParent();
			if( pParent != NULL){
				str = pParent->GetVirtualWnd();
			}
			else{
				str = _T("");
			}
		}
		return str;
	}

	void CControlUI::AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr)
	{
		if( pstrName == NULL || pstrName[0] == _T('\0') || pstrAttr == NULL || pstrAttr[0] == _T('\0') ) return;

		if (m_mCustomAttrHash.Find(pstrName) == NULL) {
			QkString* pCostomAttr = new QkString(pstrAttr);
			if (pCostomAttr != NULL) {
				m_mCustomAttrHash.Set(pstrName, (LPVOID)pCostomAttr);
			}
		}
	}

	LPCTSTR CControlUI::GetCustomAttribute(LPCTSTR pstrName) const
	{
		if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
		QkString* pCostomAttr = static_cast<QkString*>(m_mCustomAttrHash.Find(pstrName));
		if( pCostomAttr ) return pCostomAttr->GetData();
		return NULL;
	}

	bool CControlUI::RemoveCustomAttribute(LPCTSTR pstrName)
	{
		if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
		QkString* pCostomAttr = static_cast<QkString*>(m_mCustomAttrHash.Find(pstrName));
		if( !pCostomAttr ) return false;

		delete pCostomAttr;
		return m_mCustomAttrHash.Remove(pstrName);
	}

	void CControlUI::RemoveAllCustomAttribute()
	{
		QkString* pCostomAttr;
		for( int i = 0; i< m_mCustomAttrHash.GetSize(); i++ ) {
			pCostomAttr = static_cast<QkString*>(m_mCustomAttrHash.GetValueAt(i));
			if(pCostomAttr) delete pCostomAttr;
		}
		m_mCustomAttrHash.Resize();
	}

	LRESULT CControlUI::GetAttribute(LPCTSTR pstrName, LPARAM lParam, WPARAM wParam)
	{
		return 0;
	}

	void CControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		//LONG tickSt = ::GetTickCount();
		CHAR c = _totlower(pstrName[0]);
		if (c<='c') // a b c 
		{
			if( _tcsicmp(pstrName, _T("autocalcwidth")) == 0 ) {
				SetAutoCalcWidth(_tcsicmp(pstrValue, _T("true")) == 0);
			}
			else if( _tcsicmp(pstrName, _T("autocalcheight")) == 0 ) {
				SetAutoCalcHeight(_tcsicmp(pstrValue, _T("true")) == 0);
			}
			else if( _tcsicmp(pstrName, _T("autoenhance")) == 0 ) {
				VIEWSTATEMASK_APPLY(VIEWSTATEMASK_BorderAutoEnhance, _tcsicmp(pstrValue, _T("true")) == 0);
				VIEWSTATEMASK_APPLY(VIEWSTATEMASK_RedrawOnFocusChanged, true);
			}
			else if( _tcsicmp(pstrName, _T("bkcolor")) == 0 || _tcsicmp(pstrName, _T("bkcolor1")) == 0 ) {
				STR2ARGB(pstrValue, m_dwBackColor);
				VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
			}
			else if( _tcsicmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
			else if( _tcsicmp(pstrName, _T("bkcolor2")) == 0 ) {
				STR2ARGB(pstrValue, m_dwBackColor2);
				VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
			}
			else if( _tcsicmp(pstrName, _T("bkcolor3")) == 0 ) {
				STR2ARGB(pstrValue, m_dwBackColor3);
				VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
			}
			else if( _tcsicmp(pstrName, _T("bordercolor")) == 0 ) {
				STR2ARGB(pstrValue, _borderColor);
				VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
			}
			else if( _tcsicmp(pstrName, _T("bordersize")) == 0 ) SetBorderSize(m_rcBorderSize, pstrValue);
			else if( _tcsicmp(pstrName, _T("borderinset")) == 0 ) SetBorderInset(_borderInset, pstrValue);
			else if( _tcsicmp(pstrName, _T("bottombordersize")) == 0 ) SetBottomBorderSize(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("borderstyle")) == 0 ) SetBorderStyle(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("borderround")) == 0 ) SetBorderRound(m_cxyBorderRound, pstrValue);
			else if( _tcsicmp(pstrName, _T("borderarc")) == 0 ) SetBorderArc(_ttoi(pstrValue));
			//else if( _tcsicmp(pstrName, _T("borderenhance")) == 0 ) SetBorderEnhanced(_tcsicmp(pstrValue, _T("true"));
			else if( _tcsicmp(pstrName, _T("bkround")) == 0 ) SetBkRound(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("colorhsl")) == 0 ) SetColorHSL(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("cursor")) == 0 && pstrValue) {
				if( _tcsicmp(pstrValue, _T("arrow")) == 0 )			SetCursor(DUI_ARROW);
				else if( _tcsicmp(pstrValue, _T("ibeam")) == 0 )	SetCursor(DUI_IBEAM);
				else if( _tcsicmp(pstrValue, _T("wait")) == 0 )		SetCursor(DUI_WAIT);
				else if( _tcsicmp(pstrValue, _T("cross")) == 0 )	SetCursor(DUI_CROSS);
				else if( _tcsicmp(pstrValue, _T("uparrow")) == 0 )	SetCursor(DUI_UPARROW);
				else if( _tcsicmp(pstrValue, _T("size")) == 0 )		SetCursor(DUI_SIZE);
				else if( _tcsicmp(pstrValue, _T("icon")) == 0 )		SetCursor(DUI_ICON);
				else if( _tcsicmp(pstrValue, _T("sizenwse")) == 0 )	SetCursor(DUI_SIZENWSE);
				else if( _tcsicmp(pstrValue, _T("sizenesw")) == 0 )	SetCursor(DUI_SIZENESW);
				else if( _tcsicmp(pstrValue, _T("sizewe")) == 0 )	SetCursor(DUI_SIZEWE);
				else if( _tcsicmp(pstrValue, _T("sizens")) == 0 )	SetCursor(DUI_SIZENS);
				else if( _tcsicmp(pstrValue, _T("sizeall")) == 0 )	SetCursor(DUI_SIZEALL);
				else if( _tcsicmp(pstrValue, _T("no")) == 0 )		SetCursor(DUI_NO);
				else if( _tcsicmp(pstrValue, _T("hand")) == 0 )		SetCursor(DUI_HAND);
			}
			else c = 0;
		}
		else if(c<='f') // d e f
		{
			if( _tcsicmp(pstrName, _T("font")) == 0 ) SetFont(pstrValue);
			else if( _tcsicmp(pstrName, _T("forecolor")) == 0 ) {
				STR2ARGB(pstrValue, m_dwForeColor);
				VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
			}
			else if( _tcsicmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
			else if( _tcsicmp(pstrName, _T("enabled")) == 0 ) SetEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("float")) == 0 ) {
				// 动态计算相对比例
				if(_tcschr(pstrValue, ',')) {
					TPercentInfo piFloatPercent = { 0 };
					LPTSTR pstr = NULL;
					piFloatPercent.left = _tcstod(pstrValue, &pstr);  ASSERT(pstr);
					piFloatPercent.top = _tcstod(pstr + 1, &pstr);    ASSERT(pstr);
					piFloatPercent.right = _tcstod(pstr + 1, &pstr);  ASSERT(pstr);
					piFloatPercent.bottom = _tcstod(pstr + 1, &pstr); ASSERT(pstr);
					SetFloatPercent(piFloatPercent);
					SetFloat(true);
				}
				else {
					SetFloat(_tcsicmp(pstrValue, _T("true")) == 0);
				}
			}
			else if( _tcsicmp(pstrName, _T("floatalign")) == 0) {
				UINT uAlign = GetFloatAlign();
				// 解析文字属性
				while( *pstrValue != _T('\0') ) {
					QkString sValue;
					while( *pstrValue == _T(',') || *pstrValue == _T(' ') ) pstrValue = ::CharNext(pstrValue);

					while( *pstrValue != _T('\0') && *pstrValue != _T(',') && *pstrValue != _T(' ') ) {
						LPTSTR pstrTemp = ::CharNext(pstrValue);
						while( pstrValue < pstrTemp) {
							sValue += *pstrValue++;
						}
					}
					if(sValue.CompareNoCase(_T("null")) == 0) {
						uAlign = 0;
					}
					if( sValue.CompareNoCase(_T("left")) == 0 ) {
						uAlign &= ~(DT_CENTER | DT_RIGHT);
						uAlign |= DT_LEFT;
					}
					else if( sValue.CompareNoCase(_T("center")) == 0 ) {
						uAlign &= ~(DT_LEFT | DT_RIGHT);
						uAlign |= DT_CENTER;
					}
					else if( sValue.CompareNoCase(_T("right")) == 0 ) {
						uAlign &= ~(DT_LEFT | DT_CENTER);
						uAlign |= DT_RIGHT;
					}
					else if( sValue.CompareNoCase(_T("top")) == 0 ) {
						uAlign &= ~(DT_BOTTOM | DT_VCENTER);
						uAlign |= DT_TOP;
					}
					else if( sValue.CompareNoCase(_T("vcenter")) == 0 ) {
						uAlign &= ~(DT_TOP | DT_BOTTOM);
						uAlign |= DT_VCENTER;
					}
					else if( sValue.CompareNoCase(_T("bottom")) == 0 ) {
						uAlign &= ~(DT_TOP | DT_VCENTER);
						uAlign |= DT_BOTTOM;
					}
				}
				SetFloatAlign(uAlign);
			}
			else if( _tcsicmp(pstrName, _T("drag")) == 0 ) SetDragEnable(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("drop")) == 0 ) SetDropEnable(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("float")) == 0 ) SetFloat(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("focusbordercolor")) == 0 ) {
				STR2ARGB(pstrValue, _focusedBorderColor);
				VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
				VIEWSTATEMASK_APPLY(VIEWSTATEMASK_RedrawOnFocusChanged, true);
			}
			else if( _tcsncicmp(pstrName, _T("fx_"), 3) == 0 ) 
			{
				TEffectAge& m_tCurEffects = GetEffects()[0];
				if( _tcsicmp(pstrName, _T("fx_adv")) == 0 ) SetEffectsStyle(pstrValue,&m_tCurEffects);
				else if( _tcscmp(pstrName, _T("fx_do")) == 0 ) SetAnimEffects(_tcscmp(pstrValue, _T("true")) == 0);
				else if( _tcscmp(pstrName, _T("fx_ez")) == 0 ) AnyEasyEffectsPorfiles(pstrValue,&m_tCurEffects);
				else if( _tcscmp(pstrName, _T("fx_min")) == 0 ) SetEffectsStyle(pstrValue,&GetEffects()[FX_MIN]);
				else if( _tcscmp(pstrName, _T("fx_mou")) == 0 ) SetEffectsStyle(pstrValue,&GetEffects()[FX_MOU]);
				else if( _tcscmp(pstrName, _T("fx_clk")) == 0 ) SetEffectsStyle(pstrValue,&GetEffects()[FX_CLK]);
				else if( _tcscmp(pstrName, _T("fx_s")) == 0 ) SetEffectsStyle(pstrValue,&m_tCurEffects);
				else if( _tcscmp(pstrName, _T("fx_min_s")) == 0 ) SetEffectsStyle(pstrValue,&GetEffects()[FX_MIN]);
				else if( _tcscmp(pstrName, _T("fx_mou_s")) == 0 ) SetEffectsStyle(pstrValue,&GetEffects()[FX_MOU]);
				else if( _tcscmp(pstrName, _T("fx_clk_s")) == 0 ) SetEffectsStyle(pstrValue,&GetEffects()[FX_CLK]);
			}
			else c = 0;
		}
		else if(c<='m') // g h i j k l m
		{
			if( _tcsicmp(pstrName, _T("height")) == 0 ) SetFixedHeight(ParseInt(pstrValue));
			if( _tcsicmp(pstrName, _T("inset")) == 0 ) SetInset(m_rcInset, pstrValue);
			if( _tcsicmp(pstrName, _T("id")) == 0 ) SetID(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("minwidth")) == 0 ) SetMinWidth(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("minheight")) == 0 ) SetMinHeight(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("maxwidth")) == 0 ) SetMaxWidth(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("maxheight")) == 0 ) SetMaxHeight(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("leftbordersize")) == 0 ) SetLeftBorderSize(ParseInt(pstrValue));

			else if( _tcsicmp(pstrName, _T("hotbordercolor")) == 0 ) {
				STR2ARGB(pstrValue, _hotBorderColor);
				VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
				VIEWSTATEMASK_APPLY(VIEWSTATEMASK_RedrawOnFocusChanged, true);
			}
			else if( _tcsicmp(pstrName, _T("mouse")) == 0 ) SetMouseEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("menu")) == 0 ) SetContextMenuUsed(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("keyboard")) == 0 ) SetKeyboardEnabled(_tcsicmp(pstrValue, _T("true")) == 0);

			if( _tcsicmp(pstrName, _T("innerstyle")) == 0 ) {
				ApplyAttributeList(pstrValue);
			}
			else if( _tcsicmp(pstrName, _T("gradient")) == 0 ) SetGradient(pstrValue);

			else c = 0;

		}
		else if(c<='t') // n o p q r s t
		{
			if( _tcsicmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
			else if( _tcsicmp(pstrName, _T("text")) == 0 ) SetText(pstrValue);
			else if( _tcsicmp(pstrName, _T("pos")) == 0 ) {
				RECT rcPos;
				STR2Rect(pstrValue, rcPos);  
				SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
				SetFixedXY(szXY);
				SetFixedWidth(abs(rcPos.right - rcPos.left));
				SetFixedHeight(abs(rcPos.bottom - rcPos.top));
			}
			else if( _tcsicmp(pstrName, _T("padding")) == 0 ) {
				SetPadding(m_rcPadding, pstrValue);
			}
			else if( _tcsicmp(pstrName, _T("pushedbordercolor")) == 0 ) {
				STR2ARGB(pstrValue, _pushedBorderColor);
				VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
				VIEWSTATEMASK_APPLY(VIEWSTATEMASK_RedrawOnFocusChanged, true);
			}
			else if( _tcsicmp(pstrName, _T("roundclip")) == 0 ) SetRoundClip(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("static")) == 0 ) { VIEWSTATEMASK_APPLY(VIEWSTATEMASK_IsStatic, _tcsicmp(pstrValue, _T("true")) == 0); }
			else if( _tcsicmp(pstrName, _T("topbordersize")) == 0 ) SetTopBorderSize(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("rightbordersize")) == 0 ) SetRightBorderSize(ParseInt(pstrValue));

			else if( _tcsicmp(pstrName, _T("resourcetext")) == 0 ) SetResourceText(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("richevent")) == 0 ) SetRichEvent(_tcsicmp(pstrValue, _T("true")) == 0);

			else if( _tcsicmp(pstrName, _T("td")) == 0 ) SetUserDataTranslator(pstrValue);
			else if( _tcsicmp(pstrName, _T("tooltip")) == 0 ) SetToolTip(pstrValue);
			else if( _tcsicmp(pstrName, _T("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
			else if( _tcsicmp(pstrName, _T("textpadding")) == 0 ) {
				SetInset(m_rcInset, pstrValue);
			}
			else if(_tcsicmp(pstrName, _T("style")) == 0) {
				if (_manager)
				{
					Style* pStyle = _manager->GetStyleForId(pstrValue);
					if( pStyle) ApplyAttributeList(pStyle);
				}
			}
			else c = 0;
		}
		else // u v w x y z
		{
			if( _tcsicmp(pstrName, _T("width")) == 0 ) SetFixedWidth(ParseInt(pstrValue));
			else if( _tcsicmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcsicmp(pstrValue, _T("true")) == 0);
			else if( _tcsicmp(pstrName, _T("ud")) == 0 ) SetUserData(pstrValue);
			else if( _tcsicmp(pstrName, _T("userdata")) == 0 ) SetUserData(pstrValue);
			else if( _tcsicmp(pstrName, _T("virtualwnd")) == 0 ) SetVirtualWnd(pstrValue);
			else c = 0;
		}
		//tickSetAttr += ::GetTickCount() - tickSt;
		if(c==0) {
			AddCustomAttribute(pstrName, pstrValue);
		}
	}

	CControlUI* CControlUI::ApplyAttributeList(LPCTSTR pstrValue)
	{
		// 解析样式表
		if(_manager) {
			Style* pStyle = _manager->GetStyleForId(pstrValue);
			if( pStyle) return ApplyAttributeList(pStyle);
		}
		QkString sXmlData = pstrValue;
		sXmlData.Replace(_T("&quot;"), _T("\""));
		LPCTSTR pstrList = sXmlData.GetData();
		// 解析样式属性
		QkString sItem;
		QkString sValue;
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
			if( *pstrList++ != _T('=') ) return this;
			ASSERT( *pstrList == _T('\"') );
			if( *pstrList++ != _T('\"') ) return this;
			while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
				LPTSTR pstrTemp = ::CharNext(pstrList);
				while( pstrList < pstrTemp) {
					sValue += *pstrList++;
				}
			}
			ASSERT( *pstrList == _T('\"') );
			if( *pstrList++ != _T('\"') ) return this;
			SetAttribute(sItem, sValue);
			if( *pstrList++ != _T(' ') && *pstrList++ != _T(',') ) return this;
		}
		return this;
	}

	CControlUI* CControlUI::ApplyAttributeList(Style* style)
	{
		if (style)
		{
			for (size_t i = 0, length=style->styles.size(); i < length; i++)
			{
				const StyleDefine & styleDef = style->styles[i];
				SetAttribute(styleDef.name, styleDef.value);
			}
		}
		return this;
	}

	SIZE CControlUI::EstimateSize(const SIZE & szAvailable)
	{
		if (_manager && _LastScaleProfile!=_manager->GetDPIObj()->ScaleProfile())
			OnDPIChanged();
		if(m_bFillParentWidth)  m_cxyFixScaled.cx = szAvailable.cx;
		if(m_bFillParentHeight) m_cxyFixScaled.cy = szAvailable.cy;

		if (m_bAutoCalcWidth || m_bAutoCalcHeight)
		{
			if(m_bAutoCalcWidth) m_cxyFixScaled.cx += _rcBorderSizeScaled.left + _rcBorderSizeScaled.right;
			if(m_bAutoCalcHeight) m_cxyFixScaled.cy += _rcBorderSizeScaled.top + _rcBorderSizeScaled.bottom;
			if(/*m_bIsViewGroup && */m_items.GetSize()) {
				SIZE szControlAvailable = szAvailable;
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					SIZE sz = pControl->EstimateSize(szControlAvailable);
					if(m_bAutoCalcWidth) m_cxyFixScaled.cx = MAX(m_cxyFixScaled.cx, sz.cx);
					if(m_bAutoCalcHeight) m_cxyFixScaled.cy = MAX(m_cxyFixScaled.cy, sz.cy);
				}
			}
		}
		//if (!m_bAutoCalcHeight && m_bAutoCalcHeight)
		//{
			return m_cxyFixScaled;
		//}
		//else
		//{
		//	SIZE ret = m_cxyFixScaled;
		//	//ret.cx += m_rcInsetScaled.left + m_rcInsetScaled.right;
		//	ret.cy += m_rcInsetScaled.top + m_rcInsetScaled.bottom;
		//	//if(GetName()==L"ttt") ret.cy = 500;
		//	return ret;
		//}
	}

	void CControlUI::OnDPIChanged()
	{
		if (_manager) 
		{
			m_rcInsetScaled = _manager->GetDPIObj()->ScaleInset(m_rcInset);
			m_cxyFixScaled = _manager->GetDPIObj()->Scale(m_cxyFixed);
			_borderInsetScaled = _manager->GetDPIObj()->Scale(_borderInset);
			_sizeBorderRoundScaled = _manager->GetDPIObj()->Scale(m_cxyBorderRound);
			_rcBorderSizeScaled = _manager->GetDPIObj()->Scale(m_rcBorderSize);
			_LastScaleProfile=_manager->GetDPIObj()->ScaleProfile();
		}
		else 
		{
			m_rcInsetScaled = m_rcInset;
			m_cxyFixScaled = m_cxyFixed;
			_borderInsetScaled = _borderInset;
			_sizeBorderRoundScaled = m_cxyBorderRound;
			_rcBorderSizeScaled = m_rcBorderSize;
			_LastScaleProfile=100;
		}
		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
	}

	bool CControlUI::Paint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		if (pStopControl == this) return false;
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return true;
		if (!DoPaint(hDC, m_rcPaint, pStopControl)) return false;
		return true;
	}

	bool CControlUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		if (!m_bIsDirectUI)
		{
			return true;
		}

		if (_LastScaleProfile!=_manager->GetDPIObj()->ScaleProfile())
			OnDPIChanged();


#ifdef MODULE_SKIA_RENDERER
		_manager->GetSkiaCanvas()->save();

		SkRect rect = {rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom};
		_manager->GetSkiaCanvas()->clipRect(rect);
		SkRRect r_rect = SkRRect::MakeRectXY({(float)m_rcItem.left, (float)m_rcItem.top, (float)m_rcItem.right, (float)m_rcItem.bottom}, 10, 10);
		_manager->GetSkiaCanvas()->clipRRect(r_rect, true);
#endif


		// 依序绘制：背景颜色->背景图->状态图->文本->边框
		if( (_borderSizeType==2 && (GetBorderColor() & 0xFF000000) || m_bRoundClip) && (_sizeBorderRoundScaled.cx || _sizeBorderRoundScaled.cy) ) 
		{ // 为不均匀大小的边框裁切出圆角效果
			PaintBkColor(hDC);
			if (m_bRoundClip)
			{
				CRenderClip roundClip;
				RECT rc = m_rcItem;
				InflateRect(&rc, -1, -1);
				CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  rc, _sizeBorderRoundScaled.cx*2 + 3, _sizeBorderRoundScaled.cy*2  + 3, roundClip);

				PaintBkImage(hDC);
				PaintStatusImage(hDC);
				PaintForeColor(hDC);
				PaintForeImage(hDC);
			}
			else
			{
				PaintBkImage(hDC);
				PaintStatusImage(hDC);
				PaintForeColor(hDC);
				PaintForeImage(hDC);
			}
			PaintText(hDC);
			CRenderClip roundClip;
			CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, _sizeBorderRoundScaled.cx*2 - 3, _sizeBorderRoundScaled.cy*2  - 3, roundClip);
			PaintBorder(hDC);
		}
		else 
		{
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintStatusImage(hDC);
			PaintForeColor(hDC);
			PaintForeImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
		}


#ifdef MODULE_SKIA_RENDERER
		_manager->GetSkiaCanvas()->restore();
#endif

		if(!m_bIsViewGroup && m_items.GetSize()) PaintChildren(hDC, rcPaint, pStopControl);

		return true;
	}

	bool CControlUI::PaintChildren(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		RECT rc = m_rcItem;
		ApplyInsetToRect(rc);
		RECT rcTemp;
		if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( pControl == pStopControl ) return false;
				if( !pControl->IsVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
				if( pControl ->IsFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
					if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
				}
			}
		}
		else {
			CRenderClip childClip;
			CRenderClip::GenerateClip(hDC, rcTemp, childClip);
			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( pControl == pStopControl ) return false;
				if( !pControl->IsVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
				if( pControl->IsFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
					CRenderClip::UseOldClipBegin(hDC, childClip);
					if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
					CRenderClip::UseOldClipEnd(hDC, childClip);
				}
				else {
					if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
					if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
				}
			}
		}
	}

	void CControlUI::PaintBkColor(HDC hDC)
	{
		DWORD backcolor = m_dwBackColor;
		GetBkFillColor(backcolor);
		if( backcolor & 0xFF000000 ) 
		{
			backcolor = GetAdjustColor(backcolor);
			if (_sizeBorderRoundScaled.cx && m_bBkRound)
			{ // 圆角
				RECT & rc = m_rcItem;
				CRenderEngine::DrawRoundRectangle(hDC
					, rc.left
					, rc.top 
					, rc.right - rc.left - 1
					, rc.bottom - rc.top - 1
					// /(_sizeBorderRoundScaled.cx*1.0/_rcBorderSizeScaled.left)
					, _sizeBorderRoundScaled.cx?_sizeBorderRoundScaled.cx + 1:0
					, 0
					, Gdiplus::Color(backcolor)
					, true
					, Gdiplus::Color(backcolor));
				// todo 圆角渐变色
			}
			else if( m_dwBackColor2 & 0xFF000000 ) 
			{ // 渐变色
				bool bVer = (m_sGradient.CompareNoCase(_T("hor")) != 0);
				if( m_dwBackColor3 & 0xFF000000 ) 
				{
					RECT rc = m_rcItem;
					rc.bottom = (rc.bottom + rc.top) / 2;
					CRenderEngine::DrawGradient(hDC, rc, backcolor, GetAdjustColor(m_dwBackColor2), bVer, 8);
					rc.top = rc.bottom;
					rc.bottom = m_rcItem.bottom;
					CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3), bVer, 8);
				}
				else 
				{
					CRenderEngine::DrawGradient(hDC, m_rcItem, backcolor, GetAdjustColor(m_dwBackColor2), bVer, 16);
				}
			}
			else if( backcolor & 0xFF000000 ) 
			{
				CRenderEngine::DrawColor(hDC, m_rcPaint, backcolor);

				//_manager->GetSkiaFillPaint().setColor(SkColor(backcolor));
				//SkRect rect{m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom};
				//_manager->GetSkiaCanvas()->drawRect(rect, _manager->GetSkiaFillPaint());
			}
			else 
			{
				//CRenderEngine::DrawColor(hDC, m_rcItem, backcolor);
			}
		}
	}

	void CControlUI::PaintBkImage(HDC hDC)
	{
		if( m_tBkImage.sName.IsEmpty() ) return;
		if( !DrawImage(hDC, m_tBkImage) ) {}
	}

	void CControlUI::PaintStatusImage(HDC hDC)
	{
		return;
	}

	void CControlUI::PaintForeColor(HDC hDC)
	{
		CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwForeColor));
	}
	
	void CControlUI::PaintForeImage(HDC hDC)
	{
		if( m_tForeImage.sName.IsEmpty() ) return;
		DrawImage(hDC, m_tForeImage);
	}

	void CControlUI::PaintText(HDC hDC)
	{
		return;
	}

	void CControlUI::PaintBorder(HDC hDC)
	{
		//if (!_borderSizeType) return;
		DWORD bordercolor = GetBorderColor();
		if(bordercolor & 0xFF000000) 
		{
			bordercolor = GetAdjustColor(bordercolor);
			SIZE & cxyBorderRound = _sizeBorderRoundScaled;
			RECT & rcBorderSize = _rcBorderSizeScaled;
			int bordersizen = rcBorderSize.left;
			if( _borderSizeType==1 && (cxyBorderRound.cx || cxyBorderRound.cy) ) 
			{ //圆角边框，
				// 均匀大小
				//if (bordersizen<cxyBorderRound.cx/3)
				RECT rc = m_rcItem;
				::InflateRect(&rc, -bordersizen/2, -bordersizen/2);
				float width = rc.right - rc.left - 1 - _borderInset.right -_borderInset.left;
				float height = rc.bottom - rc.top - 1 - _borderInset.bottom - _borderInset.top;
				int drawAsPath = m_iBorderRoundByArc;
				if(drawAsPath==0)
					drawAsPath = bordersizen<8 && cxyBorderRound.cx<=(MIN(width,height)-bordersizen)/2+1;
				if (drawAsPath==1)
				{ // 勾勒 draw as path
					CRenderEngine::DrawRoundRectangle(hDC
						, rc.left + _borderInset.left
						, rc.top + _borderInset.top
						, width
						, height
						, cxyBorderRound.cx
						, bordersizen
						, Gdiplus::Color(bordercolor)
						, false
						, Gdiplus::Color(bordercolor));
				}
				else
				{ // 空心圆角矩形 draw as round rect hollow
					RECT & rc = m_rcItem;
					CRenderEngine::DrawRoundRectangleHollow(hDC
						, rc.left + _borderInset.left
						, rc.top + _borderInset.top
						, rc.right - rc.left - 1 - _borderInset.right -_borderInset.left
						, rc.bottom - rc.top - 1 - _borderInset.bottom - _borderInset.top
						, cxyBorderRound.cx
						, bordersizen
						, Gdiplus::Color(bordercolor)
						, true
						, Gdiplus::Color(bordercolor));
				}
				// 不均匀大小按照直角边框画，预先裁切出圆角效果。
			}
			else 
			{ // 直角边框
				if(_borderSizeType==2) 
				{ // 不等大小
					if ((bordercolor&0xFF000000)==0xFF000000)
					{
						CRenderEngine::FillRectHeteroSized(hDC, m_rcItem, rcBorderSize, _borderInsetScaled, bordercolor);
					}
					else
					{
						CRenderEngine::FillRectHeteroSizedPlus(hDC, m_rcItem, rcBorderSize, _borderInsetScaled, bordercolor);
					}
				}
				else if(bordersizen > 0) 
				{ // 均匀大小
					RECT rc = m_rcItem;
					ApplyBorderInsetToRect(rc);
					CRenderEngine::DrawRect(hDC, rc, bordersizen, bordercolor, 5);
				}
			}
			if(m_bBorderEnhanced || m_bAutoEnhanceFocus && m_bFocused) {
				int pad=2;
				RECT & rc = m_rcItem;
				CRenderEngine::DrawRoundRectangleHollow(hDC
					, rc.left + _borderInset.left - pad
					, rc.top + _borderInset.top - pad
					, rc.right - rc.left - 1 - _borderInset.right -_borderInset.left+ pad*2
					, rc.bottom - rc.top - 1 - _borderInset.bottom - _borderInset.top+ pad*2
					, cxyBorderRound.cx
					, 5
					, Gdiplus::Color(bordercolor&0x19ffffff)
					, true
					, Gdiplus::Color(bordercolor&0x19ffffff));
			}
		}
	}

	void CControlUI::ApplyBorderInsetToRect(RECT & rc) const {
		rc.left += _borderInsetScaled.left;
		rc.right -= _borderInsetScaled.right;
		rc.top += _borderInsetScaled.top;
		rc.bottom -= _borderInsetScaled.bottom;
	}

	void CControlUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
	{
		return;
	}

	void CControlUI::SetNeedAutoCalcSize() 
	{
		_view_states |= VIEWSTATEMASK_NeedEstimateSize;
	}

	int CControlUI::GetLeftBorderSize() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_rcBorderSize.left);
		return m_rcBorderSize.left;
	}

	void CControlUI::SetLeftBorderSize( int nSize )
	{
		m_rcBorderSize.left = nSize;
		Invalidate();
	}

	int CControlUI::GetTopBorderSize() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_rcBorderSize.top);
		return m_rcBorderSize.top;
	}

	void CControlUI::SetTopBorderSize( int nSize )
	{
		m_rcBorderSize.top = nSize;
		Invalidate();
	}

	int CControlUI::GetRightBorderSize() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_rcBorderSize.right);
		return m_rcBorderSize.right;
	}

	void CControlUI::SetRightBorderSize( int nSize )
	{
		m_rcBorderSize.right = nSize;
		Invalidate();
	}

	int CControlUI::GetBottomBorderSize() const
	{
		if(_manager != NULL) return _manager->GetDPIObj()->Scale(m_rcBorderSize.bottom);
		return m_rcBorderSize.bottom;
	}

	void CControlUI::SetBottomBorderSize( int nSize )
	{
		m_rcBorderSize.bottom = nSize;
		Invalidate();
	}

	int CControlUI::GetBorderStyle() const
	{
		return m_nBorderStyle;
	}

	void CControlUI::SetBorderStyle( int nStyle )
	{
		m_nBorderStyle = nStyle;
		Invalidate();
	}

	void CControlUI::SetBorderInset(const RECT& rcInset, LPCTSTR handyStr)
	{
		if (handyStr)
		{
			if(_tcschr(handyStr, ',')) {
				STR2Rect(handyStr, _borderInset);
			}
			else {
				SetRectInt(_borderInset, _ttoi(handyStr));
			}  
		}
		else
		{
			_borderInset = rcInset;
		}
		if (_manager)
			_borderInsetScaled = _manager->GetDPIObj()->ScaleInset(_borderInset);
		else
			_LastScaleProfile=-1;
		if (!handyStr)
			NeedUpdate();
	}

	////////////////////////////////
	// Container Interface
	int CControlUI::GetItemIndex(CControlUI* pControl) const
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				return it;
			}
		}

		return -1;
	}

	bool CControlUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				NeedUpdate();            
				m_items.Remove(it);
				return m_items.InsertAt(iIndex, pControl);
			}
		}

		return false;
	}

	bool CControlUI::Add(CControlUI* pControl)
	{
		if( pControl == NULL) return false;

		if( _manager != NULL ) _manager->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		//else pControl->SetInternVisible(false);
		bool ret = m_items.Add(pControl);
		if (_manager && ret && _manager->_bIsLayoutOnly)
		{
			SetPos(m_rcItem);
		}
		if(!m_bIsViewGroup) {
			pControl->_view_states &= ~VIEWSTATEMASK_Focusable;
			pControl->_view_states &= ~VIEWSTATEMASK_MouseEnabled;
		}
		return ret;   
	}

	bool CControlUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if( pControl == NULL) return false;

		if( _manager != NULL ) _manager->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		//else pControl->SetInternVisible(false);
		return m_items.InsertAt(iIndex, pControl);
	}

	bool CControlUI::Remove(CControlUI* pControl)
	{
		if( pControl == NULL) return false;

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				NeedUpdate();
				if( m_bAutoDestroy ) {
					//if( m_bDelayedDestroy && _manager ) _manager->AddDelayedCleanup(pControl);  
					//else if(pControl->m_bCustomWidget){
					//	// Intentionally leave blank
					//	if(pControl!=this) pControl->Free();
					//}           
					//else delete pControl;
				}
				return m_items.Remove(it);
			}
		}
		return false;
	}

	bool CControlUI::RemoveAt(int iIndex)
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (pControl != NULL) {
			return Remove(pControl);
		}

		return false;
	}

	void CControlUI::RemoveAll()
	{
		for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
			CControlUI* pItem = static_cast<CControlUI*>(m_items[it]);
			if( m_bDelayedDestroy && _manager && !_manager->_bIsLayoutOnly ) {
				_manager->AddDelayedCleanup(pItem);             
			}
			else if(pItem->m_bCustomWidget){
				// Intentionally leave blank
				if(pItem!=this) pItem->Free();
			}
			else {
				delete pItem;
				pItem = NULL;
			}
		}
		m_items.Empty();
		NeedUpdate();
	}

	bool CControlUI::IsAutoDestroy() const {
		return m_bAutoDestroy;
	}

	void CControlUI::SetAutoDestroy(bool bAuto) {
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_AutoDestroy, bAuto);
	}

	bool CControlUI::IsDelayedDestroy() const {
		return m_bDelayedDestroy;
	}

	void CControlUI::SetDelayedDestroy(bool bDelayed) {
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_DelayedDestroy, bDelayed);
	}

	CControlUI* CControlUI::Duplicate()
	{
		CControlUI* btn = new CControlUI();
		*btn = *this;
		btn->Init();
		return btn;
	}

} // namespace DuiLib
