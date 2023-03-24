#include "pch.h"

#include "WebViewFactory.h"
#include "CheckFailure.h"
#include <shlwapi.h>

using namespace Microsoft::WRL;

namespace DuiLib {

	HHOOK mousehook;

	class WebViewWebview2 : public CControlUI, WebViewInterface
	{
    public:
		CHAR* _dataPath = NULL;

        WebViewWebview2() : CControlUI()
        {
            _view_states &= ~VIEWSTATEMASK_IsDirectUI;
        }

        LPCTSTR GetClass() const override
        {
            return L"WebViewMiniwke";
        }



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
			case WM_MOUSEWHEEL:
				LogIs(2, "WV2 WM_MOUSEWHEEL");
				return 1;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
				break;
			}

			return 0;
		}

		static LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			WNDPROC superProc = (WNDPROC)::GetProp(hWnd, L"proc");
			switch (message)
			{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_CONTEXTMENU:
			case WM_MOUSEMOVE:
			case WM_MOUSEWHEEL:
				LogIs(2, "WM_MOUSEWHEEL");
				return 1;
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

		static LPWSTR GetLastErrorAsString()  
		{  
			HLOCAL LocalAddress=NULL;  
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,GetLastError(),0,(PTSTR)&LocalAddress,0,NULL);  
			return (LPWSTR)LocalAddress;  
		}

        void Init() override
        {
            if (!_hWnd && _hParent && !_deferedCreating)
            {
				_deferedCreating = true;
				HWND hParent = __hParent = _hParent;
				auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
				regWndClassWin(L"WNDWV2", CS_HREDRAW | CS_VREDRAW);
				_hWnd = ::CreateWindowEx(0 , L"WNDWV2" , NULL
					, WS_CHILD , 0 , 0 , 0 , 0 , hParent , NULL , ::GetModuleHandle(NULL), NULL);
				HWND hBrowser = _hWnd;
				::ShowWindow(_hWnd, 1);

				TCHAR* dataPath = NULL; // L"C:\\temp\\"
				if (_dataPath && ::PathIsDirectoryA(_dataPath))
				{
					dataPath = new TCHAR[MAX_PATH];
					MultiByteToWideChar(CP_ACP, 0, _dataPath, -1, dataPath, MAX_PATH-1);
				}

				//options->put_AdditionalBrowserArguments();

				HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
					NULL , dataPath, options.Get(),
					Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>
					([this, hBrowser, hParent](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

					//LPWSTR versionInfo; env->get_BrowserVersionString(&versionInfo);
					//LogIs(2, L"Some BUG happened, ???. %ld  %d %s", result, (int)env, versionInfo);

					m_webViewEnvironment=env;
					env->CreateCoreWebView2Controller(hBrowser, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>
						([this, hBrowser, hParent](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
						if (controller != nullptr) 
						{
							webviewController = controller;
							webviewController->get_CoreWebView2(&mWebView);
							//webviewController->add_AcceleratorKeyPressed(
							//	Callback<ICoreWebView2AcceleratorKeyPressedEventHandler>(
							//		[this](
							//			ICoreWebView2Controller* sender,
							//			ICoreWebView2AcceleratorKeyPressedEventArgs* args
							//			)
							//{
							//	COREWEBVIEW2_MOUSE_EVENT_KIND kind;
							//	args->get_KeyEventKind(&kind);
							//	switch (kind) {
							//		case COREWEBVIEW2_MOUSE_EVENT_KIND_WHEEL: 
							//			break;
							//		case COREWEBVIEW2_KEY_EVENT_KIND_SYSTEM_KEY_DOWN: 
							//			break;
							//		default: return S_OK;
							//	}
							//}).Get(), nullptr);
						}

						if (!controller || !mWebView)
						{
							LogIs(2, L"\"%s\"\n\nSome BUG happened, webview2 failed to initialzie. ret=0x%lx controller=%d"
								, GetLastError()?GetLastErrorAsString():L""
								, result, (int)controller
							);
							return -1;
						}

						// LogIs(2, L" 初始化成功！ ");

						ICoreWebView2Settings* Settings;
						mWebView->get_Settings(&Settings);
						Settings->put_IsScriptEnabled(TRUE);
						Settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						Settings->put_IsWebMessageEnabled(TRUE);
						
						
						if (_stripWrapperWnd)
						{
							_hWnd = GetFirstChild(hBrowser);
							::SetParent(_hWnd, hParent);
							webviewController->put_ParentWindow(_hWnd);
							DestroyWindow(hBrowser);
						}

						SendMessage(hParent, WM_SIZE, 0, 0);

						static EventRegistrationToken pageEventRegistrationToken = {};
						//mWebView->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
						//CHECK_FAILURE(mWebView->add_WebResourceRequested(
						//	Callback<ICoreWebView2WebResourceRequestedEventHandler>([this](
						//		ICoreWebView2* sender,
						//		ICoreWebView2WebResourceRequestedEventArgs* args) {
						//	COREWEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext;
						//	CHECK_FAILURE(args->get_ResourceContext(&resourceContext));
						//	wil::com_ptr<ICoreWebView2WebResourceRequest> req;
						//	args->get_Request(&req);
						//	wil::unique_cotaskmem_string navigationTargetUri;
						//	req->get_Uri(&navigationTargetUri);
						//	TCHAR* uriTarget=(TCHAR*)navigationTargetUri.get();
						//
						//	return E_INVALIDARG;
						//}).Get(), &pageEventRegistrationToken));

						EventRegistrationToken token;
						//if(0)
						//	mWebView->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
						//		[](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs * args) -> HRESULT {
						//	PWSTR message;
						//	args->TryGetWebMessageAsString(&message);
						//	
						//	CoTaskMemFree(message);
						//	return S_OK;
						//}).Get(), &token);
						//
						//mWebView->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
						//	[](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs * args) -> HRESULT {
						//	return S_OK;
						//}).Get(), &token);
						//
						//if(0)
						//	mWebView->AddScriptToExecuteOnDocumentCreated(
						//		L"window.chrome.webview.addEventListener(\'message\', event => alert(event.data));" \
						//		L"window.chrome.webview.postMessage(window.document.URL);",
						//		nullptr);
						// 
						//mWebView->Navigate(L"https://www.bing.com/");
						mWebView->Navigate(L"https://www.sqlite.org/support.html");
						//mWebView->Navigate(L"https://translate.google.cn/?hl=zh-TW&sl=auto&tl=zh-TW&text=happy&op=translate");

						HWND realHBrowser=0;
						//LogIs(2, "delay...");
						realHBrowser = FindChildWindow(_hWnd, L"Intermediate D3D Window");
						//realHBrowser = FindChildWindow(_hWnd, L"Chrome_RenderWidgetHostHWND");
						//realHBrowser = FindChildWindow(_hWnd, L"Chrome_WidgetWin_1");
						//realHBrowser = FindChildWindow(_hWnd, L"Chrome_WidgetWin_0");

						//LogIs(2, "delay...%p", ::GetWindowLongPtr(realHBrowser, GWLP_HINSTANCE));
						if(realHBrowser) {

							//mousehook = SetWindowsHookEx(WH_MOUSE, MouseHookProc, (HINSTANCE)::GetWindowLong(realHBrowser, GWL_HINSTANCE),  ::GetWindowThreadProcessId(realHBrowser, 0));
							
							//HWND wnd=realHBrowser;GetWindowThreadProcessId(wnd, 0);
							//if(wnd)mousehook = SetWindowsHookEx(WH_MOUSE, MouseHookProc, (HINSTANCE)::GetWindowLong(wnd, GWL_HINSTANCE),  GetWindowThreadProcessId(wnd, 0));
							//wnd=FindChildWindow(_hWnd, L"Chrome_RenderWidgetHostHWND");
							//if(wnd)mousehook = SetWindowsHookEx(WH_MOUSE, MouseHookProc, (HINSTANCE)::GetWindowLong(wnd, GWL_HINSTANCE),  ::GetWindowThreadProcessId(wnd, 0));
							//wnd=FindChildWindow(_hWnd, L"Chrome_WidgetWin_1");
							//if(wnd)mousehook = SetWindowsHookEx(WH_MOUSE, MouseHookProc, (HINSTANCE)::GetWindowLong(wnd, GWL_HINSTANCE),  ::GetWindowThreadProcessId(wnd, 0));
							//wnd=FindChildWindow(_hWnd, L"Chrome_WidgetWin_0");
							//if(wnd)mousehook = SetWindowsHookEx(WH_MOUSE, MouseHookProc, (HINSTANCE)::GetWindowLong(wnd, GWL_HINSTANCE),  ::GetWindowThreadProcessId(wnd, 0));

							//LogIs(2, "%p %xp", realHBrowser, ::GetWindowThreadProcessId(realHBrowser, 0));
						//	::SetProp(realHBrowser, L"proc", (HANDLE)GetWindowLongPtr(realHBrowser, GWLP_WNDPROC));
						//	::SetWindowLongPtr(realHBrowser, GWLP_WNDPROC, (LONG_PTR)WndProc1);
						}



						return S_OK;
					}).Get());
					return S_OK;
				}).Get());
				//if(true) return;
				if (dataPath)
				{
					delete[] dataPath;
				}
            }
        }


		static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
			//LogIs(2, "MouseHookProc √√√");
			//if (nCode >= 0) 
			{
				switch (wParam)
				{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_CONTEXTMENU:
				//case WM_MOUSEMOVE:
				case WM_MOUSEWHEEL:
					LogIs(2, "√√√ WM_MOUSEWHEEL √√√");
					break;
				}
			}
			return CallNextHookEx(mousehook, nCode, wParam, lParam);
		}


		static BOOL CALLBACK FindChildWindowProc(HWND hwndChild, LPARAM lParam)
		{
			LONG_PTR* parms = (LONG_PTR*)lParam;
			TCHAR* NameBuffer=(TCHAR*)parms[1];
			GetClassName(hwndChild, NameBuffer, 128);
			const QkString & findName = *((const QkString *)parms[0]);
			//LogIs(2, L"window class=%s %s=?%d", NameBuffer, (LPCWSTR)findName, findName.Compare(NameBuffer));
			if(!findName.Compare(NameBuffer)) {
				parms[2] = (LONG_PTR)hwndChild;
				//LogIs(2, L"found hwnd!!!", hwndChild);
				return FALSE;
			}
			return TRUE;
		}

		HWND FindChildWindow(HWND hwnd, const QkString & name) {
			LONG_PTR parms[3];
			TCHAR NameBuffer[128];
			parms[0]=(LONG_PTR)&name;
			parms[1]=(LONG_PTR)NameBuffer;
			parms[2]=0;
			EnumChildWindows(hwnd, FindChildWindowProc, (LPARAM)parms);
			return (HWND)parms[2];
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
            if(_hWnd) {
				RECT rcPos = m_rcItem;
				int w = rcPos.right - rcPos.left;
				int h = rcPos.bottom - rcPos.top;
				
				//::DeferWindowPos();

				if (_stripWrapperWnd)
				{
					if(mWebView 
						&& webviewController 
						&& (w!=lastW||h!=lastH)
						)
					{

						RECT rcTmp = {0,0,w, h};
						//webviewController->put_Bounds(rcTmp);
						webviewController->put_Bounds(rcPos);
						lastW=w;
						lastH=h;
					} 
					else ::MoveWindow(_hWnd, rcPos.left, rcPos.top, w, h, FALSE);
				}
				else {
					::MoveWindow(_hWnd, rcPos.left, rcPos.top, w, h, FALSE);
					if(mWebView 
						&& webviewController 
						&& (w!=lastW||h!=lastH)
						)
					{

						RECT rcTmp = {0,0,w, h};
						//webviewController->put_Bounds(rcTmp);
						webviewController->put_Bounds(rcTmp);
						lastW=w;
						lastH=h;
					} 
				}
                //::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
                
                //::UpdateWindow(_hWnd);
            }
            //__super::SetPos(rc, bNeedInvalidate);
        }

        void LoadURL(const char * url) 
        {
            if(mWebView)
				mWebView->Navigate(QkString(url));
        }

    private:
        HWND __hParent=0;

        wil::com_ptr<ICoreWebView2> mWebView;
        wil::com_ptr<ICoreWebView2Environment> m_webViewEnvironment;
        wil::com_ptr<ICoreWebView2Controller> webviewController;

		int lastW;
		int lastH;

		bool _deferedCreating=false;
		bool _stripWrapperWnd=false;
	};

    CControlUI* initWebViewWebview2()
    {
		return new WebViewWebview2;
    }
}