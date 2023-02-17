#include "StdAfx.h"
#include "UIButton.h"
#include "./WindowsEx/button.h"

namespace DuiLib
{
	using namespace Button;
	IMPLEMENT_QKCONTROL(Button)

	Button::Button()
		: m_iHotFont(-1)
		, m_iPushedFont(-1)
		, m_iFocusedFont(-1)
		, m_dwHotTextColor(0)
		, m_dwPushedTextColor(0)
		, m_dwFocusedTextColor(0)
		, m_dwHotBkColor(0)
		, m_dwPushedBkColor(0)
		, m_dwDisabledBkColor(0)
		, m_iBindTabIndex(-1)
		, btnForeDrawable(0)
		, _hotTrack(true)
	{
		m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
		BUTTON_Register();
		infoPtr = 0;
		Init();
	}

	Button::~Button()
	{
		if(infoPtr) {
			_Destory(infoPtr);
			infoPtr = 0;
		}
	}

	LPCTSTR Button::GetClass() const
	{
		return _T("ButtonUI");
	}

	LPVOID Button::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_BUTTON) == 0 ) return static_cast<Button*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT Button::GetControlFlags() const
	{
		return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	void Button::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( _parent != NULL ) _parent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}
		switch(event.Type)
		{
			case UIEVENT_SETFOCUS:
				m_bFocused_YES;
				infoPtr->state |= BST_FOCUS;
				Invalidate();
				return;
			case UIEVENT_KILLFOCUS:
				m_bFocused_NO;
				infoPtr->state &= ~ BST_FOCUS;
				Invalidate();
				return;
			case UIEVENT_KEYDOWN:
				if (IsKeyboardEnabled()) {
					if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
						Activate();
						return;
					}
				}
				break;
			case UIEVENT_BUTTONDOWN:
			case UIEVENT_DBLCLICK:
				if( m_bEnabled && ::PtInRect(&m_rcItem, event.ptMouse) ) {
					m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
					infoPtr->state |= BST_PUSHED;
					Invalidate();
					if(m_bRichEvent) _manager->SendNotify(this, DUI_MSGTYPE_BUTTONDOWN);
				}
				return;
			case UIEVENT_MOUSEMOVE:
				if( m_uButtonState & UISTATE_CAPTURED) 
				{
					if( ::PtInRect(&m_rcItem, event.ptMouse) ) 
					{
						m_uButtonState |= UISTATE_PUSHED;
						infoPtr->state |= BST_PUSHED;
					}
					else
					{
						m_uButtonState &= ~UISTATE_PUSHED;
						infoPtr->state &= ~BST_PUSHED;
					}
					Invalidate();
				}
				return;
			case UIEVENT_BUTTONUP:
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
				{
					m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
					infoPtr->state &= ~BST_PUSHED;
					Invalidate();
					if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();				
				}
				return;
			case UIEVENT_CONTEXTMENU:
				if( IsContextMenuUsed() ) {
					_manager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
				}
				return;
			case UIEVENT_MOUSEENTER:
			case UIEVENT_MOUSELEAVE:
				if( _hotTrack && IsEnabled()  ) {
					if (event.Type==UIEVENT_MOUSEENTER)
					{
						m_uButtonState |= UISTATE_HOT;
						infoPtr->state |= BST_HOT;
						Invalidate();

						if(m_bRichEvent) _manager->SendNotify(this, DUI_MSGTYPE_MOUSEENTER);
					}
					else
					{
						m_uButtonState &= ~UISTATE_HOT;
						infoPtr->state &= ~BST_HOT;
						Invalidate();

						if(m_bRichEvent) _manager->SendNotify(this, DUI_MSGTYPE_MOUSELEAVE);
					}
				}
				return;
			case UIEVENT_SETCURSOR:
				//if( (infoPtr->dwStyle&BS_TYPEMASK)==BS_OWNERDRAW )
				//	::SetCursor(CPaintManagerUI::hCursorHand);
				//else 
					::SetCursor(CPaintManagerUI::hCursorArrow);
				return;
		}  
		CLabelUI::DoEvent(event);
	}

	int Button::Toggle()
	{
		int type = infoPtr->dwStyle&BS_TYPEMASK;
		int state = infoPtr->state & 0x3;
		bool _3state = type==BS_3STATE||type==BS_AUTO3STATE;
		state = (state+1) % (_3state?3:2);
		infoPtr->state = infoPtr->state&~0x3|state;
		Invalidate();
		return state;
	}

	int Button::GetCheckedValue()
	{
		int type = infoPtr->dwStyle&BS_TYPEMASK;
		int state = infoPtr->state & 0x3;
		if (state>=2&&!(type==BS_3STATE||type==BS_AUTO3STATE))
		{
			state = 1;
		}
		return state;
	}

	bool Button::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		if( _manager != NULL )
		{
			_manager->SendNotify(this, DUI_MSGTYPE_CLICK);
			if (GetEffectEnabled(FX_CLK))
			{
				TriggerEffects(&GetEffects()[FX_CLK]);
			}
			BindTriggerTabSel();
		}
		if(_id!=0) 
		{
			CPaintManagerUI* manager = _manager->GetRealManager();
			::SendMessage(manager->GetPaintWindow(), WM_COMMAND, _id, (LPARAM)this);
		}
		return true;
	}

	void Button::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		infoPtr->enabled = bEnable;
		if( !IsEnabled() ) {
			m_uButtonState = UISTATE_DISABLED;
		}
		else {
			m_uButtonState = 0;
		}
	}

	
	void Button::SetHotFont(int index)
	{
		m_iHotFont = index;
		Invalidate();
	}

	int Button::GetHotFont() const
	{
		return m_iHotFont;
	}

	void Button::SetPushedFont(int index)
	{
		m_iPushedFont = index;
		Invalidate();
	}

	int Button::GetPushedFont() const
	{
		return m_iPushedFont;
	}

	void Button::SetFocusedFont(int index)
	{
		m_iFocusedFont = index;
		Invalidate();
	}

	int Button::GetFocusedFont() const
	{
		return m_iFocusedFont;
	}

	void Button::SetHotBkColor( DWORD dwColor )
	{
		m_dwHotBkColor = dwColor;
		Invalidate();
	}

	DWORD Button::GetHotBkColor() const
	{
		return m_dwHotBkColor;
	}
	
	void Button::SetPushedBkColor( DWORD dwColor )
	{
		m_dwPushedBkColor = dwColor;
		Invalidate();
	}

	DWORD Button::GetPushedBkColor() const
	{
		return m_dwPushedBkColor;
	}
		
	void Button::SetDisabledBkColor( DWORD dwColor )
	{
		m_dwDisabledBkColor = dwColor;
		Invalidate();
	}

	DWORD Button::GetDisabledBkColor() const
	{
		return m_dwDisabledBkColor;
	}
	
	void Button::SetHotTextColor(DWORD dwColor)
	{
		m_dwHotTextColor = dwColor;
	}

	DWORD Button::GetHotTextColor() const
	{
		return m_dwHotTextColor;
	}

	void Button::SetPushedTextColor(DWORD dwColor)
	{
		m_dwPushedTextColor = dwColor;
	}

	DWORD Button::GetPushedTextColor() const
	{
		return m_dwPushedTextColor;
	}

	void Button::SetFocusedTextColor(DWORD dwColor)
	{
		m_dwFocusedTextColor = dwColor;
	}

	DWORD Button::GetFocusedTextColor() const
	{
		return m_dwFocusedTextColor;
	}


	void Button::AddStatusImage(LPCTSTR pStrImage, int state, bool reset)
	{
		if(pStrImage) {
			if(reset && _statusDrawable) delete _statusDrawable;
			if(!_statusDrawable) 
			{
				_statusDrawable = new BasicStatusDrawable();
			}
			_statusDrawable->AddStatusImage(pStrImage, state, true, reset);
		}
	}

	TDrawInfo & Button::GetStateImage()
	{
		return _stateIcon;
	}

	void Button::SetStateImage( LPCTSTR pStrImage )
	{
		//imgAttrs.at(imgNormal).sName.Empty();
		//m_sStateImage = pStrImage;
		_stateIcon.Parse(pStrImage, _manager);
		if(_statusDrawable) {
			delete _statusDrawable;
			_statusDrawable = 0;
		}
		Invalidate();
	}

	void Button::BindTabIndex(int _BindTabIndex )
	{
		if( _BindTabIndex >= 0)
			m_iBindTabIndex	= _BindTabIndex;
	}

	void Button::BindTabLayoutName( LPCTSTR _TabLayoutName )
	{
		if(_TabLayoutName)
			m_sBindTabLayoutName = _TabLayoutName;
	}

	void Button::BindTriggerTabSel( int _SetSelectIndex /*= -1*/ )
	{
		LPCTSTR pstrName = GetBindTabLayoutName();
		if(pstrName == NULL || (GetBindTabLayoutIndex() < 0 && _SetSelectIndex < 0))
			return;

		CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(GetManager()->FindControl(pstrName));
		if(!pTabLayout) return;
		pTabLayout->SelectItem(_SetSelectIndex >=0?_SetSelectIndex:GetBindTabLayoutIndex());
	}

	void Button::RemoveBindTabIndex()
	{
		m_iBindTabIndex	= -1;
		m_sBindTabLayoutName.Empty();
	}

	int Button::GetBindTabLayoutIndex()
	{
		return m_iBindTabIndex;
	}

	LPCTSTR Button::GetBindTabLayoutName()
	{
		return m_sBindTabLayoutName;
	}

	void Button::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("normalimage")) == 0 ) AddStatusImage(pstrValue, 0);
		else if( _tcsicmp(pstrName, _T("hotimage")) == 0 ) AddStatusImage(pstrValue, UISTATE_HOT);
		else if( _tcsicmp(pstrName, _T("pushedimage")) == 0 ) AddStatusImage(pstrValue, UISTATE_PUSHED);
		else if( _tcsicmp(pstrName, _T("focusedimage")) == 0 ) AddStatusImage(pstrValue, UISTATE_FOCUSED);
		else if( _tcsicmp(pstrName, _T("disabledimage")) == 0 ) AddStatusImage(pstrValue, UISTATE_DISABLED);
		//else if( _tcsicmp(pstrName, _T("hotforeimage")) == 0 ) SetHotForeImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("stateimage")) == 0 ) SetStateImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("bindtabindex")) == 0 ) BindTabIndex(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("bindtablayoutname")) == 0 ) BindTabLayoutName(pstrValue);
		else if( _tcsicmp(pstrName, _T("type")) == 0 ) SetType(pstrValue);
		else if( _tcsicmp(pstrName, _T("note")) == 0 ) SetNote(pstrValue);
		else if( _tcsicmp(pstrName, _T("hotbkcolor")) == 0 )
		{
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetHotBkColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("pushedbkcolor")) == 0 )
		{
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetPushedBkColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("disabledbkcolor")) == 0 )
		{
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetDisabledBkColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("hottextcolor")) == 0 )
		{
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetHotTextColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("pushedtextcolor")) == 0 )
		{
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetPushedTextColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("focusedtextcolor")) == 0 )
		{
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetFocusedTextColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("hotfont")) == 0 ) SetHotFont(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("hottrack")) == 0 ) SetHotTack(_tcsicmp(pstrValue, _T("true")));
		else if( _tcsicmp(pstrName, _T("pushedfont")) == 0 ) SetPushedFont(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("focuedfont")) == 0 ) SetFocusedFont(_ttoi(pstrValue));
		
		else CLabelUI::SetAttribute(pstrName, pstrValue);
		infoPtr->dtStyle = m_uTextStyle;
	}

	SIZE Button::EstimateSize(const SIZE & szAvailable)
	{
		bool bNeedEstimate = m_bNeedEstimateSize || (m_cxyFixed.cx <= 0 || m_cxyFixed.cy <= 0) && (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy);
		if (bNeedEstimate) 
		{
			BUTTON_QueryPreempterSize(infoPtr, (WPARAM)&_preSizeX);
			//_preSizeX = 30;
		}
		SIZE & ret = __super::EstimateSize(szAvailable);
		if (bNeedEstimate) 
		{
			RECT & m_rcTextPadding = m_rcInsetScaled;
			infoPtr->textAutoWidth = m_bAutoCalcWidth?ret.cx-(m_rcTextPadding.left + m_rcTextPadding.right):0;
			infoPtr->textAutoHeight = m_bAutoCalcHeight?ret.cy-(m_rcTextPadding.top + m_rcTextPadding.bottom):0;
		}
		return ret;
	}

	int Button::GetType()
	{
		return infoPtr->dwStyle&BS_TYPEMASK;
	}

	void Button::SetType(LPCTSTR pstrText, int type)
	{
		if (pstrText)
		{
			int length = lstrlen(pstrText);
			if (length>=1)
			{
				TCHAR tc = pstrText[0];
				bool auto_ = false;
				if (tc==L'_' && length>1)
				{
					auto_ = true;
					tc = pstrText[1];
				}
				if(tc>=L'a'&&tc<=L'z') tc = _toupper(tc);
				switch(tc)
				{
					case L'P':
						type = auto_?BS_DEFPUSHBUTTON:BS_PUSHBUTTON;
					break;
					case L'C':
						type = auto_?BS_AUTOCHECKBOX:BS_CHECKBOX;
					break;
					case L'R':
						type = auto_?BS_AUTORADIOBUTTON:BS_RADIOBUTTON;
					break;
					case L'3':
						type = auto_?BS_AUTO3STATE:BS_3STATE;
					break;
					case L'G':
						type = BS_GROUPBOX;
					break;
					case L'U':
						type = BS_USERBUTTON;
					break;
					case L'S':
						type = auto_?BS_DEFSPLITBUTTON:BS_SPLITBUTTON;
					break;
					case L'L':
						type = BS_COMMANDLINK;
					break;
					default:
						type = BS_OWNERDRAW;
					break;
				}
			}
		}
		infoPtr->dwStyle &= ~BS_TYPEMASK;
		infoPtr->dwStyle |= type & BS_TYPEMASK;
	}

	void Button::Init()
	{
		if (!infoPtr)
		{
			infoPtr = new BUTTON_INFO{0};
			infoPtr->enabled = true;
			infoPtr->dwStyle = WS_CHILD | WS_VISIBLE | BS_OWNERDRAW;
			infoPtr->rcDraw = &m_rcItem;
			infoPtr->rcPadding = &m_rcInsetScaled;
		}

		if (!infoPtr->hwnd)
		{
			if (_parent)
			{
				_Create(_parent->GetHWND(), (WPARAM)infoPtr, 0);
			}
			if (false)
			{
				infoPtr->dwStyle |= WS_CHILD | WS_VISIBLE 
					//| infoPtr->dwStyle
					| BS_MULTILINE 
					| BS_CENTER 
					| BS_VCENTER
					;
				SetType(0,  0
					//|BS_AUTO3STATE
					//|BS_GROUPBOX
					//|BS_SPLITBUTTON
					//|BS_3STATE
					|BS_COMMANDLINK
					//|BS_OWNERDRAW
				);
			}
		}
	}

	void Button::SyncColors()
	{
		infoPtr->font = _manager->GetFont(GetFont());
		infoPtr->lineHeight = _manager->GetFontInfo(_font)->tm.tmHeight;
		infoPtr->bgrTextColor = RGB(GetBValue(m_dwTextColor), GetGValue(m_dwTextColor), GetRValue(m_dwTextColor));
		infoPtr->bgrTextColorDisabled = RGB(GetBValue(m_dwDisabledBkColor), GetGValue(m_dwDisabledBkColor), GetRValue(m_dwDisabledBkColor));

		VIEWSTATE_MARK_SYNCED(VIEW_INFO_DIRTY_COLORS);
	}

	bool Button::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		if((infoPtr->dwStyle&BS_TYPEMASK)!=BS_OWNERDRAW)
		{
			if (m_bInfoDirtyColors) SyncColors();
			infoPtr->delegated_Text = (TCHAR*)GetText().GetData();
			if (infoPtr->dwStyle&BS_COMMANDLINK)
			{
				infoPtr->note = (TCHAR*)_note.GetData();
				infoPtr->note_length = _note.GetLength();
			}

			_Paint(infoPtr, (WPARAM)hDC);

			if(m_items.GetSize()) 
				PaintChildren(hDC, rcPaint, pStopControl);

			return true;
		}
		else 
		{
			return __super::DoPaint(hDC, rcPaint, pStopControl);
		}
	}

	void Button::PaintText(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( m_dwTextColor == 0 ) m_dwTextColor = _manager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = _manager->GetDefaultDisabledColor();
		
		QkString sText = GetText();
		if( sText.IsEmpty() ) return;

		int nLinks = 0;
		RECT rc = m_rcItem;
		ApplyInsetToRect(rc);

		DWORD clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;
		
		if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0) )
			clrColor = GetPushedTextColor();
		else if( ((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0) )
			clrColor = GetHotTextColor();
		else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0) )
			clrColor = GetFocusedTextColor();

		int iFont = GetFont();
		if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedFont() != -1) )
			iFont = GetPushedFont();
		else if( ((m_uButtonState & UISTATE_HOT) != 0) && (GetHotFont() != -1) )
			iFont = GetHotFont();
		else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedFont() != -1) )
			iFont = GetFocusedFont();

		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, _manager, rc, sText, clrColor, \
			NULL, NULL, nLinks, iFont, m_uTextStyle);
		else
			CRenderEngine::DrawPlainText(hDC, _manager, rc, sText, clrColor, \
			iFont, m_uTextStyle);
	}

	void Button::GetBkFillColor(DWORD & color)
	{
		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) 
		{
			if(m_dwDisabledBkColor) color = m_dwDisabledBkColor;
		}
		else if( (m_uButtonState & UISTATE_PUSHED) != 0 && m_dwPushedBkColor!=0)
		{
			if(m_dwPushedBkColor) color = m_dwPushedBkColor;
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) 
		{
			if(m_dwHotBkColor) color = m_dwHotBkColor;
		}
	}


	void Button::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;
		//if(!::IsWindowEnabled(_manager->GetPaintWindow()))
		//	m_uButtonState &= UISTATE_DISABLED;

		if(_statusDrawable && _statusDrawable->Draw(hDC, this, m_uButtonState)) 
		{
			return;
		}
		if(!_stateIcon.sName.IsEmpty())
		{
			// 根据按下状态在不同位置绘制 StateImage
			const TImageInfo* pImage = _manager->GetImageEx(_stateIcon.sName, _stateIcon.sResType, _stateIcon.dwMask, _stateIcon.bHSL);
			if(pImage != NULL)
			{
				RECT rcItem = m_rcItem; // todo FIXME draw oversize
				bool draw = true;
				BYTE uFade = _stateIcon.uFade;
				if( (m_uButtonState & UISTATE_DISABLED)) {
					_stateIcon.uFade = uFade / 2;
				}
				else if((m_uButtonState & UISTATE_PUSHED)) {
					int delta = 1;
					rcItem.left += delta;
					rcItem.right += delta;
					rcItem.top += delta;
					rcItem.bottom += delta;
				}
				else draw = false;
				if( !draw || !DrawImage(hDC, _stateIcon, &rcItem) )
				{
					DrawImage(hDC, _stateIcon);
				}
				_stateIcon.uFade = uFade;
			}
		}
	}

	void Button::PaintForeImage(HDC hDC)
	{
		if(btnForeDrawable && btnForeDrawable->Draw(hDC, this, m_uButtonState)) {
			return;
		}
		if(!m_tForeImage.sName.IsEmpty())
		{
			// 根据按下状态在不同位置绘制 StateImage
			const TImageInfo* pImage = _manager->GetImageEx(m_tForeImage.sName, m_tForeImage.sResType, m_tForeImage.dwMask, _stateIcon.bHSL);
			if(pImage != NULL)
			{
				RECT rcItem = m_rcItem; // todo FIXME draw oversize
				bool draw = true;
				BYTE uFade = m_tForeImage.uFade;
				if( (m_uButtonState & UISTATE_DISABLED)) {
					m_tForeImage.uFade = uFade / 2;
				}
				else if((m_uButtonState & UISTATE_PUSHED)) {
					int delta = 1;
					rcItem.left += delta;
					rcItem.right += delta;
					rcItem.top += delta;
					rcItem.bottom += delta;
				}
				else draw = false;
				if( !draw || !DrawImage(hDC, m_tForeImage, &rcItem) )
				{
					DrawImage(hDC, m_tForeImage);
				}
				m_tForeImage.uFade = uFade;
			}
		}
	}

	CControlUI* Button::Duplicate()
	{
		Button* btn = new Button();
		*btn = *this;

		btn->infoPtr = 0;
		btn->Init();
		btn->SetType(0, GetType());

		return btn;
	}
}