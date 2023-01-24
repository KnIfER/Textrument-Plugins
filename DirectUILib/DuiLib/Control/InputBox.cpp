#include "StdAfx.h"
#include "WindowsEx\edit.h"
#include "core\InsituDebug.h"

namespace DuiLib
{
	IMPLEMENT_QKCONTROL(InputBox)

	static void EDITOBJ_SyncText(EDITSTATE *es)
	{
		if(es->useTextObj) 
		{
			QkString* qkStr = (QkString*)es->textObj;
			es->text = (LPWSTR)qkStr->GetData();
			es->text_length = qkStr->GetLength();
			es->buffer_size = qkStr->Capacity();
		}
	}

	static BOOL EDITOBJ_MakeFit(EDITSTATE *es, UINT size)
	{
		//LogIs("EDITOBJ_MakeFit%d < %d", ((QkString*)es->textObj)->Capacity(), size);
		if (((QkString*)es->textObj)->Capacity()<size)
		{
			if (((QkString*)es->textObj)->EnsureCapacity(size))
			{
				EDITOBJ_SyncText(es);
				return true;
			}
			return false;
		}
		return true;
	}

	static void EDITOBJ_RecalcSz(EDITSTATE *es)
	{
		es->text_length = ((QkString*)es->textObj)->RecalcSize();
	}

