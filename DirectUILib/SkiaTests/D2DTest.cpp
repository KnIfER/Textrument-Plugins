// https://blog.csdn.net/ubuntu_ai/article/details/50365978


#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

#include <wincodec.h>

extern int DuiLib_AutoRegisterDemo(LPARAM lpFun);


namespace D2DT
{
	HINSTANCE g_hinst;
	HWND g_hwnd;

	ID2D1Factory * g_factory;
	ID2D1HwndRenderTarget * g_render_target;
	ID2D1SolidColorBrush  * g_brush;

	IDWriteFactory * g_write_factory;
	IDWriteTextFormat * g_text_format;

	ID2D1GradientStopCollection * g_gradient_stop_collection;
	ID2D1LinearGradientBrush  * g_linear_gradient_brush;
	ID2D1RadialGradientBrush * g_radial_gradient_brush;

	IWICImagingFactory * g_image_factory = NULL;
	ID2D1Bitmap * g_bitmap = NULL;

	int _srcWidth ;
	int _srcHeight;

	bool AppInit()
	{
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_factory);

		RECT rc;
		GetClientRect(g_hwnd, &rc);

		g_factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(g_hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)	),
			&g_render_target);

		g_render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::WhiteSmoke), &g_brush);

		// Init Font
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(g_write_factory),reinterpret_cast<IUnknown **>(&g_write_factory));
		g_write_factory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 40, L"",&g_text_format);


		// Create Gradient Stops
		D2D1_GRADIENT_STOP gradient_stops[3];
		gradient_stops[0].color = D2D1::ColorF(D2D1::ColorF::LightGreen);
		gradient_stops[0].position = 0.0f;
		gradient_stops[1].color = D2D1::ColorF(D2D1::ColorF::DarkGreen);
		gradient_stops[1].position = 0.5f;
		gradient_stops[2].color = D2D1::ColorF(D2D1::ColorF::Green);
		gradient_stops[2].position = 1.0f;

		// Create Interface
		g_render_target->CreateGradientStopCollection(gradient_stops, 3, &g_gradient_stop_collection);

		// Create Linear Gradient Brush
		g_render_target->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(D2D1::Point2F(0,0), D2D1::Point2F(600,600)), 
			g_gradient_stop_collection, &g_linear_gradient_brush);

		g_render_target->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(D2D1::Point2F(500,500),D2D1::Point2F(),1000.0f,1000.0f),
			g_gradient_stop_collection,&g_radial_gradient_brush);

		// Initialize Image Factory
		CoCreateInstance(CLSID_WICImagingFactory,NULL,CLSCTX_INPROC_SERVER,__uuidof(g_image_factory),(LPVOID*)&g_image_factory);

		IWICBitmapDecoder *bitmapdecoder = NULL;
		g_image_factory->CreateDecoderFromFilename(L"D:\\Large-Sample-Image-download-for-Testing.jpg",NULL,GENERIC_READ,WICDecodeMetadataCacheOnDemand,&bitmapdecoder);//

		IWICBitmapFrameDecode  *pframe = NULL;
		bitmapdecoder->GetFrame(0,&pframe);

		IWICFormatConverter * fmtcovter = NULL;
		g_image_factory->CreateFormatConverter(&fmtcovter);
		fmtcovter->Initialize(pframe,GUID_WICPixelFormat32bppPBGRA,WICBitmapDitherTypeNone,NULL,0.0f,WICBitmapPaletteTypeCustom);
		g_render_target->CreateBitmapFromWicBitmap(fmtcovter, NULL, &g_bitmap);

		g_render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_FORCE_DWORD);

		auto size = g_bitmap->GetSize();  
		_srcWidth = size.width;
		_srcHeight = size.height;


		fmtcovter->Release();
		pframe->Release();
		bitmapdecoder->Release();

		return true;
	}

	void OnSize(LPARAM lparam)
	{
		//RECT rcClient = { 0 };
		//::GetClientRect(g_hwnd, &rcClient);
		if(g_render_target)
			g_render_target->Resize(D2D1::SizeU(LOWORD(lparam),HIWORD(lparam)));
			//g_render_target->Resize(D2D1::SizeU(rcClient.right-rcClient.left, rcClient.bottom-rcClient.top));
	}


	void OnPaint()
	{
		if(!g_render_target)
			return;

		g_render_target->BeginDraw();

		// Clear Background
		g_render_target->Clear(D2D1::ColorF(0.63, 0.84, 0.00)); 

		// Draw Ellipse  
		D2D1_SIZE_F size = g_render_target->GetSize();  
		//D2D1_RECT_F r = {0, 0, size.width, size.height};
		////g_render_target->FillRectangle(&r, g_linear_gradient_brush); 
		////g_render_target->FillRectangle(&ellipse, g_linear_gradient_brush); 
		//
		//
		//// Draw Image
		//int drawWidth = size.width;
		//int drawHeight = size.height;
		//
		//float left=0;
		//float top=0;
		//
		//float calcW=drawWidth;
		//float calcH=drawWidth*_srcHeight*1.0/_srcWidth;
		//
		//if (calcH>drawHeight)
		//{
		//	calcW=drawHeight*_srcWidth*1.0/_srcHeight;
		//	calcH=drawHeight;
		//	left = (drawWidth-calcW)/2;
		//}
		//else 
		//{
		//	top = (drawHeight-calcH)/2;
		//}



		g_render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_FORCE_DWORD);



		//g_render_target->DrawBitmap(g_bitmap, {left, top, left+calcW, top+calcH});

		// Draw Text 
		const wchar_t * text = L"Direct2D Draw Image";
		g_render_target->DrawText(text, wcslen(text),
			g_text_format,
			D2D1::RectF(100, 190, size.width, size.height),
			g_brush);

		g_render_target->EndDraw();
	}

	void OnDestroy()
	{
		g_bitmap->Release();
		g_image_factory->Release();

		g_linear_gradient_brush->Release();
		g_radial_gradient_brush->Release();
		g_gradient_stop_collection->Release();
		g_text_format->Release();
		g_write_factory->Release();
		g_brush->Release();
		g_render_target->Release();
		g_factory->Release();
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
	{
		switch(msg) 
		{  
		case WM_PAINT:
			OnPaint();
			break;

		case WM_SIZE:
			OnSize(lparam);
			break;

		case WM_DESTROY: 
			OnDestroy();
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

	LRESULT RunTest(HINSTANCE hinst, HWND hParent)
	{
		if (hinst==NULL) return (LRESULT)L"六、D2DTest";
		WNDCLASSEX wc;  
		MSG msg;  

		memset(&wc,0,sizeof(wc));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = WndProc; 
		wc.hInstance = hinst;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszClassName = L"WindowClass";
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);  
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); 

		if(!RegisterClassEx(&wc)) 
		{
			MessageBox(NULL, L"Window Registration Failed!", L"Error!",MB_ICONEXCLAMATION|MB_OK);
			return 0;
		}

		g_hinst = hinst;

		g_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"WindowClass", L"Direct2D Demo",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			640, 
			480, 
			NULL, NULL, hinst, NULL);

		if(g_hwnd == NULL) 
		{
			MessageBox(NULL, L"Window Creation Failed!", L"Error!",MB_ICONEXCLAMATION|MB_OK);
			return 0;
		}

		if(!AppInit()) 
		{
			MessageBox(NULL, L"Application Initialisation Failed !", L"Error",MB_ICONEXCLAMATION|MB_OK);
			return 0;
		}

		while(GetMessage(&msg, NULL, 0, 0) > 0) 
		{  
			TranslateMessage(&msg);  
			DispatchMessage(&msg); 
		}

		return msg.wParam;
	}

	static int _auto_reg = DuiLib_AutoRegisterDemo((LPARAM)RunTest);

}