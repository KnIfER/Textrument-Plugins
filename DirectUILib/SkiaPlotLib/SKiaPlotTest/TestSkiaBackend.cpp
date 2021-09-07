
#include "include/utils/SkRandom.h"
#include "include/utils/SkRandom.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkImage.h"
#include "include/core/SKImageInfo.h"
#include "include/core/SkImageGenerator.h"

#include "include/codec/SkCodec.h"


#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#include "include/core/SkFont.h"
#include "include/core/SkCanvas.h"

#include "include/gpu/GrDirectContext.h"
#include "include/gpu/GrTypes.h"
#include "include/gpu/gl/GrGLTypes.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/gl/GrGLDefines.h"
#include "src/gpu/gl/GrGLUtil.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkStream.h"
#include "include/core/SkSurface.h"

#include <matplot/matplot.h>
#include <matplot/core/figure_registry.h>

#define _WIN32_WINNT 0x0500
#include <windows.h>	
#include <tchar.h>		
#include <commctrl.h>	
#include <gl/gl.h>
#include <gl\glu.h>		

#pragma comment(lib,"ComCtl32.lib")
#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")

using namespace matplot;

using namespace matplot;

namespace SkiaPlotDemo{
#define IDC_BMPLOAD 101			
#define IDC_EXIT 105			
#define IDC_TIMERSTART 201		
#define IDC_TIMERSTOP 202	

	sk_sp<GrDirectContext> context = nullptr;
	sk_sp<const GrGLInterface> glface;
	sk_sp<SkSurface> gpuSurface;
	SkCanvas* _canvas;

	class Plotter : public backend::backend_interface{
	public:
		Plotter() { 
		}

		~Plotter() {
		}

		bool is_interactive() override { return true; } 

		const std::string &output() override {
			throw std::logic_error("output not implemented yet");
		}

		const std::string &output_format() override {
			throw std::logic_error("output_format not implemented yet");
		}

		bool output(const std::string &filename) override {
			throw std::logic_error("output not implemented yet");
		}

		bool output(const std::string &filename,
			const std::string &file_format) override {
			throw std::logic_error("output not implemented yet");
		}

		unsigned int width() override {
			GLint m_viewport[4];
			glGetIntegerv( GL_VIEWPORT, m_viewport );
			return m_viewport[2];
		}

		unsigned int height() override {
			GLint m_viewport[4];
			glGetIntegerv( GL_VIEWPORT, m_viewport );
			return m_viewport[3];
		}

		void width(unsigned int new_width) override {
			throw std::logic_error("width not implemented yet");
		}

		void height(unsigned int new_height) override {
			throw std::logic_error("height not implemented yet");
		}

		unsigned int position_x() override {
			throw std::logic_error("position_x not implemented yet");
		}

		unsigned int position_y() override {
			throw std::logic_error("position_y not implemented yet");
		}

		void position_x(unsigned int new_position_x) override {
			throw std::logic_error("position_x not implemented yet");
		}

		void position_y(unsigned int new_position_y) override {
			throw std::logic_error("position_y not implemented yet");
		}

		bool new_frame() override { return true; }

		bool render_data() override {
			return true;
		}

		void draw_rectangle(const double x1, const double x2,
			const double y1, const double y2,
			const std::array<float, 4> &color) override {

			SkPaint* fill = new SkPaint();
			fill->setColor(SkColorSetARGB((1.f - color[0])*255, (color[1])*255, (color[2])*255, (color[3])*255));
			SkRect rect{x1, y1, x2, y2};
			_canvas->drawRect(rect, *fill);
			delete fill;
		}

		void draw_background(const std::array<float, 4> &color) override {
			_canvas->clear(SkColorSetARGB((1.f - color[0])*255, (color[1])*255, (color[2])*255, (color[3])*255));
		}

		void show(class matplot::figure_type *f) override {
			backend_interface::show(f);
		}

		bool supports_fonts() override { return false; }

