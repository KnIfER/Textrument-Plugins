/****** SKia Draw String Demo *********
* Skia 绘制 Hello World 之展示程序。
*   —— 由 KnIfER 整理。
**************************************/

#include "include/utils/SkRandom.h"
#include "include/utils/SkRandom.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkPath.h"


#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#include "include/core/SkFont.h"
#include "include/core/SkCanvas.h"

#include <windows.h>
#include <tchar.h>

#include "tools/sk_app/Window.h"

HWND _hWnd;

int drawCnt = 0 ;


// |0|不绘制 |1|原生绘制 |2|Skia绘制
int Paint_Type = 2;

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

    canvas->drawString(string, 1, 16, font, textpaint);


    canvas->flush(); //这函数是用于GPU surface。这里其实不需要。例如使用OpenGL的上下文。
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch(message)
    {
    case WM_PAINT:
    {
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
                    int bmpw = rt.right - rt.left;
                    int bmph = rt.bottom - rt.top;

                    const size_t bmpSize = sizeof(BITMAPINFOHEADER) + bmpw * bmph * sizeof(uint32_t);
                    BITMAPINFO* bmpInfo = (BITMAPINFO*)new BYTE[bmpSize]();
                    bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmpInfo->bmiHeader.biWidth = bmpw;

                    // biHeight为正时，bmpInfo->bmiColors像素是从图片的左下角开始。
                    // biHeight为负时，bmpInfo->bmiColors像素是从图片的左上角开始。
                    // 这是BMP的格式中指定。而skia绘制需要从上到下。
                    bmpInfo->bmiHeader.biHeight = -bmph;

                    bmpInfo->bmiHeader.biPlanes = 1;
                    bmpInfo->bmiHeader.biBitCount = 32;  //图片每像素32位
                    bmpInfo->bmiHeader.biCompression = BI_RGB;
                    void* pixels = bmpInfo->bmiColors;  //图片像素位置指针

                                                        // kBGRA_8888_SkColorType，这参数决定SkCanvas绘图像素的格式
                                                        // BGRA，ARGB，就好象cpu大端，小端之类的。
                    SkImageInfo info = SkImageInfo::Make(bmpw, bmph,
                        kBGRA_8888_SkColorType,kPremul_SkAlphaType);

                    sk_sp<SkSurface> surface = SkSurface::MakeRasterDirect(info, pixels, bmpw * sizeof(uint32_t));
                    SkCanvas* canvas = surface->getCanvas();

                    canvas->clear(SK_ColorWHITE); //填充白色背景

                    Draw(canvas, bmpw, bmph); //这里调用函数，传入指针绘图。

                    StretchDIBits(  hdc, 0, 0, bmpw, bmph,
                        0, 0, bmpw, bmph,
                        pixels, bmpInfo,
                        DIB_RGB_COLORS, SRCCOPY );  //整张图绘制到DC

                    delete[] bmpInfo;
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


int HelloWorld_RunMain(HINSTANCE hInstance, HWND hParent)
{
    WNDCLASSEX clazz        = {0};
    clazz.cbSize            = sizeof(WNDCLASSEX);

    clazz.style = CS_HREDRAW | CS_VREDRAW;
    clazz.lpfnWndProc = WndProc;
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

    _hWnd = CreateWindow(clazz.lpszClassName,                          /* the '...if(FAILED(...' part will perform the 'return 2;' command if there was an error during the creation of the window.*/
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

    if(FAILED(_hWnd))
        return 2;

    //MSG msg{0};
    //
    //while( GetMessage( &msg, NULL, 0, 0 ) )
    //{
    //    DispatchMessage( &msg );
    //}

    return 0;
}