#include "ComboEdit.h"


using namespace DuiLib;

namespace DuiLib
{
	IMPLEMENT_QKCONTROL(CComboEditUI)

	class UILIB_API CComboEditWnd : public CWindowWnd
	{
		// https://blog.csdn.net/gongxie0235/article/details/108095287
	public:
		CComboEditWnd();

		void Init(CComboEditUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		enum {
			DEFAULT_TIMERID = 20,
		};

		CPaintManagerUI m_pm;
		CComboEditUI* m_pOwner;
		CVerticalLayoutUI* m_pLayout;
		HBRUSH m_hBkBrush;
		BOOL m_bInit;
		BOOL m_bDrawCaret;
	};

	CComboEditWnd::CComboEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(FALSE), m_bDrawCaret(FALSE)
	{
	}

	void CComboEditWnd::Init(CComboEditUI* pOwner)
	{
		m_pOwner = pOwner;
		m_pLayout = NULL;
		RECT rcPos = CalPos();
		UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
		UINT uTextStyle = m_pOwner->GetTextStyle();
		if (uTextStyle & DT_LEFT) uStyle |= ES_LEFT;
		else if (uTextStyle & DT_CENTER) uStyle |= ES_CENTER;
		else if (uTextStyle & DT_RIGHT) uStyle |= ES_RIGHT;

		Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);

		HFONT hFont = NULL;
		int iFontIndex = m_pOwner->GetFont();
		if (iFontIndex != -1)
			hFont = m_pOwner->GetManager()->GetFont(iFontIndex);
		if (hFont == NULL)
			hFont = m_pOwner->GetManager()->GetDefaultFontInfo()->hFont;

		SetWindowFont(m_hWnd, hFont, TRUE);
		Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());


