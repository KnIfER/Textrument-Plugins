/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
* Skia OpenGl Hello World
* 
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

#include <windows.h>
#include <tchar.h>	
#include <commctrl.h>

//#include <gl\gl.h>
#include <glad/glad.h>
#include <gl\glu.h>	


namespace GLSkiaHello{
	HWND Wnd;
	int drawCnt=0;

#define IDC_BMPLOAD 101									// App menu to load bitmap
#define IDC_EXIT 105									// App menu command to exit application
#define IDC_TIMERSTART 201								// App menu to start timer
#define IDC_TIMERSTOP 202								// App menu to stop timer

	/*--------------------------------------------------------------------------}
	;{                      APPLICATION STRING CONSTANTS			            }
	;{-------------------------------------------------------------------------*/
	const TCHAR* AppClassName = _T("OPENGL_DEMO_APP");
	static const TCHAR* DATABASE_PROPERTY = _T("OurDataStructure");

	/*---------------------------------------------------------------------------
	OUR OPENGL DATA RECORD DEFINITION
	---------------------------------------------------------------------------*/
	typedef struct OpenGLData {
		HGLRC Rc;											// Our render context ** Always Needed
		GLuint glTexture;									// Our texture to draw
		GLfloat	xrot;										// X Rotation
		GLfloat	yrot;										// Y Rotation
	} GLDATABASE;

	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	COMMON DIALOG CALL ROUTINES
	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



	int OpenFileDialog (TCHAR* Name, unsigned short NameBufSize, TCHAR* String, TCHAR* Ext, TCHAR* Title, HWND Wnd){
		int i;
		TCHAR FileName[256], DefExt[256], Filter[400];
		OPENFILENAME OpenFN;

		InitCommonControls();											// Initialize common dialogs
		Name[0] = 0;													// Preset name clear
		_tcscpy_s(&FileName[0], _countof(FileName), _T("*."));			// Tranfer "*."
		_tcscat_s(&FileName[0], _countof(FileName), Ext);				// Create "*.xxx" extension
		_tcscpy_s(Filter, _countof(Filter), String);					// Tranfer string
		i = _tcslen(Filter);											// Fetch that string length in TCHAR
		_tcscpy_s(&Filter[i+1], _countof(Filter)-i-1, &FileName[0]);	// Transfer "*.ext"
		i += (_tcslen(&FileName[0]) + 1);								// Advance to beyond end
		_tcscpy_s(&Filter[i + 1], _countof(Filter) - i - 1, _T("\0"));	// Must end with two 0 entries
		_tcscpy_s(&Filter[i + 2], _countof(Filter) - i - 2, _T("\0"));	// Must end with two 0 entries
		_tcscpy_s(DefExt, _countof(DefExt), Ext);						// Default ext name
		memset((void*)&OpenFN, 0, sizeof(OpenFN));					    // Zero unused fields
		OpenFN.lStructSize = sizeof(OPENFILENAME);					    // Size of structure
		OpenFN.hInstance = GetModuleHandle(NULL);						// Pointer to instance
		OpenFN.hwndOwner = Wnd;											// Owner window
		OpenFN.lpstrFilter = &Filter[0];								// Filter
		OpenFN.nFilterIndex = 1;										// 1st Filter String
		OpenFN.lpstrFile = &FileName[0];								// Return result
		OpenFN.nMaxFile = _countof(FileName);							// Max name length
		OpenFN.lpstrDefExt = Ext;										// Default extension
		OpenFN.lpstrFileTitle = &FileName[0];							// Default file title
		OpenFN.nMaxFileTitle = _countof(FileName);						// Max size of file title
		OpenFN.lpstrTitle = Title;	                     			    // Window title
		OpenFN.lpfnHook = NULL;											// No hook handler
		OpenFN.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES;               // Set flag masks
		if (GetOpenFileName(&OpenFN) != 0){
			_tcscpy_s(Name, NameBufSize, FileName);						// Return the name
			return OpenFN.nFilterIndex;									// Return filter type
		} else return 0;												// Dialog cancelled
	};
	 
