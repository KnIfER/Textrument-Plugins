

#pragma once

namespace DuiLib {
	class IDrawable
	{
	public:
		virtual bool Draw(HDC hDC, CControlUI* pControl, int state) const = 0;
	};
}