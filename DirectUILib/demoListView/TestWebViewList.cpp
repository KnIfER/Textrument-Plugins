/*
* Test ListHeader and ListHeaderItems
* 
*/
#include "pch.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
//#include <BrowserUI.h>
#include "webkit/WebViewFactory.h"
using namespace rapidjson;


namespace WebViewInList{

QkString Demo1 = L"列表堆叠网页(minibwke in list)";
QkString Demo2 = L"列表堆叠网页(miniblink in list)";
QkString Demo3 = L"列表堆叠网页(webview2 in list)";
QkString Demo4 = L"列表堆叠网页(libcef in list)";

#define kClassWindow L"MiniblinkWebs"

WebViewFactory _factory;

WebViewType _webviewType=miniwke;
HHOOK mousehook;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_MOUSEWHEEL) {
            
            //LogIs(2, "WM_MOUSEWHEEL √");
        }
    }
    return CallNextHookEx(0, nCode, wParam, lParam);
}

class ListMainForm : public WindowImplBase, public INotifyUI, public ListViewAdapter
{
public:

    ListMainForm(WebViewType webviewType_)
	{ 
       CControlFactory::GetInstance()->RegistControl(L"WebView", createWebView);
       _webviewType = webviewType_;

       mousehook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, CPaintManagerUI::GetInstance(),  0);


    };     

    static CControlUI* createWebView()
    {
        return _factory.initWebViewImpl(_webviewType);
    }

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

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        ListMainForm* pThis = reinterpret_cast<ListMainForm*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (uMsg==WM_SIZE)
        {
            RECT rc;
            ::GetClientRect(hWnd, &rc);
            pThis->m_pm.GetRoot()->SetPos(rc);

            LogIs("GetClientRect OnSize, %ld %ld", rc.right, rc.bottom);
        }
        if (uMsg==WM_DESTROY||uMsg==WM_CLOSE)
        {
            ::PostQuitMessage(0);
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    void InitWindow() override
    { 
        m_pm.SetLayered(false);
        //m_pm.SetUseGdiplusText(true);
        if (false)
        {
            m_pm._bIsLayoutOnly = true;
            SetWindowLongPtr(GetHWND(), GWLP_WNDPROC, (LONG_PTR)WndProc);
        }

        pList = dynamic_cast<ListView*>((CControlUI*)m_pm.FindControl(_T("vList")));
        lb = static_cast<CLabelUI*>(m_pm.FindControl(_T("lb")));
        if (pList)
        {

            Button* refer = new Button;

            refer->SetFixedWidth(-1);
            refer->SetFixedHeight(50);

            pList->SetReferenceItemView(refer);

            pList->SetAdapter(this);
            //for (size_t i = 0; i < 10; i++)
            //{
            //    CControlUI* pRoot =  builder.Create(L"webview_item.xml", 0, 0, &m_pm);
            //    pList->Add(pRoot);
            //}


            //pList->SetHeaderView(builder.Create(L"ListViewDemo_ListHeader.xml", 0, 0, &m_pm));

        }
    }

    size_t GetItemCount()
    {
        return 10;
		//return 1;
    }

    CControlUI* CreateItemView()
    {
        CControlUI* pRoot =  builder.Create(L"webview_item.xml", 0, 0, &m_pm);

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

        if(pList) {
            itemsBuffer.Format(L"创建了 %d 个视图！回收池大小：%d", items++, pList->GetRecyclePool().GetSize());
            lb->SetText(itemsBuffer);
        }

        return pRoot;

        //return ((Button*)viewTemplate)->Duplicate();
    }

    void OnBindItemView(CControlUI* view, size_t index)
    {
        WebViewInterface* browser = dynamic_cast<WebViewInterface*>(view);
        if(browser) browser->LoadURL("http://www.baidu.com");
        //if (horLayout)
        //{
        //    CControlUI* control = horLayout->GetItemAt(0);
        //    QkString & label = control->GetText();
        //    label.AsBuffer();
        //    label.Format(L"%d", index);
        //    control->Invalidate();
        //}
    }

    QkString GetSkinFile() override
    {
        return _T("test.xml");
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

	int items=0;
	QkString itemsBuffer;

	ListView* pList;
	CLabelUI* lb;
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

LRESULT RunWrap(HINSTANCE hInstance, HWND hParent, const QkString & Name, WebViewType webviewType)
{
    if (hInstance==NULL) return (LRESULT)Name.GetData();
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin//ListRes"));
    TestRapidJson();
    (new ListMainForm(webviewType))->Create(NULL, Name, UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW , 0, 0, 800, 600);
    return 0;
}


LRESULT RunTest1(HINSTANCE hInstance, HWND hParent)
{
    return RunWrap(hInstance, hParent, Demo1, miniwke);
}

LRESULT RunTest2(HINSTANCE hInstance, HWND hParent)
{
    return RunWrap(hInstance, hParent, Demo2, miniblink);
}

LRESULT RunTest3(HINSTANCE hInstance, HWND hParent)
{
    return RunWrap(hInstance, hParent, Demo3, webview2);
}

LRESULT RunTest4(HINSTANCE hInstance, HWND hParent)
{
    return RunWrap(hInstance, hParent, Demo4, libcef);
}

static int _auto_reg1 = AutoRegister(RunTest1);
static int _auto_reg2 = AutoRegister(RunTest2);
static int _auto_reg3 = AutoRegister(RunTest3);
static int _auto_reg4 = AutoRegister(RunTest4);

}