		void draw_path(const std::vector<double> &x,
			const std::vector<double> &y,
			const std::array<float, 4> &color) override {

			SkPaint* stroke = new SkPaint();
			stroke->setColor(SkColorSetARGB((1.f - color[0])*255, (color[1])*255, (color[2])*255, (color[3])*255));
			stroke->setAntiAlias(true);
			stroke->setStroke(true);
			stroke->setStrokeWidth(2.0f);

			SkPathBuilder* path_builder = new SkPathBuilder();
			int length = std::min(x.size(), y.size()); // , color.size()
			for (size_t i = 0; i < length; i++)
			{
				if (i==0)
				{
					path_builder->moveTo (x[i], y[i]);
				}
				else
				{
					path_builder->lineTo (x[i], y[i]);
				}
			}
			SkPath path = path_builder->detach();
			_canvas->drawPath(path, *stroke);

			delete path_builder;
			delete stroke;
		}

		void draw_markers(const std::vector<double> &x,
			const std::vector<double> &y,
			const std::vector<double> &z) override {
			throw std::logic_error("draw_markers not implemented yet");
		}

		void draw_text(const std::vector<double> &x,
			const std::vector<double> &y,
			const std::vector<double> &z) override {

			SkFont font;
			font.setSize(16);
			SkPaint textpaint;
			textpaint.reset();
			font.setSize(16);

			SkString string("draw what?");
			_canvas->drawString(string, 1, 100, font, textpaint);

		}

		void draw_image(const std::vector<std::vector<double>> &x,
			const std::vector<std::vector<double>> &y,
			const std::vector<std::vector<double>> &z) override {
			throw std::logic_error("draw_image not implemented yet");
		}

		void draw_triangle(const std::vector<double> &x,
			const std::vector<double> &y,
			const std::vector<double> &z) override {
			throw std::logic_error("draw_triangle not implemented yet");
		}

		void process_input(HWND hwnd) {
			//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			//    glfwSetWindowShouldClose(window, true);
			//}
		}

		void framebuffer_size_callback(HWND hwnd, int width, int height) {
			// Make sure the viewport matches the new window dimensions;
			// Note that width and height will be significantly larger than
			// specified on retina displays.
			glViewport(0, 0, width, height);
			auto f = gcf();
			auto b = f->backend();
			auto ogl_b = std::dynamic_pointer_cast<Plotter>(b);
			if (ogl_b != nullptr) {
				//f->draw();
			}
		}
	};

	Plotter* _plotter;	
	figure_handle _figure;
	axes_handle _ax;

	const TCHAR* AppClassName = _T("OPENGL_DEMO_APP");
	static const TCHAR* DATABASE_PROPERTY = _T("OurDataStructure");

	typedef struct OpenGLData {
		HGLRC Rc;										
		GLuint glTexture;								
		GLfloat	xrot;									
		GLfloat	yrot;									
	} GLDATABASE;

	static HGLRC InitGL (HWND Wnd) {
		HGLRC hglrc = 0;					
											
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 
			PFD_TYPE_RGBA,												
			32,															
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,															
			8,															
			0,															
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		HDC hdc = GetDC(Wnd);
		int pixelFormat = ChoosePixelFormat(hdc, &pfd); 
		if (SetPixelFormat(hdc, pixelFormat, &pfd)) {
			hglrc = wglCreateContext(hdc);
			if (hglrc != 0) {
				wglMakeCurrent(hdc, hglrc);		
				glEnable(GL_TEXTURE_2D);		
				glShadeModel(GL_SMOOTH);							
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				
				glClearDepth(1.0f);									
				glEnable(GL_DEPTH_TEST);							
				glDepthFunc(GL_LEQUAL);								
				glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
			}
			glface = GrGLMakeNativeInterface();
			}
		ReleaseDC(Wnd, hdc);


		_figure = figure<Plotter>(true);
		_plotter = (Plotter*)_figure->backend().get();

		_ax = _figure->current_axes();
		_ax->xlim({0.,2. * pi});
		_ax->ylim({-1.5,1.5});
		_ax->yticks(iota(-1.5,0.5,+1.5));
		_ax->xticks(iota(0.,1.,2. * pi));

		//TCHAR buffer[100]={0};
		//wsprintf(buffer,TEXT("position=%ld =%ld"), _figure, _plotter);
		//::MessageBox(NULL, buffer, TEXT(""), MB_OK);

		return (hglrc);			
	}

