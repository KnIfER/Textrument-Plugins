#include "StdAfx.h"
#include "UILabel.h"

#include <atlconv.h>
#include "../Core/InsituDebug.h"
namespace DuiLib
{
	IMPLEMENT_QKCONTROL(CLabelUI)

		CLabelUI::CLabelUI() : m_uTextStyle(DT_VCENTER | DT_SINGLELINE), m_dwTextColor(0), 
		m_dwDisabledTextColor(0),
		m_bShowHtml(false)
	{
		m_cxyFixedLast.cx = m_cxyFixedLast.cy = 0;
		m_szAvailableLast.cx = m_szAvailableLast.cy = 0;
		_font = -1;
	}

	CLabelUI::~CLabelUI()
	{
	}

	LPCTSTR CLabelUI::GetClass() const
	{
		return _T("LabelUI");
	}

	LPVOID CLabelUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("Label")) == 0 ) return static_cast<CLabelUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	UINT CLabelUI::GetControlFlags() const
	{
		return IsEnabled() ? UIFLAG_SETCURSOR : 0;
	}
	void CLabelUI::SetTextStyle(UINT uStyle)
	{
		m_uTextStyle = uStyle;
		Invalidate();
	}

	UINT CLabelUI::GetTextStyle() const
	{
		return m_uTextStyle;
	}

	void CLabelUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		Invalidate();
	}

	DWORD CLabelUI::GetTextColor() const
	{
		return m_dwTextColor;
	}

	void CLabelUI::SetDisabledTextColor(DWORD dwTextColor)
	{
		m_dwDisabledTextColor = dwTextColor;
		VIEWSTATE_MARK_DIRTY(VIEW_INFO_DIRTY_COLORS);
		Invalidate();
	}

	DWORD CLabelUI::GetDisabledTextColor() const
	{
		return m_dwDisabledTextColor;
	}

	bool CLabelUI::IsShowHtml()
	{
		return m_bShowHtml;
	}

	void CLabelUI::SetShowHtml(bool bShowHtml)
	{
		if( m_bShowHtml == bShowHtml ) return;

		m_bShowHtml = bShowHtml;
		_view_states |= VIEWSTATEMASK_NeedEstimateSize;
		Invalidate();
	}

	void CLabelUI::NeedRecalcAutoSize()
	{
		m_szAvailableLast.cx=0;
		m_szAvailableLast.cy=0;
	}

	SIZE CLabelUI::EstimateSize(const SIZE & szAvailable)
	{
		if (_manager && _LastScaleProfile!=_manager->GetDPIObj()->ScaleProfile())
			OnDPIChanged();
		if (m_cxyFixed.cx > 0 && m_cxyFixed.cy > 0) {
			return m_cxyFixScaled;
		}

		const RECT & rcTextPadding = m_rcInsetScaled;
		const RECT & rcBorderSize = _rcBorderSizeScaled;

		if ((szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy)) 
		{
			_view_states |= VIEWSTATEMASK_NeedEstimateSize;
		}

		if (m_bNeedEstimateSize) {
			// 自适应文本高度、宽度
			// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-drawtext
			const QkString & sText = GetText();
			_view_states &= ~VIEWSTATEMASK_NeedEstimateSize;
			m_szAvailableLast = szAvailable;
			m_cxyFixedLast = m_cxyFixScaled;
			if (m_uTextStyle & DT_SINGLELINE) 
			{
				if (m_bAutoCalcHeight) 
				{
					m_cxyFixedLast.cy = _manager->GetFontInfo(_font)->tm.tmHeight + 8;
					m_cxyFixedLast.cy += rcTextPadding.top + rcTextPadding.bottom + rcBorderSize.top + rcBorderSize.bottom;
				}
				//if (m_cxyFixedLast.cx == 0) 
				{
					if(m_bAutoCalcWidth) {
						RECT rcText = { 0, 0, 125, m_cxyFixedLast.cy };
						if( m_bShowHtml ) {
							int nLinks = 0;
							CRenderEngine::DrawHtmlText(_manager->GetPaintDC(), _manager, rcText, sText, 0, NULL, NULL, nLinks, _font, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER & ~DT_TABSTOP);
						}
						else {
							CRenderEngine::DrawPlainText(_manager->GetPaintDC(), _manager, rcText, sText, 0, _font, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER & ~DT_TABSTOP);
						}
						m_cxyFixedLast.cx = rcText.right + rcTextPadding.left + rcTextPadding.right + _preSizeX + rcBorderSize.right + rcBorderSize.left;
					}
				}
			}
			else if(m_bAutoCalcHeight) 
			{
				int fixedWidth = (m_cxyFixedLast.cx>0?m_cxyFixedLast:szAvailable).cx;
				fixedWidth -= _preSizeX;
				if (fixedWidth>0)
				{
					RECT rcText = { 0, 0, fixedWidth, -100 };
					rcText.left += rcTextPadding.left;
					rcText.right -= rcTextPadding.right;
					if( m_bShowHtml ) {
						int nLinks = 0;
						CRenderEngine::DrawHtmlText(_manager->GetPaintDC(), _manager, rcText, sText, 0, NULL, NULL, nLinks, _font, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER & ~DT_TABSTOP );
					}
					else {
						CRenderEngine::DrawPlainText(_manager->GetPaintDC(), _manager, rcText, sText, 0, _font, (m_uTextStyle & ~(DT_VCENTER | DT_BOTTOM)) | DT_CALCRECT);
					}
					if (rcText.bottom<0)
					{
						m_cxyFixedLast.cy = GetHeight(); // 维持不变
					}
					else
					{
						m_cxyFixedLast.cy = rcText.bottom - rcText.top + rcTextPadding.top + rcTextPadding.bottom + rcBorderSize.top + rcBorderSize.bottom;
						if (m_bAutoCalcWidth)
						{
							m_cxyFixedLast.cx = rcText.right + rcTextPadding.left + rcTextPadding.right + _preSizeX + rcBorderSize.right + rcBorderSize.left;
						}
					}
				}
				else
				{
					m_cxyFixedLast.cy = GetHeight(); // 维持不变
				}
			}
			// accoring to the doc, no way to handle the case !m_bAutoCalcHeight && m_bAutoCalcWidth in multiline mode

		}
		if (m_items.GetSize())
		{
			SIZE szAvail = szAvailable;
			//szAvail.cx -= m_rcInset.left + m_rcInset.right;
			//szAvail.cy -= m_rcInset.top + m_rcInset.bottom;
			for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
				SIZE childSz = pControl->EstimateSize(szAvail);
				//const SIZE & rcInset = ;
				if (m_cxyFixedLast.cx < childSz.cx + rcTextPadding.left + rcTextPadding.right)
					m_cxyFixedLast.cx = childSz.cx + rcTextPadding.left + rcTextPadding.right;
				if (m_cxyFixedLast.cy < childSz.cy + rcTextPadding.top +  rcTextPadding.bottom)
					m_cxyFixedLast.cy = childSz.cy + rcTextPadding.top +  rcTextPadding.bottom;
			}
		}
		return m_cxyFixedLast;
	}

	void CLabelUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused_YES;
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused_NO;
			return;
		}
		CControlUI::DoEvent(event);
	}

	void CLabelUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("align")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT );
				m_uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_uTextStyle |= DT_RIGHT;
			}
		}
		else if( _tcsicmp(pstrName, _T("valign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("top")) != NULL ) {
				m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER); //  | DT_WORDBREAK
				m_uTextStyle |= (DT_TOP); //  | DT_SINGLELINE
			}
			if( _tcsstr(pstrValue, _T("vcenter")) != NULL ) {
				m_uTextStyle &= ~(DT_TOP | DT_BOTTOM); // | DT_WORDBREAK 
				m_uTextStyle |= (DT_VCENTER); //  | DT_SINGLELINE
			}
			if( _tcsstr(pstrValue, _T("bottom")) != NULL ) {
				m_uTextStyle &= ~(DT_TOP | DT_VCENTER); //  | DT_WORDBREAK
				m_uTextStyle |= (DT_BOTTOM); //  | DT_SINGLELINE
			}
		}
		else if( _tcsicmp(pstrName, _T("endellipsis")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
			else m_uTextStyle &= ~DT_END_ELLIPSIS;
		}   
		else if( _tcsicmp(pstrName, _T("wordbreak")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0 ) {
				m_uTextStyle &= ~DT_SINGLELINE;
				m_uTextStyle |= DT_WORDBREAK; //  | DT_EDITCONTROL
			}
			else {
				m_uTextStyle &= ~DT_WORDBREAK; //  & ~DT_EDITCONTROL
				m_uTextStyle |= DT_SINGLELINE;
			}
		}
		else if( _tcsicmp(pstrName, _T("noprefix")) == 0 ) {
			if( _tcsicmp(pstrValue, _T("true")) == 0)
			{
				m_uTextStyle |= DT_NOPREFIX;
			}
			else
			{
				m_uTextStyle = m_uTextStyle & ~DT_NOPREFIX;
			}
		}
		else if( _tcsicmp(pstrName, _T("textcolor")) == 0 ) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetTextColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("disabledtextcolor")) == 0 ) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetDisabledTextColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("showhtml")) == 0 ) SetShowHtml(_tcsicmp(pstrValue, _T("true")) == 0);
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CLabelUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = _manager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = _manager->GetDefaultDisabledColor();

		RECT rc = m_rcItem;
		ApplyInsetToRect(rc);

		const QkString & sText = GetText();
		if( sText.IsEmpty() ) return;
		int nLinks = 0;
		if( IsEnabled() ) {
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, _manager, rc, sText, m_dwTextColor, \
				NULL, NULL, nLinks, _font, m_uTextStyle);
			else
				CRenderEngine::DrawPlainText(hDC, _manager, rc, sText, m_dwTextColor, \
				_font, m_uTextStyle);
		}
		else {
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, _manager, rc, sText, m_dwDisabledTextColor, \
				NULL, NULL, nLinks, _font, m_uTextStyle);
			else
				CRenderEngine::DrawPlainText(hDC, _manager, rc, sText, m_dwDisabledTextColor, \
				_font, m_uTextStyle);
		}
	}
	void CLabelUI::SetText( LPCTSTR pstrText )
	{
		CControlUI::SetText(pstrText);
		if(GetAutoCalcWidth() || GetAutoCalcHeight()) {
			SetNeedAutoCalcSize();
			//NeedParentUpdate();
		}
	}
}