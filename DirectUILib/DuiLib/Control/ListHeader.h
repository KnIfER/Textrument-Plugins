#ifdef QkListHeader

#pragma once
namespace DuiLib {

	class UILIB_API CListHeaderUI : public CHorizontalLayoutUI
	{
		DECLARE_QKCONTROL(CListHeaderUI)
	public:
		CListHeaderUI();
		virtual ~CListHeaderUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		SIZE EstimateSize(const SIZE & szAvailable);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetScaleHeader(bool bIsScale);
		bool IsScaleHeader() const;

		void DoInit();
		void DoPostPaint(HDC hDC, const RECT& rcPaint);
	private:
		bool m_bIsScaleHeader;
	};
}



#endif