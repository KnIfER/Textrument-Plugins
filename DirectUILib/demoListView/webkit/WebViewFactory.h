
namespace DuiLib {
	enum WebViewType{
		miniwke
		,miniblink
		,webview2
		,libcef
	};

	class WebViewInterface
	{
	public:
		virtual void LoadURL(const char * url) = 0;
	//	virtual void Refresh() = 0;
	//	virtual void GoBack() = 0;
	//	virtual void GoForward() = 0;
		//virtual void DestroyWebView(bool exit = false) = 0;
	//	virtual void EvaluateJavascript(char * JS) = 0;
		//virtual void ResetZoom() = 0;
		//virtual void ZoomOut() = 0;
		//virtual void ZoomIn() = 0;
	//	virtual void ShowDevTools(TCHAR *res_path) = 0;
	//	virtual void ShowWindow() = 0;
		//virtual void updateArticle(LONG_PTR bid, int articleType, bool softUpdate, bool update) = 0;
		//virtual void notifyWindowSizeChanged(RECT & rc) { };
		int getType() { return _type; };
	private:
		int _type=0;
	};

	class WebViewFactory
	{
	public:
		char wke_mb=0;
		CControlUI* initWebViewImpl(WebViewType type);
	};

}