	InputBox::InputBox() 
		: CContainerUI()
		, _twinkleMark(false)
		, m_uMaxChar(255)
	{
		_hCursor=::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM));
		infoPtr = new EDITSTATE{};
		infoPtr->rcDraw = &_rcEdit;
		infoPtr->clip_texts = &_clipchildren;
		infoPtr->rcDrawMax = &_rcEditMax;
		infoPtr->rcPadding = &m_rcInsetScaled;
		infoPtr->textObj = (LONG_PTR)&m_sText;
		if (!fnEDITOBJ_MakeFit)
		{
			fnEDITOBJ_MakeFit = EDITOBJ_MakeFit;
			fnEDITOBJ_SyncText = EDITOBJ_SyncText;
			fnEDITOBJ_RecalcSz = EDITOBJ_RecalcSz;
		}
		infoPtr->useTextObj = true;
		//infoPtr->flags|=EF_SHOWLNRET;
	}

	LPCTSTR InputBox::GetClass() const
	{
		return _T("InputBox");
	}

	LPVOID InputBox::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_EDIT) == 0 ) return static_cast<InputBox*>(this);
		if( _tcsicmp(pstrName, L"editor") == 0 ) return static_cast<InputBox*>(this);
		return __super::GetInterface(pstrName);
	}

	UINT InputBox::GetControlFlags() const
	{
		if( !IsEnabled() ) return CControlUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	void InputBox::Init()
	{
		if (!infoPtr->hwndSelf)
		{
			infoPtr->style |= WS_CHILD | WS_VISIBLE 
				| ES_AUTOHSCROLL
				| ES_AUTOVSCROLL
				//| WS_VSCROLL  
				//| WS_HSCROLL  
				//| WS_BORDER  
				//| ES_PASSWORD
				//| infoPtr->dwStyle
				//| ES_MULTILINE 
				//| ES_CENTER 
				| ES_NOHIDESEL 
				;
			EDITOBJ_SyncText(infoPtr);

			BOOL ret = _Create(_parent->GetHWND(), (WPARAM)infoPtr, 0);

			SyncColors();

			std::function<int(void*, UINT, WPARAM, LPARAM)> listener = [&](void* handle, UINT MSG, WPARAM wParam, LPARAM lParam) 
			{
				if (MSG==0x111)
				{
					scrollbars_set_cnt++;
					if (scrollbars_set_cnt<2) ProcessScrollBar(_rcEdit, infoPtr->text_width, infoPtr->line_count*infoPtr->line_height);
					scrollbars_set_cnt--;
					if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
					{
						m_pVerticalScrollBar->SetScrollPos(infoPtr->y_offset * infoPtr->line_height);
					}
					if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
					{
						m_pHorizontalScrollBar->SetScrollPos(infoPtr->x_offset);
					}
				}
				return 0;
			};

			infoPtr->_listener = listener;

			EDITOBJ_SyncText(infoPtr);
			EDIT_ResetText(infoPtr);

			if (false)
			{
				EDIT_LockBuffer(infoPtr);
				//_SetLimitText(infoPtr, 128);
				//_SetText(infoPtr, L"深度\n搜索深\n度\n搜\n索\n深度搜\n索深度搜\n索");
				_SetText(infoPtr, L"1 深度\n2 搜索深\n3 度\n4 搜 happy mary \n5 索\n6 深度搜\n7 索深度搜\n8 索");
				//_SetText(infoPtr, L"1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索1 深度\n2 搜索深\n3 度\n4 搜\n5 索\n6 深度搜\n7 索深度搜\n8 索");
				EDIT_UnlockBuffer(infoPtr, false);
			}
		}
	}

	void InputBox::SyncColors()
	{
		DWORD flag;
		if (!infoPtr->font || infoPtr->font!=_manager->GetFont(GetFont()))
		{
			_SetFont(infoPtr, _manager->GetFont(GetFont()), false);
		}
		infoPtr->bgrTextColor = RGB(GetBValue(m_dwTextColor), GetGValue(m_dwTextColor), GetRValue(m_dwTextColor));
		VIEWSTATE_MARK_SYNCED(VIEW_INFO_DIRTY_COLORS);
		if(VCENTER) {
			_rcEdit.top -= VCENTER_OFFSET;
			VCENTER_OFFSET = 0;
			if(!(infoPtr->style & ES_MULTILINE)) {
				VCENTER_OFFSET = ((_rcEdit.bottom-_rcEdit.top) - infoPtr->line_height) / 2;
			}
			_rcEdit.top += VCENTER_OFFSET;
		}
	}

	void InputBox::PaintText(HDC hDC)
	{
		const RECT & rcPaint = m_rcPaint;
		RECT rcTemp;
		if (infoPtr->is_delegate)
		{
			CRenderClip clip;
			RECT rcClip = _clipchildren?_rcEdit:_rcEditMax;
			::IntersectRect(&rcTemp, &rcPaint, &rcClip);
			CRenderClip::GenerateClip(hDC, rcTemp, clip);

			if (m_bInfoDirtyColors) SyncColors();

			EDITOBJ_SyncText(infoPtr);
			if(!infoPtr->font)
				_SetFont(infoPtr, _manager->GetFont(GetFont()), false);

			infoPtr->bEnableState = IsEnabled();

			_Paint(infoPtr, hDC, &rcPaint);

			ShowCaretIfVisible(true);
		}
	}

	bool InputBox::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		//infoPtr->dtStyle = m_uTextStyle;
		//infoPtr->font = _manager->GetFont(GetFont());
		CControlUI::DoPaint(hDC, rcPaint, pStopControl);

		RECT rcTemp;
		bool vertScroll = m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible();
		bool horScroll = m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible();

		if(m_items.GetSize()) PaintChildren(hDC, rcPaint, pStopControl);

		if(vertScroll) {
			if( m_pVerticalScrollBar == pStopControl ) return false;
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
				if( !m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
			}
		}

		if(horScroll) {
			if( m_pHorizontalScrollBar == pStopControl ) return false;
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
				if( !m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
			}
		}

		return true;
	}

	void InputBox::DoScroll(int dx, int dy)
	{
		if (dy)
		{
			dy = ceil(m_pVerticalScrollBar->GetScrollPos()*1.f/infoPtr->line_height)  - infoPtr->y_offset;
		}

		if (dx)
		{
			INT fw = infoPtr->format_rect.right - infoPtr->format_rect.left;
			/* check if we are going to move too far */
			if(infoPtr->x_offset + dx + fw > infoPtr->text_width)
				dx = infoPtr->text_width - fw - infoPtr->x_offset;
		}
		if(dx || dy)
			EDIT_EM_LineScroll_internal(infoPtr, dx, dy);

		Invalidate();
	}

	void InputBox::SetPos(RECT rc, bool bNeedInvalidate)
	{
		__super::SetPos(rc, bNeedInvalidate);
		_rcEdit = m_rcItem;
		short b1=0;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
		{
			_rcEdit.right -= m_pVerticalScrollBar->GetFixedWidth();
			b1 |= 0x1;
		}
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
			_rcEdit.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
			b1 |= 0x2;
		}
		_rcEditMax = _rcEdit;
		if ((b1&0x1) && _vscrollEatInset)
		{
			int vScrollW = m_rcItem.right - _rcEdit.right;
			if (m_rcInsetScaled.right<=vScrollW)
				_rcEdit.right += m_rcInsetScaled.right;
			else
				_rcEdit.right += vScrollW;
		}
		if ((b1&0x2) && _hscrollEatInset)
		{
			int hScrollH = m_rcItem.bottom - _rcEdit.bottom;
			if (m_rcInsetScaled.bottom<=hScrollH)
				_rcEdit.bottom += m_rcInsetScaled.bottom;
			else
				_rcEdit.bottom += hScrollH;
		}
		ApplyInsetToRect(_rcEdit);

		VCENTER_OFFSET = 0;
		if(VCENTER) {
			if(!(infoPtr->style & ES_MULTILINE)) {
				VCENTER_OFFSET = ((_rcEdit.bottom-_rcEdit.top) - infoPtr->line_height) / 2;
			}
			_rcEdit.top += VCENTER_OFFSET;
		}

		// Process the scrollbar
		scrollbars_set_cnt++;
		if (scrollbars_set_cnt<3) ProcessScrollBar(_rcEdit, infoPtr->text_width, infoPtr->line_count*infoPtr->line_height);
		scrollbars_set_cnt--;
	}

	void InputBox::SetVCenter(bool value)
	{
		if(VCENTER != value) {
			VCENTER = value;
			// if(rendered)
			_rcEdit.top -= VCENTER_OFFSET;
			VCENTER_OFFSET = 0;
			if(value && !(infoPtr->style & ES_MULTILINE)) {
				VCENTER_OFFSET = ((_rcEdit.bottom-_rcEdit.top) - infoPtr->line_height) / 2;
			}
			_rcEdit.top += VCENTER_OFFSET;
		}
	}

	void InputBox::SetMultiline(bool value)
	{
		if(value) infoPtr->style |= ES_MULTILINE;
		else infoPtr->style &= ~ES_MULTILINE;
	}

	void InputBox::ShowCaretIfVisible(bool update)
	{
		bool visible = IsFocused();
		if (visible)
		{
			LRESULT res = EDIT_EM_PosFromChar(infoPtr, infoPtr->selection_end, infoPtr->flags & EF_AFTER_WRAP);
			short x=(short)LOWORD(res), y=(short)HIWORD(res);
			RECT tmp;
			RECT rcMark{x, y, x+1, y+infoPtr->line_height};
			CControlUI* svp = this;
			while(svp) 
			{
				if (!::IntersectRect(&tmp, &rcMark, &svp->GetPos()))
				{
					visible = false;
					break;
				}
				rcMark = tmp;
				svp=svp->GetParent();
			}
			if (visible && tmp.bottom-tmp.top<infoPtr->line_height/2)
			{
				visible = false;
			}
		}
		//LogIs("ShowCaretIfVisible::%d %d", visible, _twinkleMark);
		if (visible ^ _twinkleMark)
		{
			if (_twinkleMark = visible)
				::ShowCaret(_manager->GetPaintWindow());
			else
				::HideCaret(_manager->GetPaintWindow());
		}

		if (update && _twinkleMark)
			EDIT_SetCaretPos(infoPtr, infoPtr->selection_end, infoPtr->flags & EF_AFTER_WRAP);
	}

	void InputBox::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( _parent != NULL ) _parent->DoEvent(event);
			else __super::DoEvent(event);
			return;
		}
		
		if( event.Type == UIEVENT_CHAR)
		{
			EDIT_LockBuffer(infoPtr);
			_Char(infoPtr, event.wParam);
			EDIT_UnlockBuffer(infoPtr, false);
			return;
		}
		if( event.Type == UIEVENT_IME_REQUEST)
		{
			if (event.wParam == IMR_QUERYCHARPOSITION)
			{
				IMECHARPOSITION *chpos = (IMECHARPOSITION *)event.lParam;
				LRESULT pos;

				pos = EDIT_EM_PosFromChar(infoPtr, infoPtr->selection_start + chpos->dwCharPos, FALSE);
				chpos->pt.x = LOWORD(pos);
				chpos->pt.y = HIWORD(pos);
				chpos->cLineHeight = infoPtr->line_height;
				chpos->rcDocument = infoPtr->format_rect;
				MapWindowPoints(GetHWND(), 0, &chpos->pt, 1);
				MapWindowPoints(GetHWND(), 0, (POINT*)&chpos->rcDocument, 2);
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONDOWN)
		{
			_manager->SetCapture();
			//__super::DoEvent(event);
			EDIT_LockBuffer(infoPtr);
			_LButtonDown(infoPtr, event.wParam, (short)LOWORD(event.lParam), (short)HIWORD(event.lParam));
			EDIT_UnlockBuffer(infoPtr, false);
			ShowCaretIfVisible(false);
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP)
		{
			_manager->ReleaseCapture();
			_LButtonUp(infoPtr);
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			// https://docs.microsoft.com/en-us/windows/win32/inputdev/using-mouse-input
			if (infoPtr->bCaptureState && (event.wParam & MK_LBUTTON))
			{
				_MouseMove(infoPtr, (short)LOWORD(event.lParam), (short)HIWORD(event.lParam));
			}
			return;
		}
		//if( event.Type == UIEVENT_SCROLLWHEEL)
		//{
		//	_MouseWheel(infoPtr, event.wParam, event.lParam);
		//	return;
		//}
		if( event.Type == UIEVENT_KEYDOWN ) 
		{
			_KeyDown(infoPtr, (INT)event.wParam);
			return;
		}
		if( event.Type == UIEVENT_DBLCLICK )
		{
			_manager->SetCapture();
			_LButtonDblClk(infoPtr);
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			EDIT_WM_ContextMenu(infoPtr, (short)LOWORD(event.lParam), (short)HIWORD(event.lParam));
			return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			m_bFocused_YES;
			infoPtr->flags |= EF_FOCUSED;
			CreateCaret(_manager->GetPaintWindow(), 0, 1, infoPtr->line_height);
			EDIT_SetCaretPos(infoPtr, infoPtr->selection_end, infoPtr->flags & EF_AFTER_WRAP);
			ShowCaretIfVisible(true);
			//Invalidate();
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			TCHAR buffer[100]={0};
			wsprintf(buffer,TEXT("UIEVENT_KILLFOCUS"), TEXT("GOGO"));
			//::MessageBox(NULL, buffer, TEXT(""), MB_OK);

			m_bFocused_NO;
			infoPtr->flags &= ~EF_FOCUSED;
			//Invalidate();
			ShowCaretIfVisible(false);
			return;
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				//if( IsFocused() && m_pWindow == NULL )
				//{
				//	m_pWindow = new CEditWnd();
				//	ASSERT(m_pWindow);
				//	m_pWindow->Init(this);
				//
				//	if( PtInRect(&m_rcItem, event.ptMouse) )
				//	{
				//		int nSize = GetWindowTextLength(*m_pWindow);
				//		if( nSize == 0 ) nSize = 1;
				//		Edit_SetSel(*m_pWindow, 0, nSize);
				//	}
				//}
				//else if( m_pWindow != NULL )
				//{
				//	if (!m_bAutoSelAll) {
				//		POINT pt = event.ptMouse;
				//		pt.x -= m_rcItem.left + m_rcTextPadding.left;
				//		pt.y -= m_rcItem.top + m_rcTextPadding.top;
				//		Edit_SetSel(*m_pWindow, 0, 0);
				//		::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
				//	}
				//}
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
				if( IsEnabled() ) {
					if( (m_uButtonState & UISTATE_HOT) == 0  ) {
						m_uButtonState |= UISTATE_HOT;
						Invalidate();
					}
				}
			}
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		__super::DoEvent(event);
	}

	void InputBox::SetText(LPCTSTR pstrText)
	{
		//_SetText(infoPtr, pstrText);

		m_sText = pstrText;
		EDITOBJ_SyncText(infoPtr);
		EDIT_ResetText(infoPtr);
		//
		//EDIT_InvalidateUniscribeData(infoPtr);

		//Invalidate();
	}

	void InputBox::SetMaxChar(UINT uMax)
	{
		//m_uMaxChar = uMax;
		//if( m_pWindow != NULL ) Edit_LimitText(*m_pWindow, m_uMaxChar);
	}

	UINT InputBox::GetMaxChar()
	{
		return m_uMaxChar;
	}

	void InputBox::SetReadOnly(bool bReadOnly)
	{
		//if( m_bReadOnly == bReadOnly ) return;
		//
		//m_bReadOnly = bReadOnly;
		//if( m_pWindow != NULL ) Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
		Invalidate();
	}

	bool InputBox::IsReadOnly() const
	{
		return false;
	}

	void InputBox::SetNumberOnly(bool bNumberOnly)
	{
	}

	bool InputBox::IsNumberOnly() const
	{
		return false;
	}

	void InputBox::SetPasswordMode(bool bPasswordMode)
	{
	}

	bool InputBox::IsPasswordMode() const
	{
		return false;
	}

	void InputBox::SetPasswordChar(TCHAR cPasswordChar)
	{
	}

	TCHAR InputBox::GetPasswordChar() const
	{
		return 0;
	}

	void InputBox::SetSel(long nStartChar, long nEndChar)
	{
		//if( m_pWindow != NULL ) Edit_SetSel(*m_pWindow, nStartChar,nEndChar);
	}

	void InputBox::SetSelAll()
	{
		//SetSel(0,-1);
	}

	void InputBox::SetReplaceSel(LPCTSTR lpszReplace)
	{
		//if( m_pWindow != NULL ) Edit_ReplaceSel(*m_pWindow, lpszReplace);
	}

	void InputBox::SetTipValue( LPCTSTR pStrTipValue )
	{
		//m_sTipValue	= pStrTipValue;
	}

	LPCTSTR InputBox::GetTipValue()
	{
		//if (!IsResourceText()) return m_sTipValue;
		//return CResourceManager::GetInstance()->GetText(m_sTipValue);
		return 0;
	}

	void InputBox::SetTipValueColor( LPCTSTR pStrColor )
	{
		STR2ARGB(pStrColor, m_dwTipValueColor);
	}

	DWORD InputBox::GetTipValueColor()
	{
		return m_dwTipValueColor;
	}

	void InputBox::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CControlUI::Move(szOffset, bNeedInvalidate);
		//if( m_pWindow != NULL ) {
		//	RECT rcPos = m_pWindow->CalPos();
		//	::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
		//		rcPos.bottom - rcPos.top, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);        
		//}
	}

	SIZE InputBox::EstimateSize(const SIZE & szAvailable)
	{
		//sif( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, _manager->GetFontInfo(GetFont())->tm.tmHeight + 6);
		return __super::EstimateSize(szAvailable);
	}

	void InputBox::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("readonly")) == 0 ) SetReadOnly(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("numberonly")) == 0 ) SetNumberOnly(_tcsicmp(pstrValue, _T("true")) == 0);
		//else if( _tcscmp(pstrName, _T("autoselall")) == 0 ) SetAutoSelAll(_tcscmp(pstrValue, _T("true")) == 0);	
		else if( _tcsicmp(pstrName, _T("password")) == 0 ) SetPasswordMode(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("passwordchar")) == 0 ) SetPasswordChar(*pstrValue);
		else if( _tcsicmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("maxchar")) == 0 ) SetMaxChar(_ttoi(pstrValue));
		//else if( _tcsicmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		//else if( _tcsicmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		//else if( _tcsicmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		//else if( _tcsicmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("tipvalue")) == 0 ) SetTipValue(pstrValue);
		else if( _tcsicmp(pstrName, _T("tipvaluecolor")) == 0 ) SetTipValueColor(pstrValue);
		else if( _tcsicmp(pstrName, _T("VCENTER")) == 0 ) SetVCenter(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("multiline")) == 0 ) SetMultiline(_tcsicmp(pstrValue, _T("true")) == 0);
		//else if( _tcsicmp(pstrName, _T("nativetextcolor")) == 0 ) SetNativeEditTextColor(pstrValue);
		else if( _tcsicmp(pstrName, _T("nativebkcolor")) == 0 ) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			//SetNativeEditBkColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("textcolor")) == 0 ) SetTextColor(m_dwTextColor, pstrValue);
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void InputBox::SetTextColor(DWORD dwTextColor, LPCTSTR handyStr)
	{
		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		if (handyStr)
		{
			STR2ARGB(handyStr, m_dwTextColor);
		}
		else
		{
			m_dwTextColor = dwTextColor;
			Invalidate();
		}
	}

	DWORD InputBox::GetTextColor() const
	{
		return m_dwTextColor;
	}

	void InputBox::SetDisabledTextColor(DWORD dwTextColor)
	{
		m_dwDisabledTextColor = dwTextColor;
		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		Invalidate();
	}

	DWORD InputBox::GetDisabledTextColor() const
	{
		return m_dwDisabledTextColor;
	}
}
