#include "pch.h"
#include "WebViewFactory.h"
#include "mb.h"

namespace DuiLib {
	#define kClassWindow L"MiniblinkWebs"

	LRESULT WINAPI testWindowProc(
		__in HWND hWnd,
		__in UINT msg,
		__in WPARAM wParam,
		__in LPARAM lParam)
	{
		LRESULT result = 0;
		mbWebView view = (mbWebView)::GetProp(hWnd, L"mb");
		if (!view)
			return ::DefWindowProc(hWnd, msg, wParam, lParam);

		switch (msg) {
		case WM_NCDESTROY:
			if (::GetProp(hWnd, L"subView")) {
				RemoveProp(hWnd, L"subView");
			}

			mbDestroyWebView(view);
			return 0;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
		{
			if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE)))
				break;
			//mbRepaintIfNeeded(view);

			PAINTSTRUCT ps = { 0 };
			HDC hdc = ::BeginPaint(hWnd, &ps);

			RECT rcClip = ps.rcPaint;

			RECT rcClient;
			::GetClientRect(hWnd, &rcClient);

			RECT rcInvalid = rcClient;
			if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
				::IntersectRect(&rcInvalid, &rcClip, &rcClient);

			int srcX = rcInvalid.left - rcClient.left;
			int srcY = rcInvalid.top - rcClient.top;
			int destX = rcInvalid.left;
			int destY = rcInvalid.top;
			int width = rcInvalid.right - rcInvalid.left;
			int height = rcInvalid.bottom - rcInvalid.top;

			if (0 != width && 0 != height) {
				HDC hMbDC = mbGetLockedViewDC(view);
				::BitBlt(hdc, destX, destY, width, height, hMbDC, srcX, srcY, SRCCOPY);
				mbUnlockViewDC(view);
			}

			::EndPaint(hWnd, &ps);
			return 1;
			break;
		}
		case WM_SIZE:
		{
			RECT rc = { 0 };
			::GetClientRect(hWnd, &rc);
			int width = rc.right - rc.left;
			int height = rc.bottom - rc.top;

			::mbResize(view, width, height);
			// mbRepaintIfNeeded(view);
			::mbWake(view);

			return 0;
		}
		case WM_KEYDOWN:
		{
			unsigned int virtualKeyCode = (int)wParam;
			unsigned int flags = 0;
			if (HIWORD(lParam) & KF_REPEAT)
				flags |= MB_REPEAT;
			if (HIWORD(lParam) & KF_EXTENDED)
				flags |= MB_EXTENDED;

			if (mbFireKeyDownEvent(view, virtualKeyCode, flags, false))
				return 0;
			break;
		}
		case WM_KEYUP:
		{
			unsigned int virtualKeyCode = (int)wParam;
			unsigned int flags = 0;
			if (HIWORD(lParam) & KF_REPEAT)
				flags |= MB_REPEAT;
			if (HIWORD(lParam) & KF_EXTENDED)
				flags |= MB_EXTENDED;

			if (mbFireKeyUpEvent(view, virtualKeyCode, flags, false))
				return 0;
			break;
		}
		case WM_CHAR:
		{
			unsigned int charCode = (int)wParam;
			unsigned int flags = 0;
			if (HIWORD(lParam) & KF_REPEAT)
				flags |= MB_REPEAT;
			if (HIWORD(lParam) & KF_EXTENDED)
				flags |= MB_EXTENDED;

			if (mbFireKeyPressEvent(view, charCode, flags, false))
				return 0;
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			if (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN) {
				if (::GetFocus() != hWnd)
					::SetFocus(hWnd);
				::SetCapture(hWnd);
			} else if (msg == WM_LBUTTONUP || msg == WM_MBUTTONUP || msg == WM_RBUTTONUP) {
				ReleaseCapture();
			}

			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			unsigned int flags = 0;

			if (wParam & MK_CONTROL)
				flags |= MB_CONTROL;
			if (wParam & MK_SHIFT)
				flags |= MB_SHIFT;

			if (wParam & MK_LBUTTON)
				flags |= MB_LBUTTON;
			if (wParam & MK_MBUTTON)
				flags |= MB_MBUTTON;
			if (wParam & MK_RBUTTON)
				flags |= MB_RBUTTON;

			if (mbFireMouseEvent(view, msg, x, y, flags))
				return 0;
			break;
		}
		case WM_CONTEXTMENU:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			if (pt.x != -1 && pt.y != -1)
				ScreenToClient(hWnd, &pt);

			unsigned int flags = 0;

			if (wParam & MK_CONTROL)
				flags |= MB_CONTROL;
			if (wParam & MK_SHIFT)
				flags |= MB_SHIFT;

			if (wParam & MK_LBUTTON)
				flags |= MB_LBUTTON;
			if (wParam & MK_MBUTTON)
				flags |= MB_MBUTTON;
			if (wParam & MK_RBUTTON)
				flags |= MB_RBUTTON;

			//if (mbFireContextMenuEvent(view, pt.x, pt.y, flags))
			//	return 0;


			break;
		}
		case WM_MOUSEWHEEL:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			::ScreenToClient(hWnd, &pt);

			int delta = GET_WHEEL_DELTA_WPARAM(wParam);

			unsigned int flags = 0;

			if (wParam & MK_CONTROL)
				flags |= MB_CONTROL;
			if (wParam & MK_SHIFT)
				flags |= MB_SHIFT;

			if (wParam & MK_LBUTTON)
				flags |= MB_LBUTTON;
			if (wParam & MK_MBUTTON)
				flags |= MB_MBUTTON;
			if (wParam & MK_RBUTTON)
				flags |= MB_RBUTTON;

			if (mbFireMouseWheelEvent(view, pt.x, pt.y, delta, flags))
				return 0;
			break;
		}
		case WM_SETFOCUS:
			mbSetFocus(view);
			return 0;

		case WM_KILLFOCUS:
			mbKillFocus(view);
			return 0;

		case WM_SETCURSOR:
			if (mbFireWindowsMessage(view, hWnd, WM_SETCURSOR, 0, 0, &result))
				return result;
			break;

		case WM_IME_STARTCOMPOSITION: {
			if (mbFireWindowsMessage(view, hWnd, WM_IME_STARTCOMPOSITION, 0, 0, &result))
				return result;
			break;
		}
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	BOOL regWndClass(LPCTSTR lpcsClassName, DWORD dwStyle)
	{
		WNDCLASS wndclass = { 0 };

		wndclass.style = dwStyle;
		wndclass.lpfnWndProc = testWindowProc;
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


	class WebViewMiniblink : public CControlUI, WebViewInterface
	{
    public:
		WebViewMiniblink() : CControlUI()
        {
            _view_states &= ~VIEWSTATEMASK_IsDirectUI;
        }

        LPCTSTR GetClass() const override
        {
            return L"WebViewMiniblink";
        }

        void Init() override
        {
            if (!_hWnd && _hParent)
            {
                {
                    mWebView = mbCreateWebView();
					regWndClass(kClassWindow, CS_HREDRAW | CS_VREDRAW);
                    HWND hWnd = ::CreateWindowEx(WS_EX_STATICEDGE
						, kClassWindow, NULL
						, WS_CHILD | WS_VISIBLE, 0, 0, 600, 800, _hParent, NULL, ::GetModuleHandle(NULL), NULL);
                    ::SetProp(hWnd, L"mb", (HANDLE)mWebView);
                    ::SetProp(hWnd, L"subView", (HANDLE)TRUE);
                    ::mbSetHandle(mWebView, hWnd);
                    _hWnd = hWnd;
					mbShowWindow(mWebView, true);
					mbLoadURL(mWebView, "http://www.bing.com");
                }
            }
        }

        void SetPos(RECT rc, bool bNeedInvalidate) override
        {
            if (_hWnd && _hParent != __hParent)
            {
                __hParent = _hParent;
                ::SetParent(_hWnd, _hParent);
                ::InvalidateRect(_hWnd, NULL, TRUE);
            }

            m_rcItem = rc;
            if(_hWnd) {
                RECT rcPos = m_rcItem;
                //::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
				::MoveWindow(_hWnd, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 1);
                //if(mWebView) mbResize(mWebView, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
				//SetTimer(0x123456, 20, true);
            }
            __super::SetPos(rc, bNeedInvalidate);
        }

        void LoadURL(const char * url) 
        {
            if(mWebView) mbLoadURL(mWebView, "https://www.bing.com");
        }


		void DoEvent(TEventUI& event) override
		{
			if( event.Type == UIEVENT_TIMER ) 
			{
				if (event.wParam==0x123456) //  && _bUseSmoothScroll
				{
					KillTimer(0x123456);
					RECT rcPos = m_rcItem;
					//::MoveWindow(_hWnd, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, TRUE);
					::InvalidateRect(_hWnd, NULL, FALSE);
					::UpdateWindow(_hWnd);
				}
				return;
			}
		}

        HWND __hParent=0;
        mbWebView mWebView=0;
	};

    CControlUI* initWebViewMiniBlink()
    {
		//mbSetMbDllPath(WKPath1);
		mbSettings settings;
		memset(&settings, 0, sizeof(settings));
		//settings.mask = MB_ENABLE_NODEJS;
		mbInit(&settings);
        {
            return new WebViewMiniblink;
        }
        return nullptr;
    }
}