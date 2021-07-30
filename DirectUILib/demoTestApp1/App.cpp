// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "APP.h"
#include "ControlEx.h"
#include "ControlEx/UIBeautifulSwitch.h"
#include "ControlEx/UIComboBoxEx.h"
#include "ControlEx/ComboEdit.h"

#ifndef _DWMAPI_H_
typedef struct DWM_BLURBEHIND
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND;

typedef struct tagDWL_MARGINS {
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} DWM_MARGINS, *PDWM_MARGINS;

// Window attributes
enum DWMWINDOWATTRIBUTE
{
    DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
    DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
    DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
    DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
    DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
    DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
    DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
    DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
    DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
    DWMWA_HAS_ICONIC_BITMAP,            // [set] Indicates an available bitmap when there is no better thumbnail representation.
    DWMWA_DISALLOW_PEEK,                // [set] Don't invoke Peek on the window.
    DWMWA_EXCLUDED_FROM_PEEK,           // [set] LivePreview exclusion information
    DWMWA_CLOAK,                        // [set] Cloak or uncloak the window
    DWMWA_CLOAKED,                      // [get] Gets the cloaked state of the window
    DWMWA_FREEZE_REPRESENTATION,        // [set] Force this window to freeze the thumbnail without live update
    DWMWA_LAST
};

// Non-client rendering policy attribute values
enum DWMNCRENDERINGPOLICY
{
    DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
    DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
    DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
    DWMNCRP_LAST
};

// Values designating how Flip3D treats a given window.
enum DWMFLIP3DWINDOWPOLICY
{
    DWMFLIP3D_DEFAULT,      // Hide or include the window in Flip3D based on window style and visibility.
    DWMFLIP3D_EXCLUDEBELOW, // Display the window under Flip3D and disabled.
    DWMFLIP3D_EXCLUDEABOVE, // Display the window above Flip3D and enabled.
    DWMFLIP3D_LAST
};

#define DWM_BB_ENABLE                 0x00000001
#define DWM_BB_BLURREGION             0x00000002
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004

#define DWM_EC_DISABLECOMPOSITION     0x00000000
#define DWM_EC_ENABLECOMPOSITION      0x00000001
#endif // _DWMAPI_H_

class CDwm
{
public:
    typedef HRESULT (WINAPI *FNDWMENABLECOMPOSITION)(UINT);
    typedef HRESULT (WINAPI *FNDWNISCOMPOSITIONENABLED)(LPBOOL);
    typedef HRESULT (WINAPI *FNENABLEBLURBEHINDWINDOW)(HWND, CONST DWM_BLURBEHIND*);
    typedef HRESULT (WINAPI *FNDWMEXTENDFRAMEINTOCLIENTAREA)(HWND, CONST DWM_MARGINS*);
    typedef HRESULT (WINAPI *FNDWMSETWINDOWATTRIBUTE)(HWND, DWORD, LPCVOID pvAttribute, DWORD);

    FNDWMENABLECOMPOSITION fnDwmEnableComposition;
    FNDWNISCOMPOSITIONENABLED fnDwmIsCompositionEnabled;
    FNENABLEBLURBEHINDWINDOW fnDwmEnableBlurBehindWindow;
    FNDWMEXTENDFRAMEINTOCLIENTAREA fnDwmExtendFrameIntoClientArea;
    FNDWMSETWINDOWATTRIBUTE fnDwmSetWindowAttribute;

    CDwm()
    {
        static HINSTANCE hDwmInstance = ::LoadLibrary(_T("dwmapi.dll"));
        if( hDwmInstance != NULL ) {
            fnDwmEnableComposition = (FNDWMENABLECOMPOSITION) ::GetProcAddress(hDwmInstance, "DwmEnableComposition");
            fnDwmIsCompositionEnabled = (FNDWNISCOMPOSITIONENABLED) ::GetProcAddress(hDwmInstance, "DwmIsCompositionEnabled");
            fnDwmEnableBlurBehindWindow = (FNENABLEBLURBEHINDWINDOW) ::GetProcAddress(hDwmInstance, "DwmEnableBlurBehindWindow");
            fnDwmExtendFrameIntoClientArea = (FNDWMEXTENDFRAMEINTOCLIENTAREA) ::GetProcAddress(hDwmInstance, "DwmExtendFrameIntoClientArea");
            fnDwmSetWindowAttribute = (FNDWMSETWINDOWATTRIBUTE) ::GetProcAddress(hDwmInstance, "DwmSetWindowAttribute");
        }
        else {
            fnDwmEnableComposition = NULL;
            fnDwmIsCompositionEnabled = NULL;
            fnDwmEnableBlurBehindWindow = NULL;
            fnDwmExtendFrameIntoClientArea = NULL;
            fnDwmSetWindowAttribute = NULL;
        }
    }

