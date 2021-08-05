#pragma once



//////////////////////////////////////////////////////////////////////////
// 虚拟列表数据提供者,抽象基类,由实际使用场景派生子类,实现对应的接口功能
//////////////////////////////////////////////////////////////////////////

/**
* @brief 虚拟列表接口类
* 提供开发者对子项数据管理个性化定制
*/
class IVirtualDataProvider
{
public:
	/**
	* @brief 创建一个子项
	* @return 返回创建后的子项指针
	*/
	virtual CControlUI* CreateElement() abstract;

	/**
	* @brief 填充指定子项
	* @param[in] control 子项控件指针
	* @param[in] index 索引
	* @return 返回创建后的子项指针
	*/
	virtual void FillElement(CControlUI *pControl, int index) abstract;

	/**
	* @brief 获取子项总数
	* @return 返回子项总数
	*/
	virtual int GetElementtCount() abstract;
};