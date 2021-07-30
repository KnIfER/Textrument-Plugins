// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Web.h"
#include "ControlEx.h"

class CFrameWindowWnd : public CWindowWnd, public INotifyUI
{
public:
    CFrameWindowWnd()  { };
    LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    void OnPrepare() 
    {

       CCheckBoxUI *pCheck = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chkOption")));

    }

    void Init() {

        //CWebBrowserUI* pBrowser = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("bw")));


        CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_pm.FindControl(_T("ie")));
        if( pActiveXUI ) {
            IWebBrowser2* pWebBrowser = NULL;
            pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
            if( pWebBrowser != NULL ) {
                //pWebBrowser->Navigate(L"https://github.com/duilib/duilib",NULL,NULL,NULL,NULL);  
                //pWebBrowser->Navigate(L"about:blank",NULL,NULL,NULL,NULL); 

                pWebBrowser->Navigate(TEXT("https://www.baidu.com"),NULL,NULL,NULL,NULL);

                pWebBrowser->Release();
            }
        }

        


    }


    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("windowinit") ) OnPrepare();
    }


    class CDialogBuilderCallbackEx : public IDialogBuilderCallback
    {
    public:
        CControlUI* CreateControl(LPCTSTR pstrClass) 
        {
            //if( _tcscmp(pstrClass, TEXT("CircleProgress")) == 0 ) return new CCircleProgressUI;

            return NULL;
        }
    };


    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if( uMsg == WM_CREATE ) {
            m_pm.Init(m_hWnd);
            CDialogBuilder builder;

            CDialogBuilderCallbackEx cb;
            CControlUI* pRoot = builder.Create(TEXT("testWeb.xml"), (UINT)0, &cb, &m_pm);
           
            //CControlUI* pRoot = builder.Create(TEXT("frame.xml"), (UINT)0, &cb, &m_pm);

            ASSERT(pRoot && "Failed to parse XML");
            m_pm.AttachDialog(pRoot);
            m_pm.AddNotifier(this);


            Init();
            return 0;
        }
        else if( uMsg == WM_DESTROY ) {
            ::PostQuitMessage(0L);
        }
        else if( uMsg == WM_NCACTIVATE ) {
            if( !::IsIconic(*this) ) return (wParam == 0) ? TRUE : FALSE;
        }

        LRESULT lRes = 0;
        if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

public:
    CPaintManagerUI m_pm;
};


int testWeb(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow) {
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    CFrameWindowWnd* pFrame = new CFrameWindowWnd();
    if( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T("这是一个最简单的测试用exe，修改test1.xml就可以看到效果"), UI_WNDSTYLE_FRAME|WS_CLIPCHILDREN, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);

    //pFrame->ShowModal();

    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
}

LRESULT WINAPI transparentWndProc(
    __in HWND hWnd,
    __in UINT msg,
    __in WPARAM wParam,
    __in LPARAM lParam)
{
    if (msg==WM_NCCREATE)
    {
        return true;
    }
    if (msg==WM_ACTIVATE )
    {
        return true;
    }
    if(false)
    if (msg==WM_PAINT)
    {
        //if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE))) break;;

        RECT rcClient;
        ::GetClientRect(hWnd, &rcClient);

        PAINTSTRUCT ps = { 0 };
        HDC hdc = ::BeginPaint(hWnd, &ps);

        RECT rect = rcClient;  

        //rect.right = rect.left+(rect.right-rect.left)/2;

        HBRUSH hbrush = CreateSolidBrush(0xafafaf);

        FillRect(hdc, &rect, hbrush);


        ::EndPaint(hWnd, &ps);
        return 1;
    }
    if (msg==WM_NCHITTEST)
    {
        return HTTRANSPARENT;
    }
    //if (msg==WM_NCACTIVATE)
    //{
    //
    //    return (wParam == 0) ? TRUE : FALSE;
    //}
    //return true;

    LRESULT result = 0;
    //mbWebView view = (mbWebView)::GetProp(hWnd, L"mb");
    //if (!view)
    //	return ::DefWindowProc(hWnd, msg, wParam, lParam);

    switch (msg) { 

    case WM_ERASEBKGND:
        return TRUE;

    case WM_SIZE:
    {
        return 0;
    }
    case WM_KEYDOWN:
    {
        break;
    }
    case WM_KEYUP:
    {
        break;
    }
    case WM_CHAR:
    {
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
        break;
    }
    case WM_CONTEXTMENU:
    {

        break;
    }
    case WM_MOUSEWHEEL:
    {
        break;
    }
    case WM_SETFOCUS:
        return 0;

    case WM_KILLFOCUS:
        return 0;

    case WM_SETCURSOR:
        break;

    case WM_IME_STARTCOMPOSITION: {
        break;
    }
    case WM_CLOSE: {
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage( 0 );
        break;
    }
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{

    LPCTSTR lpcsClassName = L"TouMing";

    WNDCLASS wndclass = { 0 };

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = transparentWndProc;
    wndclass.cbClsExtra = 200;
    wndclass.cbWndExtra = 200;
    wndclass.hInstance = ::GetModuleHandle(NULL);
    wndclass.hIcon = NULL;
    //wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = lpcsClassName;

    ::RegisterClass(&wndclass);


    HWND hwnd = ::CreateWindowEx(WS_EX_APPWINDOW , lpcsClassName , NULL
        , WS_OVERLAPPEDWINDOW | WS_VISIBLE , 0 , 0 , 840 , 680 , NULL , NULL , ::GetModuleHandle(NULL), NULL);



    ShowWindow(hwnd, SW_SHOW);


    MSG msg;

    {
        while (GetMessage(&msg, NULL, 0, 0)) {
            //if ((looper->dialog_hwnd_ && IsDialogMessage(looper->dialog_hwnd_ , &msg)))
            //  continue;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return 0;
}