	sk_sp<GrDirectContext> context = nullptr;
	sk_sp<const GrGLInterface> glface;
	sk_sp<SkSurface> gpuSurface;

	void Skia_c_example_draw(SkCanvas* canvas) {
		SkPaint* fill = new SkPaint();
		fill->setColor(SkColorSetARGB(0xFF, 0x00, 0x00, 0xFF));
		//canvas->drawPaint(*fill);

		fill->setColor(SkColorSetARGB(0xFF, 0x00, 0xFF, 0xFF));
		SkRect rect{100.0f, 100.0f, 540.0f, 380.0f};
		canvas->drawRect(rect, *fill);

		SkPaint* stroke = new SkPaint();
		stroke->setColor(SkColorSetARGB(0xFF, 0xFF, 0x00, 0x00));
		stroke->setAntiAlias(true);
		stroke->setStroke(true);
		stroke->setStrokeWidth(5.0f);

		SkPathBuilder* path_builder = new SkPathBuilder();
		path_builder->moveTo (50.0f, 50.0f);
		path_builder->lineTo (590.0f, 50.0f);
		path_builder->cubicTo(-490.0f, 50.0f, 1130.0f, 430.0f, 50.0f, 430.0f);
		path_builder->lineTo (590.0f, 430.0f);
		SkPath path = path_builder->detach();
		canvas->drawPath(path, *stroke);

		fill->setColor(SkColorSetARGB(0x80, 0x00, 0xFF, 0x00));
		SkRect rect2{120.0f, 120.0f, 520.0f, 360.0f};
		canvas->drawOval(rect2, *fill);

		delete path_builder;
		delete stroke;
		delete fill;
	}

	void SkiaDraw(SkCanvas* canvas,int w,int h) {
		//canvas->clear(SkColor(0xff0000ff));
		SkPaint paint;
		paint.setStrokeWidth(1);
		paint.setARGB(0xff, 0xff, 0, 0);
		for (int i = 0; i < h; i++) canvas->drawPoint(0, i, paint);
		for (int i = w; i > 0 ; i--) canvas->drawPoint(i-1, h-1, paint);

		SkFont font;
		font.setSize(16);
		SkPaint textpaint;
		textpaint.reset();
		font.setSize(16);
		textpaint.setColor(SkColor(0xffff0000));
		textpaint.setAntiAlias(true);

		SkString string("Hello Skia World #");

		string.appendS32(drawCnt++);
		string.appendf(" bmpSize=%.2f", 1);
		canvas->drawString(string, 1, 16, font, textpaint);

		Skia_c_example_draw(canvas);
	}


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
					//if(!gladLoadGLLoader((GLADloadproc)wglGetProcAddress)) 
				{
					throw std::runtime_error("Failed to initialize GLAD");
				}
#endif
				
