#pragma once
namespace DuiLib
{
	using DemoEntrySig = LRESULT(*)(HINSTANCE hInstance, HWND hParent);

	__declspec(selectany) std::vector<DemoEntrySig> NamedDemos;

	// |DemoEntrySig| 通用具名demo入口格式。当第一参数hInstance为空时需返回Demo名称。
	inline int AutoRegister(DemoEntrySig testEntry)
	{
		NamedDemos.push_back(testEntry);
		return NamedDemos.size();
	}

	CWindowWnd* CreateDemoBox();
}
