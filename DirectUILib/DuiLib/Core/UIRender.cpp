#include "StdAfx.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\Utils\stb_image.h"

///////////////////////////////////////////////////////////////////////////////////////
namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CRenderClip::~CRenderClip()
	{
		if (hDC)
		{
			ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
			ASSERT(::GetObjectType(hRgn)==OBJ_REGION);
			ASSERT(::GetObjectType(hOldRgn)==OBJ_REGION);
			::SelectClipRgn(hDC, hOldRgn);
			::DeleteObject(hOldRgn);
			::DeleteObject(hRgn);
		}
	}

	void CRenderClip::GenerateClip(HDC hDC, RECT rc, CRenderClip& clip)
	{
		RECT rcClip = { 0 };
		::GetClipBox(hDC, &rcClip);
		clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
		clip.hRgn = ::CreateRectRgnIndirect(&rc);
		::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
		clip.hDC = hDC;
		clip.rcItem = rc;
	}

	void CRenderClip::GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip)
	{
		RECT rcClip = { 0 };
		::GetClipBox(hDC, &rcClip);
		clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
		clip.hRgn = ::CreateRectRgnIndirect(&rc);
		HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width, height);
		::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
		::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
		clip.hDC = hDC;
		clip.rcItem = rc;
		::DeleteObject(hRgnItem);
	}

	void CRenderClip::UseOldClipBegin(HDC hDC, CRenderClip& clip)
	{
		::SelectClipRgn(hDC, clip.hOldRgn);
	}

	void CRenderClip::UseOldClipEnd(HDC hDC, CRenderClip& clip)
	{
		::SelectClipRgn(hDC, clip.hRgn);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	static const float OneThird = 1.0f / 3;

	static void RGBtoHSL(DWORD ARGB, float* H, float* S, float* L) {
		const float
			R = (float)GetRValue(ARGB),
			G = (float)GetGValue(ARGB),
			B = (float)GetBValue(ARGB),
			nR = (R<0?0:(R>255?255:R))/255,
			nG = (G<0?0:(G>255?255:G))/255,
			nB = (B<0?0:(B>255?255:B))/255,
			m = min(min(nR,nG),nB),
			M = max(max(nR,nG),nB);
		*L = (m + M)/2;
		if (M==m) *H = *S = 0;
		else {
			const float
				f = (nR==m)?(nG-nB):((nG==m)?(nB-nR):(nR-nG)),
				i = (nR==m)?3.0f:((nG==m)?5.0f:1.0f);
			*H = (i-f/(M-m));
			if (*H>=6) *H-=6;
			*H*=60;
			*S = (2*(*L)<=1)?((M-m)/(M+m)):((M-m)/(2-M-m));
		}
	}

	static void HSLtoRGB(DWORD* ARGB, float H, float S, float L) {
		const float
			q = 2*L<1?L*(1+S):(L+S-L*S),
			p = 2*L-q,
			h = H/360,
			tr = h + OneThird,
			tg = h,
			tb = h - OneThird,
			ntr = tr<0?tr+1:(tr>1?tr-1:tr),
			ntg = tg<0?tg+1:(tg>1?tg-1:tg),
			ntb = tb<0?tb+1:(tb>1?tb-1:tb),
			B = 255*(6*ntr<1?p+(q-p)*6*ntr:(2*ntr<1?q:(3*ntr<2?p+(q-p)*6*(2.0f*OneThird-ntr):p))),
			G = 255*(6*ntg<1?p+(q-p)*6*ntg:(2*ntg<1?q:(3*ntg<2?p+(q-p)*6*(2.0f*OneThird-ntg):p))),
			R = 255*(6*ntb<1?p+(q-p)*6*ntb:(2*ntb<1?q:(3*ntb<2?p+(q-p)*6*(2.0f*OneThird-ntb):p)));
		*ARGB &= 0xFF000000;
		*ARGB |= RGB( (BYTE)(R<0?0:(R>255?255:R)), (BYTE)(G<0?0:(G>255?255:G)), (BYTE)(B<0?0:(B>255?255:B)) );
	}

	static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
	{
		return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
			GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
			GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);
	}

	static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
		int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
	{
		HDC hTempDC = ::CreateCompatibleDC(hDC);
		if (NULL == hTempDC)
			return FALSE;

		//Creates Source DIB
		LPBITMAPINFO lpbiSrc = NULL;
		// Fill in the BITMAPINFOHEADER
		lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
		if (lpbiSrc == NULL)
		{
			::DeleteDC(hTempDC);
			return FALSE;
		}
		lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbiSrc->bmiHeader.biWidth = dwWidth;
		lpbiSrc->bmiHeader.biHeight = dwHeight;
		lpbiSrc->bmiHeader.biPlanes = 1;
		lpbiSrc->bmiHeader.biBitCount = 32;
		lpbiSrc->bmiHeader.biCompression = BI_RGB;
		lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
		lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
		lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
		lpbiSrc->bmiHeader.biClrUsed = 0;
		lpbiSrc->bmiHeader.biClrImportant = 0;

		COLORREF* pSrcBits = NULL;
		HBITMAP hSrcDib = CreateDIBSection (
			hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
			NULL, NULL);

		if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
		{
			delete [] lpbiSrc;
			::DeleteDC(hTempDC);
			return FALSE;
		}

		HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
		::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
		::SelectObject (hTempDC, hOldTempBmp);

		//Creates Destination DIB
		LPBITMAPINFO lpbiDest = NULL;
		// Fill in the BITMAPINFOHEADER
		lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
		if (lpbiDest == NULL)
		{
			delete [] lpbiSrc;
			::DeleteObject(hSrcDib);
			::DeleteDC(hTempDC);
			return FALSE;
		}

		lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbiDest->bmiHeader.biWidth = dwWidth;
		lpbiDest->bmiHeader.biHeight = dwHeight;
		lpbiDest->bmiHeader.biPlanes = 1;
		lpbiDest->bmiHeader.biBitCount = 32;
		lpbiDest->bmiHeader.biCompression = BI_RGB;
		lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
		lpbiDest->bmiHeader.biXPelsPerMeter = 0;
		lpbiDest->bmiHeader.biYPelsPerMeter = 0;
		lpbiDest->bmiHeader.biClrUsed = 0;
		lpbiDest->bmiHeader.biClrImportant = 0;

		COLORREF* pDestBits = NULL;
		HBITMAP hDestDib = CreateDIBSection (
			hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
			NULL, NULL);

		if ((NULL == hDestDib) || (NULL == pDestBits))
		{
			delete [] lpbiSrc;
			::DeleteObject(hSrcDib);
			::DeleteDC(hTempDC);
			return FALSE;
		}

		::SelectObject (hTempDC, hDestDib);
		::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
		::SelectObject (hTempDC, hOldTempBmp);

		double src_darken;
		BYTE nAlpha;

		for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
		{
			nAlpha = LOBYTE(*pSrcBits >> 24);
			src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
			if( src_darken < 0.0 ) src_darken = 0.0;
			*pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
		} //for

		::SelectObject (hTempDC, hDestDib);
		::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
		::SelectObject (hTempDC, hOldTempBmp);

		delete [] lpbiDest;
		::DeleteObject(hDestDib);

		delete [] lpbiSrc;
		::DeleteObject(hSrcDib);

		::DeleteDC(hTempDC);
		return TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	void CRenderEngine::DrawSkImage(CPaintManagerUI* pManager, const TImageInfo* data, const RECT& rc, const RECT& rcPaint,
		const RECT& rcBmpPart, const RECT& rcCorners, bool bAlpha, 
		BYTE uFade, bool hole, bool xtiled, bool ytiled)
	{
		if( data == NULL ) return;
#ifdef MODULE_SKIA_RENDERER
		auto decodeInfo = SkImageInfo::MakeN32(data->nX, data->nY, kPremul_SkAlphaType);
		SkBitmap skBitmap;
		skBitmap.setInfo(decodeInfo);
		skBitmap.setPixels(data->pBmBits);
		auto skImage = skBitmap.asImage();

		SkSamplingOptions options(SkFilterMode::kLinear, SkMipmapMode::kNone);
		//SkSamplingOptions options(SkFilterMode::kNearest, SkMipmapMode::kNone);

		#define DRAWRECTCONST SkCanvas::SrcRectConstraint::kStrict_SrcRectConstraint


		RECT rcTemp = {0};
		RECT rcDest = {0};
		{
			if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
				&& rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
				&& rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
			{
				//if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				//	::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
				//		hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
				//}
			}
			else
			{
				// middle
				if( !hole ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						if( !xtiled && !ytiled ) {
							rcDest.right -= rcDest.left;
							rcDest.bottom -= rcDest.top;

							pManager->GetSkiaCanvas()->drawImageRect(skImage
								, SkRect::MakeXYWH(rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
									rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
									rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom)
								, SkRect::MakeXYWH(rcDest.left, rcDest.top, rcDest.right, rcDest.bottom)
								, options, NULL, DRAWRECTCONST);
						} 
						else if( xtiled && ytiled ) {
							LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
							LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
							int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
							int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
							for( int j = 0; j < iTimesY; ++j ) {
								LONG lDestTop = rcDest.top + lHeight * j;
								LONG lDestBottom = rcDest.top + lHeight * (j + 1);
								LONG lDrawHeight = lHeight;
								if( lDestBottom > rcDest.bottom ) {
									lDrawHeight -= lDestBottom - rcDest.bottom;
									lDestBottom = rcDest.bottom;
								}
								for( int i = 0; i < iTimesX; ++i ) {
									LONG lDestLeft = rcDest.left + lWidth * i;
									LONG lDestRight = rcDest.left + lWidth * (i + 1);
									LONG lDrawWidth = lWidth;
									if( lDestRight > rcDest.right ) {
										lDrawWidth -= lDestRight - rcDest.right;
										lDestRight = rcDest.right;
									}
									//::BitBlt(hDC, rcDraw.left + lWidth * i, rcDraw.top + lHeight * j, \
									//	lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
									//	rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
								}
							}
						}
						else if( xtiled ) {
							LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
							int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
							for( int i = 0; i < iTimes; ++i ) {
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								LONG lDrawWidth = lWidth;
								if( lDestRight > rcDest.right ) {
									lDrawWidth -= lDestRight - rcDest.right;
									lDestRight = rcDest.right;
								}
								SkRect rect{lDestLeft, rcDest.top, lDestRight, rcDest.top + rcDest.bottom};
								SkRect rectSrc{rcBmpPart.left + rcCorners.left
									, rcBmpPart.top + rcCorners.top
									, rcBmpPart.left + rcCorners.left + lDrawWidth
									, rcBmpPart.top + rcCorners.top + rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom};
								pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);

							}
						}
						else { // ytiled
							LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
							int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
							for( int i = 0; i < iTimes; ++i ) {
								LONG lDestTop = rcDest.top + lHeight * i;
								LONG lDestBottom = rcDest.top + lHeight * (i + 1);
								LONG lDrawHeight = lHeight;
								if( lDestBottom > rcDest.bottom ) {
									lDrawHeight -= lDestBottom - rcDest.bottom;
									lDestBottom = rcDest.bottom;
								}
								pManager->GetSkiaCanvas()->drawImageRect(skImage
									, SkRect::MakeXYWH(rcBmpPart.left + rcCorners.left
										, rcBmpPart.top + rcCorners.top
										,rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right
										, lDrawHeight)
									, SkRect::MakeXYWH(rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop)
									, options, NULL, DRAWRECTCONST);
							}
						}
					}
				}

				//// bottom
				//if( rcCorners.bottom > 0 ) {
				//	rcDraw.left = rc.left + rcCorners.left;
				//	rcDraw.top = rc.bottom - rcCorners.bottom;
				//	rcDraw.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				//	rcDraw.bottom = rcCorners.bottom;
				//	rcDraw.right += rcDraw.left;
				//	rcDraw.bottom += rcDraw.top;
				//	if( ::IntersectRect(&rcTemp, &rcPaint, &rcDraw) ) {
				//		SkRect rect{rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom};
				//		SkRect rectSrc{rcBmpPart.left + rcCorners.left
				//			, rcBmpPart.bottom - rcCorners.bottom
				//			, rcBmpPart.left + rcCorners.left + rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right
				//			, rcBmpPart.bottom - rcCorners.bottom + rcCorners.bottom};
				//		pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);
				//
				//	}
				//
				//}

				//if(1) return;

				// left-top
				if( rcCorners.left > 0 && rcCorners.top > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.top;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						SkRect rect{rcDest.left, rcDest.top, rcDest.right, rcDest.bottom};
						SkRect rectSrc{rcBmpPart.left, rcBmpPart.top, rcBmpPart.left+rcCorners.left, rcBmpPart.top+rcCorners.top};
						pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);
					}
				}
				// top
				if( rcCorners.top > 0 ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.top;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						SkRect rect{rcDest.left, rcDest.top, rcDest.right, rcDest.bottom};
						SkRect rectSrc{rcBmpPart.left + rcCorners.left
							, rcBmpPart.top
							, rcBmpPart.left + rcCorners.left + rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right
							, rcBmpPart.top + rcCorners.top};
						pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);

					}
				}
				// right-top
				if( rcCorners.right > 0 && rcCorners.top > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.top;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						SkRect rect{rcDest.left, rcDest.top, rcDest.right, rcDest.bottom};
						SkRect rectSrc{rcBmpPart.right - rcCorners.right
							, rcBmpPart.top
							, rcBmpPart.right
							, rcBmpPart.top + rcCorners.top};
						pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);
					}
				}
				// left
				if( rcCorners.left > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {

						SkRect rect{rcDest.left, rcDest.top, rcDest.right, rcDest.bottom};
						SkRect rectSrc{rcBmpPart.left
							, rcBmpPart.top + rcCorners.top
							, rcBmpPart.left + rcCorners.left
							, rcBmpPart.top + rcCorners.top + rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom};
						pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);

					}
				}
				// right
				if( rcCorners.right > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						SkRect rect{rcDest.left, rcDest.top, rcDest.right, rcDest.bottom};
						SkRect rectSrc{rcBmpPart.right - rcCorners.right
							, rcBmpPart.top + rcCorners.top
							, rcBmpPart.right - rcCorners.right + rcCorners.right
							, rcBmpPart.top + rcCorners.top + rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom};
						pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);

					}
				}
				// left-bottom
				if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						SkRect rect{rcDest.left, rcDest.top, rcDest.right, rcDest.bottom};
						SkRect rectSrc{rcBmpPart.left
							, rcBmpPart.bottom - rcCorners.bottom
							, rcBmpPart.left+rcCorners.left
							, rcBmpPart.bottom - rcCorners.bottom+rcCorners.bottom};
						pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);
					}
				}
				// bottom
				if( rcCorners.bottom > 0 ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						SkRect rect{rcDest.left, rcDest.top, rcDest.right, rcDest.bottom};
						SkRect rectSrc{rcBmpPart.left + rcCorners.left
							, rcBmpPart.bottom - rcCorners.bottom
							, rcBmpPart.left + rcCorners.left + rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right
							, rcBmpPart.bottom - rcCorners.bottom + rcCorners.bottom};
						pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);

					}

				}
				// right-bottom
				if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {

						SkRect rect{rcDest.left, rcDest.top, rcDest.right, rcDest.bottom};
						SkRect rectSrc{rcBmpPart.right - rcCorners.right
							, rcBmpPart.bottom - rcCorners.bottom
							, rcBmpPart.right - rcCorners.right + rcCorners.right
							, rcBmpPart.bottom - rcCorners.bottom + rcCorners.bottom};
						pManager->GetSkiaCanvas()->drawImageRect(skImage, rectSrc, rect, options, NULL, DRAWRECTCONST);
					}
				}

			}
		}

