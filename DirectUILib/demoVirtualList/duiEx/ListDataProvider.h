#pragma once
#include "IVirtualDataProvider.h"

using namespace DuiLib;


class MVirtualDataProvider : public IVirtualDataProvider, public IDialogBuilderCallback
{
public:
	MVirtualDataProvider(CPaintManagerUI* pPaint);



	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);


	/**
	* @brief 创建一个子项
	* @return 返回创建后的子项指针
	*/
	virtual CControlUI* CreateElement();
	

	/**
	* @brief 填充指定子项
	* @param[in] control 子项控件指针
	* @param[in] index 索引
	* @return 返回创建后的子项指针
	*/
	virtual void FillElement(CControlUI *pControl, int index);
	

	/**
	* @brief 获取子项总数
	* @return 返回子项总数
	*/
	virtual int GetElementtCount();
	
protected:
	CPaintManagerUI* _manager;
	CDialogBuilder m_dlgBuilder;
};