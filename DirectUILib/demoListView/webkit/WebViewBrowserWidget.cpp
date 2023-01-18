#include "pch.h"

#include "WebViewFactory.h"
#include "BrowserUI.h"

namespace DuiLib {
	void* tmp=0;

	class WebViewBrowserWidget : public CControlUI, WebViewInterface
	{
    public:
		CHAR* _dataPath = NULL;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_SIZE:
				return 1;
			case WM_DESTROY:
				break;
				//case WM_MOVE:
				//	return 1;
			case WM_ERASEBKGND:
				return 1;
			//case WM_MOUSEWHEEL:
			//	LogIs(2, "WM_MOUSEWHEEL");
			//	return 1;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
				break;
			}

			return 0;
		}

		static BOOL regWndClassWin(LPCTSTR lpcsClassName, DWORD dwStyle)
		{
			WNDCLASS wndclass = { 0 };

			wndclass.style = dwStyle;
			wndclass.lpfnWndProc = WndProc;
			wndclass.cbClsExtra = 200;
			wndclass.cbWndExtra = 200;
			wndclass.hInstance = ::GetModuleHandle(NULL);
			wndclass.hIcon = NULL;
			//wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
			wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wndclass.lpszMenuName = NULL;
			wndclass.lpszClassName = lpcsClassName;

			::RegisterClass(&wndclass);
			return TRUE;
		}

		WebViewBrowserWidget() : CControlUI()
        {
            _view_states &= ~VIEWSTATEMASK_IsDirectUI;
			tmp=this;
        }

        LPCTSTR GetClass() const override
        {
            return L"WebViewMiniwke";
        }

        void SetHBrowser(HWND h)
		{
			if(!_createWrapper)
				_hWnd=h;
			_hBrowser=h;
			SendMessage(_hParent, WM_SIZE, 0, 0);

			//::SetProp(_hWnd, L"proc", (HANDLE)GetWindowLongPtr(_hWnd, GWLP_WNDPROC));
			//::SetWindowLongPtr(_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
		}

        void Init() override
        {
            if (!_hWnd && _hParent && !_deferedCreating)
            {
				_deferedCreating = true;
				HWND hParent = __hParent = _hParent;

				if(_createWrapper) 
				{
					regWndClassWin(L"WNDBW", CS_HREDRAW | CS_VREDRAW);
					_hWnd = ::CreateWindowEx(0 , L"WNDBW" , NULL
						, WS_CHILD , 0 , 0 , 800 ,  600, hParent , NULL , ::GetModuleHandle(NULL), NULL);
					ShowWindow(_hWnd, SW_SHOW);
					hParent = _hWnd;
				}

				if (bwCreateBrowser({hParent, "https://www.sqlite.org/support.html", onBrowserPrepared}))
				{

				}
            }
        }


		static void onBrowserPrepared(bwWebView browserPtr)
		{
			if (tmp) {
				((WebViewBrowserWidget*)tmp)->SetHBrowser(bwGetHWNDForBrowser(browserPtr));
			}
			//SetWindowLongPtr(presentee->hBrowser, GWLP_WNDPROC, (LONG_PTR)testWindowProc1);
		}

        void SetPos(RECT rc, bool bNeedInvalidate) override
        {
            if (_hWnd && _hParent != __hParent)
            {
                __hParent = _hParent;
                ::SetParent(_hWnd, _hParent);
                ::InvalidateRect(_hWnd, NULL, TRUE);
				//if(lastW)LogIs(2, "123");
            }

            m_rcItem = rc;
			//if(lastW==0)
            if(_hWnd) 
			{
				RECT rcPos = m_rcItem;
				int w = rcPos.right - rcPos.left;
				int h = rcPos.bottom - rcPos.top;

				//if(lastW==0) 
				{
					::MoveWindow(_hWnd, rcPos.left, rcPos.top, w, h, true);
					//LogIs(2, "123");
				}

				if(lastW!=w || lastH!=h)
				{
					if(_hBrowser 
						&& _createWrapper
						)
						::MoveWindow(_hBrowser, 0, 0, w, h, TRUE);
					lastW=w;
					lastH=h;
				}

				//::UpdateWindow(_hParent);
                
				//::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
                
            }
            //__super::SetPos(rc, bNeedInvalidate);
        }

        void LoadURL(const char * url) 
        {
            //if(mWebView)
			//	mWebView->Navigate(QkString(url));
        }

    private:
        HWND __hParent=0;
        HWND _hBrowser=0;

		int lastW;
		int lastH;

		bool _deferedCreating=false;
		bool _createWrapper=false;
	};

    CControlUI* initWebViewBrowserWidget()
    {
		TCHAR* WKPath = L"C:\\tmp\\chromium\\cef_binary\\Release_x32\\cefclient.dll";
		if(bwInit(WKPath))
		{
			return new WebViewBrowserWidget;
		}
		return NULL;
    }
}