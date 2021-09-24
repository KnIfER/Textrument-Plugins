#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API WinButton : public CControlUI
	{
		DECLARE_DUICONTROL(WinButton)
	public:
		WinButton();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void Init() override;
		void SetPos(RECT rc, bool bNeedInvalidate = true) override;

		void resize();

	private:
		HWND __hParent;
	};

} // namespace DuiLib