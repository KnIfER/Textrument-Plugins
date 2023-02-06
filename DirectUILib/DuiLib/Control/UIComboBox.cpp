#include "StdAfx.h"
#include "UIComboBox.h"

namespace DuiLib
{
	IMPLEMENT_QKCONTROL(CComboBoxUI)

	CComboBoxUI::CComboBoxUI()
	{
		m_nArrowWidth = 0;
	}

	LPCTSTR CComboBoxUI::GetClass() const
	{
		return _T("ComboBoxUI");
	}

	void CComboBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("arrowimage")) == 0)
			m_sArrowImage = pstrValue;
		else
			CComboUI::SetAttribute(pstrName, pstrValue);
	}

	void CComboBoxUI::PaintStatusImage(HDC hDC)
	{
		if (m_sArrowImage.IsEmpty())
			CComboUI::PaintStatusImage(hDC);
		else
		{
			// get index
			if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
			else m_uButtonState &= ~ UISTATE_FOCUSED;
			if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
			else m_uButtonState &= ~ UISTATE_DISABLED;

			int nIndex = 0;
			if ((m_uButtonState & UISTATE_DISABLED) != 0)
				nIndex = 4;
			else if ((m_uButtonState & UISTATE_PUSHED) != 0)
				nIndex = 2;
			else if ((m_uButtonState & UISTATE_HOT) != 0)
				nIndex = 1;
			else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
				nIndex = 3;

			// make modify string

			TDrawInfo drawableTmp;
			drawableTmp.Parse(m_sArrowImage, _manager); // todo optimise
			
			m_nArrowWidth = (drawableTmp.rcSource.right - drawableTmp.rcSource.left) / 5;
			drawableTmp.rcSource.left += nIndex * m_nArrowWidth;
			drawableTmp.rcSource.right = drawableTmp.rcSource.left + m_nArrowWidth;

			//CDuiRect rcDest(drawableTmp.rcDest);
			//rcDest.Deflate(GetBorderSize(), GetBorderSize());
			//rcDest.left = rcDest.right - m_nArrowWidth;
			//drawableTmp.rcDest = rcDest;
			
			// draw image
			if (!DrawImage(hDC, drawableTmp))
				{}
		}
	}

	void CComboBoxUI::PaintText(HDC hDC)
	{
		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		rcText.top += m_rcTextPadding.top;
		rcText.bottom -= m_rcTextPadding.bottom;

		rcText.right -= m_nArrowWidth; // add this line than CComboUI::PaintText(HDC hDC)

		if( m_iCurSel >= 0 ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pElement != NULL ) {
				pElement->DrawItemText(hDC, rcText);
			}
			else {
				RECT rcOldPos = pControl->GetPos();
				pControl->SetPos(rcText);
				pControl->DoPaint(hDC, rcText, NULL);
				pControl->SetPos(rcOldPos);
			}
		}
	}
}
