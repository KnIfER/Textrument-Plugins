#include "StdAfx.h"
#include "Core\InsituDebug.h"
#include "Core\ControlFactory.h"
#include "WindowsEx\comctl32.h"

using namespace Scintilla;

namespace DuiLib {
	IMPLEMENT_DUICONTROL(SciEditText)

	SciEditText::SciEditText()
		: CControlUI()
		, ScintillaCall()
	{
		m_dwBackColor = RGB(0, 0, 255);
		_isDirectUI = true;
	}

	LPCTSTR SciEditText::GetClass() const
	{
		return L"SciEditText";
	}

	LPVOID SciEditText::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, L"SciEditText") == 0 ) return static_cast<SciEditText*>(this);
		return __super::GetInterface(pstrName);
	}

	struct DemoData
	{
		const TCHAR* title;
		int image;
	};


	void SciEditText::Init()
	{
		_hParent = m_pParent->GetHWND();
		// see SciTEWin.cxx
		//Lexilla::SetDefaultDirectory(GetSciTEPath(FilePath()).AsUTF8());

		Scintilla_RegisterClasses(CPaintManagerUI::GetInstance());
		Lexilla::SetDefault(CreateLexer);

		//LogIs("_hParent::%d", _hParent);
		
		// see SciTEWinBar.cxx
		_hWnd = ::CreateWindowEx(
			0,
			TEXT("Scintilla"),
			TEXT("EditText"),
			WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0,
			100, 100,
			_hParent,
			NULL,
			CPaintManagerUI::GetInstance(),
			nullptr);
		
		//LogIs("Scintilla_hWnd:: %d", _hWnd);

		::ShowWindow(_hWnd, TRUE);
		//::SetWindowText(_hWnd, TEXT("TEST"));
		//if (!dynamic_cast<WinFrame*>(m_pParent))
		//{
		//	GetRoot()->_WNDList.push_back(this);
		//}

		SciFnDirectStatus fn_ = reinterpret_cast<SciFnDirectStatus>(
			::SendMessage(_hWnd, (UINT)Message::GetDirectStatusFunction, 0, 0));
		const sptr_t ptr_ = ::SendMessage(_hWnd, (UINT)Message::GetDirectPointer, 0, 0);
		SetFnPtr(fn_, ptr_);

	}

	void SciEditText::SetPos(RECT rc, bool bNeedInvalidate) 
	{
		//__super::SetPos(rc, bNeedInvalidate);
		m_rcItem = rc;
		resize();
	}

	void SciEditText::resize() 
	{
		if(_hWnd) {
			RECT rcPos = m_rcItem;

			//::SetWindowPos(_hWnd, NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
			
			::MoveWindow(_hWnd, rcPos.left, rcPos.top
				, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top
				, TRUE);

		}
	}
	

} // namespace DuiLib
