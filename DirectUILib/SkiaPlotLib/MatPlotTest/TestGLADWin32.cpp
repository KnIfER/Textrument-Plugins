
#include <matplot/matplot.h>
#include <matplot/core/figure_registry.h>

#define _WIN32_WINNT 0x0500
#include <windows.h>		// Standard windows headers
#include <tchar.h>			// Unicode support	.. we will use TCHAR rather than char	
#include <commctrl.h>		// Common controls dialogs unit
#include <glad/glad.h>
#include <gl\glu.h>			// Header File For The GLu32 Library


// This is the lazy adding libraries via #pragma rather than in linker includes
// If you are not on visual studio you will need to add the librarys via your linker
#pragma comment(lib,"ComCtl32.lib")
#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")


using namespace matplot;


namespace GLADPPDemo{

#define IDC_BMPLOAD 101			
#define IDC_EXIT 105			
#define IDC_TIMERSTART 201		
#define IDC_TIMERSTOP 202		

	const TCHAR* AppClassName = _T("OPENGL_DEMO_APP");
	static const TCHAR* DATABASE_PROPERTY = _T("OurDataStructure");

	typedef struct OpenGLData {
		HGLRC Rc;										
		GLuint glTexture;								
		GLfloat	xrot;									
		GLfloat	yrot;									
	} GLDATABASE;

	int OpenFileDialog (TCHAR* Name, unsigned short NameBufSize, TCHAR* String, TCHAR* Ext, TCHAR* Title, HWND Wnd){
		int i;
		TCHAR FileName[256], DefExt[256], Filter[400];
		OPENFILENAME OpenFN;

		InitCommonControls();											
		Name[0] = 0;													
		_tcscpy_s(&FileName[0], _countof(FileName), _T("*."));			
		_tcscat_s(&FileName[0], _countof(FileName), Ext);				
		_tcscpy_s(Filter, _countof(Filter), String);					
		i = _tcslen(Filter);											
		_tcscpy_s(&Filter[i+1], _countof(Filter)-i-1, &FileName[0]);	
		i += (_tcslen(&FileName[0]) + 1);								
		_tcscpy_s(&Filter[i + 1], _countof(Filter) - i - 1, _T("\0"));	
		_tcscpy_s(&Filter[i + 2], _countof(Filter) - i - 2, _T("\0"));	
		_tcscpy_s(DefExt, _countof(DefExt), Ext);						
		memset((void*)&OpenFN, 0, sizeof(OpenFN));					    
		OpenFN.lStructSize = sizeof(OPENFILENAME);					    
		OpenFN.hInstance = GetModuleHandle(NULL);						
		OpenFN.hwndOwner = Wnd;											
		OpenFN.lpstrFilter = &Filter[0];								
		OpenFN.nFilterIndex = 1;										
		OpenFN.lpstrFile = &FileName[0];								
		OpenFN.nMaxFile = _countof(FileName);							
		OpenFN.lpstrDefExt = Ext;										
		OpenFN.lpstrFileTitle = &FileName[0];							
		OpenFN.nMaxFileTitle = _countof(FileName);						
		OpenFN.lpstrTitle = Title;	                     			    
		OpenFN.lpfnHook = NULL;											
		OpenFN.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES;               
		if (GetOpenFileName(&OpenFN) != 0){
			_tcscpy_s(Name, NameBufSize, FileName);						
			return OpenFN.nFilterIndex;									
		} else return 0;												
	};


	static HGLRC InitGL (HWND Wnd) {
		HGLRC ourOpenGLRC = 0;											// Preset render context to zero

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
			24,															
			8,															
			0,															
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};


