#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glext.h>
#include <gl/wglext.h>


#include <strsafe.h>
#include <iostream>
#include "arb_multisample.h"

// Declarations We'll Use
#define WGL_SAMPLE_BUFFERS_ARB	0x2041
#define WGL_SAMPLES_ARB			0x2042

bool arbMultisampleSupported = false;
int arbMultisampleFormat = 0;

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}


bool WGLisExtensionSupported(const char*extension)
{
	const size_t extlen = strlen(extension);
	const char *supported = NULL;

	// Try To Use wglGetExtensionStringARB On Current DC, If Possible
	// The variable wglGetExtString is a pointer to a function that is retrieved
	// using the function wglGetProcAddress. It returns a pointer to the function
	// with the same name as that given in the string.

	
	//PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtString;

	// the const char* is the return argument of the function we wish to point to
	// WINAPI is just a calling function
	//const char* (WINAPI *wglGetExtString)(HDC hdc);

	// PROC is defined as: typedef int (_stdcall *PROC)(); in C function pointer
	// declarations are of the form: int (*ptr)(); 
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

	// Note that wglGetExtString is a pointer to a function 
	//wglGetExtString = (const char *(WINAPI *)(HDC)) wglGetProcAddress("wglGetExtensionsStringARB");

	if(wglGetExtString)
	{
		// we are now using the pointer to the function that we obtained in the
		// command above to call the function through the pointer
		// note the syntax 
		
		supported = ((char*(WINAPI *)(HDC))wglGetExtString) (wglGetCurrentDC());
		//supported = wglGetExtString(wglGetCurrentDC());

//		std::cout << supported << std::endl;

		MessageBox(NULL, TEXT("true"), TEXT("wglGetExtString"), MB_OK);
	}
	//else 
		//MessageBox(NULL, TEXT("false"), TEXT("wglGetExtString"), MB_OK);
	
	// If that failed, try standard OpenGL Extensions String
	if (supported == NULL)
	{
		MessageBox(NULL, TEXT("false"), TEXT("wglGetExtString"), MB_OK);
		ErrorExit(TEXT("error:"));
		supported = (char*)glGetString(GL_EXTENSIONS);
	}

	// If that failed too, must be no extensions supported
	if (supported == NULL)
	{
		MessageBox(NULL, TEXT("false"), TEXT("glGetString"), MB_OK);
		return false;
	}

	std::cout << extension << std::endl;

	// Begin examination at start of the string, increment by 1 on false match

	int i = 0;

	for (const char* p = supported; ;p++, i++)
	{
		
		// Advance p up to the next possible match
		p = strstr(p, extension);

		std::cout << p << " " << i << std::endl;

		
		if (p == NULL)
		{
			MessageBox(NULL, TEXT("NULL"), TEXT("p"), MB_OK);
			return false;			// No match
		}

		// Make sure that match is at the start of the string or that 
		// the previous char is a space, or else we could accidently match
		// "wglFunkywglExtension" with "wglExtension"

		// Also, make sure that the following character is space of null
		// or else "wglExtensionTwo" might match "wglExtension"

		if ((p == supported || p[-1] == ' ') && (p[extlen] == '\0' || p[extlen] == ' '))
			return true;						// match
	}
}

bool InitMultisample(HINSTANCE hInstance, HWND hWnd, PIXELFORMATDESCRIPTOR pfd)
{
	// See if the string exists in WGL!
	if (!WGLisExtensionSupported("WGL_ARB_multisample"))
	{
		MessageBox(NULL, TEXT("false"), TEXT("WGLExtensionSupported()"), MB_OK);

		arbMultisampleSupported = false;
		return false;
	}

	// Get our Pixel Format
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	if (!wglChoosePixelFormatARB)
	{
		// We didn't find support for multisampling, set our flag and exit out.
		arbMultisampleSupported = false;
		return false;
	}

	// Get our current device context. We need this in order to ask the openGL window
	// what attributes we have
	HDC hDC = GetDC(hWnd);

	int pixelFormat;
	bool valid;
	UINT numFormats;
	float fAttributes[] = {0,0};

	// These attributed are the bits we want to test for in our sample
	// everything is pretty standard, the only one we want to really
	// focus on is the SAMPLE BUFFERS ARB and WGL SAMPLES. These two are 
	// going to do the main testing for whether or not we support
	// Multisampling on this hardware
	
	int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 24,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 16,
		WGL_STENCIL_BITS_ARB, 0,
		WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,				// Check for 4x Multisampling
		0, 0};

	// First we check to see if we can get a pixel format for 4 samples
	valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat,
		&numFormats);

	// If we returned true, and out format count is greater than 1
	if (valid && numFormats >= 1)
	{
		arbMultisampleSupported = true;
		arbMultisampleFormat = pixelFormat;
		return arbMultisampleSupported;
	}

	// Our pixel format with 4 samples failed, test for 2 samples
	iAttributes[19] = 2;
	valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

	if (valid && numFormats >= 1)
	{
		arbMultisampleSupported = true;
		arbMultisampleFormat = pixelFormat;
		return arbMultisampleSupported;
	}

	// Return the valid format
	return arbMultisampleSupported;
}
