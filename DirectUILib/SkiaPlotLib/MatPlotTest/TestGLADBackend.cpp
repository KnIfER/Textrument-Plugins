
#include <matplot/matplot.h>
#include <matplot/core/figure_registry.h>

#define _WIN32_WINNT 0x0500
#include <windows.h>	
#include <tchar.h>		
#include <commctrl.h>	
#include <glad/glad.h>
#include <gl\glu.h>		

#pragma comment(lib,"ComCtl32.lib")
#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")


using namespace matplot;

using namespace matplot;

namespace GLAPPDemo{
#define IDC_BMPLOAD 101			
#define IDC_EXIT 105			
#define IDC_TIMERSTART 201		
#define IDC_TIMERSTOP 202	

	class Plotter : public backend::backend_interface{
	public:
		GLuint draw_2d_single_color_shader_program_;

		Plotter() { 
			create_shaders(); 
		}

		Plotter(bool create_shaders_at_construction) {
			if (create_shaders_at_construction) {
				create_shaders();
			}
		}

		void create_shaders() {
			// Create shaders
			const char *draw_2d_single_color_vertex_shader_source =
				"#version 330 core\n"
				"layout (location = 0) in vec2 aPos;\n"
				"uniform float windowHeight;\n"
				"uniform float windowWidth;\n"
				//"out vec4 vertexColor;\n"
				"void main()\n"
				"{\n"
				"   gl_Position = vec4((aPos.x/windowWidth)*2-1, (aPos.y/windowHeight)*2-1, 0.0, 1.0);"
				//"   vertexColor = aColor;\n"
				"}\0";
			unsigned int draw_2d_single_color_vertex_shader;
			draw_2d_single_color_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(draw_2d_single_color_vertex_shader, 1, &draw_2d_single_color_vertex_shader_source, NULL);
			glCompileShader(draw_2d_single_color_vertex_shader);
			int success;
			char info_log[512];
			glGetShaderiv(draw_2d_single_color_vertex_shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(draw_2d_single_color_vertex_shader, 512, NULL, info_log);
				throw std::runtime_error(
					std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") +
					info_log);
			}

			// create and compile fragment shader
			const char *draw_2d_single_color_fragment_shader_source =
				"#version 330 core\n"
				"out vec4 FragColor;\n"
				"\n"
				"uniform vec4 ourColor;\n"
				"\n"
				"void main()\n"
				"{\n"
				"    FragColor = ourColor;\n"
				"}";
			unsigned int draw_2d_single_color_fragment_shader;
			draw_2d_single_color_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(draw_2d_single_color_fragment_shader, 1, &draw_2d_single_color_fragment_shader_source, NULL);
			glCompileShader(draw_2d_single_color_fragment_shader);
			glGetShaderiv(draw_2d_single_color_fragment_shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(draw_2d_single_color_fragment_shader, 512, NULL, info_log);
				throw std::runtime_error(
					std::string("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n") +
					info_log);
			}

			// Link shaders into shader program
			draw_2d_single_color_shader_program_ = glCreateProgram();
			glAttachShader(draw_2d_single_color_shader_program_, draw_2d_single_color_vertex_shader);
			glAttachShader(draw_2d_single_color_shader_program_, draw_2d_single_color_fragment_shader);
			glLinkProgram(draw_2d_single_color_shader_program_);
			// check if linking was successful
			glGetProgramiv(draw_2d_single_color_shader_program_, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(draw_2d_single_color_shader_program_, 512, NULL, info_log);
				throw std::runtime_error(
					std::string("ERROR::SHADER_PROGRAM::LINKING_FAILED\n") +
					info_log);
			}

			// Delete the shader objects
			glDeleteShader(draw_2d_single_color_vertex_shader);
			glDeleteShader(draw_2d_single_color_fragment_shader);

			//        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_vertex_attributes_available_);
			//        std::cout << "Maximum number of vertex attributes supported: " << n_vertex_attributes_available_ << std::endl;
		}

		~Plotter() {
			glDeleteProgram(draw_2d_single_color_shader_program_);
		}

		//bool consumes_gnuplot_commands() override { return false; } 

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
			// Create and bind vertex array object
			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			// Copy vertex data into the buffer's memory
			std::vector<float> vertices = {
				// x, y, z, r, g, b
				static_cast<float>(x2), static_cast<float>(y2), // top right
				static_cast<float>(x2), static_cast<float>(y1), // bottom right
				static_cast<float>(x1), static_cast<float>(y1), // bottom left
				static_cast<float>(x1), static_cast<float>(y2)  // top left
			};

			// Create and bind vertex buffer object
			unsigned int VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

			std::vector<unsigned int> indices = {  // note that we start from 0!
				0, 1, 3,   // first triangle
				1, 2, 3    // second triangle
			};

			// Element buffer
			unsigned int EBO;
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

			// Set the vertex attributes pointers
			int vertex_attribute_location = 0;
			size_t stride = 2 * sizeof(float);
			glVertexAttribPointer(vertex_attribute_location, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), (void *)0);
			glEnableVertexAttribArray(0);

			// Set the color attribute pointers
			// int color_attribute_location = 1;
			// glVertexAttribPointer(color_attribute_location, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3*sizeof(float)));
			// glEnableVertexAttribArray(1);

			// Activate our shader program
			glUseProgram(draw_2d_single_color_shader_program_);

