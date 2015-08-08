
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
#include <vector>

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

/*// Better to use const global variables rather than #defines
// as stated in 'Effective C++' item 2
const GLuint GL_MULTISAMPLE_TWO_AA = 2;
const GLuint GL_MULTISAMPLE_FOUR_AA = 4;
const GLuint GL_MULTISAMPLE_EIGHT_AA = 8;
const GLuint GL_MUTLISAMPLE_SIXTEEN_AA = 16;*/

typedef struct EMultiSampleValue{
	typedef enum Enum
	{
		GL_MULTISAMPLE_TWO = 2,
		GL_MULTISAMPLE_FOUR = 4,
		GL_MULTISAMPLE_EIGHT = 8,
		GL_MULTISAMPLE_SIXTEEN = 16
	};
}MSV;

struct EFullScreenBPP{
	typedef enum Enum
	{
		GL_BPP_EIGHT = 8,
		GL_BPP_SIXTEEN = 16,
		GL_BPP_THIRTYTWO = 32,
	};
};

struct EFullScreenDispModes{
	typedef enum Enum
	{
		CGA,			// 320x200
		QVGA,			// 320x240
		CIF,			// 352x288
		SIF_PAL,		// 384x288
		HVGA,			// 480x320
		VGA,			// 640x480
		SDTV_480i,		// 720x480
		SDTV_576i,		// 720x576
		PAL_LO,			// 768x576
		WVGA,			// 800x480
		SVGA,			// 800x600
		WVGA_NTSC,		// 854x480
		PAL_HI,			// 1024x576
		XGA,			// 1024x768
		RES_1152x768,	// 1152x768
		XGA_PLUS,		// 1152x864
		HD_720,			// 1280x720
		WXGA_LO,		// 1280x768
		WXGA_HI,		// 1280x800
		RES_1280x854,	// 1280x854
		RES_1280x960,	// 1280x960
		SXGA,			// 1280x1024
		RES_1360x768,	// 1360x768
		RES_1366x768,	// 1366x768
		SXGA_PLUS,		// 1400x1050
		RES_1440x900,	// 1440x900
		RES_1440x960,	// 1440x960
		RES_1440x1080,	// 1440x1080
		RES_1600x900,	// 1600x900
		RES_1600x1024,	// 1600x1024
		UXGA,			// 1600x1200
		WSXGA_PLUS,		// 1680x1050
		HD_1080,		// 1920x1080
		WUXGA,			// 1920x1200
		RES_2K,			// 2048x1080
		QXGA,			// 2048x1536
		UWUXGA,			// 2560x1080
		WQHD,			// 2560x1440
		WQXGA,			// 2560x1600
		QSXGA,			// 2560x2048
		UHD_1,			// 3840x2160
		RES_4K,			// 4096x2160
	};
};


class CScreenMode
{
public:
	DWORD bpp, width, height;

public:
	CScreenMode() { bpp = 0;  width = 0;  height = 0; }
	CScreenMode(DWORD bitspp, DWORD sWidth, DWORD sHeight) { bpp = bitspp; width = sWidth, height = sHeight; }

	CScreenMode& set(DWORD bitspp, DWORD sWidth, DWORD sHeight) 
	{ bpp = bitspp; width = sWidth, height = sHeight; return *this; }

	bool operator == (const CScreenMode& mode) { 
		if (bpp == mode.bpp && width == mode.width && height == mode.height) 
			return true; 
		
		return false; 
	}
	CScreenMode& operator = (const CScreenMode& mode) {
		bpp = mode.bpp;
		width = mode.width;
		height = mode.height;
		return *this;
	}
};

// Defines what version of OpenGL Context we will be requesting. These are default values 
// that are used when no version is specified.
const GLuint GL_MAJOR = 3;
const GLuint GL_MINOR = 1;

//#define DEBUG

