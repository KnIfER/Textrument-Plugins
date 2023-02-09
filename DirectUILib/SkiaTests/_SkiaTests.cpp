#include <windows.h>
#include "..\DuiLib\UIlib.h"
#include "..\DuiLib\ControlEx\UIBeautifulSwitch.h"

using namespace DuiLib;


extern int HelloWorld_RunMain(HINSTANCE hInstance, HWND hParent);
extern int DuiLibTest_RunMain(HINSTANCE hInstance, HWND hParent);
extern int HelloSKIMG_RunMain(HINSTANCE hInstance, HWND hParent);
extern int SKIMG_RGN_RunMain(HINSTANCE hInstance, HWND hParent);
extern int SKIMG_VIEW_RunMain(HINSTANCE hInstance, HWND hParent);
extern int D2DT_RunMain(HINSTANCE hInstance, HWND hParent);
extern int GLDEMO_RunMain(HINSTANCE hInstance, HWND hParent);
extern int GLSkiaHello_RunMain(HINSTANCE hInstance, HWND hParent);

class SkiaTestBox : public WindowImplBase, public INotifyUI
{
public:
    SkiaTestBox() { 
        _frameLess = false;
    };     

    LPCTSTR GetWindowClassName() const override
    { 
        return _T("SkiaTestBox"); 
    }

    UINT GetClassStyle() const override
    { 
        return CS_HREDRAW | CS_VREDRAW; 
    }

    QkString GetSkinFile() override
    {
        return _T("SkiaTestBox.xml");
    }
    
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override
    {
        bHandled = TRUE;
        ::PostQuitMessage(0);
        return 0;
    }

    void Notify( TNotifyUI &msg ) override
    {
        if (msg.sType==L"click" && msg.pSender)
        {
            auto & ud = msg.pSender->GetName();
            doTest((TCHAR*)ud.GetData());
        }
        WindowImplBase::Notify(msg);
    }

    void doTest(TCHAR* ud) 
    {
        if( ud == L"T1" ) 
        { 
            HelloWorld_RunMain(CPaintManagerUI::GetInstance(), GetHWND());
            return; 
        }
        if( ud == L"T2" ) 
        { 
            DuiLibTest_RunMain(CPaintManagerUI::GetInstance(), GetHWND());
            return; 
        }
        if( ud == L"T3" ) 
        { 
            HelloSKIMG_RunMain(CPaintManagerUI::GetInstance(), GetHWND());
            return; 
        }
        if( ud == L"T4" ) 
        { 
            SKIMG_VIEW_RunMain(CPaintManagerUI::GetInstance(), GetHWND());
            return; 
        }
        if( ud == L"T5" ) 
        { 
            D2DT_RunMain(CPaintManagerUI::GetInstance(), GetHWND());
            return; 
        }
        if( ud == L"T6" ) 
        { 
            GLDEMO_RunMain(CPaintManagerUI::GetInstance(), GetHWND());
            return; 
        }
        if( ud == L"T7" ) 
        { 
            GLSkiaHello_RunMain(CPaintManagerUI::GetInstance(), GetHWND());
            return; 
        }
    }
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) /* The 'entry' point of the program. Console programs use the simple main(...)        */
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    CControlFactory::GetInstance()->RegistControl(TEXT("ImageView"), ImageView::CreateControl);
    CControlFactory::GetInstance()->RegistControl(TEXT("CBSwitchUI"), CBSwitchUI::CreateControl);



    SkiaTestBox* pFrame = new SkiaTestBox;

    if( pFrame == NULL ) return 0;

    pFrame->Create(NULL, _T("Skia Test Box"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 800, 600);

    pFrame->CenterWindow();

    pFrame->ShowWindow();

    pFrame->doTest(L"T7");


    CPaintManagerUI::MessageLoop();

    return 0;
}


