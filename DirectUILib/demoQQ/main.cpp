//
// win_impl_base.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 achellies (achellies at 163 dot com), wangchyz (wangchyz at gmail dot com)
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#include "stdafx.h"
#include "..\DuiLib\Utils\WinImplBase.h"
#include "main_frame.hpp"

#include <atlbase.h>
CComModule _Module;
#include <atlwin.h>

#include "resource.h"

void InitResource()
{	
	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch(CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
	{
		strResourcePath += _T("skin\\ADMonSetup\\");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
		// 加载资源管理器
		CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
		break;
	}
	case UILIB_RESOURCE:
	{
		strResourcePath += _T("skin\\ADMonSetup\\");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
		// 加载资源管理器
		CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
		break;
	}
	case UILIB_ZIP:
	{
		strResourcePath += _T("skin\\");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
		CPaintManagerUI::SetResourceZip(_T("ADMonSetup.zip"), true);
		// 加载资源管理器
		CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
		break;
	}
	case UILIB_ZIPRESOURCE:
	{
		//strResourcePath += _T("skin\\");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());

		
		HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));
		if( hResource != NULL ) {
			DWORD dwSize = 0;
			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
			if( hGlobal != NULL ) {
				dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( dwSize > 0 ) {

					CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
					// 加载资源管理器
					CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
				}
			}
			::FreeResource(hResource);
		}
	}
	break;
	}
}


#if defined(WIN32) && !defined(UNDER_CE)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int nCmdShow)
#endif
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourceDll(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);

	HINSTANCE hInstRich = ::LoadLibrary(_T("Riched20.dll"));

	::CoInitialize(NULL);
	::OleInitialize(NULL);

	_Module.Init( 0, hInstance );

#if defined(WIN32) && !defined(UNDER_CE)
	HRESULT Hr = ::CoInitialize(NULL);
#else
	HRESULT Hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	if( FAILED(Hr) ) return 0;


	InitResource();


	MainFrame* pFrame = new MainFrame();
	if( pFrame == NULL ) return 0;
#if defined(WIN32) && !defined(UNDER_CE)
	pFrame->Create(NULL, _T("QQ2011"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
#else
	pFrame->Create(NULL, _T("QQ2011"), UI_WNDSTYLE_FRAME, WS_EX_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();

	_Module.Term();

	::OleUninitialize();
	::CoUninitialize();

	::FreeLibrary(hInstRich);

	return 0;
}