    BOOL IsCompositionEnabled() const
    {
        HRESULT Hr = E_NOTIMPL;
        BOOL bRes = FALSE;
        if( fnDwmIsCompositionEnabled != NULL ) Hr = fnDwmIsCompositionEnabled(&bRes);
        return SUCCEEDED(Hr) && bRes;
    }

    BOOL EnableComposition(UINT fEnable)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableComposition != NULL ) bRes = SUCCEEDED(fnDwmEnableComposition(fEnable));
        return bRes;
    }

    BOOL EnableBlurBehindWindow(HWND hWnd)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableBlurBehindWindow != NULL ) {
            DWM_BLURBEHIND bb = { 0 };
            bb.dwFlags = DWM_BB_ENABLE;
            bb.fEnable = TRUE;
            bRes = SUCCEEDED(fnDwmEnableBlurBehindWindow(hWnd, &bb));
        }
        return bRes;
    }

    BOOL EnableBlurBehindWindow(HWND hWnd, CONST DWM_BLURBEHIND& bb)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableBlurBehindWindow != NULL ) {
            bRes = SUCCEEDED(fnDwmEnableBlurBehindWindow(hWnd, &bb));
        }
        return bRes;
    }

    BOOL ExtendFrameIntoClientArea(HWND hWnd, CONST DWM_MARGINS& Margins)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableComposition != NULL ) bRes = SUCCEEDED(fnDwmExtendFrameIntoClientArea(hWnd, &Margins));
        return bRes;
    }

    BOOL SetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
    {
        BOOL bRes = FALSE;
        if( fnDwmSetWindowAttribute != NULL ) bRes = SUCCEEDED(fnDwmSetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute));
        return bRes;
    }
};

#ifndef WM_DPICHANGED
#   define WM_DPICHANGED       0x02E0
#endif


