#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API WinTabbar : public CContainerUI
	{
		DECLARE_DUICONTROL(WinTabbar)
	public:
		WinTabbar();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void Init() override;
		void SetPos(RECT rc, bool bNeedInvalidate = true) override;

		void resize();

		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) override;
	};

} // namespace DuiLib