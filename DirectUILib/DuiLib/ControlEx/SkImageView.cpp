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
#include <vfw.h>
#include <tchar.h>

#include "SkImageView.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL(SkImageView)

	HDC hdcMem = NULL;
	HBITMAP hBitmap = NULL;
	HDRAWDIB m_hDrawDib = NULL;

	SkImageView::SkImageView():CControlUI()
	{

		m_hDrawDib = DrawDibOpen();

	}
	
	int SkImageView::LoadImage(CHAR* path) 
	{
		std::unique_ptr<SkStream> stream = SkStream::MakeFromFile(path);

		auto codec = SkCodec::MakeFromStream(std::move(stream));

		if (!codec)
		{
			return -1;
		}
		skBitmap = NULL;
		skImage = NULL;
		SkImageInfo codecInfo = codec->getInfo();
		auto alphaType = codecInfo.isOpaque() ? kOpaque_SkAlphaType : kPremul_SkAlphaType;

		_srcWidth = codecInfo.width();
		_srcHeight = codecInfo.height();

		auto outInfo = SkImageInfo::Make(_srcWidth
			, _srcHeight
			, kN32_SkColorType, alphaType);

		outInfo = codecInfo.makeWH(_srcWidth, _srcHeight);

		skBitmap = new SkBitmap();

		size_t bytesAllocation = outInfo.width()*outInfo.height()*32;
		if (bytesAllocation>pxSize)
		{
			if (pixels)
			{
				char* newPx = (char*)realloc(pixels, bytesAllocation);
				if (!newPx)
				{
					delete[] pixels;
					pixels = nullptr;
				}
				else
				{
					pixels = newPx;
				}
			}
			if (!pixels)
			{
				pixels = new char[bytesAllocation];
			}
		}
		skBitmap->setInfo(outInfo, _srcWidth*32);
		skBitmap->setPixels(pixels);


		auto rngOpt = new SkCodec::Options{};
		//rngOpt->fSubset = new SkIRect{0,0,100,100};
		auto decodeResult = codec->getPixels(outInfo
			, pixels
			, _srcWidth*32, rngOpt);

		if (decodeResult==SkCodec::Result::kSuccess)
		{
			skBitmap->setImmutable();
			skImage = skBitmap->asImage();


			int drawWidth, drawHeight;

			if (!hBitmap)
			{
				drawWidth = _srcWidth;
				drawHeight = _srcHeight;

				//drawWidth=100;
				//drawHeight=100;

				size_t newSz = sizeof(BITMAPINFOHEADER) + drawWidth * drawHeight * sizeof(uint32_t);
				if (newSz>bmpSize)
				{
					if (bmpInfo)
					{
						delete[] bmpInfo;
					}
					bmpInfo = (BITMAPINFO*)new BYTE[bmpSize=newSz]();
					bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bmpInfo->bmiHeader.biPlanes = 1;
					bmpInfo->bmiHeader.biBitCount = 32;   // 32位
					bmpInfo->bmiHeader.biCompression = BI_RGB;
				}
				bmpInfo->bmiHeader.biWidth = drawWidth;
				bmpInfo->bmiHeader.biHeight = -drawHeight;  // top-down image
				SkImageInfo info = SkImageInfo::Make(drawWidth, drawHeight, kBGRA_8888_SkColorType,kPremul_SkAlphaType); 
				sk_sp<SkSurface> surface = SkSurface::MakeRasterDirect(info
					, bmpInfo->bmiColors
					, drawWidth * sizeof(uint32_t));
				SkCanvas* canvas = surface->getCanvas();
				canvas->clear(SK_ColorWHITE);


				Draw(canvas, drawWidth, drawHeight);
				//canvas->drawImage(skImage, 0, 0);

				hdcMem = CreateCompatibleDC(NULL);

				//hBitmap = CreateDIBitmap(hDC, &bmpInfo->bmiHeader,CBM_INIT, bmpInfo->bmiColors, bmpInfo, DIB_RGB_COLORS);
				//HBITMAP oldbmp = (HBITMAP)::SelectObject(hdcMem, hBitmap);

				//delete[] bmpInfo;
				//bmpInfo = 0;
				//bmpSize = 0;
				delete[] pixels;
				pxSize = 0;
				skBitmap = NULL;
				skImage = NULL;
			}


			hBitmap = 0;

			return 0;
		}

		return -2;
	}

	void SkImageView::Draw(SkCanvas* canvas,int rectWidth,int rectHeight) {
		int left=0;
		int top=0;

		int calcW=rectWidth;
		int calcH=rectWidth*_srcHeight*1.0/_srcWidth;

		if (calcH>rectHeight)
		{
			calcW=rectHeight*_srcWidth*1.0/_srcHeight;
			calcH=rectHeight;
			left = (rectWidth-calcW)/2;
		}
		else 
		{
			top = (rectHeight-calcH)/2;
		}

		calcW=rectWidth;calcH=rectHeight;left=top=0;

		SkRect rect{left, top, left+calcW, top+calcH};
		//SkSamplingOptions options(SkFilterMode::kNearest, SkMipmapMode::kNearest);
		SkSamplingOptions options(SkFilterMode::kLinear, SkMipmapMode::kLinear);
		canvas->drawImageRect(skImage, rect, options, 0);
		 //canvas->drawImage(skImage, 0, 0);
		canvas->flush();
	}

	bool bPreciseDraw = false;
	HDC _hDC;

	void SkImageView::PaintBkColor(HDC hDC)
	{
		RECT rt = m_rcItem;
		_hDC = hDC;

		//if(0)
		if (!::IsRectEmpty(&rt))
		{
			int drawWidth = rt.right - rt.left;
			int drawHeight = rt.bottom - rt.top;


			//StretchBlt(  hDC, 0, 0, drawWidth, drawHeight,;

			//StretchDIBits(  hDC, 0, 0, drawWidth, drawHeight,
			//	0, 0, drawWidth, drawHeight,
			//	pixels, bmpInfo,
			//	DIB_RGB_COLORS, SRCCOPY );  //整张图绘制到DC



			if (!hBitmap)
			{
				drawWidth = _srcWidth;
				drawHeight = _srcHeight;


				hBitmap = CreateDIBitmap(hDC, &bmpInfo->bmiHeader,CBM_INIT, bmpInfo->bmiColors, bmpInfo, DIB_RGB_COLORS);
				HBITMAP oldbmp = (HBITMAP)::SelectObject(hdcMem, hBitmap);
			}


			//BitBlt(hDC, 0, 0, drawWidth, drawHeight, hdcMem, 0, 0, SRCCOPY);

			drawWidth = rt.right - rt.left;
			drawHeight = rt.bottom - rt.top;


			int left=0;
			int top=0;

			int calcW=drawWidth;
			int calcH=drawWidth*_srcHeight*1.0/_srcWidth;

			if (calcH>drawHeight)
			{
				calcW=drawHeight*_srcWidth*1.0/_srcHeight;
				calcH=drawHeight;
				left = (drawWidth-calcW)/2;
			}
			else 
			{
				top = (drawHeight-calcH)/2;
			}

			//calcW=drawWidth;calcH=drawHeight;left=top=0;

			if (0)
			{
				DrawDibRealize( m_hDrawDib, hDC, 0);
				DrawDibDraw(m_hDrawDib, hDC, left, top, calcW, calcH
					, &bmpInfo->bmiHeader, bmpInfo->bmiColors
					, 0, 0, _srcWidth, _srcHeight
					, DDF_BACKGROUNDPAL
				);;
				DrawDibDraw(m_hDrawDib, hDC, 0, 0, drawWidth, drawHeight
					, &bmpInfo->bmiHeader, bmpInfo->bmiColors
					, 0, 0, drawWidth, drawHeight
					, DDF_BACKGROUNDPAL
				);

			}
			else
			{
				//SetStretchBltMode(hDC, STRETCH_DELETESCANS);
				if (bPreciseDraw)
				{
					SetStretchBltMode(hDC, HALFTONE);
				}
				else 
				{
					SetStretchBltMode(hDC, COLORONCOLOR);
					_manager->SetTimer(this, 0x100, 200, true); // 350
				}
				if (_manager->_SIZING)
				{
					//SetStretchBltMode(hDC, COLORONCOLOR);
				}
				int W = _srcWidth/_scale;
				int H = _srcHeight/_scale;
				int translationX = (_srcWidth-_srcWidth*_scale)/2;
				int translationY = (_srcHeight-_srcHeight*_scale)/2;
				StretchBlt(hDC, left, top, calcW, calcH
					, hdcMem, -translationX/_scale, -translationY/_scale, W, H, SRCCOPY);
			}
			
		}
	}

	void SkImageView::PaintForeColor(HDC hDC)
	{
	}

	void SkImageView::PaintForeImage(HDC hDC)
	{

	}

	void SkImageView::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_SCROLLWHEEL)
		{
			float delta = 0.25;
			if (LOWORD(event.wParam)==SB_LINEDOWN)
			{
				_scale -= delta;
			}
			else
			{
				_scale += delta;
			}
			Invalidate();
			return;
		}
		if (event.Type == UIEVENT_TIMER)
		{
			if (event.wParam==0x100)
			{
				KillTimer(0x100);
				bPreciseDraw = true;
				NeedUpdate();
				SendMessage(_manager->GetPaintWindow(), WM_PAINT, 0, 0);
				bPreciseDraw = false;
				return;
			}
		}
		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused_YES;
			Invalidate();
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused_NO;
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			Invalidate();
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			//return;
		}

		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			//::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			//return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			//return;
		}
		if (event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_BUTTONDOWN)
		{
			//Selected(!m_bSelected, true);
			bPreciseDraw = true;
			//PaintBkColor(_hDC);
			NeedUpdate();
			SendMessage(_manager->GetPaintWindow(), WM_PAINT, 0, 0);
			bPreciseDraw = false;
			return;
		}
		if (event.Type == UIEVENT_SETCURSOR)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CControlUI::DoEvent(event);
	}

	LPCTSTR SkImageView::GetClass() const
	{
		return _T("SkImageView");
	}

} // namespace DuiLib
