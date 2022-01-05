#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CTextUI : public CLabelUI
	{
		DECLARE_QKCONTROL(CTextUI)
	public:
		CTextUI();
		~CTextUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		QkString* GetLinkContent(int iIndex);

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(const SIZE & szAvailable);

		void PaintText(HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int m_nLinks;
		RECT m_rcLinks[MAX_LINK];
		QkString m_sLinks[MAX_LINK];
		int m_nHoverLink;
	};

} // namespace DuiLib

#endif //__UITEXT_H__