			// Set window size
			int windowHeightLocation = glGetUniformLocation(draw_2d_single_color_shader_program_, "windowHeight");
			if (windowHeightLocation == -1) {
				throw std::runtime_error("can't find uniform location");
			}
			glUniform1f(windowHeightLocation, static_cast<float>(height()));

			int windowWidthLocation = glGetUniformLocation(draw_2d_single_color_shader_program_, "windowWidth");
			if (windowWidthLocation == -1) {
				throw std::runtime_error("can't find uniform location");
			}
			glUniform1f(windowWidthLocation, static_cast<float>(width()));

			// Set color
			int vertexColorLocation = glGetUniformLocation(draw_2d_single_color_shader_program_, "ourColor");
			if (vertexColorLocation == -1) {
				throw std::runtime_error("can't find uniform location");
			}
			glUniform4f(vertexColorLocation, color[1], color[2], color[3], 1.f-color[0]);

			// Bind element buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

			// Unbind our vertex array
			glBindVertexArray(0);

			glDeleteBuffers(1, &EBO);
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}

		void draw_background(const std::array<float, 4> &color) override {
			glClearColor(color[1], color[2], color[3], 1.f - color[0]);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void show(class matplot::figure_type *f) override {
			backend_interface::show(f);
		}

		bool supports_fonts() override { return false; }

		void draw_path(const std::vector<double> &x,
			const std::vector<double> &y,
			const std::array<float, 4> &color) override {
			// Copy vertex data into the buffer's memory
			std::vector<float> vertices;
			for (size_t i = 0; i < x.size(); ++i) {
				vertices.emplace_back(static_cast<float>(x[i]));
				vertices.emplace_back(static_cast<float>(y[i]));
			}

			// Create and bind vertex array object
			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			// Create and bind vertex buffer object
			unsigned int VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

			// Set the vertex attributes pointers
			int vertex_attribute_location = 0;
			size_t stride = 2 * sizeof(float);
			glVertexAttribPointer(vertex_attribute_location, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(stride), (void *)0);
			glEnableVertexAttribArray(0);

			// Activate our shader program
			glUseProgram(draw_2d_single_color_shader_program_);

			// Set window size
			int windowHeightLocation = glGetUniformLocation(draw_2d_single_color_shader_program_, "windowHeight");
			if (windowHeightLocation == -1) {
				throw std::runtime_error("can't find uniform location");
			}
			glUniform1f(windowHeightLocation, static_cast<float>(height()));

			int windowWidthLocation = glGetUniformLocation(draw_2d_single_color_shader_program_, "windowWidth");
			if (windowWidthLocation == -1) {
				throw std::runtime_error("can't find uniform location");
			}
			glUniform1f(windowWidthLocation, static_cast<float>(width()));

			// Set color
			int vertexColorLocation = glGetUniformLocation(draw_2d_single_color_shader_program_, "ourColor");
			if (vertexColorLocation == -1) {
				throw std::runtime_error("can't find uniform location");
			}
			glUniform4f(vertexColorLocation, color[1], color[2], color[3], 1.f-color[0]);

			// Bind element buffer
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(x.size()));

			// Unbind our vertex array
			glBindVertexArray(0);

			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}

		void draw_markers(const std::vector<double> &x,
			const std::vector<double> &y,
			const std::vector<double> &z) override {
			throw std::logic_error("draw_markers not implemented yet");
		}

		void draw_text(const std::vector<double> &x,
			const std::vector<double> &y,
			const std::vector<double> &z) override {
			throw std::logic_error("draw_text not implemented yet");
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
		HGLRC ourOpenGLRC = 0;					 
												 
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

		if(_plotter) {
			_plotter->framebuffer_size_callback(0, Width, Height);
		} else {			
			glViewport(0, 0, Width, Height);
		}

		glMatrixMode(GL_PROJECTION);									
		glLoadIdentity();												
																		
		gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);									
		glLoadIdentity();											
		ReleaseDC(Wnd, Dc);											
	}

	void DrawGLScene(GLDATABASE* db, HDC Dc) {
		if ((db == 0)) return;
		wglMakeCurrent(Dc, db->Rc);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		ylabel("Y Axis");

		// Draw the figure
		_figure->draw();
	}

	LRESULT CALLBACK GLPlotProc (HWND Wnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch (Msg){
			case WM_PAINT: {										
				PAINTSTRUCT Ps;
				GLDATABASE* db = (GLDATABASE*) GetProp(Wnd, DATABASE_PROPERTY);
				::BeginPaint(Wnd, &Ps);							
				DrawGLScene(db, Ps.hdc);						
				::SwapBuffers(Ps.hdc);							
				::EndPaint(Wnd, &Ps);
				//::InvalidateRect(Wnd, NULL, false);
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
			}  break;
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
using namespace GLAPPDemo;

/* ------------------------------------------------------------------------
The application entry point
-----------------------------------------------------------------------*/
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
		, _T("OpenGL Demo Program"), 
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

//int main() {
//
//
//    //auto f = figure<Plotter>(true);
//    //auto ax = f->current_axes();
//    //
//    //figure(f);
//    //
//    //auto x = transform(iota(1, 10), [](double x) { return pow(x, 2); });
//    //
//    //plot(x);
//    //
//    ////ylabel("Y Axis");
//    //
//    //show();
//
//    return 0;
//}