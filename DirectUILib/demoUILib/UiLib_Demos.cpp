// UiLib_Demos.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"

#include <GdiPlus.h>
#pragma comment( lib, "GdiPlus.lib" )
using namespace Gdiplus;

#define IDM_SHOW_LOG		WM_USER+2200
#define IDM_EXIT_APP		WM_USER+2300

#include <map>
#include <ShellAPI.h>

#include "..\DuiLib\UIlib.h"


using namespace DuiLib;

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd() { } 

	~CMainWnd() { }

	CDuiString GetSkinFile()
	{
		return CDuiString(_T("MainSkin.xml"));
	}

	LPCTSTR GetWindowClassName() const
	{
		return _T("UiLib_Demos");
	}

	LRESULT HandleMessageX( UINT uMsg, WPARAM wParam, LPARAM lParam )
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

	void InitWindow()
	{
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

		// the tray icon
		//#ifndef _UNICODE
		//		strcpy(nid.szTip,"双击主显示界面");//信息提示条
		//		Shell_NotifyIconA(NIM_ADD,&nid);//在托盘区添加图标
		//#else
		//		wcscpy_s(nid.szTip,L"双击主显示界面");//信息提示条
		//		Shell_NotifyIcon(NIM_ADD,&nid);//在托盘区添加图标
		//#endif
	}

	void Notify( TNotifyUI& msg )
	{
		if( msg.pSender && msg.sType == _T("click") )//鼠标点击消息
		{
			if(msg.pSender->GetName() == _T("EffectsDemo"))
			{
				//TCHAR buffer[100]={0};
				//wsprintf(buffer,TEXT("position=%d"), 1);
				//::MessageBox(NULL, buffer, TEXT(""), MB_OK);

				msg.pSender->SetAnimEffects(true);
				msg.pSender->SetAttribute(_T("fx_adv"),_T("anim='left2right' offset='180'"));
				msg.pSender->TriggerEffects();

				if (pAnimWnd)
				{
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
			}
			if( msg.pSender == mpCloseBtn )
			{
				PostQuitMessage(0);
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
		} else {
			WindowImplBase::Notify(msg);
		}


	}

	void OnFinalMessage( HWND hWnd )
	{
		__super::OnFinalMessage(hWnd);
		delete this;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		PostQuitMessage(0);
		bHandled = TRUE;
		return 0;
	}

	CHorizontalLayoutUI* pAnimWnd;
	HMENU			hMenu;
	NOTIFYICONDATA	nid;


	CButtonUI*		mpCloseBtn;
	CButtonUI*		mpMaxBtn;
	CButtonUI*		mpRestoreBtn;
	CButtonUI*		mpMinBtn;
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourceType(UILIB_FILE);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath()+L"//Skins");

	GdiplusStartupInput   gdiplusStartupInput;
	ULONG_PTR             gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	if( ::LoadLibrary(_T("d3d9.dll")) == NULL ) 
		::MessageBox(NULL, _T("加载 d3d9.dll 失败，一些特效可能无法显示！"), _T("信息提示"),MB_OK|MB_ICONWARNING);

	CMainWnd* pFrame = new CMainWnd();
	if(pFrame == NULL)
		return 0;

	pFrame->Create(NULL,_T("UiLib Demos"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 900, 600);
	pFrame->CenterWindow();

	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	GdiplusShutdown(gdiplusToken);
	return 0;
}