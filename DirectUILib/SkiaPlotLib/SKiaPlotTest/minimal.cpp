// Triangle.cpp
// Our first OpenGL program that will just draw a triangle on the screen.

//#include "esGLUT/esGLUT.h"
//#include "esGLUT/GLES2/gl2.h"
//#include "esGLUT/GLES2/gl2ext.h"
//#include "esGLUT/GLES2/gl2platform.h"

#include <windows.h>

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "GLES2/gl2platform.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "EGL/eglplatform.h"

// https://community.khronos.org/t/egl-and-sdl-context-creation/1110


#include <windows.h>			/* must include this before GL/gl.h */
//#include <GL/gl.h>			/* OpenGL header file */
//#include <GL/glu.h>			/* OpenGL utilities header file */
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

///////
GLuint BackBuffer;
GLuint Width;
GLuint Height;
GLuint BPP;
GLuint ZBufferDepth;
GLuint StencilDepth;

EGLDisplay Display = 0;
EGLContext Context = 0;
EGLConfig Config = 0;
EGLSurface Surface = 0;
HWND Handle;
HDC Device;
//////




///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader(GLenum type, const char* shaderSrc) {
    GLuint shader;
    GLint compiled;
    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0) {
        return 0;
    }
    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            //esLogMessage("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


GLuint _programObject;

// Initialize the shader and program object
//
int Init() {
    char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 vPosition;  \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vPosition;              \n"
        "}                                        \n";
    char fShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
        "}                                            \n";
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);
    // Create the program object
    programObject = glCreateProgram();
    if (programObject == 0) {
        return 0;
    }
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    // Link the program
    glLinkProgram(programObject);
    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            //esLogMessage("Error linking program:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(programObject);
        return FALSE;
    }

    // Store the program object
    _programObject = programObject;

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return TRUE;
}


void display() {
    /* rotate a triangle around */
    // glClear(GL_COLOR_BUFFER_BIT);
    //glFlush();
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    //UserData* userData = esContext->userData;
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f};


    glViewport(0, 0, Width, Height);

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Use the program object
    glUseProgram(_programObject);

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);


    glFlush();

    eglSwapBuffers(Display, Surface);
}

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static PAINTSTRUCT ps;

    switch (uMsg) {
    case WM_PAINT:
        if (!::IsIconic(hWnd)) {
            display();
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            return 0;
        }
        break;
    case WM_SIZE:
        RECT rc;
        ::GetClientRect(hWnd, &rc);
        glViewport(0, 0, Width = rc.right, Height = rc.bottom);
        PostMessage(hWnd, WM_PAINT, 0, 0);
        //InvalidateRect(hWnd, NULL, FALSE);
        return 0;

    case WM_CHAR:
        switch (wParam) {
        case 27: /* ESC key */
            PostQuitMessage(0);
            break;
        }
        return 0;

    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateOpenGLWindow(char* title, int x, int y, int width, int height, BYTE type, DWORD flags) {
    int pf;
    HDC hDC;
    HWND hWnd;
    WNDCLASS wc;
    PIXELFORMATDESCRIPTOR pfd;
    static HINSTANCE hInstance = 0;

    /* only register the window class once - use hInstance as a flag. */
    if (!hInstance) {
        hInstance = GetModuleHandle(NULL);
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = (WNDPROC)WindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "OpenGL";

        if (!RegisterClass(&wc)) {
            MessageBox(NULL,
                "RegisterClass() failed:  "
                "Cannot register window class.",
                "Error",
                MB_OK);
            return NULL;
        }
    }

    hWnd = CreateWindow("OpenGL",
        title,
        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        x,
        y,
        width,
        height,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.", "Error", MB_OK);
        return NULL;
    }

    hDC = GetDC(hWnd);

    /* there is no guarantee that the contents of the stack that become
    the pfd are zeroed, therefore _make sure_ to clear these bits. */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
    pfd.iPixelType = type;
    pfd.cColorBits = 32;

    pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0) {
        MessageBox(NULL,
            "ChoosePixelFormat() failed:  "
            "Cannot find a suitable pixel format.",
            "Error",
            MB_OK);
        return 0;
    }

    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
        MessageBox(NULL,
            "SetPixelFormat() failed:  "
            "Cannot set format specified.",
            "Error",
            MB_OK);
        return 0;
    }

    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    ReleaseDC(hWnd, hDC);

    return hWnd;
}