	static void ReSizeGLScene (HWND Wnd) {
		GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY); 
		if (db == 0) return;											
		HDC Dc = GetWindowDC(Wnd);										
		RECT r;
		GetWindowRect(Wnd, &r);											
		int Width = r.right - r.left;									
		int Height = r.bottom - r.top;									
		if (Height == 0) Height = 1;									
		wglMakeCurrent(Dc, db->Rc);		

		//if(_plotter) {
		//	_plotter->framebuffer_size_callback(0, Width, Height);
		//} else {			
		//	glViewport(0, 0, Width, Height);
		//}
		glViewport(0, 0, Width, Height);

		glMatrixMode(GL_PROJECTION);									
		glLoadIdentity();												
																		
		gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);									
		glLoadIdentity();											
		ReleaseDC(Wnd, Dc);											
	}

	int buffer_width  = 0;
	int buffer_height = 0;

	sk_sp<SkSurface> getBackbufferSurface(int width, int height) {
		if (nullptr == gpuSurface || buffer_width!=width || buffer_height!=height) {
			if (context) {
				GrGLint buffer;
				GR_GL_CALL(glface.get(), GetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer));

				GrGLFramebufferInfo fbInfo;
				fbInfo.fFBOID = buffer;
				fbInfo.fFormat = GR_GL_RGBA8;

				GrBackendRenderTarget backendRT(width,
					height,
					1,
					8,
					fbInfo);

				gpuSurface = SkSurface::MakeFromBackendRenderTarget(context.get(), backendRT,
					kBottomLeft_GrSurfaceOrigin,
					kRGBA_8888_SkColorType,
					0,
					0);

				buffer_width=width;
				buffer_height=height;
			}
		}

		return gpuSurface;
	}

	void DrawGLScene(HDC Dc, int width, int height) {
		if (!context)
		{
			GrContextOptions defaultOptions;
			context = GrDirectContext::MakeGL(glface, defaultOptions);
		}
		sk_sp<SkSurface> glSurf = getBackbufferSurface(width, height);
		_canvas = glSurf->getCanvas();

		// Create plots
		float timeValue = GetTickCount();
		timeValue = timeValue/1000;
		//timeValue = 0;
		std::vector<double> x = linspace(0., 2. * pi);
		std::vector<double> y = transform(x, [&](auto x) { return sin(x + timeValue); });
		_ax->hold(off);
		_ax->plot(x, y, "-o");
		_ax->hold(on);
		_ax->plot(x, transform(y, [](auto y) { return -y; }), "--xr");
		//_ax->plot(x, transform(x, [](auto x) { return x / pi - 1.; }), "-:gs");
		// _ax->plot({1.0, 0.7, 0.4, 0.0, -0.4, -0.7, -1}, "k");

		_ax->ylabel("Y Axis");
		ylabel("Y Axis");

		// Draw the figure
		_figure->draw();

		if (glSurf) 
		{
			//SkiaDraw(, width, height);
			glSurf->flushAndSubmit();
		}
	}

	LRESULT CALLBACK GLPlotProc (HWND Wnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch (Msg){
			case WM_PAINT: {										
				PAINTSTRUCT ps;
				RECT rc;
				::GetClientRect(Wnd, &rc);
				
				HDC hdc = BeginPaint(Wnd, &ps);
				
					DrawGLScene(hdc, rc.right, rc.bottom);
				
					HDC dc = GetDC(Wnd);
					SwapBuffers(dc);
					ReleaseDC(Wnd, dc);
				
				EndPaint(Wnd, &ps);

				return FALSE;
			}  break;
			case WM_WINDOWPOSCHANGED:											
				if ((lParam == 0) || ((((PWINDOWPOS) lParam)->flags & SWP_NOSIZE) == 0)){
					ReSizeGLScene(Wnd);									
					InvalidateRect(Wnd, 0, TRUE);						
				}
			break;
			case WM_CREATE:	{							
				HMENU SubMenu, Menu;
				Menu = CreateMenu();							
				SubMenu = CreatePopupMenu();
				AppendMenu(SubMenu, MF_STRING, IDC_TIMERSTART, _T("&Start Timer"));
				AppendMenu(SubMenu, MF_STRING, IDC_TIMERSTOP, _T("Stop &Timer"));
				AppendMenu(Menu, MF_POPUP, (UINT_PTR) SubMenu, _T("&Timer"));
				SetMenu(Wnd, Menu);									
								
				GLDATABASE* db = (GLDATABASE*) malloc(sizeof(GLDATABASE)); 
				db->Rc = InitGL(Wnd);								
				db->glTexture = 0;									
				db->xrot = 0.0f;									
				db->yrot = 0.0f;									
				SetProp(Wnd, DATABASE_PROPERTY, (HANDLE) db);		
				ReSizeGLScene(Wnd);									
			}  break;
			case WM_DESTROY: {										
				context = nullptr;
				glface = nullptr;
				gpuSurface = nullptr;
				wglMakeCurrent(NULL, NULL);						
				GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY);
				if (db != 0) {
					if (db->Rc != 0) wglDeleteContext(db->Rc);	
					if (db->glTexture != 0)
						glDeleteTextures(1, &db->glTexture);	
					free(db);									
				}
				PostQuitMessage(0);								
			}
						   break;
			case WM_COMMAND:
				switch (LOWORD(wParam)){  
				case IDC_EXIT:								
					PostMessage(Wnd, WM_CLOSE, 0, 0);		
					break;
				case IDC_TIMERSTART: {						
					SetTimer(Wnd,							
						1,									
						25,								
						0);									
				}
								   break;
				case IDC_TIMERSTOP: {						
					KillTimer(Wnd, 1);						
				}
								  break;
				};
				break;
			case WM_TIMER: {											
				GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY);
				db->xrot += 1.0f;								
				db->yrot += 1.0f;								
				InvalidateRect(Wnd, 0, TRUE);							
			}  break;
			case WM_ERASEBKGND:										
				return (FALSE);
		};
		return DefWindowProc(Wnd, Msg, wParam, lParam);
	};
}
using namespace SkiaPlotDemo;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	RECT rect;
	HWND Wnd;
	WNDCLASSEX WndClass;

	memset(&WndClass, 0, sizeof(WNDCLASSEX));						
	WndClass.cbSize = sizeof(WNDCLASSEX);							
	WndClass.style = CS_HREDRAW | CS_VREDRAW;										
	WndClass.lpfnWndProc = GLPlotProc;						
	WndClass.cbClsExtra = 200;										
	WndClass.cbWndExtra = 200;										
	WndClass.hInstance = hInstance;						
	WndClass.hIcon = LoadIcon(0, IDI_APPLICATION);					
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);					
	WndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);	
	WndClass.lpszMenuName = NULL;									
	WndClass.lpszClassName = AppClassName;							
	RegisterClassEx(&WndClass);			

	GetClientRect(GetDesktopWindow(), &rect);

	Wnd = CreateWindowEx(0
		, AppClassName
		, _T("Test Skia Plotter"), 
		WS_VISIBLE | WS_OVERLAPPEDWINDOW
		, rect.left+50, rect.top+50 
		,rect.right-rect.left-100, rect.bottom-rect.top-100
		, 0
		, 0
		, hInstance
		, NULL);					

	while(GetMessage(&msg, 0, 0, 0)){	
		TranslateMessage(&msg);			
		DispatchMessage(&msg);			
	};
	return (0);
}
