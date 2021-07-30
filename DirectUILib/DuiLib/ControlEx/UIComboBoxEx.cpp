#include "UIComboBoxEx.h"

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))
#define Arrw 20;

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CComboBoxExUI)

	class CComboEditWnd : public CWindowWnd
	{
	public:
		CComboEditWnd();
		void Init(CComboBoxExUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditUpdate(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/);
	protected:
		CComboBoxExUI* m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
		int l1;
		int l2;
	};


	CComboEditWnd::CComboEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
	}
	/*****************************************下拉列表*****************************************************************/
	class CComboDownWnd : public CWindowWnd,public INotifyUI
	{
	public:
		void Init(CComboBoxExUI* pOwner);
		LPCTSTR GetWindowClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

#if(_WIN32_WINNT >= 0x0501)
		virtual UINT GetClassStyle() const;
#endif

	public:
		CPaintManagerUI m_pm;
		CComboBoxExUI* m_pOwner;
		CVerticalLayoutUI* m_pLayout;
		int m_iOldSel;
		void Notify(TNotifyUI& msg);
	};

	void CComboEditWnd::Init(CComboBoxExUI* pOwner)
	{
		m_pOwner = pOwner;
		RECT rcPos = CalPos();
		UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;//
		Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
		HFONT hFont=NULL;
		int index=m_pOwner->GetCurSel();
		if(index>-1)
		{

		}
		int iFontIndex=m_pOwner->GetListInfo()->nFont;
		if (iFontIndex!=-1)
			hFont=m_pOwner->GetManager()->GetFont(iFontIndex);
		if (hFont==NULL)
			hFont=m_pOwner->GetManager()->GetDefaultFontInfo()->hFont;

		SetWindowFont(m_hWnd, hFont, TRUE);
		Edit_LimitText(m_hWnd,30);

		Edit_SetText(m_hWnd, m_pOwner->GetText());

		Edit_SetModify(m_hWnd, false);

		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
		Edit_Enable(m_hWnd,true);
		Edit_SetReadOnly(m_hWnd,false);
		//Styls
		LONG styleValue =::GetWindowLong(m_hWnd, GWL_STYLE);
		styleValue =styleValue |ES_LEFT ;
		::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
		//styleValue |= pOwner->GetWindowStyls();
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);

		::SetFocus(m_hWnd);
		//::SetCapture(m_hWnd);
		//Edit_SetSel(m_hWnd, 0,-1);
		m_bInit = true;    
		//::SetTimer(m_hWnd,1010,100,NULL);
		//TRACKMOUSEEVENT   tme;     
		// tme.cbSize = sizeof(tme);     
		// tme.dwFlags = TME_LEAVE | TME_NONCLIENT; //注册非客户区离开    
		// tme.hwndTrack = m_hWnd;     
		// tme.dwHoverTime = HOVER_DEFAULT; //只对HOVER有效  
		// ::TrackMouseEvent(&tme); 
	}

	RECT CComboEditWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		RECT rcInset = m_pOwner->GetTextPadding();
		rcPos.left += rcInset.left;
		rcPos.top += rcInset.top;
		rcPos.right -= rcInset.right+Arrw;
		rcPos.bottom -= rcInset.bottom;

		LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetListInfo()->nFont)->tm.tmHeight;
		if( lEditHeight < rcPos.GetHeight() ) 
		{
			rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
			rcPos.bottom = rcPos.top + lEditHeight;
		}
		return rcPos;
	}

	LPCTSTR CComboEditWnd::GetWindowClassName() const
	{
		return _T("EditWnd");
	}

	LPCTSTR CComboEditWnd::GetSuperClassName() const
	{
		return WC_EDIT;
	}

	void CComboEditWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		m_pOwner->Invalidate();
		// Clear reference and die
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		m_pOwner->p_EditWnd = NULL;
		delete this;
	}

	LRESULT CComboEditWnd::OnEditUpdate(UINT uMsg/**/, WPARAM wParam/**/, LPARAM lParam/**/, BOOL& bHandled/**/)
	{
		//	return 1;
		int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
#ifndef  _UNICODE
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		LPTSTR pstr2 = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
#else
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(wchar_t)));
		LPTSTR pstr2 = static_cast<LPTSTR>(_alloca(cchLen * sizeof(wchar_t)));
