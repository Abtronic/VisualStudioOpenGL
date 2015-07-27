
#ifndef _ARB_MULTISAMPLE_
#define _ARB_MULTISAMPLE_

#include <gl/wglext.h>		// WGL extensions
#include <gl/glext.h>		// GL extensions
#include <gl/glext.h>

// Globals
extern bool arbMultisampleSupported;
extern int arbMultisampleFormat;

// if you don't want multisampling, set this to 0
#define CHECK_FOR_MULTISAMPLE 1

// convenience for calling the wglGetExtensionsStringARB function
typedef const char*(_stdcall * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);

// to check for our sampling
bool InitMultisample(HINSTANCE hInstance, HWND hWnd, PIXELFORMATDESCRIPTOR pfd);

#endif
