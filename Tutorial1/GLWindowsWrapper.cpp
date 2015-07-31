#include "GLWindowsWrapper.h"

//int z = 0;

int CGLWindowsCreation::WindowsMessages(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)			// Check for Windows messages
	{
	case WM_ACTIVATE:		// Watch for the Window Activate message
		{
			if (!HIWORD(wParam))			// Check minimisation state
			{
				active = true;				// Program is active
			}
			else
			{
				active = false;				// Program is no longer active
			}

			return 0;						// Return to the message loop
		}
	case WM_SYSCOMMAND:						// Intercept System Commands
		{
			switch(wParam)					// Check which system call it is
			{
			case SC_SCREENSAVE:				// Is the screensaver trying to start?
			case SC_MONITORPOWER:			// Is the monitor trying to enter powersave?
				return 0;					// Prevent from happening by returning zero
			}
			break;							// Exit switch statement
		}
	case WM_CLOSE:							// If we recieve a close message
		{
			PostQuitMessage(0);				// Send a quit message
			return 0;						// Return to message loop
		}
	case WM_KEYDOWN:						// If a key is being held down
		{
			keys[wParam]= true;			// Mark the corresponding keys array entry as true
			return 0;						// Jump back to message loop
		}
	case WM_KEYUP:							// If a key is released
		{
			keys[wParam] = false;			// Mark the corresponding keys array entry as false
			return 0;						// Jump back to the message loop
		}
	case WM_SIZE:							// The window has been resized
		{
			ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));	// LOWORD = width, HIWORD = height
			if (!fullscreen_)
			{
				WindowRect_.right = LOWORD(lParam);
				WindowRect_.bottom = HIWORD(lParam);
			}
			return 0;										// Jump back to the message loop
		}
	case WM_MOVE:
		{
			if (!fullscreen_)
			{
				RECT rect;
				GetWindowRect(hWnd, &rect);
				windowXPos_ = rect.left;
				windowYPos_ = rect.top;
			}
			return 0;
		}
	}

	return 1;
}


int CGLWindowsCreation::MessageLoop(void)
{
	MSG msg;						// Windows Message structure
	bool done = false;				// variables used to exit the message loop

	while(!done)					// while done is false
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// If there is a message waiting to be processed
		{
			if (msg.message == WM_QUIT)	// If the message is a quit message
			{
				done = true;			// we are done with the loop so done is true
			}
			else						// Otherwise we need to deal with the windows message
			{
				TranslateMessage(&msg);		// Translate the message
				DispatchMessage(&msg);		// Dispatch the message, i.e. go to WndProc to deal with it
			}
		}
		else					// If there isn't a message waiting
		{
			// Draw the scene and also watch for the ESC key and quit message from DrawGLScene()
			if (active)						// If the program is active i.e. not minimised
			{
				if (keys[VK_ESCAPE])		// If ESC was pressed
				{
					done = true;			// we are done with the loop so done is true
				}
				else						// if not then 
				{
					DrawGLScene();			// Draw the Scene
					SwapBuffers(hDC);		// Swap Buffers (Double Buffering)
					/*if (keys['L'] && !lp)	// if the L key is being pressed and not held
					{
						lp = true;			// L is being pressed therefore lp is true
						light = !*/
					KeyboardInput(); // function in the main file that handles any input from the keyboard
				}
				if (keys[VK_F1])			// If F1 is being pressed
				{
					keys[VK_F1] = false;	// if so make key false so it can be pressed again
					KillGLWindow();			// Kill our current Window
					fullscreen_ = !fullscreen_;	// Toggle the fullscreen/windowed mode
					
					//std::cout << "F1 pressed " << windowXPos_ << ", " << windowYPos_ << std::endl;
					// Recreate our OpenGL window 
					if(!CreateGLWindow(TEXT("C++ OpenGL Window Class"), fullscreen_))
					{
						return 0;			// if the Window is not created then quit the program
					}
				}
				if (keys[VK_F3])
				{
					keys[VK_F3] = false;
					RECT winRect;
					GetWindowRect(hWnd, &winRect);
					std::cout << "window rect " << winRect.left << " " << winRect.top << std::endl;
					std::cout << "window Pos  " << windowXPos_ << " " << windowYPos_ << std::endl;

				}
				if (keys[VK_F4])
				{
					keys[VK_F4] = false;
					std::cout << "Anti-Aliasing Level: " << antiAliasLevel_ << std::endl;
				}
			}
		}
	}

	// Once we exit the loop we shut the window down and exit
	KillGLWindow();				// Kill the Window

	return (msg.wParam);
}



