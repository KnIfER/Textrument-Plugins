/****** SKia Decode PNG Region Test *********
* 
* Load Big Image : Success. But Slow On Resizing.
* 
**************************************/
#include "..\DuiLib\UIlib.h"

#include "..\DuiLib\ControlEx\SkImageView.h"
#include "..\DuiLib\Core\UIManager.h"
#include "../DuiLib/Core/InsituDebug.h"

using namespace DuiLib;

namespace SK_IMG_VIEW {
    class ImageViewMainForm : public WindowImplBase, public INotifyUI
    {
    public:
        ImageViewMainForm() { };     

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

        bool IsWindowLess() const override
        {
            return false;
        }

        LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override
        {
            PostQuitMessage(0);
            bHandled = TRUE;
            return 0;
        }

        void InitWindow() override
        {
            ivTest = dynamic_cast<SkImageView*>(m_pm.FindControl(L"ivTest"));
            if (ivTest)
            {
                //ivTest->LoadImage("D:\\test.png");
                ivTest->LoadImage("D:\\Large-Sample-Image-download-for-Testing.jpg");
            }
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
            return _T("testImageView.xml");
        }

        void Notify( TNotifyUI &msg ) override
        {
            if (msg.sType==L"click")
            {
            }
            // WindowImplBase::Notify(msg);
        }

    private:
        SkImageView* ivTest;
    };
}

using namespace SK_IMG_VIEW;

int SKIMG_VIEW_RunMain(HINSTANCE hInstance, HWND hParent)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    ImageViewMainForm* pFrame = new ImageViewMainForm();
    if( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T("Skia ImageView For The DuiLib !!!"), UI_WNDSTYLE_FRAME|WS_CLIPCHILDREN, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);
    return 0;
}