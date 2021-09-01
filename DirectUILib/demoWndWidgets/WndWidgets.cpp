/*
* Android-Like ListView Demo By KnIFER
* 
*/
#include "..\DuiLib\UIlib.h"

using namespace DuiLib;

class ListMainForm : public WindowImplBase, public INotifyUI
{
public:
    ListMainForm() { 
       _isRoundedRgn = false;
       _isWindowLess = false;
    };     

    LPCTSTR GetWindowClassName() const override
    { 
        return _T("不要问白天星辰的颜色"); 
    }

    UINT GetClassStyle() const override
    { 
        return CS_DBLCLKS; 
    }

    void OnFinalMessage(HWND hWnd) override
    { 
        __super::OnFinalMessage(hWnd);
        delete this;
    }
    
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override
    {
        PostQuitMessage(0);
        bHandled = TRUE;
        return 0;
    }

    void InitWindow() override
    {
        //m_pSearch = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnSearch")));
        m_pm.SetLayered(false);
    }

    class ListViewBaseAdapter : public ListViewAdapter
    {
       size_t GetItemCount()
       {
           return 100;
       }
       CControlUI* CreateItemView()
       {
           CButtonUI* btn = new CButtonUI;
           btn->SetText(L"Test");
           btn->SetTextColor(0xff0000ff);
           return btn;

       }
       void OnBindItemView(CControlUI* view, size_t index)
       {
           CButtonUI* btn = dynamic_cast<CButtonUI*>(view);

           if (btn)
           {
               CDuiString label;
               label.Format(L"#%d", index);
               btn->SetText(label);
           }
       }
    };

    CDuiString GetSkinFile() override
    {
        return _T("WndWidgets.xml");
    }

    void Notify( TNotifyUI &msg ) override
    {
        if (msg.sType==L"click")
        {
        }
        // WindowImplBase::Notify(msg);
    }

private:
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
   // CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
    //CPaintManagerUI::SetResourceZip(_T("ListRes.zip"));

    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin//ListRes"));

    ListMainForm* pFrame = new ListMainForm;

    if( pFrame == NULL ) return 0;
   
    pFrame->Create(NULL, _T("测试原生窗口控件"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 800, 600);
    
    pFrame->CenterWindow();

    pFrame->ShowModal();

    CPaintManagerUI::MessageLoop();

	return 0;
}