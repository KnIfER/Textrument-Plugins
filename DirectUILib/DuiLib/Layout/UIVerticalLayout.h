#ifndef __UIVERTICALLAYOUT_H__
#define __UIVERTICALLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CVerticalLayoutUI : public CContainerUI
	{
		DECLARE_QKCONTROL(CVerticalLayoutUI)
	public:
		CVerticalLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		//void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		SIZE EstimateSize(const SIZE & szAvailable);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
	};
}
#endif // __UIVERTICALLAYOUT_H__
