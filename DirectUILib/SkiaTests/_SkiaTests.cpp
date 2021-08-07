#include <windows.h>
#include "..\DuiLib\UIlib.h"

using namespace DuiLib;


extern int HelloWorld_RunMain(HINSTANCE hInstance, HWND hParent);
extern int DuiLibTest_RunMain(HINSTANCE hInstance, HWND hParent);

class SkiaTestBox : public WindowImplBase, public INotifyUI
{
public:
    SkiaTestBox() { };     

    LPCTSTR GetWindowClassName() const override
    { 
        return _T("SkiaTestBox"); 
    }

    UINT GetClassStyle() const override
    { 
        return CS_HREDRAW | CS_VREDRAW; 
    }

    bool IsWindowLess() const override
    {
        return false;
    }

    CDuiString GetSkinFile() override
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
        }
        WindowImplBase::Notify(msg);
    }
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) /* The 'entry' point of the program. Console programs use the simple main(...)        */
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    SkiaTestBox* pFrame = new SkiaTestBox;

    if( pFrame == NULL ) return 0;

    pFrame->Create(NULL, _T("Skia Test Box"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 800, 600);

    pFrame->CenterWindow();

    pFrame->ShowWindow();

    CPaintManagerUI::MessageLoop();

    return 0;
}



