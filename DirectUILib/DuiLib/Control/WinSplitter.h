#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API WinSplitter : public CContainerUI
	{
		DECLARE_QKCONTROL(WinSplitter)
	public:
		WinSplitter();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void Init() override;
		void SetPos(RECT rc, bool bNeedInvalidate = true) override;

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

		//////////////// Splitter Funcs ////////////////
		void SetRatio(float ratio);
		float GetRatio() { return _ratio; };
		void SetHorizontal(bool horizontal) { _horizontal = horizontal; };
		bool IsHorizontal() { return _horizontal; };

	private:
		static LRESULT CALLBACK SplitterWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT RunProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		WNDPROC _defaultProc;
		float _ratio;
		bool _horizontal;
		bool _isDragging;
		long _dragSt;
		int _dragStSz;
		int _dragStItSz;
		HWND _hSplitter;
	};

} // namespace DuiLib