typedef class CGLWindowsCreation
{
private:
	HGLRC		hRC;				// Permanent Rendering Context
	HGLRC		ehRC;				// extended Rendering Context
	HDC			hDC;				// Private GDI Device Context
	HWND		hWnd;				// Holds our Window Handle
	HINSTANCE	hInstance;			// Holds the Instance of the Application

	int			PixelFormat_;		// Holds the results of a suitable pixelformat match 
	WNDCLASS	wc_;				// Windows class structure
	DWORD		dwExStyle_;			// Windows Extended style
	DWORD		dwStyle_;			// Window Style
//public:
	RECT		WindowRect_;		// RECT that holds the windows width and height
	int			windowXPos_;		// The x-position of the window on the desktop if running in windowed mode
	int			windowYPos_;		// The y-position of the window on the desktop if running in windowed mode
	int			screenWidth_;		// The width of the screen of the primary display monitor in pixels
	int			screenHeight_;		// The height of the screen of the primary display monitor in pixels
	int			screenBits_;		// The bpp of the desktop
	int			fullScreenWidth_;	// The requested width of the full screen resolution
	int			fullScreenHeight_;	// The requested height of the full screen resolution
	int			fullScreenBits_;	// The requested bits per pixel of the full screen resolution

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

	// A vector list that holds graphics card display modes
	std::vector<CScreenMode> displayModes;

	const static std::pair<const int, const int> screenResolutions[];// =
/*	{
		{ 320, 200 }, { 320, 240 }, { 352, 288 }, { 384, 288 }, { 480, 320 }, { 640, 480 },
		{ 768, 576 }, { 800, 480 }, { 800, 600 }, { 854, 480 }, { 1024, 576 }, { 1024, 768 },
		{ 1152, 768 }, { 1152, 864 }, { 1280, 720 }, { 1280, 768 }, { 1280, 800 }, { 1280, 854 },
		{ 1280, 960 }, { 1280, 1024 }, { 1366, 768 }, { 1400, 1050 }, { 1440, 900 },
		{ 1440, 960 }, { 1440, 1080 }, { 1600, 900 }, { 1600, 1200 }, { 1680, 1050 },
		{ 1920, 1080 }, { 1920, 1200 }, { 2048, 1080 }, { 2048, 1536 }, { 2560, 1080 },
		{ 2560, 1440 }, { 2560, 1600 }, { 2560, 2048 }, { 3840, 2160 }, { 4096, 2160 }
	};*/


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
	//CGLWindowsCreation(int winWidth, int winHeight, int windowXPos, int windowYPos, int bits);
	
	// function to set the version of OpenGL you wish to use
	bool SetGLVersion(GLuint major, GLuint minor);

	// function to set the bits per pixel of the fullscreen resolution
	void SetFullscreenBPP(EFullScreenBPP::Enum bpp) { fullScreenBits_ = bpp; }

	// function to set the bpp and resolution of the fullscreen mode
	bool SetFullScreenMode(EFullScreenBPP::Enum bpp, EFullScreenDispModes::Enum res);

	// Setup file used by all constructors
	void SetupWindows(int winWidth, int winHeight);

	// Function that finds all resolutions supported by the graphics card and picks the appropriate windowed mode
	void WindowedResolution(int &winWidth, int &winHeight);

	// Gets the coordinates of the taskbar for use in calculating window position
	bool GetTaskBarRect(void);

	// Function to create the window
	bool CreateGLWindow(TCHAR *title, bool fullscreenflag);

	// Overloaded Function to create a multisampled window; calls CreateGlWindow ultimately
	bool CreateGLWindow(TCHAR *title, bool fullscreenflag, EMultiSampleValue::Enum multisampling);


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

	void findDisplayModes();
	// Initialises the multisampling variables for all the constructors
	inline void InitialiseMultisampling() { multisample_ = false; multisampleSupported_ = false; antiAliasLevel_ = EMultiSampleValue::GL_MULTISAMPLE_FOUR; }

	// Initialises the version number we need for all the constructors
	inline void InitialiseVersion() { glMajor = GL_MAJOR; glMinor = GL_MINOR; }


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
