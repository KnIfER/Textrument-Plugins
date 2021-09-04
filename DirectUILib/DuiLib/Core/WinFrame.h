#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API WinFrame : public CContainerUI
	{
		DECLARE_DUICONTROL(WinFrame)
	public:
		WinFrame();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void Init() override;
		void SetPos(RECT rc, bool bNeedInvalidate = true) override;

		void resize();

		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) override;

		bool Add(CControlUI* pControl) override;
	protected:
		CWindowWnd* _WND;
	};

} // namespace DuiLib