#include "StdAfx.h"

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_QKCONTROL(CListHeaderItemUI)

	CListHeaderItemUI::CListHeaderItemUI() : m_bDragable(true), m_uButtonState(0), m_iSepWidth(4),
	m_uTextStyle(DT_VCENTER | DT_CENTER | DT_SINGLELINE), m_dwTextColor(0), m_bShowHtml(false), m_nScale(0)
	{
		SetTextPadding(CDuiRect(2, 0, 2, 0));
		ptLastMouse.x = ptLastMouse.y = 0;
		SetMinWidth(16);
		_font = -1;
	}

	LPCTSTR CListHeaderItemUI::GetClass() const
	{
		return _T("ListHeaderItemUI");
	}

	LPVOID CListHeaderItemUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_LISTHEADERITEM) == 0) return this;
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CListHeaderItemUI::GetControlFlags() const
	{
		if (IsEnabled() && m_iSepWidth != 0) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void CListHeaderItemUI::SetEnabled(bool bEnable)
	{
		CContainerUI::SetEnabled(bEnable);
		if (!IsEnabled()) {
			m_uButtonState = 0;
		}
	}

	bool CListHeaderItemUI::IsDragable() const
	{
		return m_bDragable;
	}

	void CListHeaderItemUI::SetDragable(bool bDragable)
	{
		m_bDragable = bDragable;
		if (!m_bDragable) m_uButtonState &= ~UISTATE_CAPTURED;
	}

	DWORD CListHeaderItemUI::GetSepWidth() const
	{
		return m_iSepWidth;
	}

	void CListHeaderItemUI::SetSepWidth(int iWidth)
	{
		m_iSepWidth = iWidth;
	}

	DWORD CListHeaderItemUI::GetTextStyle() const
	{
		return m_uTextStyle;
	}

	void CListHeaderItemUI::SetTextStyle(UINT uStyle)
	{
		m_uTextStyle = uStyle;
		Invalidate();
	}

	DWORD CListHeaderItemUI::GetTextColor() const
	{
		return m_dwTextColor;
	}


	void CListHeaderItemUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
	}

	RECT CListHeaderItemUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CListHeaderItemUI::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
		Invalidate();
	}

	bool CListHeaderItemUI::IsShowHtml()
	{
		return m_bShowHtml;
	}

	void CListHeaderItemUI::SetShowHtml(bool bShowHtml)
	{
		if (m_bShowHtml == bShowHtml) return;

		m_bShowHtml = bShowHtml;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetNormalImage() const
	{
		return m_sNormalImage;
	}

	void CListHeaderItemUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetHotImage() const
	{
		return m_sHotImage;
	}

	void CListHeaderItemUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetPushedImage() const
	{
		return m_sPushedImage;
	}

	void CListHeaderItemUI::SetPushedImage(LPCTSTR pStrImage)
	{
		m_sPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetFocusedImage() const
	{
		return m_sFocusedImage;
	}

	void CListHeaderItemUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetSepImage() const
	{
		return m_sSepImage;
	}

	void CListHeaderItemUI::SetSepImage(LPCTSTR pStrImage)
	{
		m_sSepImage = pStrImage;
		Invalidate();
	}

	void CListHeaderItemUI::SetScale(int nScale)
	{
		m_nScale = nScale;
	}

	int CListHeaderItemUI::GetScale() const
	{
		return m_nScale;
	}

	void CListHeaderItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("dragable")) == 0) SetDragable(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("sepwidth")) == 0) SetSepWidth(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("align")) == 0) {
			if (_tcsstr(pstrValue, _T("left")) != NULL) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uTextStyle |= DT_LEFT;
			}
			if (_tcsstr(pstrValue, _T("center")) != NULL) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uTextStyle |= DT_CENTER;
			}
			if (_tcsstr(pstrValue, _T("right")) != NULL) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_uTextStyle |= DT_RIGHT;
			}
		}
		else if (_tcsicmp(pstrName, _T("endellipsis")) == 0) {
			if (_tcsicmp(pstrValue, _T("true")) == 0) m_uTextStyle |= DT_END_ELLIPSIS;
			else m_uTextStyle &= ~DT_END_ELLIPSIS;
		}
		else if (_tcsicmp(pstrName, _T("textcolor")) == 0) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetTextColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("textpadding")) == 0) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetTextPadding(rcTextPadding);
		}
		else if (_tcsicmp(pstrName, _T("showhtml")) == 0) SetShowHtml(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("normalimage")) == 0) SetNormalImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("pushedimage")) == 0) SetPushedImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("focusedimage")) == 0) SetFocusedImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("sepimage")) == 0) SetSepImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("scale")) == 0) {
			LPTSTR pstr = NULL;
			SetScale(_tcstol(pstrValue, &pstr, 10));

		}
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CListHeaderItemUI::DoEvent(TEventUI& event)
	{
//#define IMM_DRAG_MODE
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (_parent != NULL) _parent->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS)
		{
			Invalidate();
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			Invalidate();
		}
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{
			if (!IsEnabled()) return;
			RECT rcSeparator = GetThumbRect();
			if (m_iSepWidth >= 0)
				rcSeparator.left -= 4;
			else
				rcSeparator.right += 4;
			if (::PtInRect(&rcSeparator, event.ptMouse))
			{
				if (m_bDragable) {
					m_uButtonState |= UISTATE_CAPTURED;
					ptLastMouse = event.ptMouse;
#ifndef IMM_DRAG_MODE
					_rcLastPos = m_rcItem;
					SetTimer(0x100, 10);
#endif
				}
			}
			else 
			{
				m_uButtonState |= UISTATE_PUSHED;
				_manager->SendNotify(this, DUI_MSGTYPE_HEADERCLICK);
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
			{
				m_uButtonState &= ~UISTATE_CAPTURED;
				if (GetParent())
					GetParent()->NeedParentUpdate();
			}
			else if ((m_uButtonState & UISTATE_PUSHED) != 0)
			{
				m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
#ifndef IMM_DRAG_MODE
			KillTimer(0x100);
#endif
			return;
		}
#ifndef IMM_DRAG_MODE
		if (event.Type == UIEVENT_TIMER && event.wParam==0x100)
		{
			RECT rc = m_rcItem;
			if (m_iSepWidth >= 0) 
			{
				rc.right = _rcLastPos.right - (ptLastMouse.x - event.ptMouse.x);
				if (m_rcItem.right==rc.right) return;
			}
			else 
			{
				rc.left = _rcLastPos.left - (ptLastMouse.x - event.ptMouse.x);
				if (m_rcItem.left==rc.left) return;
			}

			RECT rcPadding = GetPadding();
			if (rc.right - rc.left - rcPadding.right > GetMinWidth()) 
			{
				m_cxyFixed.cx = rc.right - rc.left - rcPadding.right;
				if (GetParent())
					GetParent()->NeedParentUpdate();
			}
			return;
		}
#else
		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
			{
				RECT rc = m_rcItem;
				if (m_iSepWidth >= 0) 
				{
					rc.right -= ptLastMouse.x - event.ptMouse.x;
					if (m_rcItem.right==rc.right) return;
				}
				else 
				{
					rc.left -= ptLastMouse.x - event.ptMouse.x;
					if (m_rcItem.left==rc.left) return;
				}

				RECT rcPadding = GetPadding();
				if (rc.right - rc.left - rcPadding.right > GetMinWidth()) 
				{
					m_cxyFixed.cx = rc.right - rc.left - rcPadding.right;
					ptLastMouse = event.ptMouse;
					if (GetParent())
						GetParent()->NeedParentUpdate();
				}
			}
			return;
		}
#endif
		if (event.Type == UIEVENT_SETCURSOR)
		{
			RECT rcSeparator = GetThumbRect();
			if (m_iSepWidth >= 0)
				rcSeparator.left -= 4;
			else
				rcSeparator.right += 4;
			if (IsEnabled() && m_bDragable && ::PtInRect(&rcSeparator, event.ptMouse)) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
				return;
			}
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if (IsEnabled()) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if (IsEnabled()) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CContainerUI::DoEvent(event);
	}

	SIZE CListHeaderItemUI::EstimateSize(const SIZE & szAvailable)
	{
		if (!_manager) return CDuiSize(m_cxyFixed.cx, m_cxyFixed.cy);
		if (m_cxyFixed.cy == 0) return CDuiSize(m_cxyFixed.cx, _manager->GetDefaultFontInfo()->tm.tmHeight + 14);
		return CContainerUI::EstimateSize(szAvailable);
	}

	RECT CListHeaderItemUI::GetThumbRect() const
	{
		if (m_iSepWidth >= 0) return CDuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
		else return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
	}

	void CListHeaderItemUI::PaintStatusImage(HDC hDC)
	{
		if (IsFocused()) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~UISTATE_FOCUSED;

		if ((m_uButtonState & UISTATE_PUSHED) != 0) {
			if (m_sPushedImage.IsEmpty() && !m_sNormalImage.IsEmpty()) DrawImage(hDC, (LPCTSTR)m_sNormalImage);
			if (!DrawImage(hDC, (LPCTSTR)m_sPushedImage)) {}
		}
		else if ((m_uButtonState & UISTATE_HOT) != 0) {
			if (m_sHotImage.IsEmpty() && !m_sNormalImage.IsEmpty()) DrawImage(hDC, (LPCTSTR)m_sNormalImage);
			if (!DrawImage(hDC, (LPCTSTR)m_sHotImage)) {}
		}
		else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
			if (m_sFocusedImage.IsEmpty() && !m_sNormalImage.IsEmpty()) DrawImage(hDC, (LPCTSTR)m_sNormalImage);
			if (!DrawImage(hDC, (LPCTSTR)m_sFocusedImage)) {}
		}
		else {
			if (!m_sNormalImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) {}
			}
		}

		if (!m_sSepImage.IsEmpty()) {
			RECT rcThumb = GetThumbRect();
			rcThumb.left -= m_rcItem.left;
			rcThumb.top -= m_rcItem.top;
			rcThumb.right -= m_rcItem.left;
			rcThumb.bottom -= m_rcItem.top;

			//m_sSepImageModify.Empty();
			//m_sSepImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			drawableTmp.rcDest = rcThumb;
			if (!DrawImage(hDC, (LPCTSTR)m_sSepImage, &drawableTmp)) {}
		}
	}

	void CListHeaderItemUI::PaintText(HDC hDC)
	{
		const QkString & sText = GetText();
		if (sText.IsEmpty()) return;

		RECT rcText = m_rcItem;
		ApplyInsetToRect(rcText);

		if (m_dwTextColor == 0) m_dwTextColor = _manager->GetDefaultFontColor();
		int nLinks = 0;
		if (m_bShowHtml)
			CRenderEngine::DrawHtmlText(hDC, _manager, rcText, sText, m_dwTextColor, \
				NULL, NULL, nLinks, _font, m_uTextStyle);
		else
			CRenderEngine::DrawPlainText(hDC, _manager, rcText, sText, m_dwTextColor, \
				_font, m_uTextStyle);
	}

}