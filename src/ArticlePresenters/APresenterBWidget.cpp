#include "APresenterBWidget.h"
#include "PluginDefinition.h"
#include "APresentee.h"
#include "SU.h"

BJSCV* GetDocText1(LONG_PTR funcName, int argc, LONG_PTR argv, int sizeofBJSCV)
{
	//::MessageBoxA(NULL, ("GetDocText1"), (""), MB_OK);
	if(argc<0)
	{
		auto str = (BJSCV*)funcName;
		if(str->delete_internal&&str->charVal)
		{
			delete[] str->charVal;
			//HeapFree(GetProcessHeap(), 0, str->charVal);
			str->charVal = nullptr;
		}
		if(argv==sizeofBJSCV==0)
			delete (BJSCV*)funcName;
		return 0;
	}
	LONG_PTR bid=0;
	int structSize=0;
	if(argc==1)
	{
		char* args = bwParseCefV8Args(argv, structSize);
		if(structSize)
		{
			BJSCV* val = (BJSCV*)(args+0*structSize);
			if(val->value_type==typeString)
			{
				STR2LONGPTR((TCHAR*)val->charVal, bid);
			}
			// testJs('Hah')
		}
	}
	size_t len;
	bool del;
	auto ret = new BJSCV{typeString, 0, presentee->GetDocTex(len, bid, &del)};
	ret->delete_internal = del;
	return ret;
}


