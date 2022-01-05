/*
* Test ListHeader and ListHeaderItems
* 
*/
#include "pch.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;


namespace TestTreeViewAdapter{

QkString Name = L"Json树(TreeViewAdapter)";

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

            pList->SetHeaderView(builder.Create(L"ListViewDemo_ListHeader.xml", 0, 0, &m_pm));

        }
    }

    size_t GetItemCount()
    {
        //return 100;
        return 10000000;
    }

    CControlUI* CreateItemView()
    {
        CControlUI* pRoot =  builder.Create(L"ListViewDemo_item_columns.xml", 0, 0, &m_pm);

        if (pRoot == NULL) {
            QkString sError = _T("加载皮肤失败：");
            sError += GetSkinFile();
            sError += "\n\n";
            sError += builder.m_xml.GetLastErrorMessage();
            sError += "\n\n";
            sError += builder.m_xml.GetLastErrorLocation();
            MessageBox(NULL, sError, _T("WinQkUI") ,MB_OK|MB_ICONERROR);
            ExitProcess(1);
            return 0;
        }

        return pRoot;

        //return ((Button*)viewTemplate)->Duplicate();
    }

    void OnBindItemView(CControlUI* view, size_t index)
    {
        CHorizontalLayoutUI* horLayout = dynamic_cast<CHorizontalLayoutUI*>(view);

        if (horLayout)
        {
            CControlUI* control = horLayout->GetItemAt(0);
            QkString & label = control->GetText();
            label.AsBuffer();
            label.Format(L"%d", index);
            control->Invalidate();
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
        }
        // WindowImplBase::Notify(msg);
    }

private:
    CDialogBuilder builder;
    Button* m_pSearch;
};

void TestRapidJson()
{
    // 1. Parse a JSON string into DOM.
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    LogIs(buffer.GetString());
}

LRESULT RunTest(HINSTANCE hInstance, HWND hParent)
{
    if (hInstance==NULL) return (LRESULT)Name.GetData();
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin//ListRes"));
    TestRapidJson();
    (new ListMainForm)->Create(NULL, Name, UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW , 0, 0, 800, 600);
	return 0;
}

static int _auto_reg = AutoRegister(RunTest);

}