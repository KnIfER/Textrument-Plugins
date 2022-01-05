/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
* Skia IMGUI Hello World
*  // derived from opengl demo. see $(SolutionDir/SkiaTests/GLAppDemo.cpp)
*  // skia+opengl demo see $(SolutionDir/SkiaTests/GLSkiaHelloWorld.cpp)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
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

#include "imgui_draw_plus.h"
#include "imgui_internal.h";

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>	
#include <commctrl.h>

//#include <gl\gl.h>
#include <glad/glad.h>
#include <gl\glu.h>	


// This is the lazy adding libraries via #pragma rather than in linker includes
// If you are not on visual studio you will need to add the librarys via your linker
#pragma comment(lib,"ComCtl32.lib")
#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")


namespace SkimGuiHello{
	SkCanvas* _canvas;
	SkPaint _textpaint;
	SkFont _font;

	// invalidate : 0=on demand; 1=on timer.
	int invalidate_model = 1;
	// invalidation interval in millisec.
	int timer_swap_interal = 15;
	// current invalidation model.
	int invalidate_mode = invalidate_model;

	// true=limit 60FPS; false=unlimited framerate.
	bool vsync_on = false;
	// use another thread to loop-invalidate the window.
	bool benchmark = false;
	// is running or not.
	bool bAPPRunning = true;

	int frame_render_tm;

	HWND Wnd;
	int drawCnt=0;
	static const TCHAR* DATABASE_PROPERTY = _T("GLData");
	typedef struct OpenGLData {
		HGLRC Rc;			
		GLuint glTexture;	
		GLfloat	xrot;		
		GLfloat	yrot;		
		HWND hwnd;
	} GLDATABASE;


	int buffer_width  = 0;
	int buffer_height = 0;
	DWORD timer_tick_1 = 0x105;
	DWORD timer_tick_2 = 0x108;

	sk_sp<GrDirectContext> context = nullptr;
	sk_sp<const GrGLInterface> glface;
	sk_sp<SkSurface> gpuSurface;

	SkPaint paint;
	SkPaint stroke;
	SkPathBuilder path_builder;
	SkString sktring("  SKia IMGUI #");
	SkPaint fill;

	void Skia_c_example_draw(SkCanvas* canvas) {
		fill.setColor(SkColorSetARGB(0xFF, 0x00, 0x00, 0xFF));
		//canvas->drawPaint(*fill);

		fill.setColor(SkColorSetARGB(0xFF, 0x00, 0xFF, 0xFF));
		//SkRect rect{100.0f, 100.0f, 540.0f, 380.0f};
		//canvas->drawRect(rect, *fill);

		path_builder.reset();
		path_builder.moveTo (50.0f, 50.0f);
		path_builder.lineTo (590.0f, 50.0f);
		path_builder.cubicTo(-490.0f, 50.0f, 1130.0f, 430.0f, 50.0f, 430.0f);
		path_builder.lineTo (590.0f, 430.0f);
		SkPath path = path_builder.detach();
		canvas->drawPath(path, stroke);

		fill.setColor(SkColorSetARGB(0x80, 0x00, 0xFF, 0xFF));
		SkRect rect2{120.0f, 120.0f, 520.0f, 360.0f};
		canvas->drawOval(rect2, fill);
	}


	void SkiaDraw(SkCanvas* canvas,int w,int h) {
		//canvas->clear(SkColor(0xff0000ff));
		//for (int i = 0; i < h; i++) canvas->drawPoint(0, i, paint);
		//for (int i = w; i > 0 ; i--) canvas->drawPoint(i-1, h-1, paint);

		
		//sktring.resize(15);
		//sktring.appendS32(drawCnt++);
		//canvas->drawString(sktring, 1, 500, _font, _textpaint);
		
		Skia_c_example_draw(canvas);
		 
	}

	struct ImGui_ImplOpenGL2_Data
	{
		GLuint       FontTexture;
		ImGui_ImplOpenGL2_Data() { memset(this, 0, sizeof(*this)); }
		HWND MouseHwnd;
		BOOL MouseTracked;
	};

	// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
	// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
	static ImGui_ImplOpenGL2_Data* ImGui_ImplOpenGL2_GetBackendData()
	{
		return ImGui::GetCurrentContext() ? (ImGui_ImplOpenGL2_Data*)ImGui::GetIO().BackendRendererUserData : NULL;
	}

	DWORD WINAPI BenchmarkTest(LPVOID p)
	{	
		while(bAPPRunning) 
		{
			//PostMessage(Wnd, WM_COMMAND, 12323283, 0);
			InvalidateRect(Wnd, NULL, FALSE);
			//Sleep(8);
		}
		return 0;
	}

	static bool ImGui_ImplOpenGL2_CreateFontsTexture()
	{
		// Build texture atlas
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplOpenGL2_Data* bd = ImGui_ImplOpenGL2_GetBackendData();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
		
																  // Upload texture to graphics system
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &bd->FontTexture);
		glBindTexture(GL_TEXTURE_2D, bd->FontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		
		// Store our identifier
		io.Fonts->SetTexID((ImTextureID)(intptr_t)bd->FontTexture);
		
		// Restore state
		glBindTexture(GL_TEXTURE_2D, last_texture);

		return true;
	}

	float  _space_width;
	CHAR tmpchar[16];
	TCHAR tmptext[4];
	size_t tmplen;
	unsigned tmpc_;

	void setFont(char* name, float sz)
	{
		if (name)
		{
			auto pFace = SkTypeface::MakeFromName(name, SkFontStyle::Normal());
			if(pFace) _font.setTypeface(pFace);
		}
		_font.setSize(sz);
		SkRect rect;
		_font.measureText("X", 2, SkTextEncoding::kUTF8, &rect);
		_space_width = rect.width();
		_font.measureText("X X", 4, SkTextEncoding::kUTF8, &rect);
		_space_width = rect.width()-_space_width*2;
		tmptext[1]=0;
		_font_spacing = _font.getSpacing();
		ImGui::GetIO().Fonts->AddFontDummy(_font_spacing, _font_spacing/2);
		//ImGui::GetIO().Fonts->AddFontDefault(0, _font_spacing);
	}


	INT64 TicksLastDraw, TicksPerSecond;

	typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
	typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);
	PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
	PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;

	static HGLRC InitGL (HWND Wnd) {
		HGLRC hglrc = 0;											// Preset render context to zero
																		//  We need to make sure the window create in a suitable DC format
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
			PFD_TYPE_RGBA,												// The kind of framebuffer. RGBA or palette.
			32,															// Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,															// Number of bits for the depthbuffer
			8,															// Number of bits for the stencilbuffer
			0,															// Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		HDC hdc = GetDC(Wnd);
		int pixelFormat = ChoosePixelFormat(hdc, &pfd); // Let windows select an appropriate pixel format
		if (SetPixelFormat(hdc, pixelFormat, &pfd)) { // Try to set that pixel format
			hglrc = wglCreateContext(hdc);
			if (hglrc != 0) {
				wglMakeCurrent(hdc, hglrc);				// Make our render context current

#ifdef GLAPIENTRY
				if (!gladLoadGL())
				{
					throw std::runtime_error("Failed to initialize GLAD");
				}
#endif
				glEnable(GL_TEXTURE_2D);								// Enable Texture Mapping
				glShadeModel(GL_SMOOTH);								// Enable Smooth Shading
				glClearColor(0.0f, 0.0f, 120.0f, 0.0f);					// Black Background
				glClearDepth(1.0f);										// Depth Buffer Setup
				glEnable(GL_DEPTH_TEST);								// Enables Depth Testing
				glDepthFunc(GL_LEQUAL);									// The Type Of Depth Testing To Do
				glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// Really Nice Perspective Calculations

				{
					char* extensions = (char*)glGetString(GL_EXTENSIONS);
					if (strstr(extensions,"WGL_EXT_swap_control")) {
						wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)wglGetProcAddress("wglSwapIntervalEXT");
						wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
					}
					if (wglSwapIntervalEXT)
					{
						wglSwapIntervalEXT(vsync_on);
					}
				}

			}
			glface = GrGLMakeNativeInterface();
		}
		ReleaseDC(Wnd, hdc);					// Release the window device context we are done

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		// Setup backend capabilities flags
		ImGui_ImplOpenGL2_Data* bd = IM_NEW(ImGui_ImplOpenGL2_Data)();
		io.BackendRendererUserData = (void*)bd;
		io.BackendPlatformUserData = (void*)bd;
		io.BackendRendererName = "skimGui";

		io.WantSaveIniSettings = false;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)

		setFont("宋体", 20);
		//ImGui::StyleColorsLight();
		//_font.setEmbolden(false);
		ImGui_ImplOpenGL2_CreateFontsTexture();
		::QueryPerformanceFrequency((LARGE_INTEGER*)&TicksPerSecond);
		::QueryPerformanceCounter((LARGE_INTEGER*)&TicksLastDraw);

		paint.setStrokeWidth(1);
		paint.setARGB(0xff, 0xff, 0, 0);
		stroke.setColor(SkColorSetARGB(0xFF, 0xFF, 0x00, 0x00));
		stroke.setAntiAlias(true);
		stroke.setStroke(true);
		stroke.setStrokeWidth(5.0f);


		if (benchmark)
		{
			invalidate_model = invalidate_mode = 1;
			HANDLE hThread;
			DWORD  threadId;
			hThread = CreateThread(NULL, 0,	BenchmarkTest, 0, 0, &threadId);
		}
		else if (invalidate_model==1)
		{
			SetTimer(Wnd, timer_tick_1, timer_swap_interal, 0);
		}

		return (hglrc);											// Return the render context
	}

	int Width;
	int Height;

	static void ReSizeGLScene (HWND Wnd) {
		GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY); // Fetch the data base
		if (db == 0) return;											// Cant resize .. no render context
		HDC Dc = GetWindowDC(Wnd);										// Get the window DC
		RECT r;
		GetClientRect(Wnd, &r);											// Fetch the window size
		Width = r.right - r.left;									// Window width
		Height = r.bottom - r.top;									// Window height
		
		if (Height == 0) Height = 1;									// Stop divid by zero
		
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = (float)Width; io.DisplaySize.y = (float)Height;
		
		wglMakeCurrent(Dc, db->Rc);										// Make our render context current
		glViewport(0, 0, Width, Height);								// Reset The Current Viewport
		glMatrixMode(GL_PROJECTION);									// Select The Projection Matrix
		glLoadIdentity();		
		// Reset The Projection Matrix
		gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f, 100.0f);
		glMatrixMode(GL_MODELVIEW);										// Select The Modelview Matrix
		glLoadIdentity();												// Reset The Modelview Matrix
		
		ReleaseDC(Wnd, Dc);												// Release the window DC
	}

	sk_sp<SkSurface> getBackbufferSurface(int width, int height) {
		if (nullptr == gpuSurface || buffer_width!=width || buffer_height!=height)
		{
			if (context) {
				GrGLint buffer;
				//GR_GL_CALL(glface.get(), GetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer));

				GrGLFramebufferInfo fbInfo;
				fbInfo.fFBOID = buffer;
				fbInfo.fFormat = GR_GL_RGBA8;


				GrBackendRenderTarget backendRT(width
					, height
					, 1
					, 8
					, fbInfo);

				gpuSurface = SkSurface::MakeFromBackendRenderTarget(context.get()
					, backendRT
					, kBottomLeft_GrSurfaceOrigin
					, kRGBA_8888_SkColorType
					, 0
					, 0);
				
				buffer_width=width;
				buffer_height=height;
			}
		}

		return gpuSurface;
	}

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	void DrawGLScene(GLDATABASE* db, HDC Dc, int width, int height) {
		if (db == 0) return;
		//if (!db->Rc) db->Rc = InitGL(db->hwnd);
		wglMakeCurrent(Dc, db->Rc);

		//glViewport(0, 0, width, height);
		//glEnable(GL_BLEND);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		//glDisable(GL_CULL_FACE);
		//glDisable(GL_DEPTH_TEST);
		//glDisable(GL_STENCIL_TEST);
		////glEnable(GL_SCISSOR_TEST);
		////if (bd->GlVersion >= 310)
		//glDisable(GL_PRIMITIVE_RESTART);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glEnable(GL_CULL_FACE);

		if (!context)
		{
			GrContextOptions defaultOptions;
			defaultOptions.fUseDrawInsteadOfClear = GrContextOptions::Enable::kYes;
			context = GrDirectContext::MakeGL(glface, defaultOptions);
		}
		if (context)
		{
			//context->resetContext(GrGLBackendState::kALL_GrGLBackendState);
			sk_sp<SkSurface> glSurf = getBackbufferSurface(width, height);
			if (glSurf) 
			{
				glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
				glClear(GL_COLOR_BUFFER_BIT);
				_canvas = glSurf->getCanvas();
				//_canvas->clear(SkColor(0xffffffff));

				//SkiaDraw(_canvas, width, height);

				//if (false)
				{
					static bool show_demo_window = true;
					static bool show_another_window = false;
					static float f = 0.0f;
					static int counter = 0;
					ImGuiIO& io = ImGui::GetIO();
					INT64 now = 0;
					::QueryPerformanceCounter((LARGE_INTEGER*)&now);
					io.DeltaTime = (float)(now - TicksLastDraw) / TicksPerSecond;
					TicksLastDraw = now;

					ImGui::NewFrame();

					ImGui::Begin("Hello, world!");                          
					ImGui::Text("This is some useful text.");               
					ImGui::Checkbox("Demo Window", &show_demo_window);      
					ImGui::Checkbox("Another Window", &show_another_window);
					ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            
					ImGui::ColorEdit3("clear color", (float*)&clear_color); 
					if (ImGui::Button("Button"))                            
						counter++;
					ImGui::SameLine();
					ImGui::Text("counter = %d", counter);
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					ImGui::Text("Frame Render Time %.1f ms/frame", frame_render_tm/1000.f);
					ImGui::End();

					if (show_demo_window) 
					{
						ImGui::ShowDemoWindow(&show_demo_window);
					}
					if (show_another_window)
					{
						ImGui::Begin("Hello, Skia For IMGUI！");
						ImGui::Button("Chinese《 你好 》");
						ImGui::Button("English《 Hello 》");
						ImGui::Button("Russian《 Привет 》");
						ImGui::Button("Portuguese《 Olá 》");
						ImGui::Button("French《 Allô 》");
						ImGui::Button("Arabic《 أهلا 》");
						ImGui::Button("Greek《 γεια σας 》");
						ImGui::Button("Serbian《 Здраво 》");
						ImGui::Button("Mongolian《 Сайн уу 》");
						ImGui::Button("Korean《 안녕하십니까 》");
						ImGui::Button("Japanese《 こんにちは 》");
						
						ImGui::Button("FONT = 宋体");
						ImGui::End();
					}

					ImGui::EndFrame();
					
					//ImGui::Render();
				}
				_canvas->flush();
				//glSurf->flushAndSubmit();
			}
		}
	}

	int invalid_cnt=0;
	DWORD last_inval_tm=0;
	int post_invalidate_interval=10;


	void postInvalidate(HWND hwnd) 
	{
		::KillTimer(hwnd, timer_tick_2);
		DWORD now = ::GetTickCount();
		if (now-last_inval_tm>post_invalidate_interval)
		{
			::InvalidateRect(hwnd, NULL, FALSE);
			last_inval_tm = now;
		}
		else
		{
			//::SetTimer(hwnd, timer_tick_2, post_invalidate_interval, nullptr);
		}
	}

	LRESULT CALLBACK OpenGLDemoHandler (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
		bool paint=true;
		byte hold=0;
		switch (msg)
		{
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		{
			hold = 1;
			ImGuiIO& io = ImGui::GetIO();
			int button = 0;
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
			if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
			if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
				::SetCapture(hwnd);
			io.MouseDown[button] = true;
			break;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			hold = 2;
			ImGuiIO& io = ImGui::GetIO();
			int button = 0;
			if (msg == WM_LBUTTONUP) { button = 0; }
			if (msg == WM_RBUTTONUP) { button = 1; }
			if (msg == WM_MBUTTONUP) { button = 2; }
			if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
			io.MouseDown[button] = false;
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
				::ReleaseCapture();
			break;
		}
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		case WM_MOUSEMOVE:
		case WM_PAINT:
		case WM_MOVE:
		case WM_SIZE:
			break;
		default:
			paint = false;
			break;
		}
		if (paint)
		{
			if (msg==WM_PAINT)
			{
				INT64 now = 0;
				::QueryPerformanceCounter((LARGE_INTEGER*)&now);

				PAINTSTRUCT ps;
				RECT rc;
				::GetClientRect(hwnd, &rc);
				GLDATABASE* db = (GLDATABASE*) GetProp(hwnd, DATABASE_PROPERTY);
				HDC hdc = BeginPaint(hwnd, &ps);
				DrawGLScene(db, hdc, rc.right, rc.bottom);
					HDC dc = GetDC(hwnd);
					SwapBuffers(dc);
					ReleaseDC(hwnd, dc);
				EndPaint(hwnd, &ps);

				//::BeginPaint(Wnd, &ps);	
				//DrawGLScene(db, ps.hdc, rc.right, rc.bottom);
				//::SwapBuffers(ps.hdc);	
				//::EndPaint(Wnd, &ps);

				INT64 now1 = 0;
				::QueryPerformanceCounter((LARGE_INTEGER*)&now1);
				frame_render_tm = now1-now;
				//return TRUE;
				return DefWindowProc(hwnd, msg, wParam, lParam);
			} 
			else if(invalidate_mode==0)
			{
				postInvalidate(hwnd);
			}
		}

		if (invalidate_model!=1)
		{
			if (hold)
			{
				if (hold==1){
					invalidate_mode = 1;
					SetTimer(hwnd, timer_tick_1, 20, 0);
				}
				else //==2
				{
					invalidate_mode = 0;
					KillTimer(hwnd, timer_tick_1);
				}
				hold = 0;
			}
			else if (invalidate_mode==1 && ImGui::GetCurrentContext()->MovingWindow)
			{
				// reset to 0 if dragging the window.
				KillTimer(hwnd, timer_tick_1);
				invalidate_mode = 0;
			}
		}

		switch (msg){
			//case WM_SIZE:
			case WM_WINDOWPOSCHANGED:
				if ((lParam == 0) || ((((PWINDOWPOS) lParam)->flags & SWP_NOSIZE) == 0)){
					ReSizeGLScene(hwnd);			
					gpuSurface = 0;
					//::InvalidateRect(hwnd, 0, FALSE);

					//PostMessage(hwnd, WM_PAINT, 0, 0);
					postInvalidate(hwnd);
					//::UpdateWindow(Wnd);
				}
			break;
			case WM_CREATE:	{
				// Make a data structure, initialize it and attach to Wnd
				GLDATABASE* db = (GLDATABASE*) malloc(sizeof(GLDATABASE)); 
				db->Rc = InitGL(hwnd);							 
				db->glTexture = 0;								 
				db->xrot = 0.0f;								 
				db->yrot = 0.0f;								 
				db->hwnd = hwnd;								 
																 
				SetProp(hwnd, DATABASE_PROPERTY, (HANDLE) db);	 
				ReSizeGLScene(hwnd);							 
			} break;
			case WM_CLOSE:
			case WM_DESTROY: {
				context = nullptr;
				glface = nullptr;
				gpuSurface = nullptr;
				// We need to do some cleanups as program is going to exit	
				wglMakeCurrent(NULL, NULL);							// Make the rendering context not current 
				GLDATABASE* db = (GLDATABASE*) GetProp(hwnd, DATABASE_PROPERTY); // Fetch the data base
				if (db != 0) {
					if (db->Rc != 0) wglDeleteContext(db->Rc);		// If valid rendering context delete it
					if (db->glTexture != 0)
						glDeleteTextures(1, &db->glTexture);		// If valid gltexture delete it
					free(db);										// Release the data structure memory itself
				}
				::PostQuitMessage(0);
				bAPPRunning = false;
			} break;
			case WM_TIMER: {											// WM_TIMER MESSAGE
				if (wParam==timer_tick_2)
				{
					::KillTimer(hwnd, timer_tick_2);
					last_inval_tm = ::GetTickCount();
					InvalidateRect(hwnd, NULL, FALSE);
				} 
				else if(wParam==timer_tick_1){
					InvalidateRect(hwnd, NULL, FALSE);
					//PostMessage(hwnd, WM_COMMAND, 12323283, 0);
				}
			} break;
			case WM_ERASEBKGND:											// WM_ERASEBKGND MESSAGE
				return (FALSE);
			case WM_MOUSEMOVE:
			{
				ImGuiIO& io = ImGui::GetIO();
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam);

				POINT pos;
				if (::GetCursorPos(&pos) && ::ScreenToClient(hwnd, &pos))
				{
					io.MousePos = ImVec2((float)pos.x, (float)pos.y);
				}

				ImGui_ImplOpenGL2_Data* bd = ImGui_ImplOpenGL2_GetBackendData();
				// We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events
				bd->MouseHwnd = hwnd;
				if (!bd->MouseTracked)
				{
					TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
					::TrackMouseEvent(&tme);
					bd->MouseTracked = true;
				}
				return 1;
			}
			case WM_MOUSELEAVE:
			{
				ImGui_ImplOpenGL2_Data* bd = ImGui_ImplOpenGL2_GetBackendData();
				if (bd->MouseHwnd == hwnd)
					bd->MouseHwnd = NULL;
				bd->MouseTracked = false;
				break;
			}
			case WM_MOUSEWHEEL:
				ImGui::GetIO().MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
				return 0;
			case WM_MOUSEHWHEEL:
				ImGui::GetIO().MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
				return 0;
			case WM_SETFOCUS:
			case WM_KILLFOCUS:
			{
				ImGuiIO& io = ImGui::GetIO();
				io.AddFocusEvent(msg == WM_SETFOCUS);
				return 0;
			}
			case WM_COMMAND:
			{
				if (wParam==12323283)
				{
					InvalidateRect(hwnd, NULL, FALSE);
				}
				return TRUE;
			}
		};
		return DefWindowProc(hwnd, msg, wParam, lParam);
	};

	void drawWithClipping(const ImVec4& clip_rect)
	{
		_canvas->save();
		_canvas->clipRect({clip_rect.x, clip_rect.y, clip_rect.z, clip_rect.w});
	}

	void drawWithoutClipping()
	{
		_canvas->restore();
	}

	float Skia_Measure_Char(unsigned int c_) 
	{
		//if (true) return 1.4*_space_width;
		if (c_==' ')
		{
			return _space_width;
		}
		if (tmpc_!=c_)
		{
			if (c_>0x80)
			{
				tmptext[0]=c_;
				tmplen = WideCharToMultiByte(CP_ACP, 0, tmptext, 1, tmpchar, 15, 0, 0);
			}
			else
			{
				tmpchar[0]=c_;
				tmplen=1;
			}
			tmpc_ = c_;
		}
		SkRect rect;
		_font.measureText(tmpchar, tmplen, SkTextEncoding::kUTF8, &rect);

		return rect.right();
	}

	void drawChar(unsigned int c_, float x, float y, unsigned int col) 
	{
		if (tmpc_!=c_)
		{
			if (c_>0x80)
			{
				tmptext[0]=c_;
				tmplen = WideCharToMultiByte(CP_ACP, 0, tmptext, 1, tmpchar, 15, 0, 0);
			}
			else
			{
				tmpchar[0]=c_;
				tmplen=1;
			}
			tmpc_ = c_;
		}
		_textpaint.setColor(SkColorSetARGB((col>>IM_COL32_A_SHIFT)&0xFF
			, (col>>IM_COL32_R_SHIFT)&0xFF
			, (col>>IM_COL32_G_SHIFT)&0xFF
			, (col>>IM_COL32_B_SHIFT)&0xFF));
		_canvas->drawSimpleText(tmpchar, tmplen, SkTextEncoding::kUTF8, x, y+_font_spacing, _font, _textpaint);
		//_canvas->drawString(tmpchar, x, y+_font_spacing, _font, _textpaint);
	}

	float Skia_Measure_Text(const char* utf8_c_, size_t length) 
	{
		SkRect rect;
		_font.measureText(utf8_c_, length, SkTextEncoding::kUTF8, &rect);
		return rect.right();
	}

	void drawText(const char* utf8_c_, size_t length, float x, float y, unsigned int col) 
	{
		if (length>0)
		{
			_textpaint.setColor(SkColorSetARGB((col>>IM_COL32_A_SHIFT)&0xFF
				, (col>>IM_COL32_R_SHIFT)&0xFF
				, (col>>IM_COL32_G_SHIFT)&0xFF
				, (col>>IM_COL32_B_SHIFT)&0xFF));
			_canvas->drawSimpleText(utf8_c_, length, SkTextEncoding::kUTF8, x, y+_font_spacing, _font, _textpaint);
		}
	}

	void drawRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding) 
	{
		fill.setColor(SkColorSetARGB((col>>IM_COL32_A_SHIFT)&0xFF
			, (col>>IM_COL32_R_SHIFT)&0xFF
			, (col>>IM_COL32_G_SHIFT)&0xFF
			, (col>>IM_COL32_B_SHIFT)&0xFF));
		SkRect rect{p_min.x, p_min.y, p_max.x, p_max.y};
		if (rounding<=0)
		{
			_canvas->drawRoundRect(rect, rounding, rounding, fill);
		}
		else
		{
			_canvas->drawRect(rect, fill);
		}
	}
}
using namespace SkimGuiHello;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) /* The 'entry' point of the program. Console programs use the simple main(...)        */
{
	_font.setEmbolden(true);

	_textpaint.reset();
	_textpaint.setColor(SkColor(0xffffffff));
	_textpaint.setAntiAlias(true);

	bUseCustomDraw = true;
	Func_Measure_Char = Skia_Measure_Char;
	Func_Measure_Text = Skia_Measure_Text;
	Func_Draw_Char = drawChar;
	Func_Draw_Text = drawText;
	Func_Clip_Rect = drawWithClipping;
	Func_Clip_Reset = drawWithoutClipping;
	Func_Draw_Rect = drawRect;

	MSG Msg;
	RECT R;
	WNDCLASSEX WndClass;

	memset(&WndClass, 0, sizeof(WNDCLASSEX));						
	WndClass.cbSize = sizeof(WNDCLASSEX);							
	WndClass.style = CS_OWNDC;										
	WndClass.lpfnWndProc = OpenGLDemoHandler;						
	WndClass.cbClsExtra = 0;										
	WndClass.cbWndExtra = 0;										
	WndClass.hInstance = GetModuleHandle(NULL);						
	WndClass.hIcon = LoadIcon(0, IDI_APPLICATION);					
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);					
	WndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);	
	WndClass.lpszMenuName = NULL;									
	WndClass.lpszClassName = L"win32_wnd";					
	RegisterClassEx(&WndClass);										
	GetClientRect(GetDesktopWindow(), &R);									
	Wnd = CreateWindowEx(0, L"win32_wnd", L"SkimGui Test", 
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, R.left+300, R.top+300, 
		R.right-R.left-400, R.bottom-R.top-400,
		0, 0, 0, NULL);

	while (GetMessage(&Msg, 0, 0, 0))
	{
		TranslateMessage(&Msg);			
		DispatchMessage(&Msg);
	};


	return (0);
}