#endif

		memset(pstr2,0,cchLen);
		ASSERT(pstr);
		if( pstr == NULL ) return 0;
		::GetWindowText(m_hWnd, pstr, cchLen);


		int count=this->m_pOwner->GetCount();
		bool findOk=false;
		for(int i=0;i<count;i++)
		{
			CDuiString bb(this->m_pOwner->GetItemAt(i)->GetText());
			if(bb.Find(pstr)>=0)
			{
				this->m_pOwner->GetItemAt(i)->SetVisible(true);
				if(!findOk)
					this->m_pOwner->SelectItem(i);
				findOk=true;
			}else
			{
				this->m_pOwner->GetItemAt(i)->SetVisible(false);
			}
		}

		for(int i=0;i<count&&!findOk;i++)
		{
			this->m_pOwner->GetItemAt(i)->SetVisible(true);
		}
		if(count>0&&!findOk||cchLen<=1)
		{
			this->m_pOwner->SelectItem(0);
		}
		if(count>0)
		{
			// Position the popup window in absolute space
			SIZE szDrop = m_pOwner->GetDropBoxSize();
			RECT rcOwner = m_pOwner->m_pWindow->m_pOwner->GetPos();
			RECT rc = rcOwner;
			rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
			rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
			if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

			SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
			int cyFixed = 0;
			for( int it = 0; it < this->m_pOwner->GetCount(); it++ ) 
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
				if( !pControl->IsVisible() ) 
					continue;
				SIZE sz = pControl->EstimateSize(szAvailable);
				cyFixed += sz.cy;
			}
			cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
			rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);
			m_pOwner->m_pWindow->ResizeClient(rc.right-rc.left,rc.bottom-rc.top);
			::MapWindowRect(m_pOwner->m_pWindow->m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
		}
		return 1;
	}
	LRESULT CComboEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;

		if(uMsg ==WM_LBUTTONDOWN)
		{
			if(wParam==MK_LBUTTON)
			{
				//if(wParam=MK_LBUTTON)
				{
					POINT pt;
					pt.x = GET_X_LPARAM(lParam); 
					pt.y = GET_Y_LPARAM(lParam); 
					DWORD dwRet = (DWORD)::SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, pt.y));
					int     nStartChar=(int)(short)LOWORD(dwRet);
					int  nEndChar=(int)(short)LOWORD(dwRet);
					l1=nEndChar;

					//设置光标位置
					::SendMessage(m_hWnd,EM_SETSEL,nStartChar,nEndChar);
				}
			}
		}
		if(uMsg ==WM_MOUSEMOVE )
		{
			/*if(wParam==MK_LBUTTON)
			{
			POINT pt;
			pt.x = GET_X_LPARAM(lParam); 
			pt.y = GET_Y_LPARAM(lParam); 
			DWORD dwRet = (DWORD)::SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, pt.y));
			int     nStartChar=(int)(short)LOWORD(dwRet);
			int  nEndChar=(int)(short)LOWORD(dwRet);
			l2=nEndChar;
			if(l2>l1)
			::SendMessage(m_hWnd,EM_SETSEL,l1,l2);
			else
			if(l2<l1)
			::SendMessage(m_hWnd,EM_SETSEL,l2,l1);
			}*/
		}
		if( uMsg == WM_KILLFOCUS ) 
		{
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
		else 
			if( uMsg == OCM_COMMAND ) 
			{
				/*************字符已经改变了****************/
				if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) 
					lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
				else 
					if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE )
					{
						OnEditUpdate(uMsg/**/,wParam/**/, lParam/**/, bHandled/**/);
					}
			}else 
				if( uMsg == WM_KEYDOWN)
				{
					switch( wParam )
					{
					case VK_RETURN:
						::PostMessage(m_hWnd,WM_CLOSE,0,0);
						return 1;
						break;
					case VK_UP:
					case VK_DOWN:
						TEventUI event;
						event.Type = UIEVENT_KEYDOWN;
						event.chKey = (TCHAR)wParam;
						m_pOwner->DoEvent(event);
						if(wParam!=17)
							m_pOwner->m_pWindow->EnsureVisible(m_pOwner->GetCurSel());
						break;
					}
					bHandled = FALSE;
				}else 
					if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC )
					{
						::SetBkMode((HDC)wParam, TRANSPARENT);
						DWORD dwTextColor = m_pOwner->GetListInfo()->dwTextColor;
						::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
						if( m_hBkBrush == NULL ) 
						{
							DWORD clrColor = RGB(255,255,255);//m_pOwner->GetBkColor();
							m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
						}
						return (LRESULT)m_hBkBrush;
					}else{
						bHandled = FALSE;
					}
				if( !bHandled ) 
					return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
				return lRes;
	}

	LRESULT CComboEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::OutputDebugString(_T("WM_KILLFOCUS\n"));
		if(this->m_pOwner->m_pWindow!=NULL&&(HWND) wParam!=m_pOwner->m_pWindow->GetHWND())
		{
			::SendMessage(this->m_pOwner->m_pWindow->GetHWND(),WM_CLOSE,0,0);
			this->m_pOwner->m_pWindow=NULL;
		}
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		PostMessage(WM_CLOSE);
		this->m_pOwner->p_EditWnd=NULL;
		return lRes;
	}

	LRESULT CComboEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if( !m_bInit ) 
			return 0;
		if( m_pOwner == NULL ) 
			return 0;
		return 0;
	}







	void CComboDownWnd::Init(CComboBoxExUI* pOwner)
	{
		m_pOwner = pOwner;
		m_pLayout = NULL;
		m_iOldSel = m_pOwner->GetCurSel();

		// Position the popup window in absolute space
		SIZE szDrop = m_pOwner->GetDropBoxSize();
		RECT rcOwner = pOwner->GetPos();
		RECT rc = rcOwner;
		rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
		rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
		if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		int cyFixed = 0;
		for( int it = 0; it < pOwner->GetCount(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(it));
			if( !pControl->IsVisible() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;
		}
		cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
		rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

		::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		if( rc.bottom > rcWork.bottom ) {
			rc.left = rcOwner.left;
			rc.right = rcOwner.right;
			if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
			rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
			rc.bottom = rcOwner.top;
			::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
		}

		Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
		// HACK: Don't deselect the parent's caption
		HWND hWndParent = m_hWnd;
		while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
		::ShowWindow(m_hWnd, SW_SHOW);
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
	}

	LPCTSTR CComboDownWnd::GetWindowClassName() const
	{
		return _T("ComboWnd");
	}

	void CComboDownWnd::OnFinalMessage(HWND hWnd)
	{
		m_pOwner->m_pWindow = NULL;
		m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
		m_pOwner->Invalidate();
		delete this;
	}
	//bool   xxxxx=false;
	LRESULT CComboDownWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if( uMsg == WM_CREATE )
		{
			m_pm.Init(m_hWnd);
			// The trick is to add the items to the new container. Their owner gets
			// reassigned by this operation - which is why it is important to reassign
			// the items back to the righfull owner/manager when the window closes.
			m_pLayout = new CVerticalLayoutUI;
			//m_pm.UseParentResource(m_pOwner->GetManager());
			m_pLayout->SetManager(&m_pm, NULL, true);
			LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
			if( pDefaultAttributes )
			{
				m_pLayout->ApplyAttributeList(pDefaultAttributes);
			}
			m_pLayout->SetInset(CDuiRect(1, 1, 1, 1));
			m_pLayout->SetBkColor(0xFFFFFFFF);
			m_pLayout->SetBorderColor(0xFFC6C7D2);
			m_pLayout->SetBorderSize(1);
			m_pLayout->SetAutoDestroy(false);
			m_pLayout->EnableScrollBar();
			m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());
			for( int i = 0; i < m_pOwner->GetCount(); i++ )
			{
				m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
			}
			m_pm.AttachDialog(m_pLayout);
			m_pm.AddNotifier(this);  
			return 0;
		}
		else if( uMsg == WM_CLOSE ) 
		{
			m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
			m_pOwner->SetPos(m_pOwner->GetPos());
			m_pOwner->SetFocus();
		}
		else if( uMsg == WM_LBUTTONUP )
		{
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_pm.GetPaintWindow(), &pt);
			CControlUI* pControl = m_pm.FindControl(pt);
			//xxxxx=true;
			if( pControl && _tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0 ) 
				PostMessage(WM_KILLFOCUS);
		}
		else if( uMsg == WM_KEYDOWN )
		{
			switch( wParam )
			{
			case VK_ESCAPE:
				m_pOwner->SelectItem(m_iOldSel, true);
				EnsureVisible(m_iOldSel);
				// FALL THROUGH...
			case VK_RETURN:
				PostMessage(WM_KILLFOCUS);
				break;
			default:
				TEventUI event;
				event.Type = UIEVENT_KEYDOWN;
				event.chKey = (TCHAR)wParam;
				m_pOwner->DoEvent(event);
				if(wParam!=17)
					EnsureVisible(m_pOwner->GetCurSel());
				return 0;
			}
		}
		else if( uMsg == WM_MOUSEWHEEL ) 
		{
			int zDelta = (int) (short) HIWORD(wParam);
			TEventUI event = { 0 };
			event.Type = UIEVENT_SCROLLWHEEL;
			event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
			event.lParam = lParam;
			event.dwTimestamp = ::GetTickCount();
			m_pOwner->DoEvent(event);
			EnsureVisible(m_pOwner->GetCurSel());
			return 0;
		}
		else if( uMsg == WM_KILLFOCUS ) 
		{
			::OutputDebugString(_T("XWM_KILLFOCUS\n"));
			if(m_pOwner->p_EditWnd!=NULL)
			{
				if( m_hWnd != (HWND) wParam&&(HWND) wParam!=m_pOwner->p_EditWnd->GetHWND() )
				{
					m_pOwner->m_pWindow=NULL;
					PostMessage(WM_CLOSE);
				}
			}else
			{
				if( m_hWnd != (HWND) wParam)
				{
					m_pOwner->m_pWindow=NULL;
					PostMessage(WM_CLOSE);
				}
			}
		}else if(uMsg ==WM_SETFOCUS)
		{
			return 0;
		}

		LRESULT lRes = 0;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) 
			return lRes;
		return 
			CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	void CComboDownWnd::EnsureVisible(int iIndex)
	{
		if( m_pOwner->GetCurSel() < 0 ) 
			return;
		m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
		RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos(); //获取当前item的位置
		RECT rcList = m_pLayout->GetPos();                    //获取列表的位置
		CScrollBarUI* pHorizontalScrollBar = m_pLayout->GetHorizontalScrollBar();
		if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) 
			rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
		int iPos = m_pLayout->GetScrollPos().cy;
		if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) 
			return;
		int dx = 0;
		if( rcItem.top < rcList.top ) 
			dx = rcItem.top - rcList.top;
		if( rcItem.bottom > rcList.bottom ) 
			dx = rcItem.bottom - rcList.bottom;
		if(dx==1)
		{
			int i=0;
			i++;
		}
		Scroll(0, dx);
	}

	void CComboDownWnd::Scroll(int dx, int dy)
	{
		if( dx == 0 && dy == 0 ) 
			return;
		SIZE sz = m_pLayout->GetScrollPos();
		sz.cx += dx;
		sz.cy += dy;
		m_pLayout->SetScrollPos(sz);
		char pp[100]={0};
		memset(pp,0,sizeof(char)*100);
		sprintf(pp,"y:%d,%d",sz.cy + dy,dy);
		//::OutputDebugString(pp);
		//::OutputDebugString("CLose\n");
	}