		Edit_SetText(m_hWnd, m_pOwner->GetText());
		Edit_SetModify(m_hWnd, FALSE);
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
		Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == TRUE);


		//Styls
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);

		int nSize = GetWindowTextLength(m_hWnd);
		Edit_SetSel(m_hWnd, nSize, nSize);


		m_bInit = TRUE;
	}

	RECT CComboEditWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		RECT rcInset = m_pOwner->GetTextPadding();
		rcPos.left += rcInset.left;
		rcPos.top += rcInset.top;
		rcPos.right -= rcInset.right;
		rcPos.bottom -= rcInset.bottom;
		LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;
		if (lEditHeight < rcPos.GetHeight()) {
			rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
			rcPos.bottom = rcPos.top + lEditHeight;
		}

		CControlUI* pParent = m_pOwner;
		RECT rcParent;
		while (pParent = pParent->GetParent()) {
			if (!pParent->IsVisible()) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
			rcParent = pParent->GetClientPos();
			if (!::IntersectRect(&rcPos, &rcPos, &rcParent)) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
		}

		return rcPos;
	}

	LPCTSTR CComboEditWnd::GetWindowClassName() const
	{
		return _T("ComboEditWnd");
	}

	LPCTSTR CComboEditWnd::GetSuperClassName() const
	{
		return WC_EDIT;
	}

	void CComboEditWnd::OnFinalMessage(HWND hWnd)
	{

		m_pOwner->Invalidate();
		// Clear reference and die
		if (m_hBkBrush != NULL) ::DeleteObject(m_hBkBrush);
		m_pOwner->GetManager()->RemoveNativeWindow(hWnd);
		m_pOwner->m_pEditWnd = NULL;
		delete this;
	}

	LRESULT CComboEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if (uMsg == WM_CREATE) {
			m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
			if (m_pOwner->GetManager()->IsLayered()) {
				::SetTimer(m_hWnd, DEFAULT_TIMERID, ::GetCaretBlinkTime(), NULL);
			}
			bHandled = FALSE;
		}
		else if (uMsg == WM_KILLFOCUS) lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		else if (uMsg == OCM_COMMAND) {
			if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			else if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE) {
				RECT rcClient;
				::GetClientRect(m_hWnd, &rcClient);
				::InvalidateRect(m_hWnd, &rcClient, FALSE);
			}
		}
		else if (uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN) {
			m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
		}
		else if (uMsg == OCM__BASE + WM_CTLCOLOREDIT || uMsg == OCM__BASE + WM_CTLCOLORSTATIC) {
			if (m_pOwner->GetManager()->IsLayered() && !m_pOwner->GetManager()->IsPainting()) {
				m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
			}
			DWORD clrColor = m_pOwner->GetNativeEditBkColor();
			if (clrColor == 0xFFFFFFFF) return 0;
			::SetBkMode((HDC)wParam, TRANSPARENT);
			DWORD dwTextColor = m_pOwner->GetTextColor();
			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
			if (clrColor < 0xFF000000) {
				if (m_hBkBrush != NULL) ::DeleteObject(m_hBkBrush);
				RECT rcWnd = m_pOwner->GetManager()->GetNativeWindowRect(m_hWnd);
				HBITMAP hBmpEditBk = CRenderEngine::GenerateBitmap(m_pOwner->GetManager(), rcWnd, m_pOwner, clrColor);
				m_hBkBrush = ::CreatePatternBrush(hBmpEditBk);
				::DeleteObject(hBmpEditBk);
			}
			else {
				if (m_hBkBrush == NULL) {
					m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
				}
			}
			return (LRESULT)m_hBkBrush;
		}
		else if (uMsg == WM_PAINT) {
			if (m_pOwner->GetManager()->IsLayered()) {
				m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
			}
			bHandled = FALSE;
		}
		else if (uMsg == WM_PRINT) {
			if (m_pOwner->GetManager()->IsLayered()) {
				lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
				if (m_pOwner->IsEnabled() && m_bDrawCaret) { // todo:判断是否enabled
					RECT rcClient;
					::GetClientRect(m_hWnd, &rcClient);
					POINT ptCaret;
					::GetCaretPos(&ptCaret);
					RECT rcCaret = { ptCaret.x, ptCaret.y, ptCaret.x, ptCaret.y + rcClient.bottom - rcClient.top };
					CRenderEngine::DrawLine((HDC)wParam, rcCaret, 1, 0xFF000000);
				}
				return lRes;
			}
			bHandled = FALSE;
		}
		else if (uMsg == WM_TIMER) {
			if (wParam == DEFAULT_TIMERID) {
				m_bDrawCaret = !m_bDrawCaret;
				RECT rcClient;
				::GetClientRect(m_hWnd, &rcClient);
				::InvalidateRect(m_hWnd, &rcClient, FALSE);
				return 0;
			}
			bHandled = FALSE;
		}
		else bHandled = FALSE;
		if (!bHandled) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CComboEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		if ((HWND)wParam != m_pOwner->GetManager()->GetPaintWindow()) {
			::SendMessage(m_pOwner->GetManager()->GetPaintWindow(), WM_KILLFOCUS, wParam, lParam);
		}
		SendMessage(WM_CLOSE);
		return lRes;
	}

	LRESULT CComboEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (!m_bInit) return 0;
		if (m_pOwner == NULL) return 0;
		// Copy text back
		int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		ASSERT(pstr);
		if (pstr == NULL) return 0;
		::GetWindowText(m_hWnd, pstr, cchLen);
		m_pOwner->m_sText = pstr;
		m_pOwner->SetToolTip(pstr);

		m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
		if (m_pOwner->GetManager()->IsLayered()) m_pOwner->Invalidate();
		return 0;
	}


	void CComboEditWnd::EnsureVisible(int iIndex)
	{
		if (m_pOwner->GetCurSel() < 0) return;
		m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
		RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos();
		RECT rcList = m_pLayout->GetPos();
		CScrollBarUI* pHorizontalScrollBar = m_pLayout->GetHorizontalScrollBar();
		if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
		int iPos = m_pLayout->GetScrollPos().cy;
		if (rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom) return;
		int dx = 0;
		if (rcItem.top < rcList.top) dx = rcItem.top - rcList.top;
		if (rcItem.bottom > rcList.bottom) dx = rcItem.bottom - rcList.bottom;
		Scroll(0, dx);
	}
	void CComboEditWnd::Scroll(int dx, int dy)
	{
		if (dx == 0 && dy == 0) return;
		SIZE sz = m_pLayout->GetScrollPos();
		m_pLayout->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
	}


	CComboEditUI::CComboEditUI()
	{
		m_iMaxChar = -1;
		m_pEditWnd = NULL;
		m_iFont = 1;
		m_uTextStyle = DT_VCENTER | DT_SINGLELINE;
		m_dwNativeEditBkColor = 0x00000000;
		m_dwTextColor = 0x00000000;
		m_dwDisabledTextColor = 0;
	}

	LPCTSTR CComboEditUI::GetClass() const
	{
		return (_T("ComboEdit"));
	}


	LPVOID CComboEditUI::GetInterface(LPCTSTR pstrName)
	{
		//if( _tcsicmp(pstrName, DUI_CTR_COMBO) == 0 ) return static_cast<CComboUI*>(this);
		if( _tcsicmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
		if (_tcscmp(pstrName, (_T("ComboEdit"))) == 0) return static_cast<CComboEditUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}


	DuiLib::QkString & CComboEditUI::GetText()
	{
		return m_sText;
	}

	void CComboEditUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (_parent != NULL) _parent->DoEvent(event);
			else CComboUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_KILLFOCUS)
		{
			if (m_pEditWnd)
			{
				m_pEditWnd->ShowWindow(FALSE, FALSE);
			}

			return;
		}
		if (event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN)
		{
			if (m_pEditWnd)
			{
				m_pEditWnd->ShowWindow(TRUE, TRUE);
			}

			if (m_pEditWnd) return;
			m_pEditWnd = new CComboEditWnd();
			ASSERT(m_pEditWnd);
			m_pEditWnd->Init(this);
		}
		if (event.Type == UIEVENT_SCROLLWHEEL)
		{
			if (IsEnabled()) {
				bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
				//SetSelectCloseFlag(false);
				SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
				//SetSelectCloseFlag(true);
				return;
			}
		}


		CComboUI::DoEvent(event);
	}

	int CComboEditUI::GetFont()
	{
		return m_iFont;
	}

	void CComboEditUI::SetFont(int iFont)
	{
		m_iFont = iFont;

	}

	UINT CComboEditUI::GetTextStyle()
	{
		return m_uTextStyle;
	}

	void CComboEditUI::SetTextStyle(UINT iTextStyle)
	{
		m_uTextStyle = iTextStyle;
	}

	int CComboEditUI::GetMaxChar()
	{
		return m_iMaxChar;
	}

	void CComboEditUI::SetMaxChar(int iMaxChar)
	{
		m_iMaxChar = iMaxChar;
	}

	DWORD CComboEditUI::GetNativeEditBkColor()
	{
		return m_dwNativeEditBkColor;
	}

	void CComboEditUI::SetNativeEditBkColor(DWORD dwColor)
	{
		m_dwNativeEditBkColor = dwColor;
	}

	DWORD CComboEditUI::GetTextColor()
	{
		return m_dwTextColor;
	}

	void CComboEditUI::SetTextColor(DWORD dwColor)
	{
		m_dwTextColor = dwColor;
	}

	bool CComboEditUI::Add(CControlUI* pControl)
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_LISTITEM));
		if (pListItem != NULL)
		{
			pListItem->SetOwner(this);
			pListItem->SetIndex(m_items.GetSize());
		}
		return CContainerUI::Add(pControl);
	}

	bool CComboEditUI::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
	{
		//if (m_bSelectCloseFlag && m_pWindow != NULL) m_pWindow->Close();
		if (iIndex == m_iCurSel) return TRUE;
		int iOldSel = m_iCurSel;
		if (m_iCurSel >= 0) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			if (!pControl) return FALSE;

			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_LISTITEM));
			if (pListItem != NULL) pListItem->Select(FALSE); // , bTriggerEvent
			m_iCurSel = -1;
		}
		if (iIndex < 0) return FALSE;
		if (m_items.GetSize() == 0) return FALSE;
		if (iIndex >= m_items.GetSize()) iIndex = m_items.GetSize() - 1;
		CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);
		if (!pControl || !pControl->IsVisible() || !pControl->IsEnabled()) return FALSE;

		m_sText = pControl->GetText();
		SetToolTip(m_sText);
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_LISTITEM));
		if (pListItem == NULL) return FALSE;
		m_iCurSel = iIndex;
		if (m_pWindow != NULL || bTakeFocus) pControl->SetFocus();
		pListItem->Select(TRUE); // , bTriggerEvent
		if (_manager != NULL && bTriggerEvent) _manager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);


		Invalidate();
		return TRUE;
	}

	void CComboEditUI::PaintText(HDC hDC)
	{
		//if (!m_bShowText) hhh
		{
		//	return;
		}
		if (m_pEditWnd && IsWindowVisible(m_pEditWnd->GetHWND()))
		{
			return;
		}
		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		rcText.top += m_rcTextPadding.top;
		rcText.bottom -= m_rcTextPadding.bottom;

		if (m_iCurSel >= 0) {
			CListLabelElementUI* pControl = static_cast<CListLabelElementUI*>(m_items[m_iCurSel]);

			//if (pControl)
			//{
			//	QkString strIcon = pControl->GetIcon();
			//	SIZE szIcon = pControl->GetIconSize();
			//	if (strIcon != _T(""))
			//	{
			//		IListOwnerUI* pOwner = pControl->GetOwner();
			//		if (pOwner == NULL) return;
			//		TListInfoUI* pInfo = pOwner->GetListInfo();
			//		if (pInfo == NULL) return;

			//		int left, right;
			//		left = 0;
			//		right = left + szIcon.cx;
			//		TDrawInfo   diIcon;
			//		QkString pStrImage;
			//		int bottom, top = (rcText.bottom - rcText.top - szIcon.cy) / 2;
			//		bottom = top + szIcon.cy;
			//		pStrImage.SmallFormat(_T("file='%s' dest='%d,%d,%d,%d'"), strIcon.GetData(), left, top, right, bottom);
			//		diIcon.Clear();
			//		diIcon.sDrawString = pStrImage;
			//		CRenderEngine::DrawImage(hDC, _manager, rcText, m_rcPaint, diIcon);
			//		m_uTextStyle |= pInfo->uTextStyle;
			//	}

			//	rcText.left += szIcon.cx;
			//}


		}

		if (m_dwTextColor == 0) m_dwTextColor = _manager->GetDefaultFontColor();
		if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = _manager->GetDefaultDisabledColor();

		if (m_sText.IsEmpty()) return;




		if (IsEnabled()) {
			CRenderEngine::DrawPlainText(hDC, _manager, rcText, m_sText, m_dwTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
		else {
			CRenderEngine::DrawPlainText(hDC, _manager, rcText, m_sText, m_dwDisabledTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);

		}
	}


	int CComboEditUI::GetCurSel() const
	{
		return -1;
	}

}// end duilib
