/*
* Android-Like ListView Demo By KnIfER
* 
*/
#include "pch.h"
#include "../DuiLib/Utils/stb_image.h"

namespace ImageList{

QkString Name = L"图片列表";


//#define USEDDBLT

HBITMAP hBitmap;
HBITMAP hDBBitmap = 0;
BITMAPINFO bmpInfo;
LPBYTE pDest;
int _srcWidth;
int _srcHeight;

int _translationX = 0;
int _translationY = 0;

HDC hdcMem;

#define USEDDBLT

class ImageView : public CControlUI{
public:
    ImageView(){};
    ~ImageView(){};

    void PaintForeImage(HDC hDC) override
    {

        RECT rcItem = m_rcItem;

        int drawWidth = rcItem.right - rcItem.left;
        int drawHeight = rcItem.bottom - rcItem.top;

        float scaleWidth = drawWidth*1.0/_srcWidth;
        float scaleHeight = drawHeight*1.0/_srcHeight;
        float _minScale = min(scaleWidth, scaleHeight);

        bool _bFit = true;

        float _scale = 1;
        float scale = _scale;
        if (scale<_minScale || _bFit)
        {
            _scale = scale = _minScale;
            //_translationX = _translationY = 0;
            if (!_bFit)
            {
                _bFit = true;
            }
            _translationX = (drawWidth-_srcWidth*_minScale)/2;
            _translationY = (drawHeight-_srcHeight*_minScale)/2;
        }
        scale = 1 + scale - _minScale;


        int left=0;
        int top=0;

        int calcW=drawWidth;
        int calcH=scaleWidth*_srcHeight;
        bool fitWidth = true;

        if (calcH>drawHeight)
        {
            calcW=scaleHeight*_srcWidth;
            calcH=drawHeight;
            left = (drawWidth-calcW)/2;
            fitWidth = false;
        }
        else 
        {
            top = (drawHeight-calcH)/2;
        }


        int tX, tY, W, H;

        if (_bFit)
        {
            W = _srcWidth/scale;
            H = _srcHeight/scale;
            tX = -(_srcWidth-_srcWidth*scale)/2/scale;
            tY = -(_srcHeight-_srcHeight*scale)/2/scale;
        }
        else
        {
            scale = 1 + scale - _minScale;
            scale = _scale/_minScale;

            calcW = min(scale*calcW, drawWidth);
            calcH = min(scale*calcH, drawHeight);

            scale = _scale;

            if (fitWidth)
            {
                top = (drawHeight-calcH)/2;
            }
            else
            {
                left = (drawWidth-calcW)/2;
            }

            W = calcW/scale;
            H = calcH/scale;
            tX = -_translationX/_scale;
            tY = -_translationY/_scale;
            if (tX<0)
            {
                tX=0;
                _translationX = -tX*scale;
            }
            if (tX+W>_srcWidth)
            {
                tX=_srcWidth-W;
                _translationX = -tX*scale;
            }
            if (tY<0)
            {
                tY=0;
                _translationY = -tY*scale;
            }
            if (tY+H>_srcHeight)
            {
                tY=_srcHeight-H;
                _translationY = -tY*scale;
            }

            tY = -tY + _srcHeight - H;
        }


        SetStretchBltMode(hDC, COLORONCOLOR);

#ifdef USEDDBLT
        if (!hdcMem)
        {
            hdcMem = CreateCompatibleDC(NULL);
            {
                drawWidth = _srcWidth;
                drawHeight = _srcHeight;
                hDBBitmap = CreateDIBitmap(hDC, &bmpInfo.bmiHeader,CBM_INIT, pDest, &bmpInfo, DIB_RGB_COLORS);
                HBITMAP oldbmp = (HBITMAP)::SelectObject(hdcMem, hDBBitmap);
            }
        }
        StretchBlt(hDC, rcItem.left+left, rcItem.top+top, calcW, calcH
            , hdcMem, tX, tY, W, H, SRCCOPY);
#else

        StretchDIBits(  hDC, rcItem.left+left, rcItem.top+top, calcW, calcH,
            tX, tY, W, H,
            pDest, &bmpInfo,
            DIB_RGB_COLORS, SRCCOPY );
         

        //StretchDIBits(  hDC, 0, 0, 100, 100,
        //    0, 0, 100, 100,
        //    pDest, &bmpInfo,
        //    DIB_RGB_COLORS, SRCCOPY );
#endif


#define 调试图片控件
#ifdef 调试图片控件
        int fontHeight = 20;
        int lnCnt=0;
        int baseY = rcItem.top + 5;
        QkString text;
        text.Format(L"scale=%f", _scale);
        TextOut(hDC, 5, baseY+fontHeight*(lnCnt++), text.GetData(), text.GetLength());
        text.Format(L"translation= %d, %d", _translationX, _translationY);
        TextOut(hDC, 5, baseY+fontHeight*(lnCnt++), text.GetData(), text.GetLength());
        text.Format(L"drawSize= %d, %d", drawWidth, drawHeight);
        TextOut(hDC, 5, baseY+fontHeight*(lnCnt++), text.GetData(), text.GetLength());
#endif
    }

};


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


