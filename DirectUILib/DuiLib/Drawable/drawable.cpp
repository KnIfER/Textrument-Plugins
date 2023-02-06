#include "StdAfx.h"
#pragma once

namespace DuiLib {

	bool BasicStatusDrawable::Draw(HDC hDC, CControlUI* pControl, int state) const
	{
		for (size_t i = 0; i < statusIcons.size(); i++)
		{
			if(statusIcons[i].state&state || !state && !statusIcons[i].state) {
				pControl->DrawImage(hDC, statusIcons[i].info, 0);
			}
		}
		return false;
	}

	void BasicStatusDrawable::AddStatusImage(LPCTSTR pStrImage, int state, bool replace, bool reset)
	{
		if(replace) {
			for (size_t i = 0; i < statusIcons.size(); i++)
			{
				if(statusIcons[i].state==state) {
					statusIcons[i].info.Parse(pStrImage, 0);
					return;
				}
			}
		}
		int idx = statusIcons.size()+1;
		if(reset) idx = 1;
		statusIcons.resize(idx);
		statusIcons[idx - 1].state = state;
		statusIcons[idx - 1].info.Parse(pStrImage, 0);
	}
}
