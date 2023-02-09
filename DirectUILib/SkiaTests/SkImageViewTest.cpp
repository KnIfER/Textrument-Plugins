/****** SKia Decode PNG Region Test *********
* 
* ImageView. Pro And Lite. 
* 
**************************************/
#include "..\DuiLib\UIlib.h"
#include "..\DuiLib\ControlPro\ImageViewPro.h"
#include "../DuiLib/Core/InsituDebug.h"
using namespace DuiLib;

namespace SK_IMG_VIEW {
    class ImageViewMainForm : public WindowImplBase, public INotifyUI
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

        void InitWindow() override
        {
            ivTest = dynamic_cast<ImageView*>(m_pm.FindControl(L"ivTest"));
            if (ivTest)
            {
                 ivTest->LoadImageFile("D:\\test.png");
               // ivTest->LoadImageFile("D:\\Large-Sample-Image-download-for-Testing.jpg");
                //ivTest->LoadImageFile("C:\\Users\\TEST\\Pictures\\MTkzMDI1NDI2XzExNTk2Njg3MDIx_0.webp");
            }
        }

        QkString GetSkinFile() override
        {
            return _T("testImageView.xml");
        }

        void Notify( TNotifyUI &msg ) override
        {
            if (msg.sType==L"click")
            {
            }
            WindowImplBase::Notify(msg);
        }

    private:
        ImageView* ivTest;
    };
}

using namespace SK_IMG_VIEW;

int SKIMG_VIEW_RunMain(HINSTANCE hInstance, HWND hParent)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    CControlFactory::GetInstance()->RegistControl(TEXT("ImageView"), ImageViewPro::CreateControl);

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    ImageViewMainForm* pFrame = new ImageViewMainForm();
    if( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T("Skia ImageView For The DuiLib !!!"), UI_WNDSTYLE_FRAME|WS_CLIPCHILDREN, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);
    return 0;
}