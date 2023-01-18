#include "pch.h"
#include "WebViewFactory.h"

namespace DuiLib {

	extern CControlUI* initWebViewMiniwke();

	extern CControlUI* initWebViewMiniBlink();

	extern CControlUI* initWebViewWebview2();

	extern CControlUI* initWebViewBrowserWidget();

	CControlUI* WebViewFactory::initWebViewImpl(WebViewType type)
	{
		if(type==miniwke)
			return initWebViewMiniwke();
		if(type==miniblink)
			return initWebViewMiniBlink();
		if(type==webview2)
			return initWebViewWebview2();
		if(type==libcef)
			return initWebViewBrowserWidget();
	}


}