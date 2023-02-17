#include "StdAfx.h"

#ifdef QkUIList

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CListElementUI::CListElementUI() : m_iIndex(-1),
		m_pOwner(NULL),
		m_bSelected(false),
		m_uButtonState(0)
	{
	}

	LPCTSTR CListElementUI::GetClass() const
	{
		return _T("ListElementUI");
	}

	UINT CListElementUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	LPVOID CListElementUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_LISTITEM) == 0) return static_cast<IListItemUI*>(this);
		if (_tcsicmp(pstrName, DUI_CTR_LISTELEMENT) == 0) return static_cast<CListElementUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void CListElementUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = pOwner;
	}

	void CListElementUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
		if (!IsVisible() && m_bSelected)
		{
			m_bSelected = false;
			QkString buffer; buffer.Format(L"%d", -1);
			if (m_pOwner != NULL) m_pOwner->SetAttribute(L"selecteditem", buffer);
		}
	}

	void CListElementUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if (!IsEnabled()) {
			m_uButtonState = 0;
		}
	}

	int CListElementUI::GetIndex() const
	{
		return m_iIndex;
	}

	void CListElementUI::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	void CListElementUI::Invalidate()
	{
		if (!IsVisible()) return;

		if (GetParent()) {
			CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
			if (pParentContainer) {
				RECT rc = pParentContainer->GetPos();
				pParentContainer->ApplyInsetToRect(rc);
				CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if (pVerticalScrollBar && pVerticalScrollBar->IsVisible()) rc.right -= pVerticalScrollBar->GetFixedWidth();
				CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = m_rcItem;
				if (!::IntersectRect(&invalidateRc, &m_rcItem, &rc))
				{
					return;
				}

				CControlUI* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while (pParent = pParent->GetParent())
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent))
					{
						return;
					}
				}

				if (_manager != NULL) _manager->Invalidate(invalidateRc);
			}
			else {
				CControlUI::Invalidate();
			}
		}
		else {
			CControlUI::Invalidate();
		}
	}

	bool CListElementUI::Activate()
	{
		if (!CControlUI::Activate()) return false;
		if (_manager != NULL) _manager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
		return true;
	}

	bool CListElementUI::IsExpanded() const
	{
		return false;
	}

	bool CListElementUI::Expand(bool /*bExpand = true*/)
	{
		return false;
	}

	void CListElementUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pOwner != NULL) m_pOwner->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_DBLCLICK)
		{
			if (IsEnabled()) {
				Activate();
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_KEYDOWN && IsEnabled())
		{
			if (event.chKey == VK_RETURN) {
				Activate();
				Invalidate();
				return;
			}
		}
		// An important twist: The list-item will send the event not to its immediate
		// parent but to the "attached" list. A list may actually embed several components
		// in its path to the item, but key-presses etc. needs to go to the actual list.
		if (m_pOwner != NULL) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
	}

	void CListElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		//if (_tcsicmp(pstrName, _T("selected")) == 0) Select();
		CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CListElementUI::DrawItemBk(HDC hDC, const RECT& rcItem)
	{
		ASSERT(m_pOwner);
		if (m_pOwner == NULL) return;
		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return;
		CListHeaderUI* headerView = pInfo->headerView;
		DWORD iBackColor = 0;
		if (!pInfo->bAlternateBk || m_iIndex % 2 == 0) iBackColor = pInfo->dwBkColor;
		if ((m_uButtonState & UISTATE_HOT) != 0 && pInfo->dwHotBkColor > 0) {
			iBackColor = pInfo->dwHotBkColor;
		}
		//if (IsSelected() && pInfo->dwSelectedBkColor > 0) {
		//	iBackColor = pInfo->dwSelectedBkColor;
		//}
		if (!IsEnabled() && pInfo->dwDisabledBkColor > 0) {
			iBackColor = pInfo->dwDisabledBkColor;
		}

		if (iBackColor != 0) {
			CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
		}

		if (!IsEnabled()) {
			if (!pInfo->sDisabledImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage)) {}
				else return;
			}
		}
		//if (IsSelected()) {
		//	if (!pInfo->sSelectedImage.IsEmpty()) {
		//		if (!DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage)) {}
		//		else return;
		//	}
		//}
		if ((m_uButtonState & UISTATE_HOT) != 0) {
			if (!pInfo->sHotImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)pInfo->sHotImage)) {}
				else return;
			}
		}

		if (!pInfo->bAlternateBk || m_iIndex % 2 == 0) {
			if (!DrawImage(hDC, m_tBkImage)) {}
		}

		if (m_tBkImage.sName.IsEmpty()) {
			if (!pInfo->sBkImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)pInfo->sBkImage)) {}
				else return;
			}
		}

		if (pInfo->dwLineColor != 0) {
			if (pInfo->bShowRowLine) {
				RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
				CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
			}
			if (pInfo->bShowColumnLine) {
				for (int i = 0; i < pInfo->nColumns; i++) {
					const RECT & position = headerView->GetItemAt(i)->GetPos();
					RECT rcLine = { position.right - 1, m_rcItem.top, position.right - 1, m_rcItem.bottom };
					CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
				}
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_QKCONTROL(CListLabelElementUI)

		CListLabelElementUI::CListLabelElementUI()
	{
	}

	LPCTSTR CListLabelElementUI::GetClass() const
	{
		return _T("ListLabelElementUI");
	}

	LPVOID CListLabelElementUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_LISTLABELELEMENT) == 0) return static_cast<CListLabelElementUI*>(this);
		return CListElementUI::GetInterface(pstrName);
	}

	void CListLabelElementUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pOwner != NULL) m_pOwner->DoEvent(event);
			else CListElementUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_BUTTONUP)
		{
			if (IsEnabled()) {
				_manager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if (IsEnabled()) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
				if(IsRichEvent()) _manager->SendNotify(this, DUI_MSGTYPE_MOUSEENTER);
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if ((m_uButtonState & UISTATE_HOT) != 0) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
				if(IsRichEvent()) _manager->SendNotify(this, DUI_MSGTYPE_MOUSELEAVE);
			}
			return;
		}
		CListElementUI::DoEvent(event);
	}


	SIZE CListLabelElementUI::EstimateSize(const SIZE & szAvailable)
	{
		if (m_pOwner == NULL) return CDuiSize(0, 0);
		QkString sText = GetText();

		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return m_cxyFixed;
		SIZE cXY = m_cxyFixed;
		if (cXY.cy == 0 && _manager != NULL) {
			cXY.cy = _manager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
			cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		if (cXY.cx == 0) {
			cXY.cx = szAvailable.cx;
		}

		return cXY;
	}

	bool CListLabelElementUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		DrawItemBk(hDC, m_rcItem);
		DrawItemText(hDC, m_rcItem);
		return true;
	}

	void CListLabelElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		QkString sText = GetText();
		if (sText.IsEmpty()) return;

		if (m_pOwner == NULL) return;
		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return;
		DWORD iTextColor = pInfo->dwTextColor;
		if ((m_uButtonState & UISTATE_HOT) != 0) {
			iTextColor = pInfo->dwHotTextColor;
		}
		//if (IsSelected()) {
		//	iTextColor = pInfo->dwSelectedTextColor;
		//}
		if (!IsEnabled()) {
			iTextColor = pInfo->dwDisabledTextColor;
		}
		int nLinks = 0;
		RECT rcText = rcItem;
		RECT rcTextPadding = GetManager()->GetDPIObj()->Scale(pInfo->rcTextPadding);
		rcText.left += rcTextPadding.left;
		rcText.right -= rcTextPadding.right;
		rcText.top += rcTextPadding.top;
		rcText.bottom -= rcTextPadding.bottom;

		if (pInfo->bShowHtml)
			CRenderEngine::DrawHtmlText(hDC, _manager, rcText, sText, iTextColor, \
				NULL, NULL, nLinks, pInfo->nFont, pInfo->uTextStyle);
		else
			CRenderEngine::DrawPlainText(hDC, _manager, rcText, sText, iTextColor, \
				pInfo->nFont, pInfo->uTextStyle);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_QKCONTROL(CListTextElementUI)

		CListTextElementUI::CListTextElementUI() : m_nLinks(0), m_nHoverLink(-1), m_pOwner(NULL)
	{
		::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
	}

	CListTextElementUI::~CListTextElementUI()
	{
		QkString* pText;
		for (int it = 0; it < m_aTexts.GetSize(); it++) {
			pText = static_cast<QkString*>(m_aTexts[it]);
			if (pText) delete pText;
		}
		m_aTexts.Empty();
	}

	LPCTSTR CListTextElementUI::GetClass() const
	{
		return _T("ListTextElementUI");
	}

	LPVOID CListTextElementUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_LISTTEXTELEMENT) == 0) return static_cast<CListTextElementUI*>(this);
		return CListLabelElementUI::GetInterface(pstrName);
	}

	UINT CListTextElementUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN | ((IsEnabled() && m_nLinks > 0) ? UIFLAG_SETCURSOR : 0);
	}

	LPCTSTR CListTextElementUI::GetText(int iIndex) const
	{
		QkString* pText = static_cast<QkString*>(m_aTexts.GetAt(iIndex));
		if (pText) {
			if (!IsResourceText())
				return pText->GetData();
			return CResourceManager::GetInstance()->GetText(*pText);
		}
		return NULL;
	}

	void CListTextElementUI::SetText(int iIndex, LPCTSTR pstrText)
	{
		if (m_pOwner == NULL) return;

		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return;
		if (iIndex < 0 || iIndex >= pInfo->nColumns) return;
		while (m_aTexts.GetSize() < pInfo->nColumns) { m_aTexts.Add(NULL); }

		QkString* pText = static_cast<QkString*>(m_aTexts[iIndex]);
		if ((pText == NULL && pstrText == NULL) || (pText && *pText == pstrText)) return;

		if (pText) { delete pText; pText = NULL; }
		m_aTexts.SetAt(iIndex, new QkString(pstrText));

		Invalidate();
	}

	DWORD CListTextElementUI::GetTextColor(int iIndex) const
	{
		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return 0xFF333333;
		if( iIndex < 0 || iIndex >= pInfo->nColumns || m_aTextColors.GetSize() <= 0 ) return pInfo->dwTextColor;

		DWORD dwColor = (DWORD)m_aTextColors.GetAt(iIndex);
		return dwColor;
	}

	void CListTextElementUI::SetTextColor(int iIndex, DWORD dwTextColor)
	{
		if( m_pOwner == NULL ) return;

		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return;
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aTextColors.GetSize() < pInfo->nColumns ) { m_aTextColors.Add((LPVOID)pInfo->dwTextColor); }
		m_aTextColors.SetAt(iIndex, (LPVOID)dwTextColor);

		Invalidate();
	}

	void CListTextElementUI::SetOwner(CControlUI* pOwner)
	{
		CListElementUI::SetOwner(pOwner);
		m_pOwner = pOwner;
	}

	QkString* CListTextElementUI::GetLinkContent(int iIndex)
	{
		if (iIndex >= 0 && iIndex < m_nLinks) return &m_sLinks[iIndex];
		return NULL;
	}

	void CListTextElementUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pOwner != NULL) m_pOwner->DoEvent(event);
			else CListLabelElementUI::DoEvent(event);
			return;
		}

		// When you hover over a link
		if (event.Type == UIEVENT_SETCURSOR) {
			for (int i = 0; i < m_nLinks; i++) {
				if (::PtInRect(&m_rcLinks[i], event.ptMouse)) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}
		}
		if (event.Type == UIEVENT_BUTTONUP && IsEnabled()) {
			for (int i = 0; i < m_nLinks; i++) {
				if (::PtInRect(&m_rcLinks[i], event.ptMouse)) {
					_manager->SendNotify(this, DUI_MSGTYPE_LINK, i);
					return;
				}
			}
		}
		if (m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE) {
			int nHoverLink = -1;
			for (int i = 0; i < m_nLinks; i++) {
				if (::PtInRect(&m_rcLinks[i], event.ptMouse)) {
					nHoverLink = i;
					break;
				}
			}

			if (m_nHoverLink != nHoverLink) {
				Invalidate();
				m_nHoverLink = nHoverLink;
			}
		}
		if (m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE) {
			if (m_nHoverLink != -1) {
				Invalidate();
				m_nHoverLink = -1;
			}
		}
		CListLabelElementUI::DoEvent(event);
	}

	SIZE CListTextElementUI::EstimateSize(const SIZE & szAvailable)
	{
		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return m_cxyFixed;

		SIZE cXY = m_cxyFixed;
		if (cXY.cy == 0 && _manager != NULL) {
			cXY.cy = _manager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
			if (pInfo) cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		return cXY;
	}

	IListCallbackUI* CListTextElementUI::GetTextCallback() const
	{
		return m_pCallback;
	}

	void CListTextElementUI::SetTextCallback(IListCallbackUI* pCallback)
	{
		m_pCallback = pCallback;
	}

	void CListTextElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		if( m_pOwner == NULL ) return;

		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return;
		CListHeaderUI* headerView = pInfo->headerView;
		IListCallbackUI* pCallback = GetTextCallback();
		m_nLinks = 0;
		int nLinks = lengthof(m_rcLinks);
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			const RECT & position = headerView->GetItemAt(i)->GetPos();
			RECT rcItem = { position.left, m_rcItem.top, position.right, m_rcItem.bottom };
			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

			DWORD iTextColor = pInfo->dwTextColor;
			QkString strText;
			if( pCallback ) {
				strText = pCallback->GetItemText(this, m_iIndex, i);
				int iState = 0;
				if( (m_uButtonState & UISTATE_HOT) != 0 ) {
					iState = 1;
				}
				//if( IsSelected() ) {
				//	iState = 2;
				//}
				if( !IsEnabled() ) {
					iState = 3;
				}
				iTextColor = pCallback->GetItemTextColor(this, m_iIndex, i, iState);
			}
			else {
				strText.Assign(GetText(i));

				iTextColor = GetTextColor(i);
				if( (m_uButtonState & UISTATE_HOT) != 0 ) {
					iTextColor = pInfo->dwHotTextColor;
				}
				//if( IsSelected() ) {
				//	iTextColor = pInfo->dwSelectedTextColor;
				//}
				if( !IsEnabled() ) {
					iTextColor = pInfo->dwDisabledTextColor;
				}

			}
			if( pInfo->bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, _manager, rcItem, strText, iTextColor, \
					&m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, pInfo->nFont, pInfo->uTextStyle);
			else
				CRenderEngine::DrawPlainText(hDC, _manager, rcItem, strText, iTextColor, \
					pInfo->nFont, pInfo->uTextStyle);

			m_nLinks += nLinks;
			nLinks = lengthof(m_rcLinks) - m_nLinks; 
		}
		for( int i = m_nLinks; i < lengthof(m_rcLinks); i++ ) {
			::ZeroMemory(m_rcLinks + i, sizeof(RECT));
			((QkString*)(m_sLinks + i))->Empty();
		}
	}



	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_QKCONTROL(CListContainerElementUI)

		CListContainerElementUI::CListContainerElementUI() :
		m_iIndex(-1),
		m_pOwner(NULL),
		m_bSelected(false),
		m_uButtonState(0)
	{
	}

	LPCTSTR CListContainerElementUI::GetClass() const
	{
		return _T("ListContainerElementUI");
	}

	UINT CListContainerElementUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	LPVOID CListContainerElementUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_LISTITEM) == 0) return static_cast<IListItemUI*>(this);
		if (_tcsicmp(pstrName, DUI_CTR_LISTCONTAINERELEMENT) == 0) return static_cast<CListContainerElementUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	CControlUI* CListContainerElementUI::GetOwner()
	{
		return m_pOwner;
	}

	void CListContainerElementUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = pOwner;
	}

	void CListContainerElementUI::SetVisible(bool bVisible)
	{
		CContainerUI::SetVisible(bVisible);
	}

	void CListContainerElementUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if (!IsEnabled()) {
			m_uButtonState = 0;
		}
	}

	int CListContainerElementUI::GetIndex() const
	{
		return m_iIndex;
	}

	void CListContainerElementUI::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	void CListContainerElementUI::Invalidate()
	{
		if (!IsVisible()) return;

		if (GetParent()) {
			CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
			if (pParentContainer) {
				RECT rc = pParentContainer->GetPos();
				pParentContainer->ApplyInsetToRect(rc);
				CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if (pVerticalScrollBar && pVerticalScrollBar->IsVisible()) rc.right -= pVerticalScrollBar->GetFixedWidth();
				CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = m_rcItem;
				if (!::IntersectRect(&invalidateRc, &m_rcItem, &rc))
				{
					return;
				}

				CControlUI* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while (pParent = pParent->GetParent())
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent))
					{
						return;
					}
				}

				if (_manager != NULL) _manager->Invalidate(invalidateRc);
			}
			else {
				CContainerUI::Invalidate();
			}
		}
		else {
			CContainerUI::Invalidate();
		}
	}

	bool CListContainerElementUI::Activate()
	{
		if (!CContainerUI::Activate()) return false;
		if (_manager != NULL) _manager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
		return true;
	}

	bool CListContainerElementUI::IsExpanded() const
	{
		return false;
	}

	bool CListContainerElementUI::Expand(bool /*bExpand = true*/)
	{
		return false;
	}

	void CListContainerElementUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pOwner != NULL) m_pOwner->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_DBLCLICK)
		{
			if (IsEnabled()) {
				Activate();
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_KEYDOWN && IsEnabled())
		{
			if (event.chKey == VK_RETURN) {
				Activate();
				Invalidate();
				return;
			}
		}

		if (event.Type == UIEVENT_BUTTONUP)
		{
			if (IsEnabled()) {
				_manager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if (IsEnabled()) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
				if(IsRichEvent()) _manager->SendNotify(this, DUI_MSGTYPE_MOUSEENTER);
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if ((m_uButtonState & UISTATE_HOT) != 0) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
				if(IsRichEvent()) _manager->SendNotify(this, DUI_MSGTYPE_MOUSELEAVE);
			}
			return;
		}
		if (event.Type == UIEVENT_TIMER)
		{
			_manager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
			return;
		}

		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			if (IsContextMenuUsed()) {
				_manager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
				return;
			}
		}
		// An important twist: The list-item will send the event not to its immediate
		// parent but to the "attached" list. A list may actually embed several components
		// in its path to the item, but key-presses etc. needs to go to the actual list.
		if (m_pOwner != NULL) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
	}


	void CListContainerElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		//if (_tcsicmp(pstrName, _T("selected")) == 0) Select();
		//else if( _tcscmp(pstrName, _T("expandable")) == 0 ) SetExpandable(_tcscmp(pstrValue, _T("true")) == 0);
		CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	bool CListContainerElementUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		DrawItemBk(hDC, m_rcItem);
		return CContainerUI::DoPaint(hDC, rcPaint, pStopControl);
	}

	void CListContainerElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		return;
	}

	void CListContainerElementUI::DrawItemBk(HDC hDC, const RECT& rcItem)
	{
		ASSERT(m_pOwner);
		if (m_pOwner == NULL) return;
		TListInfoUI* pInfo = (TListInfoUI*)m_pOwner->GetAttribute(L"ListInfo");
		if (!pInfo) return;
		CListHeaderUI* headerView = pInfo->headerView;
		DWORD iBackColor = 0;
		if (!pInfo->bAlternateBk || m_iIndex % 2 == 0) iBackColor = pInfo->dwBkColor;

		if ((m_uButtonState & UISTATE_HOT) != 0 && pInfo->dwHotBkColor > 0) {
			iBackColor = pInfo->dwHotBkColor;
		}
		//if (IsSelected() && pInfo->dwSelectedBkColor > 0) {
		//	iBackColor = pInfo->dwSelectedBkColor;
		//}
		if (!IsEnabled() && pInfo->dwDisabledBkColor > 0) {
			iBackColor = pInfo->dwDisabledBkColor;
		}
		if (iBackColor != 0) {
			CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
		}

		if (!IsEnabled()) {
			if (!pInfo->sDisabledImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage)) {}
				else return;
			}
		}
		//if (IsSelected()) {
		//	if (!pInfo->sSelectedImage.IsEmpty()) {
		//		if (!DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage)) {}
		//		else return;
		//	}
		//}
		if ((m_uButtonState & UISTATE_HOT) != 0) {
			if (!pInfo->sHotImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)pInfo->sHotImage)) {}
				else return;
			}
		}
		if (!m_tBkImage.sName.IsEmpty()) {
			if (!pInfo->bAlternateBk || m_iIndex % 2 == 0) {
				if (!DrawImage(hDC, m_tBkImage)) {}
			}
		}

		if (m_tBkImage.sName.IsEmpty()) {
			if (!pInfo->sBkImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)pInfo->sBkImage)) {}
				else return;
			}
		}

		if (pInfo->dwLineColor != 0) {
			if (pInfo->bShowRowLine) {
				RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
				CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
			}
			if (pInfo->bShowColumnLine) {
				for (int i = 0; i < pInfo->nColumns; i++) {
					const RECT & position = headerView->GetItemAt(i)->GetPos();
					RECT rcLine = { position.right - 1, m_rcItem.top, position.right - 1, m_rcItem.bottom };
					CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
				}
			}
		}
	}

	void CListContainerElementUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		if (m_pOwner == NULL) return;

		UINT uListType = m_pOwner->GetAttribute(L"ListType");
		if (uListType == LT_LIST) {
			int nFixedWidth = GetFixedWidth();
			if (nFixedWidth > 0)
			{
				int nRank = (rc.right - rc.left) / nFixedWidth;
				if (nRank > 0)
				{
					int nIndex = GetIndex();
					int nfloor = nIndex / nRank;
					int nHeight = rc.bottom - rc.top;

					rc.top = rc.top - nHeight * (nIndex - nfloor);
					rc.left = rc.left + nFixedWidth * (nIndex % nRank);
					rc.right = rc.left + nFixedWidth;
					rc.bottom = nHeight + rc.top;
				}
			}
		}
		CHorizontalLayoutUI::SetPos(rc, bNeedInvalidate);

		if (uListType != LT_LIST && uListType != LT_TREE) return;
		CListUI* pList = static_cast<CListUI*>(m_pOwner);
		if (uListType == LT_TREE)
		{
			pList = (CListUI*)pList->CControlUI::GetInterface(_T("List"));
			if (pList == NULL) return;
		}

		CListHeaderUI *pHeader = pList->GetHeader();
		if (pHeader == NULL || !pHeader->IsVisible()) return;
		int nCount = m_items.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CControlUI *pListItem = static_cast<CControlUI*>(m_items[i]);
			CControlUI *pHeaderItem = pHeader->GetItemAt(i);
			if (pHeaderItem == NULL) return;
			RECT rcHeaderItem = pHeaderItem->GetPos();
			if (pListItem != NULL && !(rcHeaderItem.left == 0 && rcHeaderItem.right == 0))
			{
				RECT rt = pListItem->GetPos();
				rt.left = rcHeaderItem.left;
				rt.right = rcHeaderItem.right;
				pListItem->SetPos(rt);
			}
		}
	}
}


#endif