#pragma once

namespace DuiLib {
	struct StateAndIcon {
		int state;
		TDrawInfo info;
	};
	class BasicStatusDrawable : public IDrawable {
		bool Draw(HDC hDC, CControlUI* pControl, int state) const;
		void AddStatusImage(LPCTSTR pStrImage, int state, bool replace, bool reset);
	private:
		std::vector<StateAndIcon> statusIcons;
	};
}
