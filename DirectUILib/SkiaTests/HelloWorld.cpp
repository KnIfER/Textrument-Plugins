/****** SKia Draw String Demo *********
* 
* Skia 展示程序之绘制 Hello World 字符串。
* 
* 参考资料  https://blog.csdn.net/weixin_33721427/article/details/88738231
* 
*   —— 由 KnIfER 整理。
**************************************/
#include "pch.h"


namespace SK_HELLO {

HWND _hWnd;

int drawCnt = 0 ;


// |0|不绘制 |1|原生绘制 |2|Skia绘制
int Paint_Type = 2;

size_t bmpSize = 0;

DWORD lastDrawTm;
sk_sp<SkPicture> svgPicture = 0;
sk_sp<SkSVGDOM> dom;
SkRandom random;

BITMAPINFO* bmpInfo = NULL;

sk_sp<SkPicture> getPictureFromSVG(sk_sp<SkSVGDOM> & dom, int width, int height) {
    SkPictureRecorder recorder;
    SkCanvas* canvas = recorder.beginRecording(width, height);
    dom->render(canvas);
    return recorder.finishRecordingAsPicture();
}

void Draw(SkCanvas* canvas,int w,int h) {
    SkPaint paint;
    paint.setStrokeWidth(1);
    paint.setARGB(0xff, 0xff, 0, 0);
    //for (int i = 0; i < h; i++) canvas->drawPoint(0, i, paint);
    //for (int i = w; i > 0 ; i--) canvas->drawPoint(i-1, h-1, paint);

    SkFont font;
    font.setSize(16);
    SkPaint textpaint;
    textpaint.reset();
    font.setSize(16);
    textpaint.setColor(SkColor(0xffff0000));
    textpaint.setAntiAlias(true);

    SkString string("Hello Skia World #");

    string.appendS32(drawCnt++);

    string.appendf(" bmpSize=%.2f", bmpSize*1.0/1024/1024);

    canvas->drawString(string, 1, 16, font, textpaint);


    // Load the SVG file
    //sk_sp<skottie::Animation> animation = skottie::Animation::Builder()
    //        .makeFromFile("D:\\domain.svg");

    //if (animation)
    //{
    //    // Draw the SVG onto the canvas
    //    auto rc = SkRect::MakeXYWH(0, 0, 100, 100);
    //    animation->render(canvas, &rc);
    //}


    if (dom)
    {
        dom->getRoot();
        dom->setContainerSize(SkSize::Make(w, h));
        //svgPicture = getPictureFromSVG(dom, w, h);

        // Find the path node by ID
       sk_sp<SkSVGNode> node = *dom->findNodeById("path");
       if (node)
       {
           // Set the fill color attribute
           // SkColorSetA(SkColors::kYellow.toSkColor(), 0xFF)
           //SkString colorStr("#ffff");
           //colorStr.appendHex(rand()%0xff);
           //if(colorStr.size()<7) colorStr.append("0");
           //lxx(ss, STRW(colorStr.c_str()))

           SkColor color = SkColorSetARGB(0xFF, random.nextRangeScalar(0x77, 0xEE), random.nextRangeScalar(0x99, 0xFF), random.nextRangeScalar(0x77, 0xEE));
           SkString colorString;
           colorString.printf("#%02X%02X%02X", SkColorGetR(color), SkColorGetG(color), SkColorGetB(color));

           node->setAttribute("fill", colorString.c_str());
       }

       dom->render(canvas);
    }
    //if(svgPicture) {
    //    SkPaint paint;
    //    paint.setColorFilter(SkColorFilters::Blend(
    //        SkColors::kYellow.toSkColor(),      
    //        SkBlendMode::kSrcIn)); 

    //    canvas->drawPicture(svgPicture, 0, &paint);
    //}

    canvas->flush(); //这函数是用于GPU surface。这里其实不需要。例如使用OpenGL的上下文。
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch(message)
    {
    case WM_PAINT:
    {
        //Paint_Type = 0;
        if (Paint_Type && !::IsIconic(_hWnd))
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            /////////////////////////////////////////////////////
            //
            //
            //
            /////////////////////////////////////////////////////
            if (Paint_Type==1) // 原生绘制 Hello World 字符串。
            {
                TCHAR greeting[] = _T("Hello, World!");
                TextOut(hdc, 5, 5, greeting, _tcslen(greeting));
            }
            /////////////////////////////////////////////////////
            //
            //
            //
            //
            /////////////////////////////////////////////////////
            if (Paint_Type==2) // Skia 绘制 Hello World 字符串。
            {
                RECT rt;
                GetClientRect(hWnd, &rt);
                if (!::IsRectEmpty(&rt))
                {
                    int drawWidth = rt.right - rt.left;
                    int drawHeight = rt.bottom - rt.top;

                    size_t newSz = sizeof(BITMAPINFOHEADER) + drawWidth * drawHeight * sizeof(uint32_t);
                    if (newSz>bmpSize)
                    {
                        if (bmpInfo)
                        {
                            delete[] bmpInfo;
                        }
                        bmpInfo = (BITMAPINFO*)new BYTE[bmpSize=newSz]();
                    }
                    bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmpInfo->bmiHeader.biWidth = drawWidth;
                    bmpInfo->bmiHeader.biHeight = -drawHeight;  // top-down image
                    bmpInfo->bmiHeader.biPlanes = 1;
                    bmpInfo->bmiHeader.biBitCount = 32;   // 32位
                    bmpInfo->bmiHeader.biCompression = BI_RGB;

                    void* pixels = bmpInfo->bmiColors; 

                    // BGRA
                    SkImageInfo info = SkImageInfo::Make(drawWidth, drawHeight, kBGRA_8888_SkColorType,kPremul_SkAlphaType); 

                    sk_sp<SkSurface> surface = SkSurface::MakeRasterDirect(info
                        , pixels
                        , drawWidth * sizeof(uint32_t));

                    SkCanvas* canvas = surface->getCanvas();

                    canvas->clear(SK_ColorWHITE);

                    Draw(canvas, drawWidth, drawHeight);

                    StretchDIBits(  hdc, 0, 0, drawWidth, drawHeight,
                        0, 0, drawWidth, drawHeight,
                        pixels, bmpInfo,
                        DIB_RGB_COLORS, SRCCOPY );  //整张图绘制到DC
                }
            }
            EndPaint(hWnd, &ps);
        }
        break;
    }
    case WM_SIZE:
    {
        // 如果这里不触发重绘，那么缩放时会有重影。
        ::InvalidateRect(_hWnd, NULL, FALSE);
        //::UpdateWindow(_hWnd);
        break;
    }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

QkString Name = L"一、hello world";

LRESULT RunTest(HINSTANCE hInstance, HWND hParent)
{
    if (hInstance==NULL) return (LRESULT)Name.GetData();
    WNDCLASSEX clazz        = {0};
    clazz.cbSize            = sizeof(WNDCLASSEX);

    clazz.style = CS_HREDRAW | CS_VREDRAW;
    clazz.lpfnWndProc = SK_HELLO::WndProc;
    clazz.cbClsExtra = 200;
    clazz.cbWndExtra = 200;
    clazz.hInstance = hInstance;
    clazz.hIcon = NULL;
    clazz.hCursor = LoadCursor(NULL, IDC_ARROW);
    clazz.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // COLOR_BACKGROUND
    clazz.lpszMenuName = NULL;
    clazz.lpszClassName = L"SkiaWin32";

    if( FAILED(RegisterClassEx(&clazz)) )
        return 1;



    SkString path("D:\\domain.svg");
    auto stream = SkFILEStream::Make("D:\\domain.svg");
    if (stream) {
        dom = SkSVGDOM::MakeFromStream(*stream);
    }

    SK_HELLO::_hWnd = CreateWindow(L"SkiaWin32",                          /* the '...if(FAILED(...' part will perform the 'return 2;' command if there was an error during the creation of the window.*/
        L"Skia Win32 Demo",                                   /* This is the actual name shown in the 'title bar' of the window. The L before the quotes, simplified, means its UNICODE.    */
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,                        /* WS_OVERLAPPEDWINDOW gives the window a title bar, a window menu, a sizing border, and minimize and maximize buttons.        */
        0,                                                    /* Initial X position of the window.                                                                                        */
        0,                                                                     /* Initial Y position of the window.                                                                                        */
        640,                                                                 /* Initial Width of the window.                                                                                                */
        480,                                                                 /* Initial Height of the window.                                                                                            */
        hParent,                                                       /* Here could be a 'handle' to the parent or owner window of this window.                                                    */
        0,                                                       /* Here could be a 'handle' to a menu (usually constructed with the GUI tools of Visual Studio).                            */
        hInstance,                                               /* The 'handle' or 'unique id' of the instance of this program that will be associated with the to-be-created window(class).*/
        NULL);

    SetWindowLongPtr(SK_HELLO::_hWnd, GWLP_WNDPROC, (LONG_PTR)SK_HELLO::WndProc);

    if(FAILED(SK_HELLO::_hWnd))
        return 2;

    return 0;
}

static int _auto_reg = AutoRegister(RunTest);

}