const EGLint SurfaceAttribs[] = {EGL_RENDER_BUFFER, EGL_BACK_BUFFER, EGL_NONE};

const EGLint ConfigAttribs[] = {EGL_LEVEL,
0,
EGL_SURFACE_TYPE,
EGL_WINDOW_BIT,
EGL_RENDERABLE_TYPE,
EGL_OPENGL_ES2_BIT,
EGL_NATIVE_RENDERABLE,
EGL_FALSE,
EGL_DEPTH_SIZE,
EGL_DONT_CARE,
EGL_NONE};
EGLint attribList[] =
{
    EGL_RED_SIZE,       5,
    EGL_GREEN_SIZE,     6,
    EGL_BLUE_SIZE,      5,
    EGL_ALPHA_SIZE,     EGL_DONT_CARE,
    EGL_DEPTH_SIZE,     8,
    EGL_STENCIL_SIZE,   EGL_DONT_CARE,
    EGL_SAMPLE_BUFFERS, 1,
    // if EGL_KHR_create_context extension is supported, then we will use
    // EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT in the attribute list
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
};

int APIENTRY WinMaxin(HINSTANCE hCurrentInst,
    HINSTANCE hPreviousInst,
    LPSTR lpszCmdLine,
    int nCmdShow) {

    HDC hDC;   /* device context */
    HGLRC hRC; /* opengl context */
    HWND hWnd; /* window */
    MSG msg;   /* message */

    hWnd = CreateOpenGLWindow("minimal", 0, 0, 256, 256, PFD_TYPE_RGBA, 0);
    //if (hWnd == NULL) exit(1);

    hDC = GetDC(hWnd);

    BackBuffer = 0;
    Width = 800;
    Height = 480;
    BPP = 32;
    ZBufferDepth = 0;
    StencilDepth = 0;

    EGLint nConfigs;

    Handle = hWnd;
    HDC Device;

    eglBindAPI(EGL_OPENGL_ES_API);

    Device = GetDC(Handle);
    Display = eglGetDisplay(Device);


    EGLint majorVersion;
    EGLint minorVersion;

    if (Display == EGL_NO_DISPLAY) {
        throw runtime_error("Display Initialize failed 1!!!");
    }
    if (!eglInitialize(Display, &majorVersion, &minorVersion)) {
        throw runtime_error("Display Initialize failed 2!!!");
    }

    if (!eglChooseConfig(Display, ConfigAttribs, &Config, 1, &nConfigs)) {
        throw runtime_error("Configuration failed!!!");
    } 
    // eglGetConfigAttrib(Display, Config, EGL_NATIVE_VISUAL_ID, &format);


    if (nConfigs != 1) {
        throw runtime_error("Configuration failed!!!");
    }

    Surface = eglCreateWindowSurface(Display, Config, Handle, NULL);

    // if (Surface == EGL_NO_SURFACE) {
    //     //EGLErrorString();
    //     Surface = eglCreateWindowSurface(Display, Config, NULL, NULL);
    //     if (Surface == EGL_NO_SURFACE) {
    //        // PRINT_ERROR("Surface Creation failed: %s", EGLErrorString());
    //     }
    // }

    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    EGLint GiveMeGLES2[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    Context = eglCreateContext(Display, Config, EGL_NO_CONTEXT, contextAttribs);

    if (Context == EGL_NO_CONTEXT) {
        throw runtime_error("EGL_NO_CONTEXT!!!");
    }


    bool succ = eglMakeCurrent(Display, Surface, Surface, Context);
    if (!succ) {
        throw runtime_error("Make Current failed!!!");
    };

    if (!Init()) {
        throw runtime_error("Init Shader failed!!!");
    };

    ShowWindow(hWnd, nCmdShow);

    while (GetMessage(&msg, hWnd, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //eglMakeCurrent(NULL, NULL);

    ReleaseDC(hWnd, hDC);
    //wglDeleteContext(hRC);
    DestroyWindow(hWnd);

    return msg.wParam;
}