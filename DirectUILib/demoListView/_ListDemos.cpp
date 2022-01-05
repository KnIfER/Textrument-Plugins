#include "pch.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) /* The 'entry' point of the program. Console programs use the simple main(...)        */
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
    
    auto pFrame = CreateDemoBox();
    
    pFrame->Create(NULL, _T("列表测试"), UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW , 0, 0, 500, 800);
    pFrame->ShowModal();
    return 0;
}