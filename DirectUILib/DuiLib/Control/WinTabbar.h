#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API WinTabbar : public CContainerUI
	{
		DECLARE_QKCONTROL(WinTabbar)
	public:
		WinTabbar();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void Init() override;
		void SetPos(RECT rc, bool bNeedInvalidate = true) override;

		void resize();

		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) override;

		//////////////// Tabbar Funcs ////////////////
		void addTab(int position, const TCHAR *text, LPARAM lParam=NULL, int image=NULL);

		bool closeTabAt(int position);
		
		HIMAGELIST setImageList(int capatity=16, int bytesPerPixel=3, int iconWidth=24, int iconHeight=24);

		int addImageToList(HBITMAP hBitmap, COLORREF maskColor);

		bool setTabFont(int fontSize, TCHAR* fontName=NULL);

		void setTabCloseImages(int inactive, int selected, int hovered, int pushed);

		void setTabPadding(int paddingX, int paddingY);

		void DoEvent(TEventUI& event);

		void SetEventListener(Listener & listener);

		LPARAM GetTabDataAt(int position);

		void SetMultiLine(bool enabled, int maxLns=3);
		void SetVerticle(bool enabled);
		void SetBottom(bool enabled);
	private:
		static LRESULT CALLBACK TabWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		HIMAGELIST _hImageList = 0;
		WNDPROC _defaultProc;
		UINT m_uWndState;
	};

} // namespace DuiLib