#endif
	}

	DWORD CRenderEngine::AdjustColor(DWORD dwColor, short H, short S, short L)
	{
		if( H == 180 && S == 100 && L == 100 ) return dwColor;
		float fH, fS, fL;
		float S1 = S / 100.0f;
		float L1 = L / 100.0f;
		RGBtoHSL(dwColor, &fH, &fS, &fL);
		fH += (H - 180);
		fH = fH > 0 ? fH : fH + 360; 
		fS *= S1;
		fL *= L1;
		HSLtoRGB(&dwColor, fH, fS, fL);
		return dwColor;
	}

	TImageInfo* CRenderEngine::LoadImageStr(STRINGorID bitmap, LPCTSTR type, DWORD mask, HINSTANCE instance, int bytesPerPixel)
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;
		do 
		{
			if( type == NULL ) {
				QkString sFile = CPaintManagerUI::GetResourcePath();
				if( CPaintManagerUI::GetResourceZip().IsEmpty() ) {
					sFile += bitmap.m_lpstr;
					HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
						FILE_ATTRIBUTE_NORMAL, NULL);
					if( hFile == INVALID_HANDLE_VALUE ) break;
					dwSize = ::GetFileSize(hFile, NULL);
					if( dwSize == 0 ) { ::CloseHandle( hFile ); break; }

					DWORD dwRead = 0;
					pData = new BYTE[ dwSize ];
					::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
					::CloseHandle( hFile );

					if( dwRead != dwSize ) {
						delete[] pData;
						pData = NULL;
						break;
					}
				}
				else {
					sFile += CPaintManagerUI::GetResourceZip();
					QkString sFilePwd = CPaintManagerUI::GetResourceZipPwd();
					HZIP hz = NULL;
					if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
					else
					{
#ifdef UNICODE
						char* pwd = w2a((wchar_t*)sFilePwd.GetData());
						hz = OpenZip(sFile.GetData(), pwd);
						if(pwd) delete[] pwd;
#else
						hz = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
					}
					if( hz == NULL ) break;
					ZIPENTRY ze; 
					int i = 0; 
					QkString key = bitmap.m_lpstr;
					key.Replace(_T("\\"), _T("/"));
					if( FindZipItem(hz, key, true, &i, &ze) != 0 ) break;
					dwSize = ze.unc_size;
					if( dwSize == 0 ) { if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz); break; }
					pData = new BYTE[ dwSize ];
					int res = UnzipItem(hz, i, pData, dwSize);
					if( res != 0x00000000 && res != 0x00000600) {
						delete[] pData;
						pData = NULL;
						if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
						break;
					}
					if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
				}
			}
			else {
				HINSTANCE dllinstance = NULL;
				if (instance) {
					dllinstance = instance;
				}
				else {
					dllinstance = CPaintManagerUI::GetResourceDll();
				}
				HRSRC hResource = ::FindResource(dllinstance, bitmap.m_lpstr, type);
				if( hResource == NULL ) break;
				HGLOBAL hGlobal = ::LoadResource(dllinstance, hResource);
				if( hGlobal == NULL ) {
					FreeResource(hResource);
					break;
				}

				dwSize = ::SizeofResource(dllinstance, hResource);
				if( dwSize == 0 ) { ::FreeResource(hGlobal); break; }
				pData = new BYTE[ dwSize ];
				::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
				::FreeResource(hGlobal);
			}
		} while (0);

		while (!pData)
		{
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) { ::CloseHandle( hFile ); break; }

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}
		if (!pData)
		{
			return NULL;
		}

		LPBYTE pImage = NULL;
		int x,y,n;
		pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
		delete[] pData;
		if( !pImage ) {
			return NULL;
		}

		BITMAPINFO bmi;
		::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = x;
		bmi.bmiHeader.biHeight = -y;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount = bytesPerPixel*8;
		bmi.bmiHeader.biSizeImage = x * y * bytesPerPixel;

		bool bAlphaChannel = false;
		LPBYTE pDest = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
		//hBitmap = ::CreateDIBitmap(NULL, &bmi.bmiHeader, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
		if( !hBitmap ) {
			return NULL;
		}
		BYTE alphaByte;
		float alpha;
		for( int i = 0; i < x * y; i++ ) 
		{
			pDest[i*bytesPerPixel] = pImage[i*4 + 2];
			pDest[i*bytesPerPixel + 1] = pImage[i*4 + 1];
			pDest[i*bytesPerPixel + 2] = pImage[i*4]; 
			if (bytesPerPixel>3)
			{
				pDest[i*bytesPerPixel + 3] = alphaByte = pImage[i*4 + 3];
				if (alphaByte<255)
				{
					alpha = alphaByte*1.f/255;
					pDest[i*bytesPerPixel] *= alpha;
					pDest[i*bytesPerPixel + 1] *= alpha;
					pDest[i*bytesPerPixel + 2] *= alpha; 
					bAlphaChannel = true;
				}
				else if((*(DWORD*)(&pDest[i*bytesPerPixel])) == mask ) 
				{
					pDest[i*bytesPerPixel] = (BYTE)255;
					pDest[i*bytesPerPixel + 1] = (BYTE)255;
					pDest[i*bytesPerPixel + 2] = (BYTE)255; 
					pDest[i*bytesPerPixel + 3] = (BYTE)0;
					bAlphaChannel = true;
				}
			}
		}

		stbi_image_free(pImage);

		TImageInfo* data = new TImageInfo;
		data->pBmBits = pDest;
		data->pBits = NULL;
		data->pSrcBits = NULL;
		data->hBitmap = hBitmap;
		data->nX = x;
		data->nY = y;
		data->bAlpha = bAlphaChannel;
		return data;
	}
