#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "ControlEx.h"

class C360SafeFrameWnd : public WindowImplBase
{
public:
	C360SafeFrameWnd() {
		_windowless = true;
		_roundwnd = false;
	};
	LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	void InitWindow() {
		m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
		m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
		m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
		m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	}

	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if( _tcscmp(pstrClass, _T("ComputerExamine")) == 0 ) return new ComputerExamineUI;
		return NULL;
	}

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("click") ) {
			if( msg.pSender == m_pCloseBtn ) {
				PostQuitMessage(0);
				return; 
			}
			else if( msg.pSender == m_pMinBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
			else if( msg.pSender == m_pMaxBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; }
			else if( msg.pSender == m_pRestoreBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; }
		}
		else if(msg.sType==_T("selectchanged"))
		{
			CDuiString name = msg.pSender->GetName();
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			if(name==_T("examine"))
				 pControl->SelectItem(0);
			else if(name==_T("trojan"))
				 pControl->SelectItem(1);
			else if(name==_T("plugins"))
				pControl->SelectItem(2);
			else if(name==_T("vulnerability"))
				pControl->SelectItem(3);
			else if(name==_T("rubbish"))
				pControl->SelectItem(4);
			else if(name==_T("cleanup"))
				pControl->SelectItem(5);
			else if(name==_T("fix"))
				pControl->SelectItem(6);
			else if(name==_T("tool"))
				pControl->SelectItem(7);
		}
	}

	QkString GetSkinFile()
	{
		return L"skin.xml";
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( wParam == SC_CLOSE ) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if( ::IsZoomed(*this) != bZoomed ) {
			if( !bZoomed ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
			}
		}
		return lRes;
	}
private:
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
	CPaintManagerUI::SetResourceZip(_T("360SafeRes.zip"));

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	C360SafeFrameWnd* pFrame = new C360SafeFrameWnd();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("360 demo"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}