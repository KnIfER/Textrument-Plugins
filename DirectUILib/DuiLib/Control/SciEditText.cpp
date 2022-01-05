//#include "StdAfx.h"
//#include "UILib.h"

#define UILIB_API

#include "Core\UICore.h"

#include "SciEditText.h"
#include "Core\InsituDebug.h"
#include "Core\ControlFactory.h"
#include "WindowsEx\comctl32.h"

using namespace Scintilla;

namespace DuiLib {
	IMPLEMENT_QKCONTROL(SciEditText)

	SciEditText::SciEditText()
		: CControlUI()
		, ScintillaCall()
	{
		m_dwBackColor = RGB(0, 0, 255);
		SetDirectUI(false);
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
		if (!_hWnd && _parent->GetHWND())
		{
			_hParent = _parent->GetHWND();
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
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				0, 0,
				100, 100,
				_hParent,
				NULL,
				CPaintManagerUI::GetInstance(),
				nullptr);

			//LogIs("Scintilla_hWnd:: %d", _hWnd);

			//::ShowWindow(_hWnd, TRUE);
			//::SetWindowText(_hWnd, TEXT("TEST"));
			//if (!dynamic_cast<WinFrame*>(_parent))
			//{
			//	GetRoot()->_WNDList.push_back(this);
			//}

			SciFnDirectStatus fn_ = reinterpret_cast<SciFnDirectStatus>(
				::SendMessage(_hWnd, (UINT)Message::GetDirectStatusFunction, 0, 0));
			const sptr_t ptr_ = ::SendMessage(_hWnd, (UINT)Message::GetDirectPointer, 0, 0);
			SetFnPtr(fn_, ptr_);
		}
	}

	void SciEditText::SetPos(RECT rc, bool bNeedInvalidate) 
	{
		m_rcItem = rc;
		::MoveWindow(_hWnd, rc.left, rc.top
			, rc.right - rc.left, rc.bottom - rc.top
			, TRUE);
		::UpdateWindow(_hWnd);
	}
} // namespace DuiLib
