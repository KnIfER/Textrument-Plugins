#pragma once

#include <map>
#include <ShellAPI.h>

#include "..\DuiLib\UIlib.h"

using namespace DuiLib;

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);

public:
	CDuiString GetSkinFile();
	LPCTSTR GetWindowClassName() const ;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void InitWindow() override;
	//LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	void Notify(TNotifyUI& msg);
	void OnFinalMessage(HWND hWnd);

private:
	CHorizontalLayoutUI* pAnimWnd;
	HMENU			hMenu;
	NOTIFYICONDATA	nid;


	CButtonUI*		mpCloseBtn;
	CButtonUI*		mpMaxBtn;
	CButtonUI*		mpRestoreBtn;
	CButtonUI*		mpMinBtn;
};
