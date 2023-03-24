#pragma once

#include "../StdAfx.h"
#include "include/core/SkRefCnt.h"

class SkBitmap;
class SkImage;
class SkCanvas;
class SkSurface;

#undef LoadImage;

namespace DuiLib
{
	class UILIB_API SkImageView : public CControlUI
	{
		DECLARE_DUICONTROL(SkImageView)
	public:
		SkImageView();

		void DoEvent(TEventUI& event) override;

		LPCTSTR GetClass() const override;

		void PaintBkColor(HDC hDC) override;

		void PaintForeColor(HDC hDC) override;

		void PaintForeImage(HDC hDC) override;

		void Draw(SkCanvas* canvas,int w,int h);

		int LoadImage(CHAR* path);
	private:
		char* pixels = nullptr;
		size_t pxSize = 0;
		SkBitmap* skBitmap;
		sk_sp<SkImage> skImage;
		sk_sp<SkSurface> surface;

		int _srcWidth;
		int _srcHeight;
		float _scale = 1;

		int drawCnt = 0 ;
		DWORD lastDrawTm;
		BITMAPINFO* bmpInfo = NULL;
		size_t bmpSize = 0;
	};
}