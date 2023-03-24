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

#include "ImageView.h"

namespace DuiLib {
	IMPLEMENT_QKCONTROL(ImageView)
	
//#define USEDDBLT

	ImageView::ImageView():CControlUI()
	{


	}

	int ImageView::LoadImageFile(CHAR* path) 
	{
		std::unique_ptr<SkStream> stream = SkStream::MakeFromFile(path);

		auto codec = SkCodec::MakeFromStream(std::move(stream));

		if (!codec)
		{
			return -1;
		}
		skBitmap = NULL;
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
		char* pixels = new char[bytesAllocation];

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
			sk_sp<SkImage> skImage = skBitmap->asImage();

			int drawWidth, drawHeight;

			//if (!hBitmap)
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

				canvas->drawImage(skImage, 0, 0);

				_bNewImage = true;
#ifdef USEDDBLT
				if (!hdcMem)
				{
					hdcMem = CreateCompatibleDC(NULL);
				}
#endif
				//delete[] bmpInfo;
				//bmpInfo = 0;
				//bmpSize = 0;
				delete[] pixels;
				pxSize = 0;
				skBitmap = NULL;
				skImage = NULL;
			}

#ifdef USEDDBLT
			hBitmap = 0;
#endif

			return 0;
		}

		return -2;
	}

	void ImageView::ApplyStrechMode(HDC hDC)
	{
		//SetStretchBltMode(hDC, HALFTONE);
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
	}

	//void ImageView::PaintBkColor(HDC hDC)
	//{
	//}

	void ImageView::PaintBkImage(HDC hDC)
	{
		RECT rt = m_rcItem;

		if (!::IsRectEmpty(&rt))
		{
			int drawWidth = rt.right - rt.left;
			int drawHeight = rt.bottom - rt.top;

#ifdef USEDDBLT
			if (!hBitmap)
			{
				drawWidth = _srcWidth;
				drawHeight = _srcHeight;


				hBitmap = CreateDIBitmap(hDC, &bmpInfo->bmiHeader,CBM_INIT, bmpInfo->bmiColors, bmpInfo, DIB_RGB_COLORS);
				HBITMAP oldbmp = (HBITMAP)::SelectObject(hdcMem, hBitmap);
			}
#endif
			drawWidth = rt.right - rt.left;
			drawHeight = rt.bottom - rt.top;

			float scaleWidth = drawWidth*1.0/_srcWidth;
			float scaleHeight = drawHeight*1.0/_srcHeight;

			_minScale = min(scaleWidth, scaleHeight);

			float scale = _scale;

			if (scale<_minScale || _bNewImage || _bFit)
			{
				_scale = scale = _minScale;
				//_translationX = _translationY = 0;
				if (!_bFit)
				{
					_bFit = true;
				}
				_translationX = (drawWidth-_srcWidth*_minScale)/2;
				_translationY = (drawHeight-_srcHeight*_minScale)/2;
				if (_bNewImage)
				{
					_bNewImage = false;
				}
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

			ApplyStrechMode(hDC);

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

#ifdef USEDDBLT
			StretchBlt(hDC, left, top, calcW, calcH
				, hdcMem, tX, tY, W, H, SRCCOPY);
#else
			StretchDIBits(  hDC, left, top, calcW, calcH,
				tX, tY, W, H,
				bmpInfo->bmiColors, bmpInfo,
				DIB_RGB_COLORS, SRCCOPY );
#endif

#define 调试图片控件
#ifdef 调试图片控件
			int fontHeight = 20;
			int lnCnt=0;
			QkString text;
			text.Format(L"scale=%f", _scale);
			TextOut(hDC, 5, 5+fontHeight*(lnCnt++), text.GetData(), text.GetLength());
			text.Format(L"translation= %d, %d", _translationX, _translationY);
			TextOut(hDC, 5, 5+fontHeight*(lnCnt++), text.GetData(), text.GetLength());
			text.Format(L"drawSize= %d, %d", drawWidth, drawHeight);
			TextOut(hDC, 5, 5+fontHeight*(lnCnt++), text.GetData(), text.GetLength());
#endif
		}
	}

	void ImageView::PaintForeColor(HDC hDC)
	{
	}

	void ImageView::PaintForeImage(HDC hDC)
	{

	}

	void ImageView::DoEvent(TEventUI& event)
	{
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
		if (event.Type == UIEVENT_SCROLLWHEEL)
		{
			if(!_interactive) return;
			float delta = 0.25;
			float scale = _scale;
			int zDelta = (int) (short) HIWORD(event.wParam);
			if (zDelta < 0)
			{
				_scale -= delta;
				if (_scale <= _minScale)
				{
					_bFit = true;
					_scale = _minScale;
				}
			}
			else
			{
				_scale += delta;
				if (_scale > _minScale)
				{
					_bFit = false;
				}
			}
			RECT rt = m_rcItem;
			int drawWidth = rt.right - rt.left;
			int drawHeight = rt.bottom - rt.top;
			if (_bFit)
			{
				_translationX = (drawWidth-_srcWidth*_minScale)/2;
				_translationY = (drawHeight-_srcHeight*_minScale)/2;
			}
			else
			{
				_translationX -= (_scale/scale - 1)*(drawWidth/2-_translationX);
				_translationY -= (_scale/scale - 1)*(drawHeight/2-_translationY);
				//_translationX = _translationY = -100;
			}
			Invalidate();
			return;
		}
		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused_YES;
			Invalidate();
			return;
		}
		if (event.Type == UIEVENT_BUTTONDOWN)
		{
			_moving = true;
			//_moveLastX = LOWORD(event.lParam);
			//_moveLastY = HIWORD(event.lParam);
			POINT pt;
			::GetCursorPos(&pt);
			_moveLastX = pt.x;
			_moveLastY = pt.y;
			return;
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			_moving = false;
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			if(!_interactive) return;
			if (_moving)
			{
				POINT pt;
				::GetCursorPos(&pt);
				int X = pt.x; //LOWORD(event.lParam);
				int Y = pt.y; //HIWORD(event.lParam);
				_translationX += X-_moveLastX;
#ifdef USEDDBLT
				_translationY -= Y-_moveLastY;
#else
				_translationY += Y-_moveLastY;
#endif
				_moveLastX = X;
				_moveLastY = Y;
				Invalidate();
			}
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
			return;
		}
		if (event.Type == UIEVENT_SETCURSOR)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CControlUI::DoEvent(event);
	}

	LPCTSTR ImageView::GetClass() const
	{
		return _T("SkImageView");
	}

} // namespace DuiLib