#if(_WIN32_WINNT >= 0x0501)
	UINT CComboDownWnd::GetClassStyle() const
	{
		return __super::GetClassStyle() | CS_DROPSHADOW;
	}
#endif



	/******************Combox*************************/
	CComboBoxExUI::CComboBoxExUI(): m_pWindow(NULL), m_iCurSel(-1), m_uButtonState(0)
	{
		p_EditWnd=NULL;
		m_szDropBox = {0, 150}; // Size
		::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

		m_ListInfo.nColumns = 0;
		m_ListInfo.nFont = -1;
		m_ListInfo.uTextStyle = DT_VCENTER;
		m_ListInfo.dwTextColor = 0xFF000000;
		m_ListInfo.dwBkColor = 0;
		m_ListInfo.bAlternateBk = false;
		m_ListInfo.dwSelectedTextColor = 0xFF000000;
		m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
		m_ListInfo.dwHotTextColor = 0xFF000000;
		m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
		m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
		m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
		m_ListInfo.dwLineColor = 0;
		m_ListInfo.bShowHtml = false;
		m_ListInfo.bMultiExpandable = false;
		::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
		::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
	}

	LPCTSTR CComboBoxExUI::GetClass() const
	{
		return _T("ComboBoxExUI");
	}

	LPVOID CComboBoxExUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_COMBO) == 0 ) 
			return static_cast<CComboBoxExUI*>(this);
		if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) 
			return static_cast<IListOwnerUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CComboBoxExUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP;
	}

	void CComboBoxExUI::DoInit()
	{
	}

	int CComboBoxExUI::GetCurSel() const
	{
		return m_iCurSel;
	}

	bool CComboBoxExUI::SelectItem(int iIndex, bool bTakeFocus)
	{
		/* if( m_pWindow != NULL ) 
		m_pWindow->Close();*/
		if(!IsEnabled())
			return false;
		if( iIndex == m_iCurSel ) 
			return true;
		int iOldSel = m_iCurSel;
		if( m_iCurSel >= 0 ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			if( !pControl ) 
				return false;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) 
				pListItem->Select(false);
			//SignalItemChange(iIndex,this);
			m_iCurSel = -1;
		}
		if( iIndex < 0 ) 
			return false;
		if( m_items.GetSize() == 0 ) 
			return false;
		if( iIndex >= m_items.GetSize() ) iIndex = m_items.GetSize() - 1;
		CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);
		/*if( !pControl || !pControl->IsVisible() || !pControl->IsEnabled() ) 
		return false;*/
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) 
			return false;
		m_iCurSel = iIndex;
		if( m_pWindow != NULL || bTakeFocus ) 
		{
			//pControl->SetFocus();
		}
		pListItem->Select(true);
		if( m_pManager != NULL ) 
			m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);
		Invalidate();

		return true;
	}

	bool CComboBoxExUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		if(!IsEnabled())
			return false;
		int iOrginIndex = GetItemIndex(pControl);
		if( iOrginIndex == -1 ) return false;
		if( iOrginIndex == iIndex ) return true;

		IListItemUI* pSelectedListItem = NULL;
		if( m_iCurSel >= 0 ) pSelectedListItem = 
			static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(_T("ListItem")));
		if( !CContainerUI::SetItemIndex(pControl, iIndex) ) return false;
		int iMinIndex = min(iOrginIndex, iIndex);
		int iMaxIndex = max(iOrginIndex, iIndex);
		for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
		return true;
	}

	bool CComboBoxExUI::Add(CControlUI* pControl)
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) 
		{
			pListItem->SetOwner(this);
			pListItem->SetIndex(m_items.GetSize());
		}
		return CContainerUI::Add(pControl);
	}

	bool CComboBoxExUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if (!CContainerUI::AddAt(pControl, iIndex)) return false;

		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
		}

		for(int i = iIndex + 1; i < GetCount(); ++i) {
			CControlUI* p = GetItemAt(i);
			pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( m_iCurSel >= iIndex ) m_iCurSel += 1;
		return true;
	}

	bool CComboBoxExUI::Remove(CControlUI* pControl)
	{
		int iIndex = GetItemIndex(pControl);
		if (iIndex == -1) return false;

		if (!CContainerUI::RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < GetCount(); ++i) {
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}

		if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
		return true;
	}

	bool CComboBoxExUI::RemoveAt(int iIndex)
	{
		if (!CContainerUI::RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < GetCount(); ++i) {
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->SetIndex(i);
		}

		if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
		return true;
	}

	void CComboBoxExUI::RemoveAll()
	{
		m_iCurSel = -1;
		CContainerUI::RemoveAll();
	}

	void CComboBoxExUI::DoEvent(TEventUI& ev)
	{
		if( !IsMouseEnabled() && ev.Type > UIEVENT__MOUSEBEGIN && ev.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) 
				m_pParent->DoEvent(ev);
			else 
				CContainerUI::DoEvent(ev);
			return;
		}

		if( ev.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( ev.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}
		if( ev.Type == UIEVENT_BUTTONDOWN )
		{

			if( IsEnabled() ) 
			{
				POINT pt;
				pt.x = GET_X_LPARAM(ev.lParam); 
				pt.y = GET_Y_LPARAM(ev.lParam); 
				RECT rt=this->GetPos();
				rt.right-=20;
				if(::PtInRect(&rt,pt))
				{
					if( p_EditWnd ) 
					{
						int nSize = GetWindowTextLength(*p_EditWnd);
						if( nSize == 0 )
							nSize = 1;

						Edit_SetSel(*p_EditWnd, 0, nSize);
						return;
					}
					Activate();
					m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
					p_EditWnd = new CComboEditWnd();
					ASSERT(p_EditWnd);
					p_EditWnd->Init(this);
					Invalidate();
					{
						int nSize = GetWindowTextLength(*p_EditWnd);
						if( nSize == 0 )
							nSize = 1;

						Edit_SetSel(*p_EditWnd, 0, nSize);
					}
					return;
				}
				rt.right+=20;
				rt.left=rt.right- 20;
				if(::PtInRect(&rt,pt))
				{
					Activate();
					m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				}
			}
			return;
		}
		if( ev.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~ UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if( ev.Type == UIEVENT_MOUSEMOVE )
		{
			return;
		}
		if( ev.Type == UIEVENT_KEYDOWN )
		{
			switch( ev.chKey ) {
			case VK_F4:
				Activate();
				return;
			case VK_UP:
				SelectItem(FindSelectable(m_iCurSel - 1, false));
				return;
			case VK_DOWN:
				SelectItem(FindSelectable(m_iCurSel + 1, true));
				return;
			case VK_PRIOR:
				SelectItem(FindSelectable(m_iCurSel - 1, false));
				return;
			case VK_NEXT:
				SelectItem(FindSelectable(m_iCurSel + 1, true));
				return;
			case VK_HOME:
				SelectItem(FindSelectable(0, false));
				return;
			case VK_END:
				SelectItem(FindSelectable(GetCount() - 1, true));
				return;
			}
		}
		if( ev.Type == UIEVENT_SCROLLWHEEL )
		{
			bool bDownward = LOWORD(ev.wParam) == SB_LINEDOWN;
			SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
			return;
		}
		if( ev.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( ev.Type == UIEVENT_MOUSEENTER )
		{
			if( ::PtInRect(&m_rcItem, ev.ptMouse ) ) {
				if( (m_uButtonState & UISTATE_HOT) == 0  )
					m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( ev.Type == UIEVENT_MOUSELEAVE )
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CControlUI::DoEvent(ev);
	}

	SIZE CComboBoxExUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return {m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 12};//Size;
		return CControlUI::EstimateSize(szAvailable);
	}

	bool CComboBoxExUI::Activate()
	{
		if( !CControlUI::Activate() ) 
			return false;
		if( m_pWindow ) 
			return true;
		m_pWindow = new CComboDownWnd();
		ASSERT(m_pWindow);
		m_pWindow->Init(this);
		if( m_pManager != NULL) 
			m_pManager->SendNotify(this, DUI_MSGTYPE_DROPDOWN);
		Invalidate();
		return true;
	}

	CDuiString CComboBoxExUI::GetText() const
	{
		if( m_iCurSel < 0 ) return _T("");
		CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
		return pControl->GetText();
	}

	void CComboBoxExUI::SetEnabled(bool bEnable)
	{
		CContainerUI::SetEnabled(bEnable);
		if( !IsEnabled() ) m_uButtonState = 0;
		if( m_iCurSel >= 0 ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			if( pControl != NULL ) {
				pControl->SetEnabled(bEnable);
			}
		}
	}

	CDuiString CComboBoxExUI::GetDropBoxAttributeList()
	{
		return m_sDropBoxAttributes;
	}

	void CComboBoxExUI::SetDropBoxAttributeList(LPCTSTR pstrList)
	{
		m_sDropBoxAttributes = pstrList;
	}

	SIZE CComboBoxExUI::GetDropBoxSize() const
	{
		return m_szDropBox;
	}

	void CComboBoxExUI::SetDropBoxSize(SIZE szDropBox)
	{
		m_szDropBox = szDropBox;
	}

	RECT CComboBoxExUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CComboBoxExUI::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
		Invalidate();
	}

	LPCTSTR CComboBoxExUI::GetNormalImage() const
	{
		return m_sNormalImage;
	}

	void CComboBoxExUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CComboBoxExUI::GetHotImage() const
	{
		return m_sHotImage;
	}

	void CComboBoxExUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CComboBoxExUI::GetPushedImage() const
	{
		return m_sPushedImage;
	}

	void CComboBoxExUI::SetPushedImage(LPCTSTR pStrImage)
	{
		m_sPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CComboBoxExUI::GetFocusedImage() const
	{
		return m_sFocusedImage;
	}

	void CComboBoxExUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CComboBoxExUI::GetDisabledImage() const
	{
		return m_sDisabledImage;
	}

	void CComboBoxExUI::SetDisabledImage(LPCTSTR pStrImage)
	{
		m_sDisabledImage = pStrImage;
		Invalidate();
	}

	TListInfoUI* CComboBoxExUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	void CComboBoxExUI::SetItemFont(int index)
	{
		m_ListInfo.nFont = index;
		Invalidate();
	}

	void CComboBoxExUI::SetItemTextStyle(UINT uStyle)
	{
		m_ListInfo.uTextStyle = uStyle;
		Invalidate();
	}

	RECT CComboBoxExUI::GetItemTextPadding() const
	{
		return m_ListInfo.rcTextPadding;
	}

	void CComboBoxExUI::SetItemTextPadding(RECT rc)
	{
		m_ListInfo.rcTextPadding = rc;
		Invalidate();
	}

	void CComboBoxExUI::SetItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwTextColor = dwTextColor;
		Invalidate();
	}

	void CComboBoxExUI::SetItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwBkColor = dwBkColor;
	}

	void CComboBoxExUI::SetItemBkImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sBkImage = pStrImage;
	}

	DWORD CComboBoxExUI::GetItemTextColor() const
	{
		return m_ListInfo.dwTextColor;
	}

	DWORD CComboBoxExUI::GetItemBkColor() const
	{
		return m_ListInfo.dwBkColor;
	}

	LPCTSTR CComboBoxExUI::GetItemBkImage() const
	{
		return m_ListInfo.sBkImage;
	}

	bool CComboBoxExUI::IsAlternateBk() const
	{
		return m_ListInfo.bAlternateBk;
	}

	void CComboBoxExUI::SetAlternateBk(bool bAlternateBk)
	{
		m_ListInfo.bAlternateBk = bAlternateBk;
	}

	void CComboBoxExUI::SetSelectedItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwSelectedTextColor = dwTextColor;
	}

	void CComboBoxExUI::SetSelectedItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwSelectedBkColor = dwBkColor;
	}

	void CComboBoxExUI::SetSelectedItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sSelectedImage = pStrImage;
	}

	DWORD CComboBoxExUI::GetSelectedItemTextColor() const
	{
		return m_ListInfo.dwSelectedTextColor;
	}

	DWORD CComboBoxExUI::GetSelectedItemBkColor() const
	{
		return m_ListInfo.dwSelectedBkColor;
	}

	LPCTSTR CComboBoxExUI::GetSelectedItemImage() const
	{
		return m_ListInfo.sSelectedImage;
	}

	void CComboBoxExUI::SetHotItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwHotTextColor = dwTextColor;
	}

	void CComboBoxExUI::SetHotItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwHotBkColor = dwBkColor;
	}

	void CComboBoxExUI::SetHotItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sHotImage = pStrImage;
	}

	DWORD CComboBoxExUI::GetHotItemTextColor() const
	{
		return m_ListInfo.dwHotTextColor;
	}
	DWORD CComboBoxExUI::GetHotItemBkColor() const
	{
		return m_ListInfo.dwHotBkColor;
	}

	LPCTSTR CComboBoxExUI::GetHotItemImage() const
	{
		return m_ListInfo.sHotImage;
	}

	void CComboBoxExUI::SetDisabledItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwDisabledTextColor = dwTextColor;
	}

	void CComboBoxExUI::SetDisabledItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwDisabledBkColor = dwBkColor;
	}

	void CComboBoxExUI::SetDisabledItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sDisabledImage = pStrImage;
	}

	DWORD CComboBoxExUI::GetDisabledItemTextColor() const
	{
		return m_ListInfo.dwDisabledTextColor;
	}

	DWORD CComboBoxExUI::GetDisabledItemBkColor() const
	{
		return m_ListInfo.dwDisabledBkColor;
	}

	LPCTSTR CComboBoxExUI::GetDisabledItemImage() const
	{
		return m_ListInfo.sDisabledImage;
	}

	DWORD CComboBoxExUI::GetItemLineColor() const
	{
		return m_ListInfo.dwLineColor;
	}

	void CComboBoxExUI::SetItemLineColor(DWORD dwLineColor)
	{
		m_ListInfo.dwLineColor = dwLineColor;
	}

	bool CComboBoxExUI::IsItemShowHtml()
	{
		return m_ListInfo.bShowHtml;
	}

	void CComboBoxExUI::SetItemShowHtml(bool bShowHtml)
	{
		if( m_ListInfo.bShowHtml == bShowHtml ) return;

		m_ListInfo.bShowHtml = bShowHtml;
		Invalidate();
	}

	void CComboBoxExUI::SetPos(RECT rc)
	{
		// Put all elements out of sight
		RECT rcNull = { 0 };
		for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<CControlUI*>(m_items[i])->SetPos(rcNull);
		// Position this control
		CControlUI::SetPos(rc);
	}
	void CComboDownWnd::Notify(TNotifyUI& msg)  
	{  
		if(msg.sType == _T("windowinit"))  
		{  
			EnsureVisible(m_iOldSel);  
		}  
	}
	void CComboBoxExUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetTextPadding(rcTextPadding);
		}
		else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("dropbox")) == 0 ) SetDropBoxAttributeList(pstrValue);
		else if( _tcscmp(pstrName, _T("dropboxsize")) == 0)
		{
			SIZE szDropBoxSize = { 0 };
			LPTSTR pstr = NULL;
			szDropBoxSize.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szDropBoxSize.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			SetDropBoxSize(szDropBoxSize);
		}
		else if( _tcscmp(pstrName, _T("itemfont")) == 0 ) m_ListInfo.nFont = _ttoi(pstrValue);
		else if( _tcscmp(pstrName, _T("itemalign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_ListInfo.uTextStyle |= DT_RIGHT;
			}
		}
		if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetItemTextPadding(rcTextPadding);
		}
		else if( _tcscmp(pstrName, _T("itemtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itembkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itembkimage")) == 0 ) SetItemBkImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemaltbk")) == 0 ) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("itemselectedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemselectedbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemselectedimage")) == 0 ) SetSelectedItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemhottextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemhotbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemhotimage")) == 0 ) SetHotItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemdisabledimage")) == 0 ) SetDisabledItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemlinecolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemLineColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CComboBoxExUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		CControlUI::DoPaint(hDC, rcPaint, 0);
	}

	void CComboBoxExUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if( !m_sPushedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sPushedImage) ) m_sPushedImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else return;
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
			else return;
		}
	}

	void CComboBoxExUI::PaintText(HDC hDC)
	{
		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		rcText.top += m_rcTextPadding.top;
		rcText.bottom -= m_rcTextPadding.bottom;

		if( m_iCurSel >= 0 ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pElement != NULL ) {
				pElement->DrawItemText(hDC, rcText);
			}
			else {
				RECT rcOldPos = pControl->GetPos();
				pControl->SetPos(rcText);
				pControl->DoPaint(hDC, rcText, 0);
				pControl->SetPos(rcOldPos);
			}
		}
	}


}