// Constructor	
CGLWindowsCreation::CGLWindowsCreation(void)
{

	GetTaskBarRect();

	InitialiseMultisampling();
	InitialiseVersion();

	int width = GetSystemMetrics(SM_CXSCREEN);	// Gets the screenwidth in pixels of the primary display monitor
	int height = GetSystemMetrics(SM_CYSCREEN);	// Gets the screenheight in pixels of the primary display monitor
	int winWidth, winHeight;


 
	if (TaskBarRect_.top > 0)
	{
		//winWidth = width;
		//winHeight = TaskBarRect_.top;
		windowXPos_ = 0; windowYPos_ = 0;
	}
	else if (TaskBarRect_.left > 0)
	{
		//winWidth = TaskBarRect_.left;
		//winHeight = height;
		windowXPos_ = 0; windowYPos_ = 0;
	}
	else if (TaskBarRect_.right < width)
	{
		//winWidth = width - TaskBarRect_.right;
		//winHeight = height;
		windowXPos_ = TaskBarRect_.right; windowYPos_ = 0;
	}
	else
	{
		//winWidth = width;
		//winHeight = height - TaskBarRect_.bottom;
		windowXPos_ = 0; windowYPos_ = TaskBarRect_.bottom;
	}


	WindowedResolution(winWidth, winHeight);

	SetupWindows(winWidth, winHeight);
}


// Contructor where the user defines windowed dimensions and position
CGLWindowsCreation::CGLWindowsCreation(int winWidth, int winHeight, int xPos, int yPos)
{
	InitialiseMultisampling();
	InitialiseVersion();
	SetupWindows(winWidth, winHeight);

	windowXPos_ = xPos;
	windowYPos_ = yPos;
}

// Constructor where you define windowed dimensions, position and full screen bitdepth
CGLWindowsCreation::CGLWindowsCreation(int winWidth, int winHeight, int xPos, int yPos, int bits)
{
	InitialiseMultisampling();
	InitialiseVersion();
	SetupWindows(winWidth, winHeight);

	windowXPos_ = xPos;
	windowYPos_ = yPos;
	screenBits_ = bits;
}


void CGLWindowsCreation::SetupWindows(int winWidth, int winHeight)
{
	screenWidth_ = GetSystemMetrics(SM_CXSCREEN);	// Gets the screenwidth in pixels of the primary display monitor
	screenHeight_ = GetSystemMetrics(SM_CYSCREEN);	// Gets the screenheight in pixels of the primary display monitor

//	HWND hWndDesktop = FindWindowEx(NULL, NULL, TEXT("Program Manager"), NULL);
//	HDC hDcDesk = GetDC(hWndDesktop);
	
	HDC hDcDesk = GetDC(NULL);						// Gets the device context of the desktop

	screenBits_ = GetDeviceCaps(hDcDesk, BITSPIXEL);	// Gets the bpp of the desktop using the desktop DC	

	ReleaseDC(NULL, hDcDesk);						// Releases the DC

	WindowRect_.left = (long) 0;
	WindowRect_.right = (long) winWidth;
	WindowRect_.top = (long) 0;
	WindowRect_.bottom = (long) winHeight;

	hRC =	NULL;
	hDC =	NULL;
	hWnd =	NULL;

	fullscreen_ = true;
	active = true;
	showWindowFlag_ = false;
	glewInitialised_ = false;

};

