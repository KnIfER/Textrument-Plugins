#pragma once
using namespace DuiLib;

class CListDemoWnd : public WindowImplBase
{
public:
	CListDemoWnd();
	~CListDemoWnd();

	virtual void OnClick(TNotifyUI& msg) override;
	virtual void Notify(TNotifyUI& msg) override;
// 	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

//	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	void ShowRes();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;

	virtual LPCTSTR    GetWindowClassName(void) const override;
	virtual UILIB_RESTYPE GetResourceType() const;
	virtual CDuiString GetSkinType();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
private:
};