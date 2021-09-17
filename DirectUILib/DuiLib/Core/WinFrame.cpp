#include "StdAfx.h"
#include "Core\InsituDebug.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL(WinFrame)

	class WndBase : public CWindowWnd, public INotifyUI, public IMessageFilterUI
	{
	public:
		WndBase() : CWindowWnd() {

		};
		LPCTSTR GetWindowClassName() const { 
			return _T("WndFrame"); 
		};
		UINT GetClassStyle() const {
			return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; 
		};
		void Init(WinFrame* pOwner) {
			wParent = pOwner;
			RECT rcPos = pOwner->GetPos();
			UINT uStyle = uStyle = WS_CHILD;
			Create(wParent->GetParent()->GetHWND(), NULL, uStyle, 0, rcPos);

			// 关联UI管理器
			m_pm.Init(m_hWnd, NULL);
			// 注册PreMessage回调
			m_pm.AddPreMessageFilter(this);
			m_pm.reInit(wParent->GetManager());
			//m_pm.SetUseGdiplusText(true);
			//m_pm.SetLayered(true);
			//else throw "root_layout==null!";

			::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
			//LogIs(L"Create::%d::%s::%d::%s", wParent->GetHWND(), wParent->GetName().GetData(), m_hWnd, wParent->GetParent()->GetName().GetData());
			if (m_hWnd==0) throw "WndBase::Create Error";
			CControlUI* layout=wParent->GetItemAt(0);
			if (layout) initRootLayout(layout);
			//::SetFocus(m_hWnd);
		};
		void Notify(TNotifyUI& msg){
			if( msg.sType == _T("click") ) ;
		}
		void initRootLayout(CControlUI* root_layout) {
			if (root_layout)
			{
				root_layout->SetManager(&m_pm, NULL, false);
				m_pm.AttachDialog(root_layout);
				m_pm.AddNotifier(this);
				//m_pm.SetLayered(true);
				wParent->SetAutoDestroy(false);
				wParent->RemoveAll();
				root_layout->SetManager(&m_pm, NULL, 1);
				this->root_layout = root_layout;
			}
		}

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
		{
			if( uMsg == WM_CREATE ) {
				return 0;
			}
			LRESULT lRes = 0;
			if( root_layout && m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
			//if( wParent->GetManager()->MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
			return __super::HandleMessage(uMsg, wParam, lParam);
		}

		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override
		{
			if( uMsg == WM_ERASEBKGND ) {
				bHandled = true;
				return 0;
			}
			if( uMsg == WM_EXITSIZEMOVE ) {
				bHandled = true;

				::InvalidateRect(GetHWND(), NULL, TRUE);

				return 0;
			}
			return FALSE;
		}

		void OnFinalMessage(HWND hWnd) { }

		//LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		//LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//protected:
		CPaintManagerUI m_pm;
		WinFrame* wParent = 0;
		CControlUI* root_layout = 0;
	};

	WinFrame::WinFrame()
		: CContainerUI()
	{
		m_dwBackColor = RGB(0, 0, 255);
		_isDirectUI = false;
		wEmbedded = 0;
	}

	WinFrame::WinFrame(HWND windowHandle)
		: WinFrame()
	{
		_hWnd = windowHandle;
	}

	WinFrame::~WinFrame()
	{
		//CContainerUI::~CContainerUI();
		if (wEmbedded)
		{
			DestroyWindow(wEmbedded->GetHWND());
		}
	}


	LPCTSTR WinFrame::GetClass() const
	{
		return L"WinFrame";
	}

	LPVOID WinFrame::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, L"WinFrame") == 0 ) return static_cast<WinFrame*>(this);
		return __super::GetInterface(pstrName);
	}

	void WinFrame::Init()
	{
		if (!_hWnd)
		{ // create default duilib window
			wEmbedded = new WndBase;
			((WndBase*)wEmbedded)->Init(this);
			_hWnd = wEmbedded->GetHWND();
		}
	}

	void WinFrame::SetPos(RECT rc, bool bNeedInvalidate) 
	{
		m_rcItem = rc;
		::MoveWindow(GetHWND(), rc.left, rc.top, rc.right - rc.left, 
			rc.bottom - rc.top, TRUE);
	}

	CControlUI* WinFrame::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		CControlUI* ret = __super::FindControl(Proc, pData, uFlags);
		if (!ret && wEmbedded 
			&& ((WndBase*)wEmbedded)->root_layout
			&& (uFlags & UIFIND_HITTEST) == 0 
			&& (uFlags & UIFIND_UPDATETEST) == 0 
			)
		{
			ret = ((WndBase*)wEmbedded)->root_layout->FindControl(Proc, pData, uFlags);
		}
		return ret;
	}
}
