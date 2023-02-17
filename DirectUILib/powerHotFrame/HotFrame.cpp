/*
* Make taskbar highlight more obvious
* 
*/
#include "pch.h"


static DWORD dwNScStyle = WS_CAPTION|WS_THICKFRAME; 

bool init = true;

class HotFrame : public WindowImplBase, public INotifyUI
{
public:
    HotFrame() {
        _frameLess = true;
    };     

    LPCTSTR GetWindowClassName() const override
    { 
        return _T("ptHotFrame"); 
    }

    void InitHotFrame()
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) 
            & ~WS_EX_APPWINDOW | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT
            | WS_EX_LAYERED);
        init = false;
    }

    void InitWindow() override
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) 
            //& ~WS_EX_APPWINDOW | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT
            | WS_EX_LAYERED);
        SetLayeredWindowAttributes(m_hWnd, 0, 65, LWA_ALPHA);
        //::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, 0);

        auto style = GetWindowLong(m_hWnd, GWL_STYLE);
        SetWindowLong(m_hWnd, GWL_STYLE , style & ~dwNScStyle | WS_POPUP  );

        int w = ::GetSystemMetrics(SM_CXSCREEN);
        int h = ::GetSystemMetrics(SM_CYSCREEN);
        //::SetWindowPos(m_hWnd, NULL, 0, 0, w, h, 0);


        m_pm.GetShadow()->ShowShadow(true);
        m_pm.GetShadow()->SetSize(9);
        m_pm.GetShadow()->SetSharpness(5);
        //m_pm.GetShadow()->SetPosition(1,1);
        m_pm.GetShadow()->SetColor(0xe0112288);
    }

    QkString GetSkinFile() override
    {
        return _T("<window showshadow=\"true\"><VBOX bkColor=\"#FF4988ff\"><PAD/><PAD bkColor=\"#FF0000ff\" height=\"4\"/></VBOX></window>");
    }

    UINT GetClassStyle() const override
    { 
        return CS_DBLCLKS; 
    }

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override
    {
        ::DestroyWindow(GetHWND());
        bHandled = TRUE;
        ::PostQuitMessage(0);
        return 0;
    }

    void Notify( TNotifyUI &msg ) override
    {
        if (msg.sType==L"click")
        {
        }
    }
private:
    CDialogBuilder builder;
};

int tbX, tbY;
int x, y, w, h;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) /* The 'entry' point of the program. Console programs use the simple main(...)        */
{
    CPaintManagerUI::SetInstance(hInstance);
    //CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    HotFrame pFrame;

    pFrame.Create(NULL, _T("高亮框"), UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW , 0, 0, 500, 800);
    pFrame.ShowWindow();

    //HMONITOR hmon = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
    //MONITORINFO mi = { sizeof(mi) };
    //GetMonitorInfo(hmon, &mi);
    //PRINT_RECT(123 , mi.rcWork, 2);

    MSG    msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        switch (msg.message)
        {
        case WM_LBUTTONDOWN:
            ReleaseCapture();
            ::SendMessage(pFrame.GetHWND(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
            break;
        case WM_USER+1:
            if(init) pFrame.InitHotFrame();
            tbX = msg.wParam;
            tbY = msg.lParam;
            break;
        case WM_USER+2:
            x = msg.wParam;
            y = msg.lParam;
            break;
        case WM_USER+3:
            w = msg.wParam;
            h = msg.lParam;
            break;
        case WM_USER+4:
            ::SetWindowPos(pFrame.GetHWND(), HWND_TOPMOST, tbX+x, tbY+y, w, h, 0);
            SetWindowLong(pFrame.GetHWND(), GWL_EXSTYLE, GetWindowLong(pFrame.GetHWND(), GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}