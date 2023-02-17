#ifdef QkUIHorizontalLayout

#ifndef __UIHORIZONTALLAYOUT_H__
#define __UIHORIZONTALLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CHorizontalLayoutUI : public CContainerUI
	{
		DECLARE_QKCONTROL(CHorizontalLayoutUI)
	public:
		CHorizontalLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		//void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		SIZE EstimateSize(const SIZE & szAvailable);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
	};
}
#endif // __UIHORIZONTALLAYOUT_H__


#endif