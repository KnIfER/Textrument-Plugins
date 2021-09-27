#include "StdAfx.h"
#include "TextrumentDemo.h"

class EditorDemo : public WindowImplBase, public ExtensionAPI
{
public:
    EditorDemo() { 
       _isRoundedRgn = false;
       _isWindowLess = false;
    };     

    LPCTSTR GetWindowClassName() const override
    { 
        return _T("不要问星辰白天的颜色"); 
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

    ArticleBuffer* lastActivatedBuffer = 0;

    void ActivateTabAt(WinTabbar* tabbar, SciEditText* edittext, int position)
    {
        int displayLn;
        userStrip.SetSciEdit(editor=edittext);
        void* doc = edittext->DocPointer();
        LuaExtension::Instance().InitBuffer(position);
        if (position>1)
        { //tg
            //TestUserStrip();
        }
        ArticleBuffer* buffer = (ArticleBuffer*)tabbar->GetTabDataAt(position);
        if (tabbar->GetCount()>1)
        {
            tabbar->SetAutoDestroy(false);
            for (int i = tabbar->GetCount()-1; i >= 0; i--)
            {
                if(tabbar->GetItemAt(i)!=edittext) 
                {
                   tabbar->RemoveAt(i);
                }
            }
        }
        //if(false)
        if (!strcmp(buffer->init_doc, "-"))
        {
            if (!buffer->testFrame)
            {
                buffer->testFrame = builder.Create(L"TextFrameDemo_1.xml", L"", 0, &m_pm, false);
            }
            tabbar->Add(buffer->testFrame);
        }
        TabCtrl_SetCurSel(tabbar->GetHWND(), position);
        if (doc!=buffer->doc && lastActivatedBuffer!=buffer)
        {
            if (lastActivatedBuffer)
            {
                displayLn =  edittext->FirstVisibleLine();
                // save paramters to restore view states. 
                //  see SciTEBase::SetDocumentAt
                lastActivatedBuffer->scrollPosition =  edittext->DocLineFromVisible(displayLn);
                lastActivatedBuffer->scrollOffset =  edittext->XOffset();
                lastActivatedBuffer->selAnchor =  edittext->SelectionNAnchor(0);
                lastActivatedBuffer->selPosition =  edittext->CurrentPos();
            }
            lastActivatedBuffer = buffer;
            edittext->AddRefDocument(doc);

            if (buffer->doc==0)
            {
                buffer->doc = edittext->CreateDocument(strlen(buffer->init_doc), Scintilla::DocumentOption::Default);
                edittext->SetDocPointer(buffer->doc);

                edittext->ClearAll();
                edittext->AddText(strlen(buffer->init_doc), buffer->init_doc);
                edittext->EmptyUndoBuffer();
            }
            else
            {
                edittext->SetDocPointer(buffer->doc);
            }

            // resotore scrollX、scrollY、simple selection.
            //  see SciTEBase::DisplayAround
            edittext->SetSel(buffer->selAnchor, buffer->selPosition);
            displayLn =  edittext->FirstVisibleLine();
            edittext->LineScroll(0, edittext->VisibleFromDocLine(buffer->scrollPosition)-displayLn);
            edittext->SetXOffset(buffer->scrollOffset);
        }

        ::ShowWindow(edittext->GetHWND(), buffer->testFrame?SW_HIDE:SW_SHOW);
    }

    LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override
    {
        if (uMsg == WM_NOTIFY)
        {
            switch (((LPNMHDR)lParam)->code)
            {
            case NM_CLICK:
                if (tabbar && ((LPNMHDR)lParam)->hwndFrom == tabbar->GetHWND())
                { // 可达
                    //TCHAR buffer[100]={0};
                    //wsprintf(buffer,TEXT("position=%s"), TEXT("NM_CLICK"));
                    //::MessageBox(NULL, buffer, TEXT(""), MB_OK);


                    return bHandled = TRUE;
                }
                break; 
            case TCN_SELCHANGE:
                if (false && tabbar && ((LPNMHDR)lParam)->hwndFrom == tabbar->GetHWND())
                { // 可达

                    //ActivateTabAt(tabbar, TabCtrl_GetCurSel(tabbar->GetHWND()));

                    return bHandled = TRUE;
                }
                break; 
            case WM_SIZE:
                {
                    RECT rc;
                    ::GetClientRect(GetHWND(), &rc);
                    CLabelUI* demoTxt = (CLabelUI*)m_pm.FindControl(L"demo");
                    if (demoTxt)
                    {
                        static int cc;
                        CDuiString txt;
                        //txt.Format(L"demo %s %d %d", demoTxt->GetText().GetData(), cc++, demoTxt->GetManager()->GetRoot());
                        txt.Format(L"demo %d %d %d", cc++, demoTxt->GetWidth(), rc.right);
                        demoTxt->SetText(txt);
                    }
                }
                break; 
            }
        }
        return FALSE;
    }

    MultiplexExtension multiExtender;

    void WinSciTE_Register(HINSTANCE hInstance_) 
    {
        WNDCLASS wndclass {};

        const TCHAR* classNameInternal = TEXT("SciTEWindowContent");

        // Register the window that holds the two Scintilla edit windows and the separator
        wndclass.style = 0;
        wndclass.lpfnWndProc = BaseWin::StWndProc;
        wndclass.cbClsExtra = 200;
        wndclass.cbWndExtra = 200;
        wndclass.hInstance = hInstance_;
        wndclass.hIcon = 0;//::LoadIcon(hInstance, resourceName);
        wndclass.hCursor = {};
        wndclass.hbrBackground = {};
        wndclass.lpszMenuName = nullptr;
        wndclass.lpszClassName = classNameInternal;
        ::RegisterClass(&wndclass);
    }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        EditorDemo* pThis = reinterpret_cast<EditorDemo*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        
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

    size_t article_idx = 0;
    size_t article_end = 5;

    void initEdiorTabs(WinTabbar* & tabbar, SciEditText* & edittext)
    {
        edittext->SetZoom(10);
        edittext->SetTechnology(Scintilla::Technology::DirectWrite);
        //edittext->SetFontLocale("en-us");

        edittext->SetSmoothScrolling(true);
        edittext->SetMarginWidthN(0, 50);
        //edittext->SetCodePage(CP_ACP);
        edittext->SetBufferedDraw(false);
        for (; article_idx < article_end; article_idx++)
        {
            ArticleBuffer & data = demoArticles[article_idx];
            ArticleBuffer* buffer = new ArticleBuffer;
            *buffer = data;
            tabbar->addTab(-1, data.title, (LPARAM)buffer, data.image);
        }
        for (size_t i = 0; i < 80; i++)
        {
            TCHAR* title_buffer = new TCHAR[64];
            swprintf_s(title_buffer, 64, L"happy#%d", i+5);
            ArticleBuffer* buffer = new ArticleBuffer{title_buffer};
            buffer->init_doc = "";
            tabbar->addTab(-1, title_buffer, (LPARAM)buffer);
        }
        article_end+=5;
        ActivateTabAt(tabbar, edittext, 0);
        tabbar->setTabFont(13, L"宋体");
        tabbar->setImageList();
        TImageInfo* bmp = CRenderEngine::LoadImageStr(L"tab_def.bmp", NULL, 0xFFFFFFFF, NULL, 3);
        if (bmp)
        {
            HBITMAP hBitmap = bmp->hBitmap;
            tabbar->addImageToList(bmp->hBitmap, RGB(255, 255, 255));
            DeleteObject(hBitmap);
            delete bmp;
        }
        tabbar->setTabCloseImages(6, 7, 8, 7);
        tabbar->setTabPadding(12, 3);

        std::function<int(void*, UINT, WPARAM, LPARAM)> listener = [&](void* handle, UINT MSG, WPARAM wParam, LPARAM lParam) 
        {
            if (MSG==NM_CLICK)
            {
                //WinTabbar* tabbar = dynamic_cast<WinTabbar*>((CControlUI*)GetWindowLongPtr((HWND)handle, GWLP_USERDATA));
                //SciEditText* edittext = static_cast<SciEditText*>(m_pm.FindControl(_T("edittext")));
                if (tabbar)
                {
                    if (wParam==lParam)
                    { // closing
                        tabbar->closeTabAt(wParam);
                        ActivateTabAt(tabbar, edittext, TabCtrl_GetCurSel(tabbar->GetHWND()));
                    }
                    else
                    { // activate
                        ActivateTabAt(tabbar, edittext, wParam);
                    }
                }
            }
            return 0;
        };
        tabbar->SetEventListener(listener);
    }

    void InitWindow() override
    {
        m_pm.SetLayered(false);
        //m_pm.SetUseGdiplusText(true);
        //if (false)
        {
            m_pm._bIsLayoutOnly = true;
            SetWindowLongPtr(GetHWND(), GWLP_WNDPROC, (LONG_PTR)WndProc);
        }
        tabbar = static_cast<WinTabbar*>(m_pm.FindControl(_T("tabbar")));
        edittext = static_cast<SciEditText*>(m_pm.FindControl(_T("edittext")));
        tabbar_1 = static_cast<WinTabbar*>(m_pm.FindControl(_T("tabbar_1")));
        edittext_1 = static_cast<SciEditText*>(m_pm.FindControl(_T("edittext_1")));
        tabbar_2 = static_cast<WinTabbar*>(m_pm.FindControl(_T("tabbar_2")));
        edittext_2 = static_cast<SciEditText*>(m_pm.FindControl(_T("edittext_2")));
        userStripParent = static_cast<CContainerUI*>(m_pm.FindControl(_T("op")));
        console_output = static_cast<CTextUI*>(m_pm.FindControl(_T("output")));
        if (edittext)
        {
            console_output->SetAutoCalcHeight(true);
            console_output->SetAutoCalcWidth(false);
            console_output->SetText(L"");
            WinSciTE_Register(CPaintManagerUI::GetInstance());
            HWND hwnd = ::CreateWindowEx(
                0,
                TEXT("SciTEWindowContent"),
                TEXT("userStrip"),
                WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                0, 0,
                100, 100,
                GetHWND(),
                HmenuID(2001),
                CPaintManagerUI::GetInstance(),
                &userStrip);
            
            multiExtender.RegisterExtension(LuaExtension::Instance());
            multiExtender.Initialise(this);
            initEdiorTabs(tabbar, edittext);
            tabbar->SetMultiLine(true, 5);
            initEdiorTabs(tabbar_1, edittext_1);
            initEdiorTabs(tabbar_2, edittext_2);
            ActivateTabAt(tabbar, edittext, 2);
            TestUserStrip();
            // see http://lua-users.org/wiki/UsingLuaWithScite
            LuaExtension::Instance().OnExecute("print (\"Hello, World!\")");
        }
        RECT rc;GetClientRect(GetHWND(), &rc);
        LogIs("GetClientRect, %ld %ld", rc.right, rc.bottom);
    }

    void TestUserStrip()
    {
        if (userStripParent && edittext)
        {
            UserStripShow("!'解释:'{}(&Search)\n'名称:'[Name](OK)(Cancel)");
        }
    }

    CDuiString GetSkinFile() override
    {
        return _T("TextrumentDemo.xml");
    }

    void Notify( TNotifyUI &msg ) override
    {
        if (msg.pSender)
        {
            if (msg.sType==L"click")
            {
                CButtonUI* button = dynamic_cast<CButtonUI*>(msg.pSender);
                if (button)
                {
                    button->Toggle();
                }
                if (msg.pSender->GetName()==L"Run")
                {
                    LuaExtension::Instance().OnExecute("dostring dostring(editor:GetText())");
                }
            }
        }
        if (msg.sType==L"click")
        {
           // 点击时间需由控件触发。 doEvent -> activate
           //TCHAR buffer[100]={0};
           //wsprintf(buffer,TEXT("position=%s"), TEXT("GOGO"));
           //::MessageBox(NULL, buffer, TEXT(""), MB_OK);
    
        }
        // WindowImplBase::Notify(msg);
    }

    // extender    
    // extender    
    // extender    
    intptr_t Send(Pane p, Scintilla::Message msg, uintptr_t wParam=0, intptr_t lParam=0){
        return editor->Call(msg, wParam, lParam);
    }
    std::string Range(Pane p, Scintilla::Span range){
        return editor->StringOfSpan(range);
    }
    void Remove(Pane p, Scintilla::Position start, Scintilla::Position end){
        return editor->DeleteRange(start, end-start);
    }
    void Insert(Pane p, Scintilla::Position pos, const char *s){
        if (p==paneEditor)
        {
            editor->InsertText(pos, s);
        }
        else
        {
            Trace(s);
        }
    }
    void Trace(const char *s){
        LogIs(s);
        if (console_output)
        {
            TCHAR buffer[256];
            size_t len = MultiByteToWideChar(CP_ACP, 0, s, strlen(s), buffer, 255);
            buffer[len]=0;
            console_output->GetText().Append(buffer);
            console_output->NeedRecalcAutoSize();
            console_output->NeedParentUpdate();
            CScrollBarUI* scrollbar = ((CContainerUI*)console_output->GetParent())->GetScrollbar();
            if (scrollbar)
            {
                scrollbar->SetScrollPos(scrollbar->GetScrollRange());
            }
            //console_output->GetParent()->Scroll
        }
    }
    std::string Property(const char *key){
        // return props.GetExpandedString(key);
        // see CheckStartupScript() "ext.lua.startup.script"
        return "";
    }
    void SetProperty(const char *key, const char *val){
        //return 0;
    }
    void UnsetProperty(const char *key){
        //return 0;
    }
    uintptr_t GetInstance(){
        return (uintptr_t)CPaintManagerUI::GetInstance();
    }
    void ShutDown(){
        //return 0;
    }
    void Perform(const char *actions){
        //return 0;
    }
    void DoMenuCommand(int cmdID){
        //return 0;
    }
    void UpdateStatusBar(bool bUpdateSlowData){
        //return 0;
    }
    void UserStripShow(const char *description){
        //userStrip.Creation();
        //userStrip.Show(true);
        userStrip.visible = *description != 0;
        if (userStrip.visible) {
            userStrip.SetSciEdit(edittext);
            userStrip.SetExtender(&multiExtender);
            userStrip.SetDescription(description);

            //SizeSubWindows();
            HWND hwnd = userStrip.Hwnd();
            ::SetParent(hwnd, GetHWND());
            ::ShowWindow(hwnd, SW_SHOW);

            userStripParent->SetAutoDestroy(true);
            userStripParent->RemoveAll();
            userStripParent->SetAutoDestroy(false);
            userStripParent->Add(new WinFrame(hwnd));
            userStripParent->NeedUpdate();
        } else {
            HWND hwnd = userStrip.Hwnd();
            //::ShowWindow(hwnd, SW_HIDE);
            userStrip.SetDescription(description);
            //userStripParent->SetAutoDestroy(true);
            //userStripParent->RemoveAll();
            //userStripParent->NeedParentUpdate();
        }
    }
    void UserStripSet(int control, const char *value){
        //return 0;
    }
    void UserStripSetList(int control, const char *value){
        //return 0;
    }
    std::string UserStripValue(int control){
        return userStrip.GetValue(control);
    }
    Scintilla::ScintillaCall &PaneCaller(Pane p) noexcept {
        return *edittext;
    }
//private:
    SciEditText* editor = 0;

    WinTabbar* tabbar = 0;
    SciEditText* edittext = 0;
    WinTabbar* tabbar_1 = 0;
    SciEditText* edittext_1 = 0;
    WinTabbar* tabbar_2 = 0;
    SciEditText* edittext_2 = 0;
    CContainerUI* userStripParent = 0;
    UserStrip userStrip;
    CTextUI* console_output = 0;
    CDialogBuilder builder;
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    REGIST_DUICONTROL(SciEditText);

    CPaintManagerUI::SetInstance(hInstance);
    //CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
    //CPaintManagerUI::SetResourceZip(_T("ListRes.zip"));

    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    EditorDemo* pFrame = new EditorDemo;

    if( pFrame == NULL ) return 0;
   
    HWND hwnd = pFrame->Create(NULL, _T("Textrument Demo"), UI_WNDSTYLE_FRAME & ~WS_VISIBLE , WS_EX_STATICEDGE | WS_EX_APPWINDOW 
        , 0, 0 , 800, 600);

    pFrame->CenterWindow();

    pFrame->ShowWindow();

    CPaintManagerUI::MessageLoop();

	return 0;
}