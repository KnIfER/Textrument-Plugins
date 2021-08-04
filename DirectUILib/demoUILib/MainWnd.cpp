#include "StdAfx.h"
#include "MainWnd.h"

#define IDM_SHOW_LOG		WM_USER+2200
#define IDM_EXIT_APP		WM_USER+2300

CMainWnd::CMainWnd(void)
{
} 


CMainWnd::~CMainWnd(void)
{
}


CDuiString CMainWnd::GetSkinFile()
{
	try
	{
		return CDuiString(_T("MainSkin.xml"));
	}
	catch (...)
	{
		throw "CMainWnd::GetSkinFile";
	}
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	try
	{
		return _T("UiLib_Demos");
	}
	catch (...)
	{
		throw "CMainWnd::GetWindowClassName";
	}
}

LRESULT CMainWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	try
	{
		if(lParam == WM_RBUTTONUP)
		{
			::SetForegroundWindow(this->m_hWnd);

			hMenu = CreatePopupMenu();
			AppendMenu(hMenu,MF_STRING,IDM_SHOW_LOG,_T("显示日志"));
			AppendMenu(hMenu,MF_STRING,IDM_EXIT_APP,_T("退出程序"));

			POINT point;
			point.x=LOWORD(lParam);
			point.y=HIWORD(lParam);
			GetCursorPos(&point);
			TrackPopupMenu(hMenu,TPM_RIGHTALIGN,point.x,point.y,0,m_hWnd,NULL);

			::PostMessage(m_hWnd,WM_NULL,0,0);

			return TRUE;
		}
		else if(wParam == IDM_SHOW_LOG || lParam == WM_LBUTTONDBLCLK)
		{
			this->ShowWindow();
			return TRUE;
		}
		else if(wParam == IDM_EXIT_APP)
		{
			Close(IDOK);
			return TRUE;
		}
		return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
	}
	catch (...)
	{
		throw "CMainWnd::HandleMessage";
	}
}

void CMainWnd::InitWindow()
{
	try
	{
		//m_pm.AttachDialog(pRoot);
		//m_pm.AddNotifier(this);
		pAnimWnd	= static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("AnimWnd")));

		mpCloseBtn					= static_cast<CButtonUI*>(m_pm.FindControl(_T("SysCloseBtn")));
		mpMaxBtn					= static_cast<CButtonUI*>(m_pm.FindControl(_T("SysMaxBtn")));
		mpRestoreBtn				= static_cast<CButtonUI*>(m_pm.FindControl(_T("SysRestoreBtn")));
		mpMinBtn					= static_cast<CButtonUI*>(m_pm.FindControl(_T("SysMinBtn")));

		nid.cbSize				= (DWORD)sizeof(NOTIFYICONDATA);
		nid.hWnd				= m_hWnd;
		nid.uID					= IDI_UILIB_DEMOS;
		nid.uFlags				= NIF_ICON|NIF_MESSAGE|NIF_TIP ;
		nid.uCallbackMessage	= WM_USER + 1;
		nid.hIcon				= LoadIcon(m_pm.GetInstance(),MAKEINTRESOURCE(IDI_UILIB_DEMOS));

#ifndef _UNICODE
		strcpy(nid.szTip,"双击主显示界面");//信息提示条
		Shell_NotifyIconA(NIM_ADD,&nid);//在托盘区添加图标
#else
		wcscpy_s(nid.szTip,L"双击主显示界面");//信息提示条
		Shell_NotifyIcon(NIM_ADD,&nid);//在托盘区添加图标
#endif
	}
	catch (...)
	{
		throw "CMainWnd::Init";
	}
}

void CMainWnd::Notify( TNotifyUI& msg )
{
	try
	{
		if(msg.pSender->GetName() == _T("EffectsDemo"))
		{

			//TCHAR buffer[100]={0};
			//wsprintf(buffer,TEXT("position=%d"), 1);
			//::MessageBox(NULL, buffer, TEXT(""), MB_OK);


			//msg.pSender->SetAnimEffects(true);
			//msg.pSender->SetAttribute(_T("fx_adv"),_T("anim='left2right' offset='180'"));
			//msg.pSender->TriggerEffects();

			pAnimWnd->SetAnimEffects(true);
			msg.pSender->SetTag(msg.pSender->GetTag()+1);

			if(msg.pSender->GetTag() == 1)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("anim='left2right' offset='180'"));
			else if(msg.pSender->GetTag() == 2)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("anim='right2left' offset='180'"));
			else if(msg.pSender->GetTag() == 3)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("anim='top2bottom' offset='180'"));
			else if(msg.pSender->GetTag() == 4)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("anim='bottom2top' offset='180'"));
			else if(msg.pSender->GetTag() == 5)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("anim='zoom+' offset='180'"));
			else if(msg.pSender->GetTag() == 6)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("anim='zoom-' offset='180'"));
			else if(msg.pSender->GetTag() == 7)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("offsetx='180' rotation='0.3'"));
			else if(msg.pSender->GetTag() == 8)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("offsetx='180' rotation='-0.3'"));
			else if(msg.pSender->GetTag() == 9)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("offsety='180' rotation='0.3'"));
			else if(msg.pSender->GetTag() == 10)
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("offsety='180' rotation='-0.3'"));
			else
			{
				msg.pSender->SetTag(1);
				pAnimWnd->SetAttribute(_T("fx_adv"),_T("anim='left2right' offset='80'"));
			}
			pAnimWnd->TriggerEffects();
		}

		//WindowImplBase::Notify(msg); //close min max restore

		if( msg.sType == _T("click") )//鼠标点击消息
		{
			if( msg.pSender == mpCloseBtn )
			{
				Close(IDOK);
			}
			else if( msg.pSender == mpMinBtn )
			{ 
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);return;
			}
			else if( msg.pSender == mpMaxBtn)
			{
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);return;
			}
			else if( msg.pSender == mpRestoreBtn)
			{
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return;
			}
		}
	}
	catch (...)
	{
		throw "CMainWnd::Notify";
	}
}

//************************************
// Method:    OnFinalMessage
// FullName:  CMainWnd::OnFinalMessage
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HWND hWnd
// Note:	  
//************************************
void CMainWnd::OnFinalMessage( HWND hWnd )
{
	try
	{
		::Shell_NotifyIcon(NIM_DELETE,&nid);

		CWindowWnd::OnFinalMessage(hWnd);
		PostQuitMessage(0);
	}
	catch (...)
	{
		throw "CMainWnd::OnFinalMessage";
	}
}