#ifdef USE_XIMAGE_EFFECT
	static DWORD LoadImage2Memory(const STRINGorID &bitmap, LPCTSTR type,LPBYTE &pData)
	{
		assert(pData == NULL);
		pData = NULL;
		DWORD dwSize(0U);
		do 
		{
			if( type == NULL )
			{
				QkString sFile = CPaintManagerUI::GetResourcePath();
				if( CPaintManagerUI::GetResourceZip().IsEmpty() )
				{
					sFile += bitmap.m_lpstr;
					HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
						FILE_ATTRIBUTE_NORMAL, NULL);
					if( hFile == INVALID_HANDLE_VALUE ) break;
					dwSize = ::GetFileSize(hFile, NULL);
					if( dwSize == 0 ) { ::CloseHandle( hFile ); break; }

					DWORD dwRead = 0;
					pData = new BYTE[ dwSize + 1 ];
					memset(pData,0,dwSize+1);
					::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
					::CloseHandle( hFile );

					if( dwRead != dwSize ) 
					{
						delete[] pData;
						pData = NULL;
						dwSize = 0U;
						break;
					}
				}
				else 
				{
					sFile += CPaintManagerUI::GetResourceZip();
					HZIP hz = NULL;
					if( CPaintManagerUI::IsCachedResourceZip() ) 
						hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
					else {
						QkString sFilePwd = CPaintManagerUI::GetResourceZipPwd();
#ifdef UNICODE
						char* pwd = w2a((wchar_t*)sFilePwd.GetData());
						hz = OpenZip((void*)sFile.GetData(), pwd);
						if(pwd) delete[] pwd;
#else
						hz = OpenZip((void*)sFile.GetData(), sFilePwd.GetData());
#endif
					}
					if( hz == NULL ) break;
					ZIPENTRY ze; 
					int i = 0; 
					QkString key = bitmap.m_lpstr;
					key.Replace(_T("\\"), _T("/")); 
					if( FindZipItem(hz, key, true, &i, &ze) != 0 ) break;
					dwSize = ze.unc_size;
					if( dwSize == 0 ) break;
					pData = new BYTE[ dwSize ];
					int res = UnzipItem(hz, i, pData, dwSize, 3);
					if( res != 0x00000000 && res != 0x00000600)
					{
						delete[] pData;
						pData = NULL;
						dwSize = 0U;
						if( !CPaintManagerUI::IsCachedResourceZip() )
							CloseZip(hz);
						break;
					}
					if( !CPaintManagerUI::IsCachedResourceZip() )
						CloseZip(hz);
				}
			}
			else 
			{
				HINSTANCE hDll = CPaintManagerUI::GetResourceDll();
				HRSRC hResource = ::FindResource(hDll, bitmap.m_lpstr, type);
				if( hResource == NULL ) break;
				HGLOBAL hGlobal = ::LoadResource(hDll, hResource);
				if( hGlobal == NULL ) 
				{
					FreeResource(hResource);
					break;
				}

				dwSize = ::SizeofResource(hDll, hResource);
				if( dwSize == 0 ) break;
				pData = new BYTE[ dwSize ];
				::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
				::FreeResource(hGlobal);
			}
		} while (0);

		while (!pData)
		{
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) { ::CloseHandle( hFile ); break; }

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) 
			{
				delete[] pData;
				pData = NULL;
				dwSize = 0U;
			}
			break;
		}
		return dwSize;
	}
	CxImage* CRenderEngine::LoadGifImageX(STRINGorID bitmap, LPCTSTR type , DWORD mask)
	{
		//write by wangji
		LPBYTE pData = NULL;
		DWORD dwSize = LoadImage2Memory(bitmap,type,pData);
		if(dwSize == 0U || !pData)
			return NULL;
		CxImage * pImg = NULL;
		if(pImg = new CxImage())
		{
			pImg->SetRetreiveAllFrames(TRUE);
			if(!pImg->Decode(pData,dwSize,CXIMAGE_FORMAT_GIF))
			{
				delete pImg;
				pImg = nullptr;
			}
		}
		delete[] pData;
		pData = NULL;
		return pImg;
	}
