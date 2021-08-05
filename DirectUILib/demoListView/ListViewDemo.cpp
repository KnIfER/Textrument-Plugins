/*
* Android-Like ListView Demo By KnIFER
* 
*/

#include "..\DuiLib\UIlib.h"

using namespace DuiLib;

class ListMainForm : public WindowImplBase, public INotifyUI
{
public:
    ListMainForm() { };     

    LPCTSTR GetWindowClassName() const override
    { 
        return _T("ListMainForm"); 
    };

    UINT GetClassStyle() const override
    { 
        return CS_DBLCLKS; 
    };

    void OnFinalMessage(HWND hWnd) override
    { 
        __super::OnFinalMessage(hWnd);
        delete this;
    };

    void InitWindow() override
    {
        m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
        m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
        m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
        m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
        m_pSearch = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnSearch")));
        if (m_pSearch)
        {
            m_pSearch->Activate();
        }
    }

    class ListViewBaseAdapter : public ListViewAdapter
    {
       size_t GetItemCount()
       {
           return 5000;
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
        return _T("ListViewDemo.xml");
    }

    void Notify( TNotifyUI &msg ) override
    {
        if (msg.sType==L"click")
        {
            if( msg.pSender == m_pCloseBtn ) 
            { 
                PostQuitMessage(0); // 因为activex的原因，使用close可能会出现错误
                return; 
            }
            else if( msg.pSender == m_pMaxBtn ) 
            { 
                SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; 
            }
            else if( msg.pSender == m_pRestoreBtn ) 
            { 
                SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; 
            }
            else if(msg.pSender == m_pSearch)
            {
                ListView* pList = static_cast<ListView*>(m_pm.FindControl(_T("vList")));
                if (pList)
                {
                    m_pSearch->SetTextColor(0xff0000ff);

                    //TCHAR buffer[100]={0};
                    //wsprintf(buffer,TEXT("position=%s"), pList->GetClass());
                    //::MessageBox(NULL, buffer, TEXT(""), MB_OK);

                    ListViewAdapter* ada = new ListViewBaseAdapter();

                    CButtonUI* refer = new CButtonUI;

                    refer->SetFixedWidth(-1);
                    refer->SetFixedHeight(50);

                    pList->SetReferenceItemView(refer);

                    pList->SetAdapter(ada);
                }
            }
            else if( msg.sType == _T("itemclick") ) 
            {
            }
        }
        // WindowImplBase::Notify(msg);
    }

private:
    CButtonUI* m_pCloseBtn;
    CButtonUI* m_pMaxBtn;
    CButtonUI* m_pRestoreBtn;
    CButtonUI* m_pMinBtn;
    CButtonUI* m_pSearch;
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
   // CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
    //CPaintManagerUI::SetResourceZip(_T("ListRes.zip"));

    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin//ListRes"));

    ListMainForm* pFrame = new ListMainForm;

    if( pFrame == NULL ) return 0;
   
    pFrame->Create(NULL, _T("ListDemo"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 800, 600);
    
    pFrame->CenterWindow();

    pFrame->ShowModal();

    CPaintManagerUI::MessageLoop();

	return 0;
}