// This function cycles through the graphics modes of the current display 
// looking for a resolution that is one step smaller than the current screen resolution
// using this width and height as the windowed width and height of the program if 
// fullscreen is not selected. 
void CGLWindowsCreation::WindowedResolution(int &winWidth, int &winHeight)
{
	bool match = false;
	DEVMODE dm = {0};
	dm.dmSize = sizeof(dm);

	// Queries information of the graphics mode for the current display device.
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

	// Saves the current bits, width, and height information 
	int bits = dm.dmBitsPerPel, width = dm.dmPelsWidth, height = dm.dmPelsHeight;

	//MessageBoxPrintf(TEXT("Screen Resolution"), TEXT("%dx%d, color depth = %d"), width, height, bits);

	int prevBits, prevWidth, prevHeight;

	// Cycles through the graphics display modes for the current display.
	for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0 && match == false; iModeNum++)
	{
		//int iModeNum = 0;
		//EnumDisplaySettings(NULL, iModeNum, &dm);
		//MessageBoxPrintf(TEXT("Screen Resolutions"), TEXT("Mode %d = %dx%d, color depth = %d"), iModeNum, dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel);

		// if the bpp and height equal the screen resolution
		if (bits == dm.dmBitsPerPel && height == dm.dmPelsHeight)
		{
			//MessageBoxPrintf(TEXT("YES"), TEXT("Saved:%dx%d, %d, Compared:%dx%d, %d"), 
			//	width, height, bits, dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel);
			match = true;
			// we use the previous graphics mode resolution to build our window with.
			bits = prevBits; width = prevWidth; height = prevHeight;
		}
		else
		{
			// otherwise save our information as the previous graphcis mode info.
			prevBits = dm.dmBitsPerPel; prevWidth = dm.dmPelsWidth; prevHeight = dm.dmPelsHeight;
		}
	}
	//MessageBoxPrintf(TEXT("Screen Resolution"), TEXT("%dx%d, color depth = %d"), width, height, bits);

	// set our window width and height
	winWidth = width; winHeight = height;
	
}

// Gets the dimensions of the taskbar and stores in a rect class
// for use in determining window position when created.
bool CGLWindowsCreation::GetTaskBarRect(void)
{
	APPBARDATA TaskBarData = {0};
	TaskBarData.cbSize = sizeof(APPBARDATA);

	HWND hWnd = FindWindow(TEXT("Shell_TrayWnd"), NULL);

	if (!hWnd)
		return false;

	TaskBarData.hWnd = hWnd;
	SHAppBarMessage(ABM_GETTASKBARPOS, &TaskBarData);

	TaskBarRect_ = TaskBarData.rc;

	return true;
}