#endif//USE_XIMAGE_EFFECT

	Gdiplus::Image* CRenderEngine::GdiplusLoadImage(LPCTSTR pstrPath1)
	{
		tagTDrawInfo drawInfo;
		drawInfo.Parse(pstrPath1, NULL);
		QkString sName = drawInfo.sName;

		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		do 
		{
			QkString sFile = CPaintManagerUI::GetResourcePath();
			if( CPaintManagerUI::GetResourceZip().IsEmpty() ) {
				sFile += sName;
				HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
					FILE_ATTRIBUTE_NORMAL, NULL);
				if( hFile == INVALID_HANDLE_VALUE ) break;
				dwSize = ::GetFileSize(hFile, NULL);
				if( dwSize == 0 ) { ::CloseHandle( hFile ); break; }

				DWORD dwRead = 0;
				pData = new BYTE[ dwSize ];
				::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
				::CloseHandle( hFile );

				if( dwRead != dwSize ) {
					delete[] pData;
					pData = NULL;
					break;
				}
			}
			else {
				sFile += CPaintManagerUI::GetResourceZip();
				HZIP hz = NULL;
				if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
				else {
					QkString sFilePwd = CPaintManagerUI::GetResourceZipPwd();
#ifdef UNICODE
					char* pwd = w2a((wchar_t*)sFilePwd.GetData());
					hz = OpenZip(sFile.GetData(), pwd);
					if(pwd) delete[] pwd;
#else
					hz = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
				}
				if( hz == NULL ) break;
				ZIPENTRY ze; 
				int i = 0; 
				QkString key = sName;
				key.Replace(_T("\\"), _T("/"));
				if( FindZipItem(hz, key, true, &i, &ze) != 0 ) break;
				dwSize = ze.unc_size;
				if( dwSize == 0 ) { if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz); break; }
				pData = new BYTE[ dwSize ];
				int res = UnzipItem(hz, i, pData, dwSize);
				if( res != 0x00000000 && res != 0x00000600) {
					delete[] pData;
					pData = NULL;
					if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
					break;
				}
				if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
			}

		} while (0);

		while (!pData)
		{
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile(sName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) { ::CloseHandle( hFile ); break; }

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}

		Gdiplus::Image* pImage = NULL;
		if(pData != NULL) {
			pImage = GdiplusLoadImage(pData, dwSize);
			delete[] pData;
			pData = NULL;
		}
		return pImage;
	}

	TImageInfo* CRenderEngine::LoadImageStr(LPCTSTR pStrImage, LPCTSTR type, DWORD mask, HINSTANCE instance)
	{	
		if(pStrImage == NULL) 
		{
			return NULL;
		}
		if( type == NULL )  
		{
			pStrImage = CResourceManager::GetInstance()->MapImagePath(pStrImage);
		}
		return LoadImageStr(STRINGorID(pStrImage), type, mask, instance);
	}

	TImageInfo* CRenderEngine::LoadImageStr(UINT nID, LPCTSTR type, DWORD mask, HINSTANCE instance)
	{
		return LoadImageStr(STRINGorID(nID), type, mask, instance);
	}

	Gdiplus::Image* CRenderEngine::GdiplusLoadImage( LPVOID pBuf,size_t dwSize )
	{
		HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
		BYTE* pMem = (BYTE*)::GlobalLock(hMem);
		memcpy(pMem, pBuf, dwSize);
		IStream* pStm = NULL;
		::CreateStreamOnHGlobal(hMem, TRUE, &pStm);
		Gdiplus::Image *pImg = Gdiplus::Image::FromStream(pStm);
		if(!pImg || pImg->GetLastStatus() != Gdiplus::Ok)
		{
			pStm->Release();
			::GlobalUnlock(hMem);
			return 0;
		}
		return pImg;
	}

	void CRenderEngine::FreeImage(TImageInfo* bitmap, bool bDelete)
	{
		if (bitmap == NULL) return;
		if (bitmap->hBitmap) {
			::DeleteObject(bitmap->hBitmap);
		}
		bitmap->hBitmap = NULL;
		if (bitmap->pBits) {
			//delete[] bitmap->pBits;
		}
		bitmap->pBits = NULL;
		if (bitmap->pSrcBits) {
			delete[] bitmap->pSrcBits;
		}
		bitmap->pSrcBits = NULL;
		if (bDelete) {
			delete bitmap;
			bitmap = NULL;
		}
	}

	bool CRenderEngine::MakeImageDest(const RECT& rcControl, const CDuiSize& szIcon, short iAlign, const RECT& rcPadding, RECT& rcDest)
	{
		if(szIcon.cx>0)
		if((iAlign&GRAVITY_HCENTER)==GRAVITY_HCENTER)
		{
			rcDest.left = rcControl.left + ((rcControl.right - rcControl.left) - szIcon.cx)/2;  
			rcDest.right = rcDest.left + szIcon.cx;
		}
		else if(iAlign&GRAVITY_LEFT)
		{
			rcDest.left = rcControl.left;  
			rcDest.right = rcDest.left + szIcon.cx;
		}
		else if(iAlign&GRAVITY_RIGHT)
		{
			rcDest.left = rcControl.right - szIcon.cx;  
			rcDest.right = rcDest.left + szIcon.cx;
		}

		if(szIcon.cy>0)
		if((iAlign&GRAVITY_VCENTER)==GRAVITY_VCENTER)
		{
			rcDest.top = rcControl.top + ((rcControl.bottom - rcControl.top) - szIcon.cy)/2;
			rcDest.bottom = rcDest.top + szIcon.cy;
		}
		else if(iAlign&GRAVITY_TOP)
		{
			rcDest.top = rcControl.top;
			rcDest.bottom = rcDest.top + szIcon.cy;
		}
		else if(iAlign&GRAVITY_BOTTOM)
		{
			rcDest.top = rcControl.bottom - szIcon.cy;
			rcDest.bottom = rcDest.top + rcDest.top;
		}

		rcDest.left +=   rcPadding.left;
		rcDest.top +=    rcPadding.top;
		rcDest.right -=  rcPadding.right;
		rcDest.bottom -= rcPadding.bottom;

		if (rcDest.right > rcControl.right) 
			rcDest.right = rcControl.right;

		if (rcDest.bottom > rcControl.bottom) 
			rcDest.bottom = rcControl.bottom;

		return true;
	}

	void CRenderEngine::DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint,
		const RECT& rcBmpPart, const RECT& rcCorners, bool bAlpha, 
		BYTE uFade, bool hole, bool xtiled, bool ytiled)
	{
		ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

		typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
		static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

		if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
		if( hBitmap == NULL ) return;

		HDC hCloneDC = ::CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
		::SetStretchBltMode(hDC, HALFTONE);
		//::SetStretchBltMode(hDC, COLORONCOLOR);

		RECT rcTemp = {0};
		RECT rcDest = {0};
		if( lpAlphaBlend && (bAlpha || uFade < 255) ) 
		//if( 0 ) 
		{
			BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
			// middle
			if( !hole ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					if( !xtiled && !ytiled ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
							rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
					}
					else if( xtiled && ytiled ) {
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int j = 0; j < iTimesY; ++j ) {
							LONG lDestTop = rcDest.top + lHeight * j;
							LONG lDestBottom = rcDest.top + lHeight * (j + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom ) {
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							for( int i = 0; i < iTimesX; ++i ) {
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								LONG lDrawWidth = lWidth;
								if( lDestRight > rcDest.right ) {
									lDrawWidth -= lDestRight - rcDest.right;
									lDestRight = rcDest.right;
								}
								lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
									lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
									rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, bf);
							}
						}
					}
					else if( xtiled ) {
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if( lDestRight > rcDest.right ) {
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
						}
					}
					else { // ytiled
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestTop = rcDest.top + lHeight * i;
							LONG lDestBottom = rcDest.top + lHeight * (i + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom ) {
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);                    
						}
					}
				}
			}

			// left-top
			if( rcCorners.left > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
				}
			}
			// top
			if( rcCorners.top > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
						rcCorners.left - rcCorners.right, rcCorners.top, bf);
				}
			}
			// right-top
			if( rcCorners.right > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
				}
			}
			// left
			if( rcCorners.left > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
						rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				}
			}
			// right
			if( rcCorners.right > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
						rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				}
			}
			// left-bottom
			if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
				}
			}
			// bottom
			if( rcCorners.bottom > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
						rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
				}
			}
			// right-bottom
			if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
						rcCorners.bottom, bf);
				}
			}
		}
		else 
		{
			if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
				&& rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
				&& rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
			{
				if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
					::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
						hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
				}
			}
			else
			{
				// middle
				if( !hole ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						if( !xtiled && !ytiled ) {
							rcDest.right -= rcDest.left;
							rcDest.bottom -= rcDest.top;
							::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
								rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
								rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
						}
						else if( xtiled && ytiled ) {
							LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
							LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
							int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
							int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
							for( int j = 0; j < iTimesY; ++j ) {
								LONG lDestTop = rcDest.top + lHeight * j;
								LONG lDestBottom = rcDest.top + lHeight * (j + 1);
								LONG lDrawHeight = lHeight;
								if( lDestBottom > rcDest.bottom ) {
									lDrawHeight -= lDestBottom - rcDest.bottom;
									lDestBottom = rcDest.bottom;
								}
								for( int i = 0; i < iTimesX; ++i ) {
									LONG lDestLeft = rcDest.left + lWidth * i;
									LONG lDestRight = rcDest.left + lWidth * (i + 1);
									LONG lDrawWidth = lWidth;
									if( lDestRight > rcDest.right ) {
										lDrawWidth -= lDestRight - rcDest.right;
										lDestRight = rcDest.right;
									}
									::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, \
										lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
										rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
								}
							}
						}
						else if( xtiled ) {
							LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
							int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
							for( int i = 0; i < iTimes; ++i ) {
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								LONG lDrawWidth = lWidth;
								if( lDestRight > rcDest.right ) {
									lDrawWidth -= lDestRight - rcDest.right;
									lDestRight = rcDest.right;
								}
								::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
									hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
									lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
							}
						}
						else { // ytiled
							LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
							int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
							for( int i = 0; i < iTimes; ++i ) {
								LONG lDestTop = rcDest.top + lHeight * i;
								LONG lDestBottom = rcDest.top + lHeight * (i + 1);
								LONG lDrawHeight = lHeight;
								if( lDestBottom > rcDest.bottom ) {
									lDrawHeight -= lDestBottom - rcDest.bottom;
									lDestBottom = rcDest.bottom;
								}
								::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
									hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
									rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, SRCCOPY);                    
							}
						}
					}
				}

				//if(1) return;

				// left-top
				if( rcCorners.left > 0 && rcCorners.top > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.top;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
					}
				}
				// top
				if( rcCorners.top > 0 ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.top;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
							rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
					}
				}
				// right-top
				if( rcCorners.right > 0 && rcCorners.top > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.top;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
					}
				}
				// left
				if( rcCorners.left > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
							rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					}
				}
				// right
				if( rcCorners.right > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
							rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					}
				}
				// left-bottom
				if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
					}
				}
				// bottom
				if( rcCorners.bottom > 0 ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
					}
				}
				// right-bottom
				if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
							rcCorners.bottom, SRCCOPY);
					}
				}
			}
		}

		::SelectObject(hCloneDC, hOldBitmap);
		::DeleteDC(hCloneDC);
	}

	bool CRenderEngine::DrawImageInfo(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint
			, const TDrawInfo* pDrawInfo, const RECT* rcDest, HINSTANCE instance)
	{
		if( pManager == NULL || hDC == NULL || pDrawInfo == NULL ) return false;

		const TImageInfo* data = GetImageInfo(pManager, pDrawInfo, instance);
		if(!data) return false;
		RECT rcDraw; // 默认绘制于 rcItem
		CDuiSize szDraw{};

		// 计算绘制目标区域
		if(rcDest) {
			// 绘制在 rcDest 参数指定的位置
			rcDraw = *rcDest;
		}
		else 
		{
			// 默认绘制于 rcItem
			rcDraw = rcItem;
			szDraw = pDrawInfo->szIcon;
		}
		if(szDraw.cx==-2 || szDraw.cy==-2) 
		{
			// 尺寸 -2 代表绘制时使用原图宽高
			if (!pDrawInfo->sName.IsEmpty()) {
				if(szDraw.cx==-2) szDraw.cx = data->nX;
				if(szDraw.cy==-2) szDraw.cy = data->nY;
			}
		}

		// 根据对齐方式计算目标区域
		MakeImageDest(rcItem, szDraw, pDrawInfo->iAlign, pDrawInfo->rcPadding, rcDraw);


		RECT rcTemp;
		if( !::IntersectRect(&rcTemp, &rcItem, &rcItem) ) return true;
		if( !::IntersectRect(&rcTemp, &rcItem, &rcPaint) ) return true;

		//bool ret = DuiLib::GetImageInfoAndDraw(hDC, pManager, rcDraw, rcPaint
		//	, pDrawInfo->sName, pDrawInfo->sResType
		//	, pDrawInfo->rcSource, pDrawInfo->rcCorner
		//	, pDrawInfo->dwMask, pDrawInfo->uFade, pDrawInfo->bHole
		//	, pDrawInfo->bTiledX, pDrawInfo->bTiledY, instance);

		RECT rcBmpPart = pDrawInfo->rcSource;
		const RECT & rcCorner = pDrawInfo->rcCorner;

		if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) {
			rcBmpPart.right = data->nX;
			rcBmpPart.bottom = data->nY;
		}
		if (rcBmpPart.right > data->nX) rcBmpPart.right = data->nX;
		if (rcBmpPart.bottom > data->nY) rcBmpPart.bottom = data->nY;


#ifdef MODULE_SKIA_RENDERER
		if(true)
			CRenderEngine::DrawSkImage(pManager, data, rcItem, rcPaint, rcBmpPart, rcCorner, pManager->IsLayered() ? true : data->bAlpha, bFade, bHole, bTiledX, bTiledY);
		else
#endif
			CRenderEngine::DrawImage(hDC, data->hBitmap, rcDraw, rcPaint, rcBmpPart, rcCorner, pManager->IsLayered() ? true : data->bAlpha, pDrawInfo->uFade, pDrawInfo->bHole, pDrawInfo->bTiledX, pDrawInfo->bTiledY);

		//if (false)
		//{
		//	HDC hCloneDC = ::CreateCompatibleDC(hDC);
		//	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, data->hBitmap);
		//	::SetStretchBltMode(hDC, HALFTONE);
		//	::StretchBlt(hDC, rcItem.left, rcItem.top, rcItem.right-rcItem.left, rcItem.bottom-rcItem.top, hCloneDC, \
		//		0, 0, data->nX, data->nY, SRCCOPY);
		//	::SelectObject(hCloneDC, hOldBitmap);
		//	::DeleteDC(hCloneDC);
		//}
		//else
		//{
		//	// kOpaque_SkAlphaType
		//	auto decodeInfo = SkImageInfo::MakeN32(data->nX, data->nY, kPremul_SkAlphaType);
		//
		//	SkBitmap skBitmap;
		//
		//	skBitmap.setInfo(decodeInfo);
		//
		//	skBitmap.setPixels(data->pBits);
		//
		//	SkRect rect{rcItem.left, rcItem.top, rcItem.right, rcItem.bottom};
		//
		//	SkSamplingOptions options(SkFilterMode::kNearest, SkMipmapMode::kNone);
		//
		//	pManager->GetSkiaCanvas()->drawImageRect(skImage, rect, options);
		//}
		return true;
	}

	const TImageInfo* CRenderEngine::GetImageInfo(CPaintManagerUI* pManager, const TDrawInfo* pDrawInfo, HINSTANCE instance)
	{
		//sconst TDrawInfo* pDrawInfo = pManager->GetDrawInfo(pStrImage);
		if( pManager == NULL || pDrawInfo == NULL ) return false;
		if (pDrawInfo->sName.IsEmpty()) {
			return NULL;
		}
		const TImageInfo* data = NULL;
		if( pDrawInfo->sResType.IsEmpty() ) {
			data = pManager->GetImageEx(pDrawInfo->sName, NULL, pDrawInfo->dwMask, false, instance);
		}
		else {
			data = pManager->GetImageEx(pDrawInfo->sName, pDrawInfo->sResType, pDrawInfo->dwMask, false, instance);
		}
		return data;  
	}

	bool CRenderEngine::DrawImageString(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, LPCTSTR pStrImage, const RECT* rcDest, HINSTANCE instance)
	{
		if ((pManager == NULL) || (hDC == NULL)) return false;
		const TDrawInfo* pDrawInfo = pManager->GetDrawInfo(pStrImage);
		return DrawImageInfo(hDC, pManager, rcItem, rcPaint, pDrawInfo, rcDest, instance);
	}

	void CRenderEngine::DrawColor(HDC hDC, const RECT& rc, DWORD color)
	{
		if( (color & 0xFF000000)==0 ) return;

		Gdiplus::Graphics graphics( hDC );
		Gdiplus::SolidBrush brush(Gdiplus::Color((LOBYTE((color)>>24)), GetBValue(color), GetGValue(color), GetRValue(color)));
		graphics.FillRectangle(&brush, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}

	void CRenderEngine::FillRectHeteroSized(HDC hDC, const RECT& frameRect, const RECT& rcBorderSize, const RECT& rcborderInset, DWORD bordercolor)
	{
		RECT rcBorder;
		BYTE bR = (BYTE) GetBValue(bordercolor);
		BYTE bG = (BYTE) GetGValue(bordercolor);
		BYTE bB = (BYTE) GetRValue(bordercolor);
		HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
		if(rcBorderSize.left)
		{ // left
		    rcBorder = {frameRect.left + rcborderInset.left
		    	, frameRect.top + rcborderInset.top
		    	, frameRect.left + rcBorderSize.left + rcborderInset.left
		    	, frameRect.bottom - rcborderInset.bottom};
		    ::FillRect(hDC, &rcBorder, hBrush);
		}
		if(rcBorderSize.top)
		{ // top
			rcBorder = {frameRect.left + rcBorderSize.left + rcborderInset.left
				, frameRect.top + rcborderInset.top
				, frameRect.right - rcborderInset.right
				, frameRect.top + rcBorderSize.top + rcborderInset.top};
			::FillRect(hDC, &rcBorder, hBrush);
		}
		if(rcBorderSize.right)
		{ // right
			rcBorder = {frameRect.right - rcBorderSize.right - rcborderInset.right
				, frameRect.top + rcBorderSize.top + rcborderInset.top
				, frameRect.right - rcborderInset.right
				, frameRect.bottom - rcborderInset.bottom};
			::FillRect(hDC, &rcBorder, hBrush);
		}
		if(rcBorderSize.bottom)
		{ // bottom
			rcBorder = {frameRect.left + rcBorderSize.left + rcborderInset.left
				, frameRect.bottom - rcBorderSize.top - rcborderInset.bottom
				, frameRect.right - rcBorderSize.right - rcborderInset.right
				, frameRect.bottom - rcborderInset.bottom};
			::FillRect(hDC, &rcBorder, hBrush);
		}
		::DeleteObject(hBrush);
	}
	
	void CRenderEngine::FillRectHeteroSizedPlus(HDC hDC, const RECT& frameRect, const RECT& rcBorderSize, const RECT& rcborderInset, DWORD bordercolor)
	{
		RECT rcBorder;
		Gdiplus::Graphics graphis (hDC);
		Gdiplus::SolidBrush solidbrush (Gdiplus::Color((LOBYTE((bordercolor)>>24)), GetBValue(bordercolor), GetGValue(bordercolor), GetRValue(bordercolor)));
		LONG leftVal = frameRect.left + rcBorderSize.left + rcborderInset.left;
		if(rcBorderSize.left)
		{ // left
			graphis.FillRectangle(&solidbrush, frameRect.left + rcborderInset.left
		    	, frameRect.top + rcborderInset.top
		    	, rcBorderSize.left
		    	, frameRect.bottom - rcborderInset.bottom - (frameRect.top + rcborderInset.top));
		}
		if(rcBorderSize.top)
		{ // top
			graphis.FillRectangle(&solidbrush, leftVal
				, frameRect.top + rcborderInset.top
				, frameRect.right - rcborderInset.right - leftVal
				, rcBorderSize.top);
		}
		if(rcBorderSize.right)
		{ // right
			graphis.FillRectangle(&solidbrush, frameRect.right - rcBorderSize.right - rcborderInset.right
				, frameRect.top + rcBorderSize.top + rcborderInset.top
				, rcBorderSize.right
				, frameRect.bottom - rcborderInset.bottom - (frameRect.top + rcBorderSize.top + rcborderInset.top));
		}
		if(rcBorderSize.bottom)
		{ // bottom
			graphis.FillRectangle(&solidbrush, leftVal
				, frameRect.bottom - rcBorderSize.top - rcborderInset.bottom
				, frameRect.right - rcBorderSize.right - rcborderInset.right - leftVal
				, rcBorderSize.top);
		}
	}

	void CRenderEngine::DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps)
	{
		typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
		static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
		if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
		typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
		static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");

		BYTE bAlpha = (BYTE)(((dwFirst >> 24) + (dwSecond >> 24)) >> 1);
		if( bAlpha == 0 ) return;
		int cx = rc.right - rc.left;
		int cy = rc.bottom - rc.top;
		RECT rcPaint = rc;
		HDC hPaintDC = hDC;
		HBITMAP hPaintBitmap = NULL;
		HBITMAP hOldPaintBitmap = NULL;
		if( bAlpha < 255 ) {
			rcPaint.left = rcPaint.top = 0;
			rcPaint.right = cx;
			rcPaint.bottom = cy;
			hPaintDC = ::CreateCompatibleDC(hDC);
			hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
			ASSERT(hPaintDC);
			ASSERT(hPaintBitmap);
			hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
		}
		if( lpGradientFill != NULL ) 
		{
			TRIVERTEX triv[2] = 
			{
				{ rcPaint.left, rcPaint.top, 
				static_cast<COLOR16>(GetBValue(dwFirst) << 8),
				static_cast<COLOR16>(GetGValue(dwFirst) << 8),
				static_cast<COLOR16>(GetRValue(dwFirst) << 8), 0xFF00 },
				{ rcPaint.right, rcPaint.bottom, 
				static_cast<COLOR16>(GetBValue(dwSecond) << 8),
				static_cast<COLOR16>(GetGValue(dwSecond) << 8),
				static_cast<COLOR16>(GetRValue(dwSecond) << 8), 0xFF00 }
			};
			GRADIENT_RECT grc = { 0, 1 };
			lpGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
		}
		else 
		{
			// Determine how many shades
			int nShift = 1;
			if( nSteps >= 64 ) nShift = 6;
			else if( nSteps >= 32 ) nShift = 5;
			else if( nSteps >= 16 ) nShift = 4;
			else if( nSteps >= 8 ) nShift = 3;
			else if( nSteps >= 4 ) nShift = 2;
			int nLines = 1 << nShift;
			for( int i = 0; i < nLines; i++ ) {
				// Do a little alpha blending
				BYTE bR = (BYTE) ((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
				BYTE bG = (BYTE) ((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
				BYTE bB = (BYTE) ((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
				// ... then paint with the resulting color
				HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
				RECT r2 = rcPaint;
				if( bVertical ) {
					r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
					r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);
					if( (r2.bottom - r2.top) > 0 ) ::FillRect(hDC, &r2, hBrush);
				}
				else {
					r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
					r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);
					if( (r2.right - r2.left) > 0 ) ::FillRect(hPaintDC, &r2, hBrush);
				}
				::DeleteObject(hBrush);
			}
		}
		if( bAlpha < 255 ) {
			BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
			lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
			::SelectObject(hPaintDC, hOldPaintBitmap);
			::DeleteObject(hPaintBitmap);
			::DeleteDC(hPaintDC);
		}
	}

	void CRenderEngine::DrawLine( HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/ )
	{
		ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

		LOGPEN lg;
		lg.lopnColor = RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor));
		lg.lopnStyle = nStyle;
		lg.lopnWidth.x = nSize;
		HPEN hPen = CreatePenIndirect(&lg);
		HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
		POINT ptTemp = { 0 };
		::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
		::LineTo(hDC, rc.right, rc.bottom);
		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);
	}

	void CRenderEngine::DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/)
	{
		// Gdiplus更易于叠加透明图形，性能上没啥区别。
		if ((dwPenColor&0xFF000000)==0xFF000000)
		{
			ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
			HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
			HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
			::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
			::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
			::SelectObject(hDC, hOldPen);
			::DeleteObject(hPen);
		}
		else
		{
			ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
			Gdiplus::Graphics graphics(hDC);
			Gdiplus::Pen pen(Gdiplus::Color(dwPenColor), (Gdiplus::REAL)nSize);
			pen.SetAlignment(Gdiplus::PenAlignmentInset);

			graphics.DrawRectangle(&pen, rc.left, rc.top, rc.right - rc.left - 1, rc.bottom - rc.top - 1);
		}
	}

	// 绘制及填充圆角矩形
	void CRenderEngine::DrawRoundBorder(HDC hDC, RECT frameRect, float arcSize, RECT borderSize, Gdiplus::Color lineColor, Gdiplus::Color fillColor)
	{
		// 创建GDI+对象
		Gdiplus::Graphics  graphics(hDC);
		//if (1)
		//{
		//	// Create a path that consists of a single polygon.
		//	Point polyPoints[] = {Point(10, 10), Point(150, 10), 
		//		Point(100, 75), Point(100, 150)};
		//	GraphicsPath path;
		//	path.AddPolygon(polyPoints, 4);
		//	// Construct a region based on the path.
		//	Region region(&path);
		//	// Draw the outline of the region.
		//	Pen pen(Color(255, 0, 0, 0));
		//	graphics.DrawPath(&pen, &path);
		//	// Set the clipping region of the Graphics object.
		//	graphics.SetClip(&region);
		//	// Draw some clipped strings.
		//	FontFamily fontFamily(L"Arial");
		//	Font font(&fontFamily, 36, FontStyleBold, UnitPixel);
		//	SolidBrush solidBrush(Color(255, 255, 0, 0));
		//	graphics.DrawString(L"A Clipping Region", 20, &font, 
		//		PointF(15, 25), &solidBrush);
		//	graphics.DrawString(L"A Clipping Region", 20, &font, 
		//		PointF(15, 68), &solidBrush);
		//}
		float arcDiameter = arcSize * 2;
		//设置画图时的滤波模式为消除锯齿现象
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		//g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::SolidBrush brush(lineColor);

		// 绘图路径
		Gdiplus::GraphicsPath roundRectPath;

		float x = frameRect.left;
		float y = frameRect.top;
		float x1 = frameRect.right;
		float y1 = frameRect.bottom;
		
		float width = x1-x;
		float height = y1-y;

		x += borderSize.left/2;
		y += borderSize.top/2;

		x1 -= borderSize.right/2;
		y1 -= borderSize.bottom/2;

		Gdiplus::Pen pen(lineColor);

		roundRectPath.AddLine(x+borderSize.left/2, y, x1-borderSize.right/2, y);  // 顶部横线
		pen.SetWidth(borderSize.top);	
		graphics.DrawPath(&pen, &roundRectPath);
		graphics.FillPie(&brush, x1-borderSize.right/2-borderSize.right, (float)frameRect.top, (float)borderSize.right*2, (float)borderSize.top, 270, 90);


		roundRectPath.Reset();
		roundRectPath.AddLine(x1, y, x1, y1);  // 右侧竖线
		//roundRectPath.AddArc(x + width - arcDiameter, y + height - arcDiameter, arcDiameter, arcDiameter, 0, 90); // 右下圆角
		pen.SetWidth(borderSize.right);	
		graphics.DrawPath(&pen, &roundRectPath);
		graphics.FillPie(&brush, x1-borderSize.right/2-borderSize.right, (float)frameRect.bottom-borderSize.bottom, (float)borderSize.right*2, (float)borderSize.bottom, 0, 90);

		
		roundRectPath.Reset();
		roundRectPath.AddLine(x1-borderSize.right/2, y1, x+borderSize.left/2, y1);  // 底部横线
		//roundRectPath.AddArc(x, y + height - arcDiameter, arcDiameter, arcDiameter, 90, 90); // 左下圆角
		pen.SetWidth(borderSize.bottom);	
		graphics.DrawPath(&pen, &roundRectPath);
		graphics.FillPie(&brush, x-borderSize.left/2, (float)frameRect.bottom-borderSize.bottom, (float)borderSize.left*2, (float)borderSize.bottom, 90, 90);


		
		roundRectPath.Reset();
		roundRectPath.AddLine(x, y1, x, y);  // 左侧竖线
		//roundRectPath.AddArc(x, y, arcDiameter, arcDiameter, 180, 90 + 10); // 左上圆角
		pen.SetWidth(borderSize.left);	
		graphics.FillPie(&brush, x-borderSize.left/2, (float)frameRect.top, (float)borderSize.left*2, (float)borderSize.top, 180, 90);
		graphics.DrawPath(&pen, &roundRectPath);

												

	}

	// 绘制及填充圆角矩形
	void CRenderEngine::DrawRoundRectangle(HDC hDC, float x, float y, float width, float height, float arcSize, float lineWidth, Gdiplus::Color lineColor, bool fillPath, Gdiplus::Color fillColor)
	{
		float arcDiameter = arcSize * 2;
		// 创建GDI+对象
		Gdiplus::Graphics  graphics(hDC);
		//设置画图时的滤波模式为消除锯齿现象
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		//graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		// 绘图路径
		Gdiplus::GraphicsPath roundRectPath;

		// 保存绘图路径
		roundRectPath.AddLine(x + arcSize, y, x + width - arcSize, y);  // 顶部横线
		roundRectPath.AddArc(x + width - arcDiameter, y, arcDiameter, arcDiameter, 270, 90); // 右上圆角

		roundRectPath.AddLine(x + width, y + arcSize, x + width, y + height - arcSize);  // 右侧竖线
		roundRectPath.AddArc(x + width - arcDiameter, y + height - arcDiameter, arcDiameter, arcDiameter, 0, 90); // 右下圆角

		roundRectPath.AddLine(x + width - arcSize, y + height, x + arcSize, y + height);  // 底部横线
		roundRectPath.AddArc(x, y + height - arcDiameter, arcDiameter, arcDiameter, 90, 90); // 左下圆角

		roundRectPath.AddLine(x, y + height - arcSize, x, y + arcSize);  // 左侧竖线
		roundRectPath.AddArc(x, y, arcDiameter, arcDiameter, 180, 90 ); // 左上圆角

		if (lineWidth)
		{ // 勾勒
			Gdiplus::Pen pen(lineColor, lineWidth);
			graphics.DrawPath(&pen, &roundRectPath);
		}

		if(fillPath) 
		{ // 填充
			if(fillColor.GetAlpha() == 0) {
				fillColor = lineColor; // 若未指定填充色，则用线条色填充
			}
			Gdiplus::SolidBrush brush(fillColor);
			graphics.FillPath(&brush, &roundRectPath);
		}

		//LinearGradientBrush gradBrush(Point(x, y), Point(x, y+height), Color(255, 255, 0, 0), Color(255, 0, 0, 255));
		//Gdiplus::Color colors[] = {
		//	Gdiplus::Color(255, 255,	0,	 0),		//red   
		//	Gdiplus::Color(255, 255,	255, 0),		//yellow   
		//	Gdiplus::Color(255, 0,	0,	 255),		//blue   
		//	Gdiplus::Color(255, 0,	255, 0)			//green 
		//};		  
		//
		////按红黄蓝绿的顺序四种颜色渐变,四种颜色,各占四分之一
		//REAL positions[] = {
		//	0.0f,
		//	0.33f,
		//	0.66f,
		//	1.0f 
		//};
		//
		////设置插补颜色（插值法）
		//gradBrush.SetInterpolationColors(colors, positions, 4);
		//
		////填充指定区域矩形 
		//graphics.DrawPath(&gradBrush, &roundRectPath); 
	}

	// 绘制空心圆角矩形
	void CRenderEngine::DrawRoundRectangleHollow(HDC hDC, float x, float y, float width, float height, float arcSize, float lineWidth, Gdiplus::Color lineColor, bool fillPath, Gdiplus::Color fillColor)
	{
		float arcDiameter = arcSize * 2;
		// 创建GDI+对象
		Gdiplus::Graphics  graphics(hDC);
		//设置画图时的滤波模式为消除锯齿现象
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		//graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		// 绘图路径
		Gdiplus::GraphicsPath roundRectPath;

		// 保存绘图路径
		roundRectPath.AddLine(x + arcSize, y, x + width - arcSize, y);  // 顶部横线
		roundRectPath.AddArc(x + width - arcDiameter, y, arcDiameter, arcDiameter, 270, 90); // 右上圆角

		roundRectPath.AddLine(x + width, y + arcSize, x + width, y + height - arcSize);  // 右侧竖线
		roundRectPath.AddArc(x + width - arcDiameter, y + height - arcDiameter, arcDiameter, arcDiameter, 0, 90); // 右下圆角

		roundRectPath.AddLine(x + width - arcSize, y + height, x + arcSize, y + height);  // 底部横线
		roundRectPath.AddArc(x, y + height - arcDiameter, arcDiameter, arcDiameter, 90, 90); // 左下圆角

		roundRectPath.AddLine(x, y + height - arcSize, x, y + arcSize);  // 左侧竖线
		roundRectPath.AddArc(x, y, arcDiameter, arcDiameter, 180, 90 ); // 左上圆角

		if(fillPath) 
		{ // 填充
			if(fillColor.GetAlpha() == 0) {
				fillColor = lineColor; // 若未指定填充色，则用线条色填充
			}
			Gdiplus::SolidBrush brush(fillColor);
			if(lineWidth<arcSize*0.3)
				lineWidth = arcSize*0.3;
			Rect rect = {(int)(x+lineWidth), (int)(y+lineWidth), (int)(width-lineWidth*2), (int)(height-lineWidth*2)};
			graphics.ExcludeClip(rect);
			graphics.FillPath(&brush, &roundRectPath);
		}
	}

	void CRenderEngine::DrawRoundRectangleTest(HDC hDC, float x, float y, float width, float height, float arcSize, float lineWidth, Gdiplus::Color lineColor, bool fillPath, Gdiplus::Color fillColor)
	{
		float arcDiameter = arcSize * 2;
		Gdiplus::Graphics  graphics(hDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		//graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::GraphicsPath roundRectPath;

		roundRectPath.AddLine(x + arcSize, y, x + width - arcSize, y);  // 顶部横线
		roundRectPath.AddArc(x + width - arcDiameter, y, arcDiameter, arcDiameter-lineWidth, 270, 90); // 右上圆角

		//roundRectPath.AddLine(x + width, y + arcSize, x + width, y + height - arcSize);  // 右侧竖线
		//roundRectPath.AddArc(x + width - arcDiameter, y + height - arcDiameter, arcDiameter, arcDiameter, 0, 90); // 右下圆角
		//
		//roundRectPath.AddLine(x + width - arcSize, y + height, x + arcSize, y + height);  // 底部横线
		//roundRectPath.AddArc(x, y + height - arcDiameter, arcDiameter, arcDiameter, 90, 90); // 左下圆角
		//
		//roundRectPath.AddLine(x, y + height - arcSize, x, y + arcSize);  // 左侧竖线
		//roundRectPath.AddArc(x, y, arcDiameter, arcDiameter, 180, 90 ); // 左上圆角

		if (lineWidth)
		{ // 勾勒
			Gdiplus::Pen pen(lineColor, lineWidth);
			graphics.DrawPath(&pen, &roundRectPath);
		}

		if(fillPath) 
		{ // 填充
			if(fillColor.GetAlpha() == 0) {
				fillColor = lineColor; // 若未指定填充色，则用线条色填充
			}
			Gdiplus::SolidBrush brush(fillColor);
			graphics.FillPath(&brush, &roundRectPath);
		}
	}

	void CRenderEngine::DrawRoundRect(HDC hDC, const RECT& rc, int nSize, int width, int height, DWORD dwPenColor,int nStyle /*= PS_SOLID*/)
	{
#ifdef USE_GDI_RENDER
		ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		HPEN hPen = ::CreatePen(nStyle, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
		HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
		::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
		::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, width, height);
		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);
#else
		DrawRoundRectangle(hDC, rc.left, rc.top, rc.right - rc.left - 1, rc.bottom - rc.top - 1, width / 2, nSize, Gdiplus::Color(dwPenColor), false, Gdiplus::Color(dwPenColor));
#endif
	}

	void CRenderEngine::DrawPlainText(HDC hDC, CPaintManagerUI* pManager, RECT& rc
			, const QkString & pstrText, DWORD dwTextColor, int iFont, UINT uStyle)
	{
		ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		if( /*pstrText == NULL || */pManager == NULL ) return;

		if ( pManager->IsLayered() || pManager->IsUseGdiplusText())
		{
			HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
			Gdiplus::Graphics graphics( hDC );
			Gdiplus::Font font(hDC, pManager->GetFont(iFont));
			Gdiplus::TextRenderingHint trh = Gdiplus::TextRenderingHintSystemDefault;
			switch(pManager->GetGdiplusTextRenderingHint()) 
			{
			case 0: {trh = Gdiplus::TextRenderingHintSystemDefault; break;}
			case 1: {trh = Gdiplus::TextRenderingHintSingleBitPerPixelGridFit; break;}
			case 2: {trh = Gdiplus::TextRenderingHintSingleBitPerPixel; break;}
			case 3: {trh = Gdiplus::TextRenderingHintAntiAliasGridFit; break;}
			case 4: {trh = Gdiplus::TextRenderingHintAntiAlias; break;}
			case 5: {trh = Gdiplus::TextRenderingHintClearTypeGridFit; break;}
			}
			graphics.SetTextRenderingHint(trh);
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality); 
			graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

			Gdiplus::RectF rectF((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)(rc.right - rc.left), (Gdiplus::REAL)(rc.bottom - rc.top));
			Gdiplus::SolidBrush brush(Gdiplus::Color(254, GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

			Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();

			if ((uStyle & DT_END_ELLIPSIS) != 0) {
				stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
			}

			int formatFlags = 0;
			if ((uStyle & DT_NOCLIP) != 0) {
				formatFlags |= Gdiplus::StringFormatFlagsNoClip;
			}
			if ((uStyle & DT_SINGLELINE) != 0) {
				formatFlags |= Gdiplus::StringFormatFlagsNoWrap;
			}

			stringFormat.SetFormatFlags(formatFlags);

			if ((uStyle & DT_LEFT) != 0) {
				stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
			}
			else if ((uStyle & DT_CENTER) != 0) {
				stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
			}
			else if ((uStyle & DT_RIGHT) != 0) {
				stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
			}
			else {
				stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
			}
			stringFormat.GenericTypographic();
			if ((uStyle & DT_TOP) != 0) {
				stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			}
			else if ((uStyle & DT_VCENTER) != 0) {
				stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			}
			else if ((uStyle & DT_BOTTOM) != 0) {
				stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
			}
			else {
				stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			}
#ifdef UNICODE
			if ((uStyle & DT_CALCRECT) != 0)
			{
				Gdiplus::RectF bounds;

				graphics.MeasureString(pstrText, pstrText.GetLength(), &font, rectF, &stringFormat, &bounds);

				// MeasureString存在计算误差，这里加一像素
				rc.bottom = rc.top + (long)bounds.Height + 1;
				rc.right = rc.left + (long)bounds.Width + 1;
			}
			else
			{
				graphics.DrawString(pstrText, pstrText.GetLength(), &font, rectF, &stringFormat, &brush);
			}
#else
			DWORD dwSize = MultiByteToWideChar(CP_ACP, 0, pstrText, -1, NULL, 0);
			WCHAR * pcwszDest = new WCHAR[dwSize + 1];
			memset(pcwszDest, 0, (dwSize + 1) * sizeof(WCHAR));
			MultiByteToWideChar(CP_ACP, NULL, pstrText, -1, pcwszDest, dwSize);
			if(pcwszDest != NULL)
			{
				if ((uStyle & DT_CALCRECT) != 0)
				{
					Gdiplus::RectF bounds;
					graphics.MeasureString(pcwszDest, -1, &font, rectF, &stringFormat, &bounds);
					rc.bottom = rc.top + (long)(bounds.Height * 1.06);
					rc.right = rc.left + (long)(bounds.Width * 1.06);
				}
				else
				{
					graphics.DrawString(pcwszDest, -1, &font, rectF, &stringFormat, &brush);
				}
				delete []pcwszDest;
			}
#endif
			::SelectObject(hDC, hOldFont);
		}
		else
		{
			::SetBkMode(hDC, TRANSPARENT);
			::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
			HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
			int fonticonpos = pstrText.Find(_T("&#x"));
			if (fonticonpos != -1) {
				QkString strUnicode = pstrText.Mid(fonticonpos + 3);
				if (strUnicode.GetLength() > 4) strUnicode = strUnicode.Mid(0,4);
				if (strUnicode.Right(1).CompareNoCase(_T(" ")) == 0) {
					strUnicode = strUnicode.Mid(0, strUnicode.GetLength() - 1);
				}
				if (strUnicode.Right(1).CompareNoCase(_T(";")) == 0) {
					strUnicode = strUnicode.Mid(0,strUnicode.GetLength()-1);
				}
				wchar_t wch[2] = { 0 };
				wch[0] = static_cast<wchar_t>(_tcstol(strUnicode.GetData(), 0, 16));
				::DrawTextW(hDC, wch, -1, &rc, uStyle);
			}
			else {
				::DrawText(hDC, pstrText, pstrText.GetLength(), &rc, uStyle);
			}
			::SelectObject(hDC, hOldFont);
		}
	}

	void CRenderEngine::DrawPlainText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, const QkString & pstrText,DWORD dwTextColor, \
		int iFont, UINT uStyle, DWORD dwTextBKColor)
	{
		ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		if( /*pstrText == NULL || */pManager == NULL ) return;
		DrawColor(hDC, rc, dwTextBKColor);
		DrawPlainText(hDC, pManager, rc, pstrText, dwTextColor, iFont, uStyle);
	}

	HBITMAP CRenderEngine::GenerateBitmap(CPaintManagerUI* pManager, RECT rc, CControlUI* pStopControl, DWORD dwFilterColor)
	{
		if (pManager == NULL) return NULL;
		int cx = rc.right - rc.left;
		int cy = rc.bottom - rc.top;

		bool bUseOffscreenBitmap = true;
		HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
		ASSERT(hPaintDC);
		HBITMAP hPaintBitmap = NULL;
		//if (pStopControl == NULL && !pManager->IsLayered()) hPaintBitmap = pManager->Get();
		if( hPaintBitmap == NULL ) {
			bUseOffscreenBitmap = false;
			hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
			ASSERT(hPaintBitmap);
		}
		HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
		if (!bUseOffscreenBitmap) {
			CControlUI* pRoot = pManager->GetRoot();
			pRoot->Paint(hPaintDC, rc, pStopControl);
		}

		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = cx;
		bmi.bmiHeader.biHeight = cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = cx * cy * sizeof(DWORD);
		LPDWORD pDest = NULL;
		HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
		HBITMAP hBitmap = ::CreateDIBSection(pManager->GetPaintDC(), &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
		ASSERT(hCloneDC);
		ASSERT(hBitmap);
		if( hBitmap != NULL )
		{
			HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
			::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
			RECT rcClone = {0, 0, cx, cy};
			if (dwFilterColor & 0xFF000000) DrawColor(hCloneDC, rcClone, dwFilterColor);
			::SelectObject(hCloneDC, hOldBitmap);
			::DeleteDC(hCloneDC);  
			::GdiFlush();
		}

		// Cleanup
		::SelectObject(hPaintDC, hOldPaintBitmap);
		if (!bUseOffscreenBitmap) ::DeleteObject(hPaintBitmap);
		::DeleteDC(hPaintDC);

		return hBitmap;
	}

	HBITMAP CRenderEngine::GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc, DWORD dwFilterColor)
	{
		if (pManager == NULL || pControl == NULL) return NULL;
		int cx = rc.right - rc.left;
		int cy = rc.bottom - rc.top;

		HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
		HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
		ASSERT(hPaintDC);
		ASSERT(hPaintBitmap);
		HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
		pControl->Paint(hPaintDC, rc, NULL);

		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = cx;
		bmi.bmiHeader.biHeight = cy;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = cx * cy * sizeof(DWORD);
		LPDWORD pDest = NULL;
		HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
		HBITMAP hBitmap = ::CreateDIBSection(pManager->GetPaintDC(), &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
		ASSERT(hCloneDC);
		ASSERT(hBitmap);
		if( hBitmap != NULL )
		{
			HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
			::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
			RECT rcClone = {0, 0, cx, cy};
			if (dwFilterColor & 0xFF000000) DrawColor(hCloneDC, rcClone, dwFilterColor);
			::SelectObject(hCloneDC, hOldBitmap);
			::DeleteDC(hCloneDC);  
			::GdiFlush();
		}

		// Cleanup
		::SelectObject(hPaintDC, hOldPaintBitmap);
		::DeleteObject(hPaintBitmap);
		::DeleteDC(hPaintDC);

		return hBitmap;
	}

	SIZE CRenderEngine::GetTextSize( HDC hDC, CPaintManagerUI* pManager , LPCTSTR pstrText, int iFont, UINT uStyle )
	{
		SIZE size = {0,0};
		ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		if( pstrText == NULL || pManager == NULL ) return size;
		::SetBkMode(hDC, TRANSPARENT);
		HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
		GetTextExtentPoint32(hDC, pstrText, _tcslen(pstrText) , &size);
		::SelectObject(hDC, hOldFont);
		return size;
	}

	void CRenderEngine::CheckAlphaColor(DWORD& dwColor)
	{
		//RestoreAlphaColor认为0x00000000是真正的透明，其它都是GDI绘制导致的
		//所以在GDI绘制中不能用0xFF000000这个颜色值，现在处理是让它变成RGB(0,0,1)
		//RGB(0,0,1)与RGB(0,0,0)很难分出来
		if((0x00FFFFFF & dwColor) == 0)
		{
			dwColor += 1;
		}
	}

	HBITMAP CRenderEngine::CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE** pBits)
	{
		LPBITMAPINFO lpbiSrc = NULL;
		lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
		if (lpbiSrc == NULL) return NULL;

		lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbiSrc->bmiHeader.biWidth = cx;
		lpbiSrc->bmiHeader.biHeight = cy;
		lpbiSrc->bmiHeader.biPlanes = 1;
		lpbiSrc->bmiHeader.biBitCount = 32;
		lpbiSrc->bmiHeader.biCompression = BI_RGB;
		lpbiSrc->bmiHeader.biSizeImage = cx * cy;
		lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
		lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
		lpbiSrc->bmiHeader.biClrUsed = 0;
		lpbiSrc->bmiHeader.biClrImportant = 0;

		HBITMAP hBitmap = CreateDIBSection (hDC, lpbiSrc, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);
		delete [] lpbiSrc;
		return hBitmap;
	}

	void CRenderEngine::AdjustImage(bool bUseHSL, TImageInfo* imageInfo, short H, short S, short L)
	{
		if( imageInfo == NULL || imageInfo->bUseHSL == false || imageInfo->hBitmap == NULL || 
			imageInfo->pBits == NULL || imageInfo->pSrcBits == NULL ) 
			return;
		if( bUseHSL == false || (H == 180 && S == 100 && L == 100)) {
			::CopyMemory(imageInfo->pBits, imageInfo->pSrcBits, imageInfo->nX * imageInfo->nY * 4);
			return;
		}

		float fH, fS, fL;
		float S1 = S / 100.0f;
		float L1 = L / 100.0f;
		for( int i = 0; i < imageInfo->nX * imageInfo->nY; i++ ) {
			RGBtoHSL(*(DWORD*)(imageInfo->pSrcBits + i*4), &fH, &fS, &fL);
			fH += (H - 180);
			fH = fH > 0 ? fH : fH + 360; 
			fS *= S1;
			fL *= L1;
			HSLtoRGB((DWORD*)(imageInfo->pBits + i*4), fH, fS, fL);
		}
	}

} // namespace DuiLib
