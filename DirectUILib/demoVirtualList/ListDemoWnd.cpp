#include "stdafx.h"
#include "ListDemoWnd.h"
#include "duiEx/VirtualListUI.h"
#include "duiEx/ListDataProvider.h"


CListDemoWnd::CListDemoWnd()
{

}

CListDemoWnd::~CListDemoWnd()
{

}

// 继承 notify 后不再经过此。
void CListDemoWnd::OnClick(TNotifyUI& msg)
{
}

void CListDemoWnd::ShowRes()
{
	CVirtualListUI* pList = static_cast<CVirtualListUI*>(m_pm.FindControl(L"ListDemo"));
	if (NULL != pList)
	{
		pList->RemoveAll();

		MVirtualDataProvider* pFileListProvider = new MVirtualDataProvider(&m_pm);
		

		//初始化之前需要清空列表和数据

		pList->SetDataProvider(pFileListProvider);
		pList->SetElementHeight(22);
		
		pList->InitElement();
		
	}
}

DuiLib::CControlUI* CListDemoWnd::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, L"VirList") == 0)
	{
		return new CVirtualListUI();
	}
	return NULL;
}

LPCTSTR CListDemoWnd::GetWindowClassName(void) const
{
	return L"CListDemoWnd";
}


CDuiString CListDemoWnd::GetSkinType()
{
	return TEXT("");
}

CDuiString CListDemoWnd::GetSkinFile()
{
	return TEXT("VirtualListWnd.xml");
}

CDuiString CListDemoWnd::GetSkinFolder()
{
	return _T("");
}

UILIB_RESTYPE CListDemoWnd::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

void OnPrepare(TNotifyUI& msg){}

void CListDemoWnd::Notify(TNotifyUI& msg)
{
    if( msg.sType == _T("windowinit") )  OnPrepare(msg);

    else if( msg.sType == _T("click") ) 
    {
		if (msg.pSender->GetName() == L"btnSearch")
		{
			ShowRes();

			// 		CVirTileLayoutUI*  pTileLayout = static_cast<CVirTileLayoutUI*>(m_pm.FindControl(L"VirTileDemo"));
			// 		MVirtualDataProvider* pDataProvider = new MVirtualDataProvider(&m_pm);
			// 		pTileLayout->SetDataProvider(pDataProvider);
			// 		pTileLayout->SetElementHeight(50);
			// 		pTileLayout->InitElement(100);
		}
		return __super::OnClick(msg);
    }
}

//LRESULT CListDemoWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	//LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
//	//styleValue &= ~WS_CAPTION;
//	//::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
//	//
//	//
//	//m_pm.Init(m_hWnd);
//	////m_pm.SetTransparent(100);
//	//CDialogBuilder builder;
//	//CControlUI* pRoot = builder.Create(_T("skin.xml"), (UINT)0, NULL, &m_pm);
//	//ASSERT(pRoot && "Failed to parse XML");
//	//m_pm.AttachDialog(pRoot);
//	//m_pm.AddNotifier(this);
//	////Init();
//	//
//	////m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
//	////m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
//	////m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
//	////m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
//	////m_pSearch = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn")));
//	//return 0;
//
//	return __super::OnCreate(uMsg, wParam, lParam, bHandled);
//}