		HDC ourWindowHandleToDeviceContext = GetDC(Wnd);				
		int letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd); 
		if (SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd)) { 
			ourOpenGLRC = wglCreateContext(ourWindowHandleToDeviceContext);

			if (ourOpenGLRC != 0) {
				wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRC); 

#ifdef GLAPIENTRY
				if (!gladLoadGL())
				//if(!gladLoadGLLoader((GLADloadproc)wglGetProcAddress)) 
				{
					throw std::runtime_error("Failed to initialize GLAD");
				}
#endif
				glEnable(GL_TEXTURE_2D);								
				glShadeModel(GL_SMOOTH);								
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					
				glClearDepth(1.0f);										
				glEnable(GL_DEPTH_TEST);								
				glDepthFunc(GL_LEQUAL);									
				glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		
			}
		}
		ReleaseDC(Wnd, ourWindowHandleToDeviceContext);					
		return (ourOpenGLRC);											
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
		glViewport(0, 0, Width, Height);								
		glMatrixMode(GL_PROJECTION);									
		glLoadIdentity();												
																		
		gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);									
		glLoadIdentity();											
		ReleaseDC(Wnd, Dc);											
	}

	void DrawGLScene(GLDATABASE* db, HDC Dc) {
		if ((db == 0) || (db->glTexture == 0)) return;				
		wglMakeCurrent(Dc, db->Rc);									

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
	}

	GLuint BMP2GLTexture(TCHAR* fileName, HWND Wnd, GLDATABASE* db)	{
		HBITMAP hBMP;                                                
		BITMAP  BMP;                                                 

		HDC Dc = GetWindowDC(Wnd);									
		if ((db != 0) && (db->Rc != 0)) wglMakeCurrent(Dc, db->Rc);	

		hBMP = (HBITMAP) LoadImage(GetModuleHandle(NULL), fileName,
			IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);	
		if (!hBMP) return (0);											
		GetObject(hBMP, sizeof(BMP), &BMP);								
		int P2Width = (BMP.bmWidth) >> 2;								
		if ((P2Width << 2) < (BMP.bmWidth)) P2Width++;					
		P2Width = P2Width << 2;											
		long imageSize = (long) P2Width * (long) BMP.bmHeight * sizeof(RGBQUAD);
		BYTE* lpPixels = (BYTE*) malloc(imageSize);						

																		
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

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);						
		GLuint texture;
		glGenTextures(1, &texture);									

																	
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, 0x80E0, GL_UNSIGNED_BYTE, lpPixels);

		free(lpPixels);												
		ReleaseDC(Wnd, Dc);											
		DeleteObject(hBMP);											
		return (texture);											
	}


	/*---------------------------------------------------------------------------
	Application handler. 
	---------------------------------------------------------------------------*/
	LRESULT CALLBACK OpenGLDemoHandler (HWND Wnd, UINT Msg, WPARAM wParam, LPARAM lParam){

		switch (Msg){
		case WM_CREATE:	{									

															
			HMENU SubMenu, Menu;
			Menu = CreateMenu();							
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

																
			GLDATABASE* db = (GLDATABASE*) malloc(sizeof(GLDATABASE)); 
			db->Rc = InitGL(Wnd);								
			db->glTexture = 0;									
			db->xrot = 0.0f;									
			db->yrot = 0.0f;									
			SetProp(Wnd, DATABASE_PROPERTY, (HANDLE) db);		
			ReSizeGLScene(Wnd);									
		}
					  break;
		case WM_DESTROY: {										
																
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
			case IDC_BMPLOAD: {                              
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
						InvalidateRect(Wnd, 0, TRUE);	
					}
				}
			}
							break;
			case IDC_EXIT:								
				PostMessage(Wnd, WM_CLOSE, 0, 0);		
				break;
			case IDC_TIMERSTART: {						
				SetTimer(Wnd,							
					1,									
					100,								
					0);									
			}
							   break;
			case IDC_TIMERSTOP: {						
				KillTimer(Wnd, 1);						
			}
							  break;
			};
			break;
		case WM_PAINT: {										
			PAINTSTRUCT Ps;
			GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY);
			::BeginPaint(Wnd, &Ps);							
			DrawGLScene(db, Ps.hdc);						
			::SwapBuffers(Ps.hdc);							
			::EndPaint(Wnd, &Ps);							
			return 0;
		}
					 break;
		case WM_TIMER: {											
			GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY);
			db->xrot += 1.0f;								
			db->yrot += 1.0f;								
			InvalidateRect(Wnd, 0, TRUE);							
		}
					 break;
		case WM_WINDOWPOSCHANGED:								
																
			if ((lParam == 0) || ((((PWINDOWPOS) lParam)->flags & SWP_NOSIZE) == 0)){
				ReSizeGLScene(Wnd);									
				InvalidateRect(Wnd, 0, TRUE);						
			}
			break;
		case WM_ERASEBKGND:										
			return (FALSE);
		default: return DefWindowProc(Wnd, Msg, wParam, lParam);
		};// end switch case
		return 0;
	};
}
using namespace GLADPPDemo;

/* ------------------------------------------------------------------------
The application entry point
-----------------------------------------------------------------------*/
int WINAPI GLADPPDemo_RUN(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	RECT R;
	HWND Wnd;
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
	WndClass.lpszClassName = AppClassName;							
	RegisterClassEx(&WndClass);										
	GetClientRect(GetDesktopWindow(), &R);							


	Wnd = CreateWindowEx(0, AppClassName, _T("OpenGL Demo Program"), 
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, R.left+50, R.top+50, 
		R.right-R.left-100, R.bottom-R.top-100,
		0, 0, 0, NULL);					

	while (GetMessage(&Msg, 0, 0, 0)){	
		TranslateMessage(&Msg);			
		DispatchMessage(&Msg);			
	};
	return (0);
}