				glEnable(GL_TEXTURE_2D);								// Enable Texture Mapping
				glShadeModel(GL_SMOOTH);								// Enable Smooth Shading
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					// Black Background
				glClearDepth(1.0f);										// Depth Buffer Setup
				glEnable(GL_DEPTH_TEST);								// Enables Depth Testing
				glDepthFunc(GL_LEQUAL);									// The Type Of Depth Testing To Do
				glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// Really Nice Perspective Calculations
			}
			glface = GrGLMakeNativeInterface();
		}
		ReleaseDC(Wnd, hdc);					// Release the window device context we are done
		
		SetTimer(Wnd, 1, 100, 0);	
		
		return (hglrc);											// Return the render context
	}

	static void ReSizeGLScene (HWND Wnd) {
		GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY); // Fetch the data base
		if (db == 0) return;											// Cant resize .. no render context
		HDC Dc = GetWindowDC(Wnd);										// Get the window DC
		RECT r;
		GetWindowRect(Wnd, &r);											// Fetch the window size
		int Width = r.right - r.left;									// Window width
		int Height = r.bottom - r.top;									// Window height
		if (Height == 0) Height = 1;									// Stop divid by zero
		wglMakeCurrent(Dc, db->Rc);										// Make our render context current
		glViewport(0, 0, Width, Height);								// Reset The Current Viewport
		glMatrixMode(GL_PROJECTION);									// Select The Projection Matrix
		glLoadIdentity();												// Reset The Projection Matrix
																		// Calculate The Aspect Ratio Of The Window
		gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);										// Select The Modelview Matrix
		glLoadIdentity();												// Reset The Modelview Matrix
		ReleaseDC(Wnd, Dc);												// Release the window DC
		
	}

	int buffer_width  = 0;
	int buffer_height = 0;

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

	void DrawGLScene(GLDATABASE* db, HDC Dc, int width, int height) {
		if ((db == 0) || (db->glTexture == 0)) return;
		wglMakeCurrent(Dc, db->Rc);
		if (!context)
		{
			GrContextOptions defaultOptions;
			//defaultOptions.fUseDrawInsteadOfClear = GrContextOptions::Enable::kYes;
			context = GrDirectContext::MakeGL(glface, defaultOptions);
		}



		//glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		//
		//// Reset 
		//glDisable(GL_BLEND);
		//glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		////glBindVertexArray(vertexArrayObject); // Restore default VAO 
		//glFrontFace(GL_CCW);
		//glEnable(GL_FRAMEBUFFER_SRGB);
		//glActiveTexture(0);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glDrawBuffer(GL_BACK);
		//glEnable(GL_DITHER);
		//glDepthMask(true);
		//glEnable(GL_MULTISAMPLE);
		//glDisable(GL_SCISSOR_TEST);



		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
		glLoadIdentity();									
		glTranslatef(0.0f, 0.0f, -5.0f);

		glRotatef(db->xrot, 1.0f, 0.0f, 0.0f);
		glRotatef(db->yrot, 0.0f, 1.0f, 0.0f);

		glBindTexture(GL_TEXTURE_2D, db->glTexture);


		// Draw our texture cube to screen :-)
		glBegin(GL_QUADS);
		// Front Face
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
		// Back Face
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
		// Top Face
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
		// Bottom Face
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
		// Right face
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
		// Left Face
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
		glEnd();


		context->resetContext(GrGLBackendState::kALL_GrGLBackendState);
		sk_sp<SkSurface> glSurf = getBackbufferSurface(width, height);
		if (glSurf) 
		{
			//glSurf->getCanvas()->clear(SkColor(0xff0000ff));
			SkiaDraw(glSurf->getCanvas(), width, height);
			//glSurf->flushAndSubmit();
			glSurf->getCanvas()->flush();
		}


	}


	/*-[ BMP2GLTexture ]--------------------------------------------------------

	This converts a bitmap file from the filename path and converts it to an
	OpenGL texture. If the filename is invalid of the file not a bitmap the
	routine will return a GLuint of zero.

	15Apr16 LdB
	--------------------------------------------------------------------------*/
	GLuint BMP2GLTexture(TCHAR* fileName, HWND Wnd, GLDATABASE* db)	{
		HBITMAP hBMP;                                                   // Handle Of The Bitmap
		BITMAP  BMP;                                                    // Bitmap Structure

		HDC Dc = GetWindowDC(Wnd);										// Fetch the window DC
		if ((db != 0) && (db->Rc != 0)) wglMakeCurrent(Dc, db->Rc);		// Make our render context current

		hBMP = (HBITMAP) LoadImage(GetModuleHandle(NULL), fileName,
			IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);	// Load the bitmap from file
		if (!hBMP) return (0);											// If bitmap does not exist return false
		GetObject(hBMP, sizeof(BMP), &BMP);								// Get The bitmap details
		int P2Width = (BMP.bmWidth) >> 2;								// Divid width by 4
		if ((P2Width << 2) < (BMP.bmWidth)) P2Width++;					// Inc by 1 if width x 4 is less than original
		P2Width = P2Width << 2;											// Power of two width
		long imageSize = (long) P2Width * (long) BMP.bmHeight * sizeof(RGBQUAD);
		BYTE* lpPixels = (BYTE*) malloc(imageSize);						// Create the pixel buffer					

																		// Create and fill BITMAPINFO structure to pass to GetDIBits
		BITMAPINFO bmi;
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = BMP.bmWidth;
		bmi.bmiHeader.biHeight = BMP.bmHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;
		bmi.bmiHeader.biCompression = 0;
		bmi.bmiHeader.biSizeImage = imageSize;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed = 0;
		bmi.bmiHeader.biClrImportant = 0;

		// Put DIBBits into memory buffer
		GetDIBits(Dc, hBMP, 0, BMP.bmHeight, lpPixels, &bmi, DIB_RGB_COLORS);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);							// Pixel Storage Mode (Word Alignment / 4 Bytes)
		GLuint texture;
		glGenTextures(1, &texture);										// Create a GL texture

																		// Create Nearest Filtered Texture
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, 0x80E0, GL_UNSIGNED_BYTE, lpPixels);
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, lpPixels);

		free(lpPixels);													// Free allocated pixel memory
		ReleaseDC(Wnd, Dc);												// Release the window DC
		DeleteObject(hBMP);												// Delete The Object
		return (texture);												// Return the texture
	}


	/*---------------------------------------------------------------------------
	Application handler. 
	---------------------------------------------------------------------------*/
	LRESULT CALLBACK OpenGLDemoHandler (HWND Wnd, UINT Msg, WPARAM wParam, LPARAM lParam){

		switch (Msg){
			case WM_PAINT: {
				PAINTSTRUCT ps;
				RECT rc;
				::GetClientRect(Wnd, &rc);


				GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY);// Fetch the data base
				
				//BeginPaint(Wnd, &ps);						
				//DrawGLScene(db, ps.hdc, rc.right, rc.bottom);
				//SwapBuffers(ps.hdc);
				//EndPaint(Wnd, &ps);							


				HDC hdc = BeginPaint(Wnd, &ps);
				
					DrawGLScene(db, hdc, rc.right, rc.bottom);
				
					HDC dc = GetDC(Wnd);
					SwapBuffers(dc);
					ReleaseDC(Wnd, dc);
				
				EndPaint(Wnd, &ps);

				return 0;
			} break;
			case WM_WINDOWPOSCHANGED:	
				if (drawCnt>100)
				{
					break;
				}
				if ((lParam == 0) || ((((PWINDOWPOS) lParam)->flags & SWP_NOSIZE) == 0)){
					ReSizeGLScene(Wnd);									// Rescale the GL window							
					InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us
				}
			break;
			case WM_CREATE:	{			//  First manually build a menu for a window
				HMENU SubMenu, Menu;
				Menu = CreateMenu();								// Create a menu and populate it
				SubMenu = CreatePopupMenu();
				AppendMenu(SubMenu, MF_STRING, IDC_BMPLOAD, _T("&Load Bitmap"));
				AppendMenu(SubMenu, MF_SEPARATOR, 0, NULL);
				AppendMenu(SubMenu, MF_STRING, IDC_EXIT, _T("E&xit"));
				AppendMenu(Menu, MF_POPUP, (UINT_PTR) SubMenu, _T("&File"));
				SubMenu = CreatePopupMenu();
				AppendMenu(SubMenu, MF_STRING, IDC_TIMERSTART, _T("&Start Timer"));
				AppendMenu(SubMenu, MF_STRING, IDC_TIMERSTOP, _T("Stop &Timer"));
				AppendMenu(Menu, MF_POPUP, (UINT_PTR) SubMenu, _T("&Timer"));
				SubMenu = CreatePopupMenu();
				AppendMenu(SubMenu, MF_STRING, 301, _T("&Vectorize"));
				AppendMenu(Menu, MF_POPUP, (UINT_PTR) SubMenu, _T("&Process"));
				SetMenu(Wnd, Menu);									
				// Make a data structure, initialize it and attach to Wnd
				GLDATABASE* db = (GLDATABASE*) malloc(sizeof(GLDATABASE)); // Allocate structure
				db->Rc = InitGL(Wnd);								// Initialize OpenGL and get render context
				db->glTexture = 0;									// Zero the texture
				db->xrot = 0.0f;									// Zero x rotation
				db->yrot = 0.0f;									// Zero y rotation

				db->glTexture = BMP2GLTexture(L"D:\\Code\\Skija\\opengl\\OpenGLAppDemo\\Dragons.bmp", Wnd, db);

				
				SetProp(Wnd, DATABASE_PROPERTY, (HANDLE) db);		// Set the database structure to a property on window
				ReSizeGLScene(Wnd);									// Rescale the OpenGL window
			} break;
			case WM_DESTROY: {
				context = nullptr;
				glface = nullptr;
				gpuSurface = nullptr;
				// We need to do some cleanups as program is going to exit	
				wglMakeCurrent(NULL, NULL);							// Make the rendering context not current 
				GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY); // Fetch the data base
				if (db != 0) {
					if (db->Rc != 0) wglDeleteContext(db->Rc);		// If valid rendering context delete it
					if (db->glTexture != 0)
						glDeleteTextures(1, &db->glTexture);		// If valid gltexture delete it
					free(db);										// Release the data structure memory itself
				}
			} break;
			case WM_COMMAND:
				switch (LOWORD(wParam)){
					case IDC_BMPLOAD: {                                 // LOAD BITMAP COMMAND
						TCHAR FileName[256];
						int i = OpenFileDialog(&FileName[0], _countof(FileName),
							_T("BMP file:"), _T("BMP"), _T("BMP FILE TO LOAD"), Wnd);
						if (i != 0){
							// Fetch that childs data base
							GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY);
							if (db != 0) {
								// Now check if texture exists and if so delete it
								if (db->glTexture) {
									glDeleteTextures(1, &db->glTexture);
									db->glTexture = 0;
								}
								// Create new texture
								db->glTexture = BMP2GLTexture(&FileName[0], Wnd, db);
								InvalidateRect(Wnd, 0, TRUE);	// Force redraw of window
							}
						}
					} break;
					case IDC_EXIT:								
						PostMessage(Wnd, WM_CLOSE, 0, 0);		
						break;
					case IDC_TIMERSTART: {						
						SetTimer(Wnd,							
							1,									
							100,								
							0);									
					}  break;
					case IDC_TIMERSTOP: {						
						KillTimer(Wnd, 1);						
					} break;
				};
				break;
			case WM_TIMER: {											// WM_TIMER MESSAGE
				GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY);// Fetch the data base
				db->xrot += 1.0f;									// Inc x rotation
				db->yrot += 1.0f;									// Inc y rotation
				InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us			
			} break;
			case WM_ERASEBKGND:											// WM_ERASEBKGND MESSAGE
				return (FALSE);
			default: return DefWindowProc(Wnd, Msg, wParam, lParam);	// Default handler
		};// end switch case
		return 0;
	};
}
using namespace GLSkiaHello;

