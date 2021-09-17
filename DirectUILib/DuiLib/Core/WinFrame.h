#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API WinFrame : public CContainerUI
	{
		DECLARE_DUICONTROL(WinFrame)
	public:
		WinFrame();
		~WinFrame();
		WinFrame(HWND windowHandle);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void Init() override;
		void SetPos(RECT rc, bool bNeedInvalidate = true) override;
		CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags) override;
	protected:
		CWindowWnd* wEmbedded;
	};

} // namespace DuiLib