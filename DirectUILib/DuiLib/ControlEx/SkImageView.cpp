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

#include "SkImageView.h"

namespace DuiLib {
	IMPLEMENT_DUICONTROL(SkImageView)

	SkImageView::SkImageView():CControlUI()
	{
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
			return 0;
		}

		return -2;
	}

	void SkImageView::Draw(SkCanvas* canvas,int rectWidth,int rectHeight) {
		//SkPaint paint;
		//paint.setStrokeWidth(1);
		//paint.setARGB(0xff, 0xff, 0, 0);

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


		SkRect rect{left, top, left+calcW, top+calcH};
		SkSamplingOptions options(SkFilterMode::kNearest, SkMipmapMode::kNearest);
		canvas->drawImageRect(skImage, rect, options, 0);
		 //canvas->drawImage(skImage, 0, 0);

		if (false)
		{
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
		}

		canvas->flush();
	}

	void SkImageView::PaintBkColor(HDC hDC)
	{
		RECT rt = m_rcItem;
		//if(0)
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
				bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmpInfo->bmiHeader.biPlanes = 1;
				bmpInfo->bmiHeader.biBitCount = 32;   // 32位
				bmpInfo->bmiHeader.biCompression = BI_RGB;
			}
			bmpInfo->bmiHeader.biWidth = drawWidth;
			bmpInfo->bmiHeader.biHeight = -drawHeight;  // top-down image

			void* pixels = bmpInfo->bmiColors; 

			SkImageInfo info = SkImageInfo::Make(drawWidth, drawHeight, kBGRA_8888_SkColorType,kPremul_SkAlphaType); 

			sk_sp<SkSurface> surface = SkSurface::MakeRasterDirect(info
				, pixels
				, drawWidth * sizeof(uint32_t));

			SkCanvas* canvas = surface->getCanvas();

			canvas->clear(SK_ColorWHITE);

			Draw(canvas, drawWidth, drawHeight);

			StretchDIBits(  hDC, 0, 0, drawWidth, drawHeight,
				0, 0, drawWidth, drawHeight,
				pixels, bmpInfo,
				DIB_RGB_COLORS, SRCCOPY );  //整张图绘制到DC
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
		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused = true;
			Invalidate();
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused = false;
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
