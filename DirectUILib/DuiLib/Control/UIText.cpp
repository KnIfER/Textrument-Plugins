#include "StdAfx.h"
#include "UIText.h"

namespace DuiLib
{
	IMPLEMENT_QKCONTROL(CTextUI)

	CTextUI::CTextUI() : m_nLinks(0), m_nHoverLink(-1)
	{
		m_uTextStyle = DT_WORDBREAK;
		::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
	}

	CTextUI::~CTextUI()
	{
	}

	LPCTSTR CTextUI::GetClass() const
	{
		return _T("TextUI");
	}

	LPVOID CTextUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_TEXT) == 0 ) return static_cast<CTextUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CTextUI::GetControlFlags() const
	{
		if( IsEnabled() && m_nLinks > 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	QkString* CTextUI::GetLinkContent(int iIndex)
	{
		if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
		return NULL;
	}

	void CTextUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( _parent != NULL ) _parent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					Invalidate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONUP && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					_manager->SendNotify(this, DUI_MSGTYPE_LINK, i);
					return;
				}
			}
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		// When you move over a link
		if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE && IsEnabled() ) {
			int nHoverLink = -1;
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					nHoverLink = i;
					break;
				}
			}

			if(m_nHoverLink != nHoverLink) {
				m_nHoverLink = nHoverLink;
				Invalidate();
				return;
			}      
		}
		if( event.Type == UIEVENT_MOUSELEAVE ) {
			if( m_nLinks > 0 && IsEnabled() ) {
				if(m_nHoverLink != -1) {
					m_nHoverLink = -1;
					Invalidate();
					return;
				}
			}
		}

		CLabelUI::DoEvent(event);
	}

	SIZE CTextUI::EstimateSize(const SIZE & szAvailable)
	{
		//QkString sText = GetText();
		//RECT m_rcTextPadding = GetTextPadding();
		//
		//RECT rcText = { 0, 0, m_bAutoCalcWidth ? 9999 : GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx), 9999 };
		//rcText.left += m_rcTextPadding.left;
		//rcText.right -= m_rcTextPadding.right;
		//
		//if( m_bShowHtml ) {   
		//	int nLinks = 0;
		//	CRenderEngine::DrawHtmlText(_manager->GetPaintDC(), _manager, rcText, sText, m_dwTextColor, NULL, NULL, nLinks, _font, DT_CALCRECT | m_uTextStyle);
		//}
		//else {
		//	CRenderEngine::DrawPlainText(_manager->GetPaintDC(), _manager, rcText, sText, m_dwTextColor, _font, DT_CALCRECT | m_uTextStyle);
		//}
		//SIZE cXY = {rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
		//	rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom};
		//
		//if (m_bAutoCalcWidth)
		//{
		//	m_cxyFixed.cx = MulDiv(cXY.cx, 100.0, GetManager()->GetDPIObj()->GetScale());
		//}

		return __super::EstimateSize(szAvailable);
	}

	void CTextUI::PaintText(HDC hDC)
	{
		QkString sText = GetText();
		if( sText.IsEmpty() ) {
			m_nLinks = 0;
			return;
		}

		if( m_dwTextColor == 0 ) m_dwTextColor = _manager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = _manager->GetDefaultDisabledColor();

		m_nLinks = lengthof(m_rcLinks);
		RECT rc = m_rcItem;
		ApplyInsetToRect(rc);
		if( IsEnabled() ) {
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, _manager, rc, sText, m_dwTextColor, \
				m_rcLinks, m_sLinks, m_nLinks, _font, m_uTextStyle);
			else
				CRenderEngine::DrawPlainText(hDC, _manager, rc, sText, m_dwTextColor, \
				_font, m_uTextStyle);

				//m_cxyFixed.cy = rc.bottom;
		}
		else {
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, _manager, rc, sText, m_dwDisabledTextColor, \
				m_rcLinks, m_sLinks, m_nLinks, _font, m_uTextStyle);
			else
				CRenderEngine::DrawPlainText(hDC, _manager, rc, sText, m_dwDisabledTextColor, \
				_font, m_uTextStyle);
		}
	}
}
