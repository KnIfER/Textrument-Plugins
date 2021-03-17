#include "APresenterMiniblink.h"
#include "MDTextDlg.h"


void WKE_CALL_TYPE onDidCreateScriptContextCallback(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int extensionGroup, int worldId)
{

}

// 回调：点击了关闭、返回 true 将销毁窗口，返回 false 什么都不做。
bool handleWindowClosing(wkeWebView webWindow, void* param)
{
	return true;
}

// 回调：窗口已销毁
void handleWindowDestroy(wkeWebView webWindow, void* param)
{
}

// 回调：文档加载成功
void handleDocumentReady(wkeWebView webWindow, void* param)
{
	wkeShowWindow(webWindow, true);
}

// 回调：页面标题改变
void handleTitleChanged(wkeWebView webWindow, void* param, const wkeString title)
{
	wkeSetWindowTitleW(webWindow, wkeGetStringW(title));
}

// 回调：创建新的页面，比如说调用了 window.open 或者点击了 <a target="_blank" .../>
wkeWebView onCreateView(wkeWebView webWindow, void* param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features)
{
	//wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, features->x, features->y, features->width, features->height);
	//wkeShowWindow(newWindow, true);
	//return newWindow;
	wkeLoadURL(webWindow, (CHAR*)url);
	return 0;
}

bool onLoadUrlBegin(wkeWebView webView, void* param, const char* url, void *job)
{
	if(strncmp(url, InternalResHead1, 12)==0)
	{
		auto path = url+12;
		const utf8* decodeURL = wkeUtilDecodeURLEscape(path);
		if(decodeURL)
		{
			if(strstr(decodeURL, "..")) // security check
			{
				return false;
			}
			DWORD dataLen;
			auto buffer = _MDText.loadPluginAsset(path, dataLen);
			if(buffer)
			{
				wkeNetSetData(job, buffer, dataLen);
				return true;
			}
		}
	}
	return false;
}

void onLoadUrlEnd(wkeWebView webView, void* param, const char *url, void *job, void* buf, int len)
{
	//wkeRunJS();
	return;
}

//synchronous callback
jsValue WKE_CALL_TYPE GetDocText(jsExecState es, void* param)
{
	//if (0 == jsArgCount(es))
	//	return jsUndefined();
	//jsValue arg0 = jsArg(es, 0);
	//if (!jsIsString(arg0))
	//	return jsUndefined();
	//
	//std::string path;
	//path = jsToTempString(es, arg0);
	//if ("runEchars" == path) {
	//	createECharsTest();
	//} else if ("wkeBrowser" == path) {
	//	wkeBrowserMain(nullptr, nullptr, nullptr, TRUE);
	//}
	//
	//path += "\n";
	//OutputDebugStringA(path.c_str());

	size_t len;
	auto ret=jsString(es, _MDText.GetDocTex(len, 0, 0));
	//::MessageBox(NULL, TEXT("111"), TEXT(""), MB_OK);
	return ret;
	//return jsString(es, "# Hello `md.html` World!");
}

jsValue WKE_CALL_TYPE ScintillaScroll1(jsExecState es, void* param)
{
	TCHAR buffer[256]={0};
	wsprintf(buffer,TEXT("position=%s"), 0);
	::SendMessage(nppData._nppHandle, NPPM_SETSTATUSBAR, STATUSBAR_DOC_TYPE, (LPARAM)buffer);

	if(GetUIBoolReverse(0) && GetUIBoolReverse(2) || jsArgCount(es)>1)
	{
		if(jsArgCount(es)<=2)
		{
			jsValue arg0 = jsArg(es, 0);
			if (!jsIsNumber(arg0))
				return jsUndefined();
			LONG_PTR bid = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
			if(_MDText.lastBid==bid)
			{
				int val=jsToInt(es, arg0);
				_MDText.doScintillaScroll(val);
			}
		}
	}
	return 0;
}



////////////////////////////////////////////////
////// class definition starts here       //////
////////////////////////////////////////////////



