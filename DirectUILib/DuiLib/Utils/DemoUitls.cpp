#include "stdafx.h"
namespace DuiLib
{
    class UniDemoBox : public WindowImplBase, public INotifyUI
    {
    public:
        UniDemoBox() { 
            _frameLess = false;
        };     

        LPCTSTR GetWindowClassName() const override
        { 
            return _T("UniDemoBox"); 
        }

        UINT GetClassStyle() const override
        { 
            return CS_HREDRAW | CS_VREDRAW; 
        }

        QkString GetSkinFile() override
        {
            return _T("");
        }

        LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            // 关联UI管理器
            m_pm.Init(m_hWnd, GetManagerName());
            // 注册PreMessage回调
            m_pm.AddPreMessageFilter(this);
            m_pm.SetDefaultFont(L"", 22, false, false, false);
            // 创建主窗口
            QkString sSkinType = GetSkinType();

            CVerticalLayoutUI* vLayout = new CVerticalLayoutUI;
            vLayout->SetBkColor(0xFFFFFFFF);
            vLayout->EnableScrollBar(true);
            m_pm.AttachDialog(vLayout);
            QkString buffer;
            for (size_t i = 0; i < NamedDemos.size(); i++)
            {
                LPCTSTR name = (LPCTSTR)NamedDemos[i](NULL, NULL);

                Button* btn = new Button;
                btn->SetText(name);
                buffer.Format(L"%d", i);
                btn->SetName(buffer);
                btn->SetType(L"push");
                btn->SetInset({10, 6, 10, 6});
                btn->SetAutoCalcHeight(true);
                CControlUI* pad = new CControlUI;
                pad->SetFixedHeight(10);
                vLayout->Add(pad);
                vLayout->Add(btn);
            }
            vLayout->SetInset({10, 6, 10, 6});


            // 添加Notify事件接口
            m_pm.AddNotifier(this);
            // 窗口初始化完毕
            InitWindow();
            return 0;
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
            int demoIndex = ParseInt(ud);
            if (demoIndex>=0 && demoIndex<NamedDemos.size())
            {
                NamedDemos[demoIndex](CPaintManagerUI::GetInstance(), 0);
            }
        }
    };

    CWindowWnd* CreateDemoBox()
    {
        return new UniDemoBox;
    }

}