
// The ifndef to #endif block is only included as code when 
// _WINDOWS_WRAPPER_ is not defined. If it is defined then 
// the code in not included. This makes sure the same header 
// file is not included multiple times.
#ifndef _WINDOWS_WRAPPER_
#define _WINDOWS_WRAPPER_

#include <windows.h>
#include <gl\glew.h>		// The OpenGL Extension Wrangler Library
#include <gl\wglew.h>		// Supports WGL extensions
#include <gl\gl.h>
#include <gl\glu.h>

#include <gl\wglext.h>		// WGL extensions
#include <gl\glext.h>		// GL extensions

#include <iostream>
#include <string>

//#include "arb_multisample.h"

// Function declarations so that the class members can use the functions 
// defined in windowMain.cpp

// declaration for the windproc function in windowMain.cpp
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Function to resize and initialise the GL window
GLvoid ReSizeGLScene(GLsizei width, GLsizei height);	

// Initialisation of a new OpenGL Window
int InitGL(GLvoid);

// MessageBox with printf style formatting
int CDECL MessageBoxPrintf (TCHAR * szCaption, TCHAR * szFormat, ...);

// Function to draw the OpenGL scene
int DrawGLScene(GLvoid);

// Function where certain keys can be tested to see if they were pressed
void KeyboardInput(void);

// The user of this class uses the following constants to define 
// the type of anti-aliasing to use. It is passed to the class in the 
// CreateGLWindow function.
/*#define GL_MULTISAMPLE_TWO_AA 2
#define GL_MULTISAMPLE_FOUR_AA 4
#define GL_MULTISAMPLE_EIGHT_AA 8
#define GL_MUTLISAMPLE_SIXTEEN_AA 16*/

// Better to use const global variables rather than #defines
// as stated in 'Effective C++' item 2
const GLuint GL_MULTISAMPLE_TWO_AA = 2;
const GLuint GL_MULTISAMPLE_FOUR_AA = 4;
const GLuint GL_MULTISAMPLE_EIGHT_AA = 8;
const GLuint GL_MUTLISAMPLE_SIXTEEN_AA = 16;

typedef struct MultiSampleValue{
	typedef enum Enum
	{
		GL_MULTISAMPLE_TWO = 2,
		GL_MULTISAMPLE_FOUR = 4,
		GL_MULTISAMPLE_EIGHT = 8,
		GL_MULTISAMPLE_SIXTEEN = 16
	};
}MSV;

// Defines what version of OpenGL Context we will be requesting. These are default values 
// that are used when no version is specified.
const GLuint GL_MAJOR = 3;
const GLuint GL_MINOR = 1;

typedef class CGLWindowsCreation
{
	HGLRC		hRC;				// Permanent Rendering Context
	HGLRC		ehRC;				// extended Rendering Context
	HDC			hDC;				// Private GDI Device Context
	HWND		hWnd;				// Holds our Window Handle
	HINSTANCE	hInstance;			// Holds the Instance of the Application

	int			PixelFormat_;		// Holds the results of a suitable pixelformat match 
	WNDCLASS	wc_;				// Windows class structure
	DWORD		dwExStyle_;			// Windows Extended style
	DWORD		dwStyle_;			// Window Style
	GLuint		glMajorVersion;		// The major version number of the OpenGL Context we want
	GLuint		glMinorVersion;		// The minor version number of the OpenGL Context we want
public:
	RECT		WindowRect_;		// RECT that holds the windows width and height
	int			windowXPos_;		// The x-position of the window on the desktop if running in windowed mode
	int			windowYPos_;		// The y-position of the window on the desktop if running in windowed mode
	int			screenWidth_;		// The width of the screen of the primary display monitor in pixels
	int			screenHeight_;		// The height of the screen of the primary display monitor in pixels

	int			screenBits_;		// The bpp of the desktop
	bool		fullscreen_;		// Stores true or false for fullscreen window or not
	bool		multisample_;		// Stores true or false for multisampling required or not
	bool		multisampleSupported_;	// Stores true or false for multisampling supported or not
	bool		glewInitialised_;	// Stores true or false for glew being initialised or not
	GLuint		antiAliasLevel_;	// Stores the level of anti-aliasing required
	int			arbMultisampleFormat_;	// Stores the multisample pixelformat
	int			extendedGLFormat_;	// Stores the extended OpenGL pixelformat
	RECT		TaskBarRect_;		// Stores the coordinates of the task bar
	bool		showWindowFlag_;	// Used to stop the WM_MOVE message modifying the window position
									// when ShowWindow is called
	GLuint		glMajor;			// Stores the major version number of OpenGL we need
	GLuint		glMinor;			// Stores the minor version number of OpenGL we need




public:
	bool keys[256];
	bool active;
//	bool lp;						// Boolean variable to show if L is being pressed
//	bool fp;						// Boolean variable to show if F is being pressed
//	bool bp;						// Boolean variable to show if B is being pressed
	

	// Constructor which takes no arguments
	CGLWindowsCreation(void);
	// Constructor where you can also specify the position of the window and width and height
	CGLWindowsCreation(int winWidth, int winHeight, int windowXPos, int windowYPos);
	// Constructor where you can also specify the bit depth of the window
	CGLWindowsCreation(int winWidth, int winHeight, int windowXPos, int windowYPos, int bits);
	
	// function to set the version of OpenGL you wish to use
	bool SetOGLVersion(GLuint major, GLuint minor);
	// Setup file used by all constructors
	void SetupWindows(int winWidth, int winHeight);

	// Initialises the multisampling variables for all the constructors
	inline void InitialiseMultisampling() {multisample_ = false; multisampleSupported_ = false; antiAliasLevel_ = GL_MULTISAMPLE_FOUR_AA;}
	
	// Initialises the version number we need for all the constructors
	inline void InitialiseVersion() { glMajor = GL_MAJOR; glMinor = GL_MINOR; }

	// Function that finds all resolutions supported by the graphics card and picks the appropriate windowed mode
	void WindowedResolution(int &winWidth, int &winHeight);

	// Gets the coordinates of the taskbar for use in calculating window position
	bool GetTaskBarRect(void);

	// Function to create the window
	bool CreateGLWindow(TCHAR *title, bool fullscreenflag);

	// Overloaded Function to create a multisampled window; calls CreateGlWindow ultimately
	bool CreateGLWindow(TCHAR *title, bool fullscreenflag, MultiSampleValue::Enum multisampling);

	// Function to kill the window
	void KillGLWindow(void);

	// Function to handle Window Messages
	int WindowsMessages(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Function of the main windows message loop
	int MessageLoop(void);

private:

	// Function to initialise Multisampling
	bool InitMultisample(const PIXELFORMATDESCRIPTOR &pfd);
	bool IsWGLExtensionSupported(const char *extension);


}CGLWindows;

#endif


/*	Program Flow

Constructor CGLWindowsCreation
	Taskbar Rect is found
		GetTaskBarRect
	Windows X, Y pos is defined due to the taskbar position
	Finds the best window resolution
		WindowedResolution
	Sets up the Fullscreen window resolution in case fullscreen is used
		SetupWindows
End Constructor

*/