int GLSkiaHello_RunMain(HINSTANCE hInstance, HWND hParent)
{
	MSG Msg;
	RECT R;
	WNDCLASSEX WndClass;

	memset(&WndClass, 0, sizeof(WNDCLASSEX));						// Clear the class record
	WndClass.cbSize = sizeof(WNDCLASSEX);							// Size of this record
	WndClass.style = CS_OWNDC;										// Class styles
	WndClass.lpfnWndProc = OpenGLDemoHandler;						// Handler for this class
	WndClass.cbClsExtra = 0;										// No extra class data
	WndClass.cbWndExtra = 0;										// No extra window data
	WndClass.hInstance = GetModuleHandle(NULL);						// This instance
	WndClass.hIcon = LoadIcon(0, IDI_APPLICATION);					// Set icon
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);					// Set cursor
	WndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);	// Set background brush
	WndClass.lpszMenuName = NULL;									// No menu yet
	WndClass.lpszClassName = AppClassName;							// Set class name
	RegisterClassEx(&WndClass);										// Register the class
	GetClientRect(GetDesktopWindow(), &R);							// Get desktop area					
	Wnd = CreateWindowEx(0, AppClassName, _T("OpenGL Demo Program"), 
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, R.left+300, R.top+300, 
		R.right-R.left-400, R.bottom-R.top-400,
		0, 0, 0, NULL);
	return (0);
}