LRESULT WINAPI testWindowProc1(
	__in HWND hWnd,
	__in UINT msg,
	__in WPARAM wParam,
	__in LPARAM lParam)
{
	LRESULT result = 0;

	switch (msg) {
	case WM_NCDESTROY:
		return 0;

	case WM_ERASEBKGND:
		return TRUE;

	case WM_SIZE:
	{
		return 0;
	}
	case WM_KEYDOWN:
	{
		break;
	}
	case WM_KEYUP:
	{
		//::MessageBox(NULL, TEXT("111"), TEXT(""), MB_OK);
		break;
	}
	case WM_CHAR:
	{
		break;
	}
	case WM_MOUSEWHEEL:
	{
		break;
	}
	case WM_SETFOCUS:
		return 0;

	case WM_KILLFOCUS:
		return 0;

	case WM_SETCURSOR:
		break;

	case WM_IME_STARTCOMPOSITION: {
		break;
	}
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

BJSCV* ScintillaScroll(LONG_PTR funcName, int argc, LONG_PTR argv, int sizeofBJSCV)
{
	if(GetUIBoolReverse(0) && GetUIBoolReverse(2) || argc>1)
	{
		int structSize=0;
		if(argc<=2)
		{
			LONG_PTR bid = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
			if(presentee->lastBid==bid)
			{
				char* args = bwParseCefV8Args(argv, structSize);
				if(structSize)
				{
					BJSCV* val = (BJSCV*)(args+0*structSize);
					if(val->value_type==typeInt)
					{
						presentee->doScintillaScroll(val->intVal);
					}
				}
			}
		}
	}
	return 0;
}


url_intercept_result* InterceptBrowserWidget(const char* url, const url_intercept_result* ret)
{
	if(!url)
	{
		if(ret)
			delete ret;
		return 0;
	}
	if(strncmp(url, "https://tests/home", 18)==0)
	{
		presentee->RefreshWebview(false);
		return new url_intercept_result{(CHAR*)"Markdown Text", 14, 200, (CHAR*)"OK"};
	}

	if(strncmp(url, InternalResHead1, 12)==0)
	{
		auto path = url+12;
		if(path)
		{
			if(strstr(path, "..")) // security check
			{
				return false;
			}
			char decodedUrl[MAX_PATH];
			UrlDecode(decodedUrl, path);

			DWORD dataLen;
			auto bufferRes = presentee->loadPluginAsset(decodedUrl, dataLen);

			if(bufferRes)
			{
				return new url_intercept_result{bufferRes, dataLen, 200, (CHAR*)"OK", "", true};
			}
		}
	}


	if(strncmp(url, "http://tests/MDT/", 17)==0)
	{
		LONG_PTR bid=0;
		int from = STR2LONGPTRA((CHAR*)url+17, bid);
		if(bid&&from) {
			auto path = url+18+from;
			char decodedUrl[MAX_PATH];
			UrlDecode(decodedUrl, path);
			DWORD dataLen;
			auto data = presentee->loadSourceAsset(bid, decodedUrl, dataLen);
			if(data) {
				url_intercept_result* result = new url_intercept_result{data, dataLen, 200, (CHAR*)"OK"};
				result->delete_internal = 1;
				return result;
			}
		}
	}

	return nullptr;
}

void onBrowserPrepared(bwWebView browserPtr)
{
	bwInstallJsNativeToWidget(browserPtr, "GetDocText1", GetDocText1);
	bwInstallJsNativeToWidget(browserPtr, "Scinllo", ScintillaScroll);
	if (APresenterBWidget* wv = dynamic_cast<APresenterBWidget*>(presentee->mWebView0)) {
		wv->mWebView = browserPtr;
		presentee->hBrowser = bwGetHWNDForBrowser(browserPtr);
		::SendMessage(presentee->getHWND(), WM_SIZE, 0, 0);
		DefferedLoadingData* parms = defferedLoad;
		if (parms)
		{
			wv->updateArticle(parms->bid, parms->articleType, false, true);
			defferedLoad = NULL;
			delete parms;
		}
	}
	//_MDText.RefreshWebview(); 没有用
	//SetWindowLongPtr(_MDText.hBrowser, GWLP_WNDPROC, (LONG_PTR)testWindowProc1);
}

bool onBrowserFocused(bwWebView browserPtr)
{
	HWND hwnd = bwGetHWNDForBrowser(browserPtr);
	if(hwnd==presentee->hBrowser) {
		::SendMessage(nppData._nppHandle, NPPM_SETDOCKFOCUS, (WPARAM)hwnd, 0);
	}
	return false;
}

bool onBrowserClose(bwWebView browserPtr)
{
	HWND hwnd = bwGetHWNDForBrowser(browserPtr);
	if(hwnd==presentee->hBrowser) {
		::SendMessage(nppData._nppHandle, NPPM_CLOSEDOCK, (WPARAM)hwnd, 0);
	}
	return true;
}



////////////////////////////////////////////////
////// class definition starts here       //////
////////////////////////////////////////////////



APresenterBWidget::APresenterBWidget(TCHAR* WKPath, int & error_code, HWND & hBrowser, HWND hwnd) 
{
	error_code=1;
	if(PathFileExists(WKPath))
	{
		if(bwInit(WKPath) && bwCreateBrowser({hwnd, "https://tests/home", onBrowserPrepared, InterceptBrowserWidget, onBrowserFocused, onBrowserClose}))
		{
			error_code=0;
			presentee->browser_deferred_create_time = clock();
			//::MessageBox(NULL, TEXT("111"), TEXT(""), MB_OK);
		}
	}
	else if(presentee->RequestedSwitch)
	{
		::MessageBox(nppData._nppHandle, presentee->GetLocalWText("no_rt", TEXT("Runtime Not Found !")).c_str(), TEXT("cefclient.dll"), MB_OK);
		presentee->RequestedSwitch = false;
	}
}

void APresenterBWidget::GoBack() 
{
	//if(bwCanGoBack(mWebView))
	bwGoBack(mWebView);
}

void APresenterBWidget::GoForward() 
{
	//if(bwCanGoForward(mWebView))
	bwGoForward(mWebView);
}

void APresenterBWidget::DestroyWebView(bool exit) 
{
	bwDestroyWebview(mWebView, exit);
}

void APresenterBWidget::EvaluateJavascript(char * JS) 
{
	bwExecuteJavaScript(mWebView, JS);
}

void APresenterBWidget::ResetZoom() 
{
	bwSetZoomLevel(mWebView, 0);
}

void APresenterBWidget::ZoomOut() 
{
	bwZoomLevelDelta(mWebView, -1);
}

void APresenterBWidget::ZoomIn() 
{
	bwZoomLevelDelta(mWebView, 1);
}

void APresenterBWidget::ShowDevTools(TCHAR *res_path) {
	bwShowDevTools(mWebView);
}

void APresenterBWidget::ShowWindow() 
{
}

void APresenterBWidget::updateArticle(LONG_PTR bid, int articleType, bool softUpdate, bool update) 
{
	if (!mWebView)
	{
		defferedLoad = new DefferedLoadingData{bid, articleType};
		return;
	}
	CHAR* page_id = new CHAR[64];  // LIBCEF 需要拟构网址。 传文件名，只传ID吧。 http://tests/MDT/{bid}/text.html
	int st,ed;
	strcpy(page_id, "http://tests/MDT/");
	LONGPTR2STR(page_id+(st=strlen(page_id)), bid);
	if(articleType==1) {
		if(softUpdate||update) {
			strcpy(page_id+(ed=strlen(page_id)), "/doc.html");
			size_t len;
			bwLoadStrData(mWebView, page_id+13, presentee->GetDocTex(len, bid, 0), 0);
			if(update) {
				presentee->lastBid=bid;
				lstrcpy(last_updated, last_actived);
			}
		}
		return;
	}
	strcpy(page_id+(ed=strlen(page_id)), "/text.html");
	if(softUpdate && STRSTARTWITH(bwGetUrl(mWebView), page_id))
	{
		bwExecuteJavaScript(mWebView, "update()"); // bw soft update
	}
	else if(update)
	{
		CHAR* page_content = new CHAR[512];
		strcpy(page_content, "<!doctype html><meta charset=\"utf-8\"><script src=\"http://mdbr/ui.js\"></script><script>window.update=function(){window.APMD(GetDocText1('");

		auto nxt_st=page_content+136;
		strncpy(nxt_st, page_id+st, ed-st);
		nxt_st+=ed-st;

		strcpy(nxt_st, "'));}</script><body><script src=\"http://mdbr/");

		presentee->AppendPageResidue(nxt_st+45); // 加载bw
		bwLoadStrData(mWebView, page_id+13, page_content, 0);
		presentee->lastBid=bid;
		lstrcpy(last_updated, last_actived);
	}
	delete[] page_id;
	//bwLoadStrData(mWebView, url_, "<!doctype html><meta charset=\"utf-8\"><script src=\"http://mdbr/main.js\"></script><body><script>window.APMD(GetDocText1(0));</script></body>", 0);
}

