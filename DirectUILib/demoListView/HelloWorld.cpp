/*
* Android-Like ListView Demo By KnIfER
* 
*/
#include "pch.h"

namespace BasicButtonsList{

QkString Name = L"Basic Buttons List";

class ListMainForm : public WindowImplBase, public INotifyUI, public ListViewAdapter
{
public:
    ListMainForm() { };     

    LPCTSTR GetWindowClassName() const override
    { 
        return _T("ListMainForm"); 
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
        ::DestroyWindow(GetHWND());
        bHandled = TRUE;
        return 0;
    }

    CControlUI* viewTemplate;


    void InitWindow() override
    {
        viewTemplate = builder.Create(L"ListViewDemo_item.xml", 0, 0, &m_pm);

        ListView* pList = static_cast<ListView*>(m_pm.FindControl(_T("vList")));
        if (pList)
        {
            //TCHAR buffer[100]={0};
            //wsprintf(buffer,TEXT("position=%s"), pList->GetClass());
            //::MessageBox(NULL, buffer, TEXT(""), MB_OK);


            Button* refer = new Button;

            refer->SetFixedWidth(-1);
            refer->SetFixedHeight(50);

            pList->SetReferenceItemView(refer);

            pList->SetAdapter(this);
        }

        CHorizontalLayoutUI* menuBar = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("menuBar")));
        for (size_t i = 0; i < 10; i++)
        {
            auto menu = builder.Create(L"menu_item.xml", 0, 0, &m_pm);
            menu->SetFixedWidth(0);
            menu->GetText().Format(L"菜单#%d", i);
            menu->GetText().Format(L"文件#%d", i);
            menu->GetText().Format(L"文件(&F)", i);
            menuBar->Add(menu);
        }
    }

    size_t GetItemCount()
    {
        //return 10;
        return 1000;
        // return 10000000;
    }

    CControlUI* CreateItemView()
    {
        return builder.Create(L"ListViewDemo_item.xml", 0, 0, &m_pm);
        //return ((Button*)viewTemplate)->Duplicate();
    }

    void OnBindItemView(CControlUI* view, size_t index)
    {
        CControlUI* btn = dynamic_cast<CControlUI*>(view);

         //QkString label;
         //label.Format(L"#%d", index);
         ////btn->SetText(label);
         //btn->GetText().Assign(label);
         //btn->SetNeedAutoCalcSize();
        
        if (btn)
        {
            QkString & label = btn->GetText();
            label.AsBuffer();
            label.Append(L"\n\n");
            label.Format(L"#%d", index);
            for (size_t i = 0; i < index*2; i++)
            {
                //label.Append(L" <b>测试</b> ");
                if (i > 5) break;
            }
            label.Append(L"\r\n哈哈哈无语了");
            label.Append(L"\r\n哈哈哈无语了\n\n\n");
            btn->SetText(label);
            btn->SetNeedAutoCalcSize();
            //btn->Invalidate();
        }
    }

    QkString GetSkinFile() override
    {
        return _T("ListViewDemo.xml");
    }

    void Notify( TNotifyUI &msg ) override
    {
        if (msg.sType==L"click")
        {
            if(msg.pSender == m_pSearch)
            {
            }
            else if( msg.sType == _T("itemclick") ) 
            {
            }

            auto bRoot = builder.Create(L"<Window><Button/></Window>", TEXT("str"), 0, &m_pm);
            ASSERT(bRoot);
        }
        // WindowImplBase::Notify(msg);
    }

private:
    CDialogBuilder builder;
    Button* m_pSearch;
};

LRESULT RunTest(HINSTANCE hInstance, HWND hParent)
{
    if (hInstance==NULL) return (LRESULT)Name.GetData();
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin//ListRes"));
    (new ListMainForm)->Create(NULL, Name, UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW , 0, 0, 800, 600);
	return 0;
}

static int _auto_reg = AutoRegister(RunTest);

}