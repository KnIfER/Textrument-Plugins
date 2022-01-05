#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	// Depreacated. Test-Only.
	class UILIB_API WinButton : public CControlUI
	{
		DECLARE_QKCONTROL(WinButton)
	public:
		WinButton();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void Init() override;
		void SetPos(RECT rc, bool bNeedInvalidate = true) override;

		void SetText( LPCTSTR pstrText ) override;
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

		void SetType(LPCTSTR pstrText);

	private:
		HWND __hParent;
		int _type;
	};

} // namespace DuiLib