APresenterMiniWke::APresenterMiniWke(TCHAR* WKPath, int & error_code, HWND & hBrowser, HWND hwnd) {
	wkeSetWkeDllPath(WKPath);
	error_code = -1;
	if(wkeInitialize()) 
	{
		mWebView = wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, hwnd , 0, 0, 640, 480); 
		if (mWebView)
		{
			hBrowser = wkeGetWindowHandle(mWebView);
			//setMoveWindowArea(0, 0, 640, 30); // 设置窗口可拖动区域，用于无边框窗体
			//wkeSetWindowTitleW(mWebView, NPP_PLUGIN_NAME);
			wkeOnDidCreateScriptContext(mWebView, onDidCreateScriptContextCallback, this);
			wkeOnWindowClosing(mWebView, handleWindowClosing, this);
			wkeOnWindowDestroy(mWebView, handleWindowDestroy, this);
			wkeOnDocumentReady(mWebView, handleDocumentReady, this);
			wkeOnTitleChanged(mWebView, handleTitleChanged, this);
			wkeOnCreateView(mWebView, onCreateView, this);
			wkeOnLoadUrlBegin(mWebView, onLoadUrlBegin, this);
			wkeOnLoadUrlEnd(mWebView, onLoadUrlEnd, this);
			wkeSetDebugConfig(mWebView, "decodeUrlRequest", nullptr);
			wkeJsBindFunction("GetDocText", &GetDocText, nullptr, 1);
			wkeJsBindFunction("Scinllo", &ScintillaScroll1, nullptr, 1);
			error_code = 0;
		} else {
			error_code = 101;
		}
	} else {
		error_code = 100;
	}
}

void APresenterMiniWke::GoBack() {
	wkeGoBack(mWebView);
}

void APresenterMiniWke::GoForward() {
	wkeGoForward(mWebView);
}

void APresenterMiniWke::DestroyWebView(bool exit) {
	wkeDestroyWebView(mWebView);
}

void APresenterMiniWke::EvaluateJavascript(char * JS) {
	wkeRunJS(mWebView, JS);
}

void APresenterMiniWke::ResetZoom() {
	wkeSetZoomFactor(mWebView, 1);
}

void APresenterMiniWke::ZoomOut() {
	float zoom=wkeZoomFactor(mWebView)-mbzd;
	if(zoom<0.25) zoom=0.25;
	wkeSetZoomFactor(mWebView, zoom);
}

void APresenterMiniWke::ZoomIn() {
	float zoom=wkeZoomFactor(mWebView)+mbzd;
	if(zoom>5) zoom=5;
	wkeSetZoomFactor(mWebView, zoom);
}

void APresenterMiniWke::ShowDevTools(TCHAR *res_path) {
	wkeShowDevtools(mWebView, res_path, 0, 0);
}

void APresenterMiniWke::ShowWindow() {
	wkeShowWindow(mWebView, TRUE);
}

void APresenterMiniWke::updateArticle(LONG_PTR bid, int articleType, bool softUpdate, bool update) {

	if(softUpdate)
	{
		wkeRunJS(mWebView, "update()"); // wke soft update
	}
	else if(update)
	{
		if(_MDText.checkRenderMarkdown()) {
			CHAR* page_content = new CHAR[512];
			strcpy(page_content, "<!doctype html><meta charset=\"utf-8\"><script src=\"http://mdbr/ui.js\"></script><script>window.update=function(){APMD(GetDocText(''))}</script><body><script src=\"http://mdbr/");	
			_MDText.AppendPageResidue(page_content+172); // 加载wke
			wkeLoadHTML(mWebView, page_content);
			_MDText.lastBid=bid;
			lstrcpy(last_updated, last_actived);
			//wkeLoadHTML(mWebView, "<!doctype html><meta charset=\"utf-8\"> <script src=\"http://mdbr/main.js\"></script><body><script>window.update=function(){APMD(GetDocText(''))};update();</script></body>");
		}
	}
}