    bool copyimage(const wchar_t* filename)
    {
        //initialize Gdiplus once:
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        bool result = false;
        Gdiplus::Bitmap *gdibmp = Gdiplus::Bitmap::FromFile(filename);
        LogIs(2, "succ= %d  ", gdibmp);
        if (gdibmp)
        {
            HBITMAP hbitmap;
            gdibmp->GetHBITMAP(0, &hbitmap);
            if (OpenClipboard(NULL))
            {
                EmptyClipboard();
                DIBSECTION ds;
                if (GetObject(hbitmap, sizeof(DIBSECTION), &ds))
                {
                    HDC hdc = GetDC(HWND_DESKTOP);
                    //create compatible bitmap (get DDB from DIB)
                    HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
                        ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
                    ReleaseDC(HWND_DESKTOP, hdc);
                    SetClipboardData(CF_BITMAP, hbitmap_ddb);
                    DeleteObject(hbitmap_ddb);
                    result = true;
                }
                CloseClipboard();
            }

            //cleanup:
            DeleteObject(hbitmap);  
            delete gdibmp;              
        }
        return result;
    }


    bool copyimage(HBITMAP hbitmap)
    {
        if (hbitmap && OpenClipboard(NULL)) {
            EmptyClipboard();
            SetClipboardData(CF_BITMAP, hbitmap);
            CloseClipboard();
            return 1;
        }
        return 0;
    }

    bool copyimage_1(HBITMAP hbitmap)
    {
        if (hbitmap)
        {
            DIBSECTION ds;
            if (GetObject(hbitmap, sizeof(DIBSECTION), &ds))
            {
                HDC hdc = GetDC(HWND_DESKTOP);
                //create compatible bitmap (get DDB from DIB)
                HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
                    ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
                ReleaseDC(HWND_DESKTOP, hdc);
                copyimage(hbitmap_ddb);
                DeleteObject(hbitmap_ddb);
            }
            return 1;
        }
        return 0;
    }


    void copyimage_1(const char* psz_filepath)
    {
        int x=0,y=0,n;
        unsigned char *pImage = stbi_load(psz_filepath, &x, &y, &n, 0);

        int bytesPerPixel = 3;
        auto & info = bmpInfo;
        ::ZeroMemory(&info, sizeof(BITMAPINFO));
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = x;
        info.bmiHeader.biHeight = -y;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biCompression = BI_RGB;
        info.bmiHeader.biBitCount = bytesPerPixel*8;
        info.bmiHeader.biSizeImage = x * y * bytesPerPixel;


        _srcWidth = x;
        _srcHeight = y;

        pDest = NULL;
        HBITMAP hBitmap = ::CreateDIBSection(NULL, &info, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
        //hBitmap = CreateBitmap(100, 100, 1, 24, NULL);
        
        BYTE alphaByte;
        float alpha;
        //if(false)
        for( int i = 0; i < x * y; i++ ) 
        {
            pDest[i*bytesPerPixel] = pImage[i*n + 2];
            pDest[i*bytesPerPixel + 1] = pImage[i*n + 1];
            pDest[i*bytesPerPixel + 2] = pImage[i*n]; 
            if (bytesPerPixel>3)
            {
                pDest[i*bytesPerPixel + 3] = alphaByte = pImage[i*n + 3];
                if (alphaByte<255)
                {
                    alpha = alphaByte*1.f/255;
                    //pDest[i*bytesPerPixel] *= alpha;
                    //pDest[i*bytesPerPixel + 1] *= alpha;
                    //pDest[i*bytesPerPixel + 2] *= alpha; 
                }
            }
        }
         
        if(hBitmap) {
            //stbi_image_free(pImage);

            //bool succ = copyimage_1(hBitmap);

            //DeleteObject(hBitmap);  

            //LogIs(2, "succ = %d, n= %d", succ, n);
        }

         //copyimage(L"G:\\IMG\\123.png");
    }




    void InitWindow() override
    {
        viewTemplate = builder.Create(L"ListViewDemo_item.xml", 0, 0, &m_pm);

        copyimage_1("G:\\IMG\\123.png");
        //copyimage_1("F:\\IMG\\123.jpg");

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

            //pList->_bUseSmoothScroll = false;
        }

        //CHorizontalLayoutUI* menuBar = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("menuBar")));
        //for (size_t i = 0; i < 10; i++)
        //{
        //    auto menu = builder.Create(L"menu_item.xml", 0, 0, &m_pm);
        //    menu->SetFixedWidth(0);
        //    menu->GetText().Format(L"菜单#%d", i);
        //    menu->GetText().Format(L"文件#%d", i);
        //    menu->GetText().Format(L"文件(&F)", i);
        //    menuBar->Add(menu);
        //}
    }

    size_t GetItemCount()
    {
        //return 10;
        return 1000;
        // return 10000000;
    }

    CControlUI* CreateItemView(CControlUI* view, int type)
    {
        //return builder.Create(L"ListViewDemo_item.xml", 0, 0, &m_pm);
        CControlUI * ret = new ImageView;
        ret->SetBkColor(0x5800FFFF);
        ret->SetBorderSize({5,5,5,5});
        ret->SetBorderColor(0x58F00FFF);
        ret->SetFixedHeight(500);
        return ret;
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
            label.Append(L"\r\n哈哈哈无语了");
            btn->SetText(label);
            btn->SetNeedAutoCalcSize();

            //btn->SetBkImage();

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