// if multisampling is specified by the user then 
bool CGLWindowsCreation::CreateGLWindow(TCHAR *title, bool fullscreenflag, MultiSampleValue::Enum multisampling)
{
	multisample_ = true;

	if (multisampling > 16)
	{
		MessageBox(NULL, TEXT("Anti-Aliasing is greater than 16"),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	/* Taken from:
	Some sites that document and explain this and other bit twiddling hacks are:
    http://www-graphics.stanford.edu/~seander/bithacks.html
    (http://www-graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2)
    http://bits.stephan-brumme.com/
    (http://bits.stephan-brumme.com/isPowerOfTwo.html)
	And the grandaddy of them, the book "Hacker's Delight" by Henry Warren, Jr.:
    http://www.hackersdelight.org/
	As Sean Anderson's page explains, the expression ((x & (x - 1)) == 0)incorrectly 
	indicates that 0 is a power of 2. He suggests to use:
	(!(x & (x - 1)) && x)
	to correct that problem.*/
	if (multisampling && !(multisampling & (multisampling - 1)) )
		antiAliasLevel_ = multisampling;
	else
	{
		MessageBox(NULL, TEXT("Anti-Aliasing Level is not a power of 2"),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return CreateGLWindow(title, fullscreenflag);
}

bool CGLWindowsCreation::CreateGLWindow(TCHAR *title, bool fullscreenflag)
{
	int width, height, xpos, ypos;
	RECT winRect = WindowRect_;

	//std::cout << "Window Creation First " << windowXPos_ << ", " << windowYPos_ << std::endl;

//	std::cout << multisample_ << " " << multisampleSupported_ << " " << antiAliasLevel_ << " " << z << std::endl;
//	z++;

	fullscreen_ = fullscreenflag;		// set the fullscreen flag


	if (fullscreen_)
	{
		winRect.right = screenWidth_; winRect.bottom = screenHeight_;
		winRect.top = winRect.left = 0;
		xpos = ypos = 0;
	}
	else
	{
		xpos = windowXPos_; ypos = windowYPos_;
	}

	hInstance = GetModuleHandle(NULL);		// Grab an Instance for our  window

	// Setting up our windows class
	wc_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Redraw on move, and own DC for Windows
	wc_.lpfnWndProc = (WNDPROC)WndProc;			// WndProc handles messages
	wc_.cbClsExtra = 0;
	wc_.cbWndExtra = 0;							// No extra Windows data
	wc_.hInstance = hInstance;					// Set the Instance
	wc_.hIcon = LoadIcon(NULL, IDI_WINLOGO);	// Load the default Icon
	wc_.hCursor = LoadCursor(NULL, IDC_ARROW);	// Load the default Arrow
	wc_.hbrBackground = NULL;							// No Background is required for OpenGL
	wc_.lpszMenuName = NULL;							// We dont want a menu
	wc_.lpszClassName = TEXT("OpenGL");				// Set the name of the window Class


	if (!RegisterClass(&wc_))
	{
		MessageBox(NULL, TEXT("Failed to Register the Window Class"),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;					// Exit and return false
	}

	if (fullscreen_)					// Are we in fullscreen mode?
	{
		DEVMODE dmScreenSettings;		// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Make sure memory is cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);	// Size of the Devmode structure
		dmScreenSettings.dmPelsWidth = screenWidth_;			// Selected Screen Width
		dmScreenSettings.dmPelsHeight = screenHeight_;			// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = screenBits_;			// Selected Bits per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//MessageBoxPrintf(TEXT("Resolution Info"), TEXT("w:%d h:%d b:%d"), winRect.right - winRect.left, 
		//	winRect.bottom - winRect.top, screenBits_);

		// Try to set the Selected Mode and get results. NOTE: CDS_FULLSCREEN gets rid of the Start Bar
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL, TEXT("The Requested Fullscreen Mode is not Supported by\nyour Video Card. Use Windowed Mode Instead?"),
				TEXT("OpenGL Wrapper"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				fullscreen_ = false;			// Switch to Windowed Mode
			}
			else
			{
				// Pop up a MessageBox letting the user know the Program is Closing
				MessageBox(NULL, TEXT("Program will now Close."), TEXT("ERROR"), MB_OK | MB_ICONSTOP);
				return false;			// exit the program and return false
			}
		}
	}
	if (fullscreen_)	// Checking to see if we are still in Windowed Mode
	{
		dwExStyle_ = WS_EX_APPWINDOW;	// Window Extended Style
		dwStyle_ = WS_POPUP;				// Windows Style
		ShowCursor(false);				// Hides the Mouse Pointer
	}
	else
	{
		dwExStyle_ = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// Window Extended Style
		dwStyle_ = WS_OVERLAPPEDWINDOW;					// Windows Style
	}
	//	std::cout << "Before adjust, winRect " << winRect.right << " " << winRect.left << " " << winRect.top << " " << winRect.bottom << std::endl;
	AdjustWindowRectEx(&winRect, dwStyle_, false, dwExStyle_); // Adjust the window to the true requested size
	//	std::cout << "After adjust, winRect " << winRect.right << " " << winRect.left << " " << winRect.top << " " << winRect.bottom << std::endl;
	//	std::cout << "X and Y pos " << xpos << " " << ypos << std::endl;

	/*if (!fullscreen_)
	{
	MessageBoxPrintf(TEXT("Resolution Info"), TEXT("w:%d h:%d b:%d"), winRect.right - winRect.left,
	winRect.bottom - winRect.top, screenBits_);
	}*/

	if (!(hWnd = CreateWindowEx(dwExStyle_,					// Extended style for the Window
		TEXT("OpenGL"),				// Class Name
		title,						// Title
		WS_CLIPSIBLINGS |			// Required Window Style
		WS_CLIPCHILDREN |			// Required Window Style
		dwStyle_,					// Selected Window Style
		xpos, ypos,					// Window Position
		winRect.right - winRect.left, // Calculate adjusted Window Width
		winRect.bottom - winRect.top, // Calculate adjusted Window Height
		NULL,						// No Parent Window
		NULL,						// No Menu
		hInstance,					// Instance
		NULL)))						// Don't pass anything to WM_CREATE
	{
		KillGLWindow();				// Kill the Window
		MessageBox(NULL, TEXT("Window Creation Error"), TEXT("ERROR"),
			MB_OK | MB_ICONEXCLAMATION);	// Inform the user of the error
		return false;						// Quit the program
	}

	static PIXELFORMATDESCRIPTOR pfd =		// pfd tells Windows specific things about this window creation
	{
		sizeof(PIXELFORMATDESCRIPTOR),		// Size of this Pixel Format Descriptor
		1,									// Version Number
		PFD_DRAW_TO_WINDOW |				// Format must support Window
		PFD_SUPPORT_OPENGL |				// Format must support OpenGL
		PFD_DOUBLEBUFFER,					// Must support Double Buffering
		PFD_TYPE_RGBA,						// Request an RGBA format
		screenBits_,						// Colour Bit depth
		0, 0, 0, 0, 0, 0, 0, 0,				// Colour and colour shift bits ignored
		0, 0, 0, 0, 0,						// Accumulation bits ignored
		24,									// No. of bits for Z-Buffer (depth buffer)
		8,									// No. of bits for Stencil Buffer
		0,									// Number of Aux buffers
		PFD_MAIN_PLANE,						// Main Drawing Layer
		0,									// Reserved
		0, 0, 0								// Layer masks ignored
	};
	if (!(hDC = GetDC(hWnd)))				// If we didn't get a device context
	{
		KillGLWindow();						// Kill the window
		MessageBox(NULL, TEXT("Can't Create a GL Device Context."),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
		return false;						// Exit Code
	}

	// The next block of code should only be run if we don't have glew initialised or we 
	// only need an OpenGL context that is less than or equal to 2.0 i.e. a fixed
	// rendering pipeline.
	// If we need OpenGL > 2.0 then we need to create an OpenGl context initially in
	// order to query if the OpenGL extension functions are available.
	if (!glewInitialised_ || glMajor <=2)
	{
		PixelFormat_ = ChoosePixelFormat(hDC, &pfd);	// Find a compatible Pixel Format

		if (PixelFormat_ == 0)		// Did we find a compatible format?
		{
			// Failed
			KillGLWindow();					// Kill the Window
			MessageBox(NULL, TEXT("Can't find a matching PixelFormat."),
				TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION); // Tell the user
			return false;
		}
	}
	else
	// else block should only run if we have queried successfully that OpenGL extensions are evailable
	// AND we also need an OpenGL context greater than 2.0
	{
		if (WGLEW_ARB_pixel_format)
		{
			UINT numFormats;
			int mPixelFormat;
			float fAttributes[] = { 0, 0 };	// Is used as a passing array to wglChoosePixelFormatARB 
			// specifying several parameters but in float format
			if (multisample_)	// if we want multisampling
			{
				int pixelFormatAttribList[] =	// A multisample pixel format attribute list
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
//					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, screenBits_,
//					WGL_ALPHA_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
					WGL_SAMPLE_BUFFERS_ARB, 1,
					WGL_SAMPLES_ARB, antiAliasLevel_,
					0
				};
				if (!wglChoosePixelFormatARB(hDC, pixelFormatAttribList, fAttributes,
					1, &mPixelFormat, &numFormats))
				{
					KillGLWindow();						// Kill the Window
					MessageBox(NULL, TEXT("Can't find a matching extended multisample PixelFormat."),
						TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION); // Tell the user
					return false;						// Exit Code
				}
				else
				{
					PixelFormat_ = mPixelFormat;
				}
			}
			else // Otherwise we need an attribute list without mutlisampling
			{
				int pixelFormatAttribList[] =	// A pixel format attribute list without musltisampling
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, screenBits_,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
					0
				};
				if (!wglChoosePixelFormatARB(hDC, pixelFormatAttribList, fAttributes,
					1, &mPixelFormat, &numFormats))
				{
					KillGLWindow();						// Kill the Window
					MessageBox(NULL, TEXT("Can't find a matching extended PixelFormat."),
						TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION); // Tell the user
					return false;						// Exit Code
				}
				else
				{
					PixelFormat_ = mPixelFormat;
				}
			}
		}
		else
		{
			MessageBox(NULL, TEXT("WGLEW_ARB_pixel_format unavailable"),
				TEXT("glewInit"), MB_OK);
			KillGLWindow();
			return false;
		} 
	}


	/*	if (!(PixelFormat_ = ChoosePixelFormat(hDC, &pfd)))	// If Windows can't find a matching Pixel Format?
		{
		KillGLWindow();						// Kill the Window
		MessageBox(NULL, TEXT("Can't find a matching PixelFormat."),
		TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION); // Tell the user
		return false;						// Exit Code
		}		*/

	// Set the pixel format
	if (!SetPixelFormat(hDC, PixelFormat_, &pfd))	
	{
		KillGLWindow();						// Kill the Window
		MessageBox(NULL, TEXT("Can't set the PixelFormat."),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
		return false;						// Exit Code
	}

	// Same as with the pixel format if this is our first pass (i.e. glew is not initialised
	// or we simply do not need the OpenGL extensions (i.e. need OpenGL version 2.0 or less)
	// then we only need a normal Rendering Context
	if (!glewInitialised_ || glMajor <=2)
	{
		if (!(hRC = wglCreateContext(hDC)))		// If Windows can't get a Rendering Context
		{
			KillGLWindow();						// Kill the Window
			MessageBox(NULL, TEXT("Can't create a GL Rendering Context."),
				TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
			return false;						// Exit Code
		}
	}
	else // This block only runs if we have initialised glew AND we need a an OpenGL version higher than 2.0
	{
		int contextAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, glMajor,
			WGL_CONTEXT_MINOR_VERSION_ARB, glMinor,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0	// End of attributes list
		};
		int mPixelFormat;
		UINT numFormats;
		if (!(hRC = wglCreateContextAttribsARB(hDC, 0, contextAttribs)))
		{
			KillGLWindow();						// Kill the Window
			MessageBox(NULL, TEXT("Can't create a GL Rendering Context."),
				TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
			return false;						// Exit Code
		}
	}

	if (!wglMakeCurrent(hDC, hRC))			// If the Rendering Context doesn't activate
	{
		KillGLWindow();						// Kill the Window
		MessageBox(NULL, TEXT("Can't Activate the GL Rendering Context."),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
		return false;						// Exit Code
	}

	// Now that the window is created, we want to query what samples are available
	// if we need an OpenGL context of a version higher than 2.0
	if (!glewInitialised_ && glMajor > 2)
	{
		if (glewInit() == GLEW_OK)
		{
			glewInitialised_ = true;
			KillGLWindow();
			return CreateGLWindow(title, fullscreenflag);
		}
		else
		{
			MessageBox(NULL, TEXT("GLEW Initialisation Failed"), TEXT("glewInit"), MB_OK);
			KillGLWindow();
			return false;
		}
	}


	ShowWindow(hWnd, SW_SHOW);				// Show the Window
	SetForegroundWindow(hWnd);				// Set this Window slightly higher priority
	SetFocus(hWnd);							// Sets the keyboards focus to the Window

	if (fullscreen_)
	{
		ReSizeGLScene(screenWidth_, screenHeight_);	// Setup the correct viewport
	}
	else
	{
		ReSizeGLScene(WindowRect_.right, WindowRect_.bottom);	// The viewport is different if in a window
	}

	if (!InitGL())			// If the Initialisation function for the GL window returns an error code
	{
		KillGLWindow();		// Kill the Window
		MessageBox(NULL, TEXT("Initialisation Failed."),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
		return false;
	}

	return true;							// Success code
}


bool CGLWindowsCreation::InitMultisample(const PIXELFORMATDESCRIPTOR& pfd)
{

	// See if the extension WGL_ARB_multisample is supported on this system
	if (!IsWGLExtensionSupported("WGL_ARB_multisample"))
	{
		// if not then multisampling is not supported and we return failure
		multisampleSupported_ = false;
		return false;
	}


	// The declaration below is to make wglChoosePixelFormatARB a function pointer to 
	// functions of type: 
	// typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
	// wglGetProcAddress returns the function pointer to the function whose name matches 
	// the string "wglChoosePixelFormatARB"
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = 
		(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	// If wglChoosePixelFormatARB is a NULL pointer i.e. wglGetProcAddress couldn't find 
	// the function wglChoosePixelFormatARB
	if (!wglChoosePixelFormatARB)
	{
		// multisampling is not supported and we return failure.
		multisampleSupported_= false;
		return false;
	}

	int pixelFormat;		// Stores the pixelFormat 
	bool valid;				// Stores true or false if the pixelformat is valid
	UINT numFormats;		// Stores the number of formats (we can have more than one 
							// valid pixelformat that supports multisampling and is returned)
	float fAttributes[] = {0,0};	// Is used as a passing array to wglChoosePixelFormat 
									// specifying several parameters but in float format

	// These attributes defined below specify how we want our pixel format and are used as a
	// passing variable to wglChoosePixelFormat, which returns the pixelformats that conform
	// to these parameters. The two parameters that are most important for multisampling are
	// WGL_SAMPLE_BUFFERS and WGL_SAMPLES, where we say we want multisampling and how many 
	// passes to make, respectively.

	int iAttributes[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, screenBits_,	// Note that the colour bit depth is given a variable
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 16,
		WGL_STENCIL_BITS_ARB, 0,
		WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, antiAliasLevel_,	// Checks for the specified sampling level
		0, 0};	

		while (antiAliasLevel_ >= 2)
		{
		//	std::cout << antiAliasLevel_ << std::endl;
			// Checks to see if the pixel format is valid for the number of samples specified
			// in antiAliasLevel_
			valid = wglChoosePixelFormatARB(hDC, iAttributes,fAttributes, 1, &pixelFormat, &numFormats);
			if (valid && numFormats >=1)
			{
				// if valid multisample is supported and success returned
				multisampleSupported_ = true;
				arbMultisampleFormat_ = pixelFormat;
				return true;
			}

			if (antiAliasLevel_ == 2)
			{
				// if even the lowest AA is not supported then return failure;
				multisampleSupported_ = false;
				return false;
			}

			antiAliasLevel_ = antiAliasLevel_/2;
			iAttributes[19] = antiAliasLevel_;
		};


	return false;
}

bool CGLWindowsCreation::IsWGLExtensionSupported(const char * extension)
{
	const size_t extlen = strlen(extension);	// Stored the string length of extension
	const char *supported = NULL;		// Will be used to store the string list of supported extensions
	
	// The variable wglGetExtString is a pointer to a function that is retrieved
	// using the function wglGetProcAddress. It returns a pointer to the function
	// with the same name as that given in the string.
	// PROC is defined as: typedef int (_stdcall *PROC)(); in C function pointers
	// declarations are of the form: int (*ptr)(); 
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

	if (wglGetExtString)
	{
		// wglGetExtString points to something (i.e. not NULL)
		// we use wglGetExtString to call the function it points to. 
		// Note to do that the type of the function pointer needs to be changed to type
		// (char * (WINAPI *)(HDC)) - function type which returns char * and needs HDC as 
		// an argument.
		supported = ((char *(WINAPI *)(HDC))wglGetExtString)(hDC);
	}

	if (supported == NULL)
	{
		// if supported is NULL i.e. wglGetExtString fails to return extension list
		// try glGetString to get extension list
		supported = (char *)glGetString(GL_EXTENSIONS);
	}

	if (supported == NULL)
	{
		// if glGetString also failed and supported is NULL then no extensions are supported
		// return failure
		return false;
	}

	//std::cout << supported << std::endl;

	//std::cout << (char *)glGetString(GL_EXTENSIONS) << std::endl;

	// Otherwise begin examination of supported (extension string list)
	for (const char * p = supported; ; p++)
	{
		// p is now the extension list and will be incremented along the list
		// by one char each time.
		// We match our extension list with the extension required and point p to
		// a possible match in the list
		p = strstr(p, extension);

		if (p == NULL)
		{
			// i.e. extension is not in the extension list at all
			// return failure 
			return false;
		}
		// Now we need to make sure that the match is correct and not just part of 
		// an extension name that includes extension. We do this by checking the preceeding
		// char and also the next character after the match.
		if (( p == supported || p[-1] == ' ') && (p[extlen] == '\0' || p[extlen] == ' '))
		{
			// the match is correct return success!
			return true;
		}
	}
}


void CGLWindowsCreation::KillGLWindow(void)	// Function to kill an existing Window
{
	if (fullscreen_)						// If the Window is in fullscreen mode
	{
		ChangeDisplaySettings(NULL, 0);		// Change to windowed mode
		ShowCursor(true);					// Show the mouse pointer
	}

	if (hRC)								// If there is a Rendering Context (i.e. it isnt NULL)
	{
		if (!wglMakeCurrent(NULL, NULL))	// If releasing the DC and RC failed
		{
			MessageBox(NULL, TEXT("Release of DC and RC Failed."),
				TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);	// Tell the user
		}
		if (!wglDeleteContext(hRC))			// If we cannot delete the RC
		{
			MessageBox(NULL, TEXT("Release Rendering Context Failed."),
				TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);	// Tell the user
		}
		hRC = NULL;							// Set RC to NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))		// If there is a Device Context and we cant release it
	{
		MessageBox(NULL, TEXT("Release Device Context Failed."), 
			TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);	// Tell the user
		hDC = NULL;							// Set DC to NULL
	}

	if (hWnd && !DestroyWindow(hWnd))		// If there is a Window Handle and it can't be destroyed
	{
		MessageBox(NULL, TEXT("Could Not Release hWnd."),
			TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);	// Tell the user
		hWnd = NULL;						// Set hWnd to NULL
	}

	if (!UnregisterClass(TEXT("OpenGL"), hInstance))	// If we cannot Unregister the class
	{
		MessageBox(NULL, TEXT("Could Not Unregister Class."),
			TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);	// Tell the user
		hInstance = NULL;					// Set the hInstance to NULL
	}
}




