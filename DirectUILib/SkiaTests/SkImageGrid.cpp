/****** FT Explorer demo_0 *********
* 
**************************************/
#include "pch.h"
using namespace DuiLib;

namespace SkImageGrid {
    class ImageViewMainForm : public WindowImplBase, public INotifyUI, public ListViewAdapter
    {
    public:
        ImageViewMainForm() {
            _frameLess = false;
        };     
        LPCTSTR GetWindowClassName() const override
        { 
            return _T("ImageViewMainForm"); 
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

        QkString GetSkinFile() override
        {
            return _T("testImageGrid.xml");
        }

        void Notify( TNotifyUI &msg ) override
        {
            if (msg.sType==L"click")
            {
            }
            WindowImplBase::Notify(msg);
        }

        void InitWindow() override
        {
            pList = dynamic_cast<ListView*>(m_pm.FindControl(L"lv"));
            if (pList)
            {
                pList->SetAdapter(this);
                pList->SetItemHeight(300);
                pList->SetItemHeight(-2);
                pList->SetSmoothScrollMode(true, false);
            }
        }

        size_t GetItemCount() {
            return 3;
        }
        CControlUI* CreateItemView(CControlUI* view, int type) {
            //lxx(CreateItemView)
            auto ret = builder.Create(L"testImageGridRow.xml", 0, 0, &m_pm);
            return ret;
            //return ((Button*)viewTemplate)->Duplicate();
        }
        void OnBindItemView(CControlUI* view, size_t index) {
            CControlUI* hbox = dynamic_cast<CControlUI*>(view);

            for (size_t i = 0; i < hbox->GetCount(); i++)
            {
                ImageView* img = dynamic_cast<ImageView*>(hbox->GetItemAt(i));
                if (img)
                {
                    img->SetInset({10, 10, 10, 10});
                    //img->SetBorderSize({10, 10, 10, 10});
                    //img->LoadImageFile("D:\\Large-Sample-Image-download-for-Testing.webp");
                    img->LoadImageFile("D:\\女大学生在线求野王哥哥带飞～溪溪甜又乖.webp");
                }
            }
        }
    private:
        CDialogBuilder builder;
        ListView* pList;
    };

    QkString Name = L"九、SkImageGrid";

    LRESULT RunTest(HINSTANCE hInstance, HWND hParent)
    {
        if (hInstance==NULL) return (LRESULT)Name.GetData();
        CPaintManagerUI::SetInstance(hInstance);
        CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

        CControlFactory::GetInstance()->RegistControl(TEXT("ImageView"), ImageView::CreateControl);
        //CControlFactory::GetInstance()->RegistControl(TEXT("ImageView"), SkImageView::CreateControl);

        HRESULT Hr = ::CoInitialize(NULL);
        if( FAILED(Hr) ) return 0;

        ImageViewMainForm* pFrame = new ImageViewMainForm();
        if( pFrame == NULL ) return 0;
        pFrame->Create(NULL, _T("Skia ImageView Grid"), UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW , 0, 0, 800, 500);
        pFrame->CenterWindow();
        pFrame->ShowWindow(true);
        return 0;
    }

    static int _auto_reg = AutoRegister(RunTest);

}