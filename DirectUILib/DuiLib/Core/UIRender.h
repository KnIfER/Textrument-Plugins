#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#pragma once
namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CRenderClip
	{
	public:
		~CRenderClip();
		RECT rcItem;
		HDC hDC = 0;
		HRGN hRgn;
		HRGN hOldRgn;

		static void GenerateClip(HDC hDC, RECT rc, CRenderClip& clip);
		static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip);
		static void UseOldClipBegin(HDC hDC, CRenderClip& clip);
		static void UseOldClipEnd(HDC hDC, CRenderClip& clip);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CRenderEngine
	{
	public:
		static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);
		static HBITMAP CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE** pBits);
		static void AdjustImage(bool bUseHSL, TImageInfo* imageInfo, short H, short S, short L);
		static TImageInfo* LoadImageStr(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0, HINSTANCE instance = NULL, int bytesPerPixel = 4);
#ifdef USE_XIMAGE_EFFECT
		static CxImage *LoadGifImageX(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
#endif
		static void FreeImage(TImageInfo* bitmap, bool bDelete = true);
		static TImageInfo* LoadImageStr(LPCTSTR pStrImage, LPCTSTR type = NULL, DWORD mask = 0, HINSTANCE instance = NULL);
		static TImageInfo* LoadImageStr(UINT nID, LPCTSTR type = NULL, DWORD mask = 0, HINSTANCE instance = NULL);

		static Gdiplus::Image*	GdiplusLoadImage(LPCTSTR pstrPath);
		static Gdiplus::Image* GdiplusLoadImage(LPVOID pBuf, size_t dwSize);

		static bool MakeImageDest(const RECT& rcControl, const CDuiSize& szImage, short iAlign, const RECT& rcPadding, RECT& rcDest);

		static void DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint, \
			const RECT& rcBmpPart, const RECT& rcCorners, bool bAlpha, BYTE uFade = 255, 
			bool hole = false, bool xtiled = false, bool ytiled = false);

		static void DrawSkImage(CPaintManagerUI* pManager, const TImageInfo* data, const RECT& rc, const RECT& rcPaint,
			const RECT& rcBmpPart, const RECT& rcCorners, bool bAlpha, 
			BYTE uFade, bool hole, bool xtiled, bool ytiled);

		static bool DrawImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const QkString& sImageName, \
			const QkString& sImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner, DWORD dwMask, BYTE bFade, \
			bool bHole, bool bTiledX, bool bTiledY, HINSTANCE instance = NULL);
		static const TImageInfo* ParseImageString(CPaintManagerUI* pManager, LPCTSTR pStrImage, LPCTSTR pStrModify, HINSTANCE instance = NULL);
		static bool DrawImageInfo(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, const TDrawInfo* pDrawInfo, HINSTANCE instance = NULL);
		static bool DrawImageString(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, LPCTSTR pStrImage, TDrawInfo* modify, HINSTANCE instance = NULL);

		static void DrawColor(HDC hDC, const RECT& rc, DWORD color);
		static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);
		
		static void FillRectHeteroSized(HDC hDC, const RECT& frameRect, const RECT& rcBorderSize, const RECT& rcborderInset, DWORD bordercolor);
		static void FillRectHeteroSizedPlus(HDC hDC, const RECT& frameRect, const RECT& rcBorderSize, const RECT& rcborderInset, DWORD bordercolor);

		// 以下函数中的颜色参数alpha值无效
		static void DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
		static void DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
		static void DrawRoundRectangle(HDC hDC, float x, float y, float width, float height, float arcSize, float lineWidth, Gdiplus::Color lineColor, bool fillPath, Gdiplus::Color fillColor);
		static void DrawRoundRectangleHollow(HDC hDC, float x, float y, float width, float height, float arcSize, float lineWidth, Gdiplus::Color lineColor, bool fillPath, Gdiplus::Color fillColor);
		static void DrawRoundRectangleTest(HDC hDC, float x, float y, float width, float height, float arcSize, float lineWidth, Gdiplus::Color lineColor, bool fillPath, Gdiplus::Color fillColor);
		static void DrawRoundBorder(HDC hDC, RECT frameRect, float arcSize, RECT borderSize, Gdiplus::Color lineColor, Gdiplus::Color fillColor);
		static void DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
		static void DrawPlainText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, const QkString & pstrText, \
			DWORD dwTextColor, int iFont, UINT uStyle);
		static void DrawPlainText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, const QkString & pstrText,DWORD dwTextColor, \
			int iFont, UINT uStyle, DWORD dwTextBKColor);
		static void DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, 
			DWORD dwTextColor, RECT* pLinks, QkString* sLinks, int& nLinkRects, int iFont, UINT uStyle);
		static HBITMAP GenerateBitmap(CPaintManagerUI* pManager, RECT rc, CControlUI* pStopControl = NULL, DWORD dwFilterColor = 0);
		static HBITMAP GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc, DWORD dwFilterColor = 0);
		static SIZE GetTextSize(HDC hDC, CPaintManagerUI* pManager , LPCTSTR pstrText, int iFont, UINT uStyle);

		//alpha utilities
		static void CheckAlphaColor(DWORD& dwColor);
	};

} // namespace DuiLib

#endif // __UIRENDER_H__