class CFrameWindowWnd : public CWindowWnd, public INotifyUI//, public CDwm
{
public:
    CFrameWindowWnd()  { };
    LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };


    bool OnHChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), S, L);
        }
        return true;
    }

    bool OnSChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, H, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), L);
        }
        return true;
    }

    bool OnLChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, H, S, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
        }
        return true;
    }

    bool OnAlphaChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            m_pm.SetOpacity((static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
        }
        return true;
    }

    bool OnValueChanged(void* param) {
        //TNotifyUI* pMsg = (TNotifyUI*)param;
        //if( pMsg->sType == _T("valuechanged") ) {
        //    m_pm.SetOpacity((static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
        //}
        return true;
    }

    void OnPrepare() 
    {

       CCheckBoxUI *pCheck = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chkOption")));
       if (!pCheck)
       {
           return;
       }
       pCheck->Activate();
       pCheck->SetMaxHeight(40);
       pCheck->SetMaxWidth(60);
       
       CSliderUI* pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("alpha_controlor")));
       if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnAlphaChanged);
       pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("h_controlor")));
       if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnHChanged);
       pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("s_controlor")));
       if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnSChanged);
       pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("l_controlor")));
       if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnLChanged);


       CCircleProgressUI* pSwitch = static_cast<CCircleProgressUI*>(m_pm.FindControl(_T("circle_progress")));

       if( pSwitch ) {
           pSwitch->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnValueChanged);
       }

        //pCheck->SetAttribute(_T("normalimage"), _T("file='image\\switchbutton.png' source='0,0,143,91'"));
        //pCheck->SetAttribute(_T("selectedimage"), _T("file='image\\switchbutton.png' source='0,182,143,273'"));
    
    }

    void Init() {
        CComboUI* pAccountCombo = static_cast<CComboUI*>(m_pm.FindControl(_T("accountcombo")));
        CEditUI* pAccountEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("accountedit")));
        CControlUI* pad = static_cast<CControlUI*>(m_pm.FindControl(_T("pad")));

        CLabelUI* testedit = static_cast<CLabelUI*>(m_pm.FindControl(_T("te")));
        testedit->SetText(TEXT("<a>text</a>"));
        testedit->SetShowHtml(true);


        CRichEditUI* testedit1 = static_cast<CRichEditUI*>(m_pm.FindControl(_T("te1")));
        testedit1->SetText(TEXT("rich"));
        testedit1->SetRich(true);

        if( pAccountCombo && pAccountEdit ) pAccountEdit->SetText(pAccountCombo->GetText());

        pAccountEdit->SetFocus();

        if( pAccountCombo ) {
            pAccountCombo->itemRightClickable = true;
        }

    }


    static int CALLBACK BrowseForFolderCallBack(HWND hwnd, UINT message, LPARAM lParam, LPARAM lpData) {
        if (message == BFFM_INITIALIZED) {
            // SendMessage(hwnd, BFFM_SETSELECTION, 1, (LPARAM)struOptions.pythonPath.c_str());

             ::SendMessage(hwnd, BFFM_SETSELECTION, 1, (LPARAM)TEXT("D:\\Code\\FigureOut\\"));

        }
        return 0;
    }


    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("windowinit") ) OnPrepare();
        else if( msg.sType == _T("click") ) {

            if(msg.pSender->GetName() == _T("tt"))
            {

                CControlUI * root = m_pm.GetRoot();
                CContainerUI * thisVG = dynamic_cast<CContainerUI*>(root);
                std::vector<CContainerUI *> vgs;
                if(thisVG){
                    // traverse
                    vgs.push_back(thisVG);
                    while(vgs.size()) {
                        thisVG = vgs[vgs.size()-1];
                        vgs.pop_back();
                        int cc = thisVG->GetCount();
                        for(int i=0;i<cc;i++)
                        {
                            CControlUI * item = thisVG->GetItemAt(i);
                            auto & name = item->GetName();
                            if(!name.IsEmpty()&&!item->GetText().IsEmpty())
                            {
                                item->SetText(name);
                            }
                            CContainerUI * vgTest = dynamic_cast<CContainerUI*>(item);
                            if(vgTest)
                            {
                                vgs.push_back(vgTest);
                            }
                        }
                    }


                }
            }


            if( msg.pSender->GetName() == _T("closebtn") ) { PostQuitMessage(0); return; }
            else if( msg.pSender->GetName() == _T("loginBtn") ) { Close(); return; }
            else if( msg.pSender->GetName() == _T("insertimagebtn") ) {
                CRichEditUI* pRich = static_cast<CRichEditUI*>(m_pm.FindControl(_T("testrichedit")));
                if( pRich ) {
                    pRich->RemoveAll();
                }
            }
            else if( msg.pSender->GetName() == _T("changeskinbtn") ) {
                if( CPaintManagerUI::GetResourcePath() == CPaintManagerUI::GetInstancePath() )
                    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\FlashRes"));
                else
                    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
                CPaintManagerUI::ReloadSkin();
            }
        }
        else if( msg.sType == _T("itemselect") ) {
            if( msg.pSender->GetName() == _T("accountcombo") ) {
                CEditUI* pAccountEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("accountedit")));
                if( pAccountEdit ) pAccountEdit->SetText(msg.pSender->GetText());
            }
        }
        else if( msg.sType == DUI_MSGTYPE_ITEMMENU ) {

            int index  = msg.wParam; // DUI_CTR_LISTLABELELEMENT


            if(index==1) 
            {
                ::MessageBox(NULL, TEXT("111"), TEXT(""), MB_OK);
            }

           // TCHAR path[MAX_PATH];
           // BROWSEINFO bi = { 0 };
           // bi.ulFlags = BIF_USENEWUI;
           // bi.lpfn = BrowseForFolderCallBack;
           // bi.lpszTitle = TEXT("Pick LibCef folder:");
           // bi.hwndOwner = GetHWND();
           // LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
           //
           // if (pidl != 0) {
           //     SHGetPathFromIDList(pidl, path);
           //
           //     //::MessageBox(NULL, TEXT("111"), path, MB_OK);
           //
           // }

        }


    }


    class CDialogBuilderCallbackEx : public IDialogBuilderCallback
    {
    public:
        CControlUI* CreateControl(LPCTSTR pstrClass) 
        {
            //if( _tcscmp(pstrClass, DUI_CTR_BTN_PROGRESS) == 0 ) return new CBtnProgressUI;
            if( _tcscmp(pstrClass, TEXT("CircleProgress")) == 0 ) return new CCircleProgressUI;
            //if( _tcscmp(pstrClass, TEXT("BSwitch")) == 0 ) return new CBSwitchUI;
            return NULL;
        }
    };


    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if( uMsg == WM_CREATE ) {
            m_pm.Init(m_hWnd);
            CDialogBuilder builder;

            CDialogBuilderCallbackEx cb;
            CControlUI* pRoot = builder.Create(TEXT("test3.xml"), (UINT)0, &cb, &m_pm);
           
            //CControlUI* pRoot = builder.Create(TEXT("frame.xml"), (UINT)0, &cb, &m_pm);

            ASSERT(pRoot && "Failed to parse XML");
            m_pm.AttachDialog(pRoot);
            m_pm.AddNotifier(this);

            //m_pm.SetDPI(100);  // Set the new DPI, retrieved from the wParam

            //m_pWndShadow = new CWndShadow;
            //m_pWndShadow->Create(m_hWnd);
            //RECT rcCorner = {3,3,4,4};
            //RECT rcHoleOffset = {0,0,0,0};
            //m_pWndShadow->SetImage(_T("LeftWithFill.png"), rcCorner, rcHoleOffset);

            //DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
            //SetWindowAttribute(m_hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &ncrp, sizeof(ncrp));

            //DWM_BLURBEHIND bb = {0};
            //bb.dwFlags = DWM_BB_ENABLE;
            //bb.fEnable = true;
            //bb.hRgnBlur = NULL;
            //EnableBlurBehindWindow(m_hWnd, bb);

            //DWM_MARGINS margins = {-1}/*{0,0,0,25}*/;
            //ExtendFrameIntoClientArea(m_hWnd, margins);

            //Init();

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

CFrameWindowWnd* pFrame;

LRESULT WINAPI transparentWndProc(
    __in HWND hWnd,
    __in UINT msg,
    __in WPARAM wParam,
    __in LPARAM lParam)
{
    //if (msg==WM_NCCREATE)
    //{
    //    return true;
    //}
    //if (msg==WM_ACTIVATE )
    //{
    //    return true;
    //}
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
    //if (msg==WM_NCHITTEST)
    //{
    //    return HTTRANSPARENT;
    //}
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
        if (pFrame)
        {
            //pFrame->HandleMessage(msg, wParam, lParam);

            RECT rc;
            ::GetClientRect(hWnd, &rc);
            int toolbarHeight = 0;
            ::MoveWindow(pFrame->GetHWND(), rc.left, rc.top+toolbarHeight, rc.right, rc.bottom-toolbarHeight,1);

        }
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
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;


    CControlFactory::GetInstance()->RegistControl(TEXT("CBSwitchUI"), CBSwitchUI::CreateControl);
    // CControlFactory::GetInstance()->RegistControl(TEXT("CComboBoxExUI"), CComboBoxExUI::CreateControl);
    CControlFactory::GetInstance()->RegistControl(TEXT("CComboEditUI"), CComboEditUI::CreateControl);




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


    
    HWND hwnd = ::CreateWindowEx(0 , lpcsClassName , NULL
        , WS_OVERLAPPED | WS_CAPTION |  WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX
        , 0 , 0 , 840 , 680 , NULL , NULL , ::GetModuleHandle(NULL), NULL);




    ShowWindow(hwnd, SW_SHOW);



    pFrame = new CFrameWindowWnd();

    pFrame->Create(hwnd, _T("embed")
        , WS_CHILD | WS_VISIBLE
        , 0
    );


    if( pFrame == NULL ) return 0;

    ::SendMessage(hwnd, WM_SIZE, 0, 0);

    //pFrame->Create(NULL, _T("这是一个最简单的测试用exe，修改test1.xml就可以看到效果"), UI_WNDSTYLE_FRAME|WS_CLIPCHILDREN, WS_EX_WINDOWEDGE);
    //pFrame->CenterWindow();
    //pFrame->ShowWindow(true);



    //pFrame->ShowModal();

    //CPaintManagerUI::MessageLoop();


    MSG msg;
    //if(0)
    {
        while (GetMessage(&msg, NULL, 0, 0)) {
            //if ((looper->dialog_hwnd_ && IsDialogMessage(looper->dialog_hwnd_ , &msg)))
            //  continue;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


    ::CoUninitialize();
    return 0;
}
