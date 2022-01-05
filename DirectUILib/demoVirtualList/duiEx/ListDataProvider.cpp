#include "stdafx.h"
#include "ListDataProvider.h"


MVirtualDataProvider::MVirtualDataProvider(CPaintManagerUI* pPaint)
{
	_manager = pPaint;
}

DuiLib::CControlUI* MVirtualDataProvider::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

DuiLib::CControlUI* MVirtualDataProvider::CreateElement()
{
	CListTextElementUI* pListItem = new CListTextElementUI();
// 	if (!m_dlgBuilder.GetMarkup()->IsValid())
// 	{
// 		pCon = static_cast<CControlUI*>(m_dlgBuilder.Create(L"listitem.xml", (UINT)0, this, _manager));
// 	}
// 	else
// 	{
// 		pCon = static_cast<CControlUI*>(m_dlgBuilder.Create(this, _manager));
// 	}
	return pListItem;
}

void MVirtualDataProvider::FillElement(CControlUI *pControl, int index)
{
	CDuiString strBuffer;
	strBuffer.Format(L"%d", index);
	CDuiString strToolTip;
	strToolTip.Format(L"Index : %d", index);

	CListTextElementUI* pListItem = static_cast<CListTextElementUI*>(pControl);
	if (pListItem)
	{
		pListItem->SetText(0, strBuffer);
		pListItem->SetText(1, strToolTip);
	}
}

int MVirtualDataProvider::GetElementtCount()
{
	return 500000;
}