#pragma once

#include "../StdAfx.h"

class SkBitmap;

namespace DuiLib
{
	class UILIB_API ImageView : public CControlUI
	{
		DECLARE_QKCONTROL(ImageView)
	public:
		ImageView();

		void DoEvent(TEventUI& event) override;

		LPCTSTR GetClass() const override;

		//void PaintBkColor(HDC hDC) override;

		void PaintBkImage(HDC hDC) override;

		void PaintForeColor(HDC hDC) override;

		void PaintForeImage(HDC hDC) override;

		virtual int LoadImageFile(CHAR* path);

		bool _interactive = false;

	protected:
		virtual void ApplyStrechMode(HDC hDC);

		bool bPreciseDraw = false;

	private:
		size_t pxSize = 0;
		SkBitmap* skBitmap;

		HDC hdcMem = NULL;
		HBITMAP hBitmap = NULL;

		int _srcWidth;
		int _srcHeight;
		float _scale = 1;

		int drawCnt = 0 ;
		DWORD lastDrawTm;
		BITMAPINFO* bmpInfo = NULL;
		size_t bmpSize = 0;


		float _minScale = 1;
		bool _bNewImage = false;
		bool _bFit = true;

		int _translationX = 0;
		int _translationY = 0;
		bool _moving = false;
		int _moveLastX = 0;
		int _moveLastY = 0;
	};
}