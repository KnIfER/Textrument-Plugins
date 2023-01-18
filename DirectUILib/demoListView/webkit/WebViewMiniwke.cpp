#include "pch.h"
#include "WebViewFactory.h"
#include "wke.h"

namespace DuiLib {

	class WebViewMiniwke : public CControlUI, WebViewInterface
	{
    public:
        WebViewMiniwke() : CControlUI()
        {
            _view_states &= ~VIEWSTATEMASK_IsDirectUI;
        }

        LPCTSTR GetClass() const override
        {
            return L"WebViewMiniwke";
        }

        void Init() override
        {
            if (!_hWnd && _hParent)
            {
                {
                    mWebView = wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, _hParent , 0, 0, 640, 480); 
                    if (mWebView)
                    {
                        _hWnd = wkeGetWindowHandle(mWebView);

                        //wkeOnCreateView(mWebView, onCreateView, this);
                        //wkeOnLoadUrlBegin(mWebView, onLoadUrlBegin, this);

                        wkeShowWindow(mWebView, true);

                        wkeLoadURL(mWebView, "https://www.bing.com");
                    }
                    //LogIs(2, "mWebView %p %p %p ", this, _hWnd, mWebView);
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
                ::MoveWindow(_hWnd, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0);
                //::UpdateWindow(_hWnd);
                //if(mWebView) wkeResize(mWebView, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
            }
            __super::SetPos(rc, bNeedInvalidate);
        }

        void LoadURL(const char * url) 
        {
            //if(mWebView) wkeLoadURL(mWebView, "https://www.baidu.com");
        }

        HWND __hParent=0;
        wkeWebView mWebView=0;
	};

    CControlUI* initWebViewMiniwke()
    {
        //wkeSetWkeDllPath(WKPath)
        if(wkeInitialize()) 
        {
            return new WebViewMiniwke;
        }
        return nullptr;
    }
}