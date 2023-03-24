/****** SKia Decode PNG Region Test *********
* 
* Region Decode : Failed, It's "Unimplemeted". 
*   https://groups.google.com/g/skia-discuss/c/yKbvJrdxmkY
* 
* Load Big Image : Success.
* 
**************************************/
#include "include/utils/SkRandom.h"
#include "include/utils/SkRandom.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkPath.h"
#include "include/core/SkImage.h"
#include "include/core/SKImageInfo.h"
#include "include/core/SkImageGenerator.h"

#include "include/codec/SkCodec.h"

#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#include "include/core/SkFont.h"
#include "include/core/SkCanvas.h"

#include <windows.h>
#include <tchar.h>

#include "tools/sk_app/Window.h"


#include "..\DuiLib\UIlib.h"

#include "..\DuiLib\Core\UIManager.h"

#include "../DuiLib/Core/InsituDebug.h"

using namespace DuiLib;

namespace SK_IMG_RNG {

HWND _hWnd;

int drawCnt = 0 ;

size_t bmpSize = 0;

DWORD lastDrawTm;

BITMAPINFO* bmpInfo = NULL;

SkBitmap* skBitmap;

sk_sp<SkImage> skImage;

void Draw(SkCanvas* canvas,int w,int h) {
    //canvas->scale(0.05f, 0.05f);
    //canvas->scale(10, 10);

    SkPaint paint;
    paint.setStrokeWidth(1);
    paint.setARGB(0xff, 0xff, 0, 0);

    SkRect rect{0,0,100,100};
    SkSamplingOptions options(SkFilterMode::kNearest, SkMipmapMode::kNearest);
   // canvas->drawImageRect(skImage, rect, options, 0);
    canvas->drawImage(skImage, 0, 0);

    auto pFace = SkTypeface::MakeFromName("宋体", SkFontStyle::Normal());
    SkFont font;
    font.setSize(28);
    font.setTypeface(pFace);
    font.setEmbolden(true);
    SkPaint textpaint;
    textpaint.reset();
    textpaint.setColor(SkColor(0xffff0000));
    textpaint.setAntiAlias(true);

    SkString string("加载图片 #");

    string.appendS32(drawCnt++);

    string.appendf(" bmpSize=%.2f", bmpSize*1.0/1024/1024);

    canvas->drawString(string, 1, 28, font, textpaint);

    canvas->flush();
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch(message)
    {
    case WM_PAINT:
    {
        if (!::IsIconic(_hWnd))
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
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
            EndPaint(hWnd, &ps);
        }
        break;
    }
    case WM_SIZE:
    {
        ::InvalidateRect(_hWnd, NULL, FALSE);
        //::UpdateWindow(_hWnd);
        break;
    }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

QkString Name = L"四、SkImageRegion";

LRESULT RunTest(HINSTANCE hInstance, HWND hParent)
{
    if (hInstance==NULL) return (LRESULT)Name.GetData();
    DWORD fileLength;
    char* memFile;
#define 从文件加载二进制数据
#ifdef 从文件加载二进制数据
    FILE *fp;

    fopen_s(&fp, "D:\\test.png", "rb");

    struct stat file_state;
    int fd = _fileno(fp);
    fstat(fd, &file_state);
    //fseek(fp, 0, SEEK_SET);
    fileLength = file_state.st_size;

    memFile = new char[fileLength];

    size_t readCount = fread(memFile, 1, fileLength, fp);
    LogIs(2, "readCount=%ld, %ld", readCount, fileLength);

    //char* readBuf = memFile;
    //size_t remainingBytes = fileLength;
    //while(remainingBytes > 0) {
    //    int readCount = fread(readBuf, 1, remainingBytes, fp);
    //    if (readCount<=0) {
    //        break;
    //    }
    //    remainingBytes -= readCount;
    //    readBuf += readCount;
    //}

    //LogIs(2, "st_size=%ld", fileLength);
#else
    //CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
    //CPaintManagerUI::ExtractItem(TEXT("skin\\QQRes\\bg12.png"), &memFile, fileLength);
    //CPaintManagerUI::ExtractItem(TEXT("skin\\QQRes\\bg0.png"), &memFile, fileLength);
    //CPaintManagerUI::ExtractItem(TEXT("skin\\QQRes\g0.png"), &memFile, fileLength);
    //CPaintManagerUI::ExtractItem(TEXT("winbk.bmp"), &memFile, fileLength);
#endif


    sk_sp<SkData> data = SkData::MakeWithoutCopy(memFile, fileLength);

    std::unique_ptr<SkStream> stream = SkStream::MakeFromFile("D:\\Large-Sample-Image-download-for-Testing.jpg");


    //auto codec = SkCodec::MakeFromData(data);

    auto codec = SkCodec::MakeFromStream(std::move(stream));


   // codec->startScanlineDecode

    if (!codec) {
        LogIs(2, "FAILED DECODING FILE!");
    }
    
    SkImageInfo codecInfo = codec->getInfo();
    
    auto alphaType = codecInfo.isOpaque() ? kOpaque_SkAlphaType : kPremul_SkAlphaType;
    
    auto outInfo = SkImageInfo::Make(codecInfo.width()
        , codecInfo.height()
        , kN32_SkColorType, alphaType);

    //outInfo = outInfo.makeWH(codecInfo.width()/2, codecInfo.height()/2);

    outInfo = codecInfo.makeWH(codecInfo.width(), codecInfo.height());


    SK_IMG_RNG::skBitmap = new SkBitmap();

    char* pixels = new char[outInfo.width()*outInfo.height()*32];


    SK_IMG_RNG::skBitmap->setInfo(outInfo, outInfo.width()*32);


    SK_IMG_RNG::skBitmap->setPixels(pixels);




    auto rngOpt = new SkCodec::Options{};

    

    //rngOpt->fSubset = new SkIRect{0,0,100,100};


    auto decodeResult = codec->getPixels(outInfo
        , pixels
        , outInfo.width()*32, rngOpt);

    SK_IMG_RNG::skBitmap->setImmutable();

    SK_IMG_RNG::skImage = SK_IMG_RNG::skBitmap->asImage();

    LogIs(2, "codecInfo=%dx%d, decodeResult=%ld", codecInfo.width(), codecInfo.height(), decodeResult);

    WNDCLASSEX clazz        = {0};
    clazz.cbSize            = sizeof(WNDCLASSEX);

    clazz.style = CS_HREDRAW | CS_VREDRAW;
    clazz.lpfnWndProc = SK_IMG_RNG::WndProc;
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

    SK_IMG_RNG::_hWnd = CreateWindow(clazz.lpszClassName,     
        L"Skia Win32 Demo",                                
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,                    
        0,                                                 
        0,                                                 
        640,                                               
        480,                                               
        hParent,                                           
        0,                                                 
        hInstance,                                         
        NULL);

    SetWindowLongPtr(SK_IMG_RNG::_hWnd, GWLP_WNDPROC, (LONG_PTR)SK_IMG_RNG::WndProc);

    if(FAILED(SK_IMG_RNG::_hWnd))
        return 2;

    return 0;
}


static int _auto_reg = AutoRegister(RunTest);
}
