#include "StdAfx.h"
#include "UIOption.h"

namespace DuiLib
{
	IMPLEMENT_QKCONTROL(OptionBtn)
	OptionBtn::OptionBtn() : m_bSelected(false) ,m_iSelectedFont(-1), m_dwSelectedTextColor(0), m_dwSelectedBkColor(0), m_nSelectedStateCount(0)
	{
	}

	OptionBtn::~OptionBtn()
	{
		if( !m_sGroupName.IsEmpty() && _manager ) _manager->RemoveOptionGroup(m_sGroupName, this);
	}

	LPCTSTR OptionBtn::GetClass() const
	{
		return _T("OptionUI");
	}

	LPVOID OptionBtn::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_OPTION) == 0 ) return static_cast<OptionBtn*>(this);
		return Button::GetInterface(pstrName);
	}

	void OptionBtn::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit, bool setChild)
	{
		__super::SetManager(pManager, pParent, bInit, setChild);
		if( bInit && !m_sGroupName.IsEmpty() ) {
			if (_manager) _manager->AddOptionGroup(m_sGroupName, this);
		}
	}

	LPCTSTR OptionBtn::GetGroup() const
	{
		return m_sGroupName;
	}

	void OptionBtn::SetGroup(LPCTSTR pStrGroupName)
	{
		if( pStrGroupName == NULL ) {
			if( m_sGroupName.IsEmpty() ) return;
			m_sGroupName.Empty();
		}
		else {
			if( m_sGroupName == pStrGroupName ) return;
			if (!m_sGroupName.IsEmpty() && _manager) _manager->RemoveOptionGroup(m_sGroupName, this);
			m_sGroupName = pStrGroupName;
		}

		if( !m_sGroupName.IsEmpty() ) {
			if (_manager) _manager->AddOptionGroup(m_sGroupName, this);
		}
		else {
			if (_manager) _manager->RemoveOptionGroup(m_sGroupName, this);
		}

		Selected(m_bSelected);
	}

	bool OptionBtn::IsSelected() const
	{
		return m_bSelected;
	}

	void OptionBtn::Selected(bool bSelected, bool bMsg/* = true*/)
	{
		if(m_bSelected == bSelected) return;

		m_bSelected = bSelected;
		if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if( _manager != NULL ) {
			if( !m_sGroupName.IsEmpty() ) {
				if( m_bSelected ) {
					CStdPtrArray* aOptionGroup = _manager->GetOptionGroup(m_sGroupName);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
						OptionBtn* pControl = static_cast<OptionBtn*>(aOptionGroup->GetAt(i));
						if( pControl != this ) {
							pControl->Selected(false, bMsg);
						}
					}
					if(bMsg) {
						_manager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
					}
				}
			}
			else {
				if(bMsg) {
					_manager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
			}
		}

		Invalidate();
	}

	bool OptionBtn::Activate()
	{
		if( !Button::Activate() ) return false;
		if( !m_sGroupName.IsEmpty() ) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void OptionBtn::SetEnabled(bool bEnable)
	{
		__super::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			if( m_bSelected ) m_uButtonState = UISTATE_DISABLED;
			else m_uButtonState = UISTATE_DISABLED;
		}
		else {
			m_uButtonState = 0;
		}
	}

	LPCTSTR OptionBtn::GetSelectedImage()
	{
		return m_sSelectedImage;
	}

	void OptionBtn::SetSelectedImage(LPCTSTR pStrImage)
	{
		m_sSelectedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR OptionBtn::GetSelectedHotImage()
	{
		return m_sSelectedHotImage;
	}

	void OptionBtn::SetSelectedHotImage( LPCTSTR pStrImage )
	{
		m_sSelectedHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR OptionBtn::GetSelectedPushedImage()
	{
		return m_sSelectedPushedImage;
	}

	void OptionBtn::SetSelectedPushedImage(LPCTSTR pStrImage)
	{
		m_sSelectedPushedImage = pStrImage;
		Invalidate();
	}

	void OptionBtn::SetSelectedTextColor(DWORD dwTextColor)
	{
		m_dwSelectedTextColor = dwTextColor;
	}

	DWORD OptionBtn::GetSelectedTextColor()
	{
		if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = _manager->GetDefaultFontColor();
		return m_dwSelectedTextColor;
	}

	void OptionBtn::SetSelectedBkColor( DWORD dwBkColor )
	{
		m_dwSelectedBkColor = dwBkColor;
	}

	DWORD OptionBtn::GetSelectBkColor()
	{
		return m_dwSelectedBkColor;
	}

	LPCTSTR OptionBtn::GetSelectedForedImage()
	{
		return m_sSelectedForeImage;
	}

	void OptionBtn::SetSelectedForedImage(LPCTSTR pStrImage)
	{
		m_sSelectedForeImage = pStrImage;
		Invalidate();
	}

	void OptionBtn::SetSelectedStateCount(int nCount)
	{
		m_nSelectedStateCount = nCount;
		Invalidate();
	}

	int OptionBtn::GetSelectedStateCount() const
	{
		return m_nSelectedStateCount;
	}

	LPCTSTR OptionBtn::GetSelectedStateImage()
	{
		return m_sSelectedStateImage;
	}

	void OptionBtn::SetSelectedStateImage( LPCTSTR pStrImage )
	{
		m_sSelectedStateImage = pStrImage;
		Invalidate();
	}
	void OptionBtn::SetSelectedFont(int index)
	{
		m_iSelectedFont = index;
		Invalidate();
	}

	int OptionBtn::GetSelectedFont() const
	{
		return m_iSelectedFont;
	}
	void OptionBtn::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("group")) == 0 ) SetGroup(pstrValue);
		else if( _tcsicmp(pstrName, _T("selected")) == 0 ) Selected(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("selectedimage")) == 0 ) SetSelectedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedhotimage")) == 0 ) SetSelectedHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedpushedimage")) == 0 ) SetSelectedPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedforeimage")) == 0 ) SetSelectedForedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedstateimage")) == 0 ) SetSelectedStateImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedstatecount")) == 0 ) SetSelectedStateCount(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("selectedbkcolor")) == 0 ) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetSelectedBkColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("selectedtextcolor")) == 0 ) {
			DWORD clrColor;
			STR2ARGB(pstrValue, clrColor);
			SetSelectedTextColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("selectedfont")) == 0 ) SetSelectedFont(_ttoi(pstrValue));
		else Button::SetAttribute(pstrName, pstrValue);
	}

	void OptionBtn::PaintBkColor(HDC hDC)
	{
		if(IsSelected()) {
			if(m_dwSelectedBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
			}
		}
		else {
			return Button::PaintBkColor(hDC);
		}
	}

	void OptionBtn::PaintStatusImage(HDC hDC)
	{
		if(IsSelected()) {
			if(!m_sSelectedStateImage.IsEmpty() && m_nSelectedStateCount > 0)
			{
				TDrawInfo info;
				info.Parse(m_sSelectedStateImage, _T(""), _manager);
				const TImageInfo* pImage = _manager->GetImageEx(info.sImageName, info.sResType, info.dwMask, info.bHSL);
				if(m_sSelectedImage.IsEmpty() && pImage != NULL)
				{
					SIZE szImage = {pImage->nX, pImage->nY};
					SIZE szStatus = {pImage->nX / m_nSelectedStateCount, pImage->nY};
					if( szImage.cx > 0 && szImage.cy > 0 )
					{
						RECT rcSrc = {0, 0, szImage.cx, szImage.cy};
						if(m_nSelectedStateCount > 0) {
							int iLeft = rcSrc.left + 0 * szStatus.cx;
							int iRight = iLeft + szStatus.cx;
							int iTop = rcSrc.top;
							int iBottom = iTop + szStatus.cy;
							m_sSelectedImage.Format(_T("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.GetData(), info.sResType.GetData(), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
						}
						if(m_nSelectedStateCount > 1) {
							int iLeft = rcSrc.left + 1 * szStatus.cx;
							int iRight = iLeft + szStatus.cx;
							int iTop = rcSrc.top;
							int iBottom = iTop + szStatus.cy;
							m_sSelectedHotImage.Format(_T("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.GetData(), info.sResType.GetData(), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
							m_sSelectedPushedImage.Format(_T("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.GetData(), info.sResType.GetData(), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
						}
						if(m_nSelectedStateCount > 2) {
							int iLeft = rcSrc.left + 2 * szStatus.cx;
							int iRight = iLeft + szStatus.cx;
							int iTop = rcSrc.top;
							int iBottom = iTop + szStatus.cy;
							m_sSelectedPushedImage.Format(_T("res='%s' restype='%s' dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"), info.sImageName.GetData(), info.sResType.GetData(), info.rcDest.left, info.rcDest.top, info.rcDest.right, info.rcDest.bottom, iLeft, iTop, iRight, iBottom);
						}
					}
				}
			}


			if( (m_uButtonState & UISTATE_PUSHED) != 0 && !m_sSelectedPushedImage.IsEmpty()) {
				if( !DrawImage(hDC, (LPCTSTR)m_sSelectedPushedImage) ) {}
				else return;
			}
			else if( (m_uButtonState & UISTATE_HOT) != 0 && !m_sSelectedHotImage.IsEmpty()) {
				if( !DrawImage(hDC, (LPCTSTR)m_sSelectedHotImage) ) {}
				else return;
			}

			if( !m_sSelectedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sSelectedImage) ) {}
			}
		}
		else {
			Button::PaintStatusImage(hDC);
		}
	}

	void OptionBtn::PaintForeImage(HDC hDC)
	{
		if(IsSelected()) {
			if( !m_sSelectedForeImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sSelectedForeImage) ) {}
				else return;
			}
		}

		return Button::PaintForeImage(hDC);
	}

	void OptionBtn::PaintText(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_SELECTED) != 0 )
		{
			DWORD oldTextColor = m_dwTextColor;
			if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

			if( m_dwTextColor == 0 ) m_dwTextColor = _manager->GetDefaultFontColor();
			if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = _manager->GetDefaultDisabledColor();

			int iFont = GetFont();
			if(GetSelectedFont() != -1) {
				iFont = GetSelectedFont();
			}
			QkString sText = GetText();
			if( sText.IsEmpty() ) return;
			int nLinks = 0;
			RECT rc = m_rcItem;
			ApplyInsetToRect(rc);
			
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, _manager, rc, sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				NULL, NULL, nLinks, iFont, m_uTextStyle);
			else
				CRenderEngine::DrawPlainText(hDC, _manager, rc, sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				iFont, m_uTextStyle);

			m_dwTextColor = oldTextColor;
		}
		else
			Button::PaintText(hDC);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	IMPLEMENT_QKCONTROL(CCheckBoxUI)

	CCheckBoxUI::CCheckBoxUI() : m_bAutoCheck(false)
	{

	}

	LPCTSTR CCheckBoxUI::GetClass() const
	{
		return _T("CheckBoxUI");
	}
	LPVOID CCheckBoxUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_CHECKBOX) == 0 ) return static_cast<CCheckBoxUI*>(this);
		return OptionBtn::GetInterface(pstrName);
	}

	void CCheckBoxUI::SetCheck(bool bCheck)
	{
		Selected(bCheck);
	}

	bool  CCheckBoxUI::GetCheck() const
	{
		return IsSelected();
	}

	void CCheckBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("EnableAutoCheck")) == 0 ) SetAutoCheck(_tcsicmp(pstrValue, _T("true")) == 0);
		
		OptionBtn::SetAttribute(pstrName, pstrValue);
	}

	void CCheckBoxUI::SetAutoCheck(bool bEnable)
	{
		m_bAutoCheck = bEnable;
	}

	void CCheckBoxUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( _parent != NULL ) _parent->DoEvent(event);
			else OptionBtn::DoEvent(event);
			return;
		}

		if( m_bAutoCheck && (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
				SetCheck(!GetCheck()); 
				_manager->SendNotify(this, DUI_MSGTYPE_CHECKCLICK, 0, 0);
				Invalidate();
			}
			return;
		}
		OptionBtn::DoEvent(event);
	}

	void CCheckBoxUI::Selected(bool bSelected, bool bMsg/* = true*/)
	{
		if( m_bSelected == bSelected ) return;

		m_bSelected = bSelected;
		if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if( _manager != NULL ) {
			if( !m_sGroupName.IsEmpty() ) {
				if( m_bSelected ) {
					CStdPtrArray* aOptionGroup = _manager->GetOptionGroup(m_sGroupName);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
						OptionBtn* pControl = static_cast<OptionBtn*>(aOptionGroup->GetAt(i));
						if( pControl != this ) {
							pControl->Selected(false, bMsg);
						}
					}
					if(bMsg) {
						_manager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED, m_bSelected, 0);
					}
				}
			}
			else {
				if(bMsg) {
					_manager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED, m_bSelected, 0);
				}
			}
		}

		Invalidate();
	}
}