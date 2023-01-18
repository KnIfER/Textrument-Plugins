#include "pch.h"

namespace DuiLib {
    HHOOK mousehook_;

	class WndControl : public CControlUI
	{
    public:
		WndControl() : CControlUI()
        {
            _view_states &= ~VIEWSTATEMASK_IsDirectUI;
        }

        static CControlUI* createControl() {
            return new WndControl;
        }

        LPCTSTR GetClass() const override
        {
            return L"WndControl";
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
            //case WM_ERASEBKGND:
            //    return 1;
            case WM_MOUSEWHEEL:
                LogIs(2, "WndControl::WM_MOUSEWHEEL");
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

        static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
            //LogIs(2, "MouseHookProc √√√");
            //if (nCode >= 0) 
            {
                switch (wParam)
                {
                case WM_MOUSEWHEEL:
                    LogIs(2, "√√√ MouseHookProc √√√");
                    break;
                }
            }
            return CallNextHookEx(mousehook_, nCode, wParam, lParam);
        }

        void Init() override
        {
            if (!_hWnd && _hParent)
            {
                regWndClassWin(L"WNDCTRL", CS_HREDRAW | CS_VREDRAW);
                _hWnd = ::CreateWindowEx(
                    0
                   // | WS_EX_TRANSPARENT 
                    | WS_EX_NOACTIVATE
                    | WS_EX_LAYERED
                    , L"WNDCTRL" 
                    , NULL
                    , WS_CHILD | WS_VISIBLE
                    , 0 , 0 , 800 , 600 
                    , _hParent 
                    , NULL 
                    , ::GetModuleHandle(NULL), NULL);


                //mousehook_ = SetWindowsHookEx(WH_MOUSE, MouseHookProc, (HINSTANCE)::GetWindowLong(_hWnd, GWL_HINSTANCE),  ::GetWindowThreadProcessId(_hWnd, 0));

            }
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
                ::MoveWindow(_hWnd, rcPos.left, rcPos.top, w, h, TRUE);
            }

            //__super::SetPos(rc, bNeedInvalidate);
        }

    private:
        HWND __hParent=0;
		int lastW;
		int lastH;

	};

    static int initWebViewWebview2()
    {
        CControlFactory::GetInstance()->RegistControl(L"WndControl", WndControl::createControl);
		return 1;
    }

    static int auto_reg_wndctrl = initWebViewWebview2();
}