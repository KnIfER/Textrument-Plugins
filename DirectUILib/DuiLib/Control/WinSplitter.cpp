#include "StdAfx.h"
#include "Core\InsituDebug.h"
#include "WindowsEx\comctl32.h"

extern void TAB_Register();

namespace DuiLib {
	IMPLEMENT_QKCONTROL(WinSplitter)

		WinSplitter::WinSplitter()
		: CContainerUI()
	{
		m_dwBackColor = RGB(0, 0, 255);
		//_view_states &= ~VIEWSTATEMASK_IsDirectUI;
		_ratio = 0.5f;
		_horizontal = false;
		_isDragging = false;
		_hSplitter = 0;
	}

	LPCTSTR WinSplitter::GetClass() const
	{
		return L"WinSplitter";
	}

	LPVOID WinSplitter::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, L"WinSplitter") == 0 ) return static_cast<WinSplitter*>(this);
		return __super::GetInterface(pstrName);
	}

	LRESULT CALLBACK WinSplitter::SplitterWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		WinSplitter* sPane = (WinSplitter*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		return sPane->RunProc(hWnd, message, wParam, lParam);
	}
	
	LRESULT WinSplitter::RunProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_SETCURSOR: {
				if (_horizontal)
				{
					::SetCursor(LoadCursor(0, (LPWSTR)IDC_SIZENS));
					return true;
				}
			} break;

			case WM_LBUTTONDOWN: {
				if (!_isDragging && m_items.GetSize()>0)
				{
					_isDragging = true;
					::SetCapture(hWnd);
					CControlUI* pControl_A = static_cast<CControlUI*>(m_items[0]);
					POINT pt;
					::GetCursorPos(&pt);
					if (_horizontal)
					{
						_dragSt = pt.y;
						_dragStSz = GetHeight();
						_dragStItSz = pControl_A->GetHeight();
					}
					else
					{
						_dragSt = pt.x;
						_dragStSz = GetWidth();
						_dragStItSz = pControl_A->GetWidth();
					}
				}
			} return true;

			case WM_LBUTTONUP: {
				_isDragging = false;
				::ReleaseCapture();
			} return true;

			case WM_MOUSEMOVE: {
				if (_isDragging && _dragStSz>0)
				{
					POINT pt;
					::GetCursorPos(&pt);
					_ratio = (_dragStItSz+((_horizontal?pt.y:pt.x)-_dragSt))*1.f/_dragStSz;
					if (_ratio<0)
					{
						_ratio = 0;
					}
					if (0)
					{
						NeedParentUpdate();
					} 
					else 
					{
						SetPos(GetPos(), true);
						//SendMessage(_hParent, WM_SIZE, 0, 0);
						//::UpdateWindow(_hParent);
					}
				}
			} return true;

			case WM_PAINT: {
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				RECT rc;

				::GetClientRect(hWnd, &rc);
				SelectBrush(hdc, GetSysColorBrush(COLOR_BTNHIGHLIGHT));
				//::Rectangle(hdc, 0, 0, rc.right, rc.bottom);
				::FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNHIGHLIGHT));

				::EndPaint(hWnd, &ps);
				return true;
			} break;
			case WM_ERASEBKGND: {
				return true;
			} break;
		}
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	const TCHAR* wndClassName = L"wnd_splt";

	void WinSplitter::Init()
	{
		if (!_hSplitter && _parent->GetHWND())
		{
			_hParent = _parent->GetHWND();
			{
				WNDCLASSW wndClass;

				ZeroMemory (&wndClass, sizeof(WNDCLASSW));
				wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
				wndClass.lpfnWndProc   = SplitterWndProc;
				wndClass.cbClsExtra    = 0;
				wndClass.cbWndExtra    = 0;
				wndClass.hCursor       = LoadCursorW (0, (LPWSTR)IDC_SIZEWE);
				wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
				wndClass.lpszClassName = wndClassName;

				RegisterClassW (&wndClass);
			}

			//LogIs("_hParent::%d", _hParent);
			DWORD style = WS_CHILD | WS_VISIBLE 
				//| WS_THICKFRAME // 0x40000L  resizable
				;

			_hSplitter = ::CreateWindowEx(
				0,
				wndClassName,
				TEXT("sep"),
				style,
				0, 0, 0, 0,
				_hParent,
				NULL,
				CPaintManagerUI::GetInstance(),
				0);

			SetWindowLongPtr(_hSplitter, GWLP_USERDATA, (LONG_PTR)this);
		}
	}
	 
	void WinSplitter::SetPos(RECT rc, bool bNeedInvalidate) 
	{
		int sepWidth = 10;
		m_rcItem = rc;
		ApplyInsetToRect(rc);
		if (m_items.GetSize()>=2)
		{
			if (_horizontal)
			{
				float bottom = rc.bottom;
				float height = rc.bottom - rc.top;
				CControlUI* pControl_A = static_cast<CControlUI*>(m_items[0]);
				CControlUI* pControl_B = static_cast<CControlUI*>(m_items[1]);
				rc.bottom = rc.top + height*_ratio;
				if (rc.bottom+sepWidth>bottom)
				{
					rc.bottom = bottom-sepWidth;
				}
				::MoveWindow(_hSplitter, rc.left, rc.bottom, rc.right-rc.left, sepWidth, TRUE);
				::UpdateWindow(_hSplitter);
				pControl_A->SetPos(rc, bNeedInvalidate);
				
				rc.top = rc.bottom + sepWidth;
				rc.bottom = bottom;
				pControl_B->SetPos(rc, bNeedInvalidate);
			}
			else
			{
				float right = rc.right;
				float width = rc.right - rc.left;
				CControlUI* pControl_A = static_cast<CControlUI*>(m_items[0]);
				CControlUI* pControl_B = static_cast<CControlUI*>(m_items[1]);
				rc.right = rc.left + width*_ratio;
				if (rc.right+sepWidth>right)
				{
					rc.right = right-sepWidth;
				}
				//BringWindowToTop(hSplitter);
				//ShowWindow(hSplitter, SW_HIDE);
				::MoveWindow(_hSplitter, rc.right, rc.top, sepWidth, rc.bottom-rc.top, TRUE);
				::UpdateWindow(_hSplitter);

				pControl_A->SetPos(rc, bNeedInvalidate);
				rc.left = rc.right + sepWidth;
				rc.right = right;
				pControl_B->SetPos(rc, bNeedInvalidate);
			}
		}
		else if (m_items.GetSize()==1)
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[0]);
			pControl->SetPos(rc);
		}
	}

	void WinSplitter::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("horizon")) == 0 ) {
			_horizontal = _tcsicmp(pstrValue, _T("true")) == 0;
		} else {
			__super::SetAttribute(pstrName, pstrValue);
		}
	}

	void WinSplitter::SetRatio(float ratio)
	{

	}
} // namespace DuiLib
