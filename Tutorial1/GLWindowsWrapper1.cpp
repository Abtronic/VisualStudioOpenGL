
//#include "GLWindowsWrapper.h"


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
			return 0;										// Jump back to the message loop
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
					// Recreate our OpenGL window 
					if(!CreateGLWindow(TEXT("C++ OpenGL Window Class"), fullscreen_))
					{
						return 0;			// if the Window is not created then quit the program
					}
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


// Contructor where you define windowed dimensions and position
CGLWindowsCreation::CGLWindowsCreation(int winWidth, int winHeight, int xPos, int yPos)
{
	InitialiseMultisampling();
	SetupWindows(winWidth, winHeight);

	windowXPos_ = xPos;
	windowYPos_ = yPos;
}

// Constructor where you define windowed dimensions, position and full screen bitdepth
CGLWindowsCreation::CGLWindowsCreation(int winWidth, int winHeight, int xPos, int yPos, int bits)
{
	InitialiseMultisampling();
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

bool CGLWindowsCreation::CreateGLWindow(TCHAR *title, bool fullscreenflag, unsigned int multisampling)
{
	multisample_ = true;

	antiAliasLevel_ = multisampling;

	return CreateGLWindow(title, fullscreenflag);
}

bool CGLWindowsCreation::CreateGLWindow(TCHAR *title, bool fullscreenflag)
{
	int width, height, xpos, ypos;
	RECT winRect = WindowRect_;

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

	hInstance			= GetModuleHandle(NULL);		// Grab an Instance for our  window
	
	// Setting up our windows class
	wc_.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Redraw on move, and own DC for Windows
	wc_.lpfnWndProc		= (WNDPROC) WndProc;			// WndProc handles messages
	wc_.cbClsExtra		= 0;
	wc_.cbWndExtra		= 0;							// No extra Windows data
	wc_.hInstance		= hInstance;					// Set the Instance
	wc_.hIcon			= LoadIcon(NULL, IDI_WINLOGO);	// Load the default Icon
	wc_.hCursor			= LoadCursor(NULL, IDC_ARROW);	// Load the default Arrow
	wc_.hbrBackground	= NULL;							// No Background is required for OpenGL
	wc_.lpszMenuName	= NULL;							// We dont want a menu
	wc_.lpszClassName	= TEXT("OpenGL");				// Set the name of the window Class


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
	AdjustWindowRectEx(&winRect, dwStyle_, false, dwExStyle_); // Adjust the window to the true requested size

/*	if (!fullscreen_)
	{
		//MessageBoxPrintf(TEXT("Resolution Info"), TEXT("w:%d h:%d b:%d"), winRect.right - winRect.left, 
			//winRect.bottom - winRect.top, screenBits_);
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
		0, 0, 0, 0, 0, 0,					// Color bits ignored
		0,									// Shift bit ignored
		0,									// No Accumulation bits ignored
		16,									// 16 bit Z-Buffer (depth buffer)
		0,									// No Stencil Buffer
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

	// Our first pass, Multisampling hasn't been created yet, so we create a window normally
	// if it is supported, then we're on our second pass. That means we want to use our 
	// pixel format for sampling so set pixelFormat to arbMultisampleFormat instead
	// This replaces the choosepixelformat if block below
	/*if (arbMultisampleSupported) // shows the user what arbMutlisampleSupported equals
		MessageBoxPrintf(TEXT("arbMultisampleSupported"), TEXT("true"));
	else
		MessageBoxPrintf(TEXT("arbMultisampleSupported"), TEXT("false"));*/

	if (!arbMultisampleSupported)
	{
		PixelFormat_ = ChoosePixelFormat (hDC, &pfd);	// Find a compatible Pixel Format
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
	{
		PixelFormat_ = arbMultisampleFormat;
	}


/*	if (!(PixelFormat_ = ChoosePixelFormat(hDC, &pfd)))	// If Windows can't find a matching Pixel Format?
	{
		KillGLWindow();						// Kill the Window
		MessageBox(NULL, TEXT("Can't find a matching PixelFormat."),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION); // Tell the user
		return false;						// Exit Code
	}		*/

	if (!SetPixelFormat(hDC, PixelFormat_, &pfd))	// If Windows can't set the pixel format
	{
		KillGLWindow();						// Kill the Window
		MessageBox(NULL, TEXT("Can't set the PixelFormat."),	
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
		return false;						// Exit Code
	}

	if (!(hRC = wglCreateContext(hDC)))		// If Windows can't get a Rendering Context
	{
		KillGLWindow();						// Kill the Window
		MessageBox(NULL, TEXT("Can't create a GL Rendering Context."),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
		return false;						// Exit Code
	}

	if (!wglMakeCurrent(hDC, hRC))			// If the Rendering Context doesn't activate
	{
		KillGLWindow();						// Kill the Window
		MessageBox(NULL, TEXT("Can't Activate the GL Rendering Context."),
			TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);	// Tell the user
		return false;						// Exit Code
	}

	// Now that the window is created, we want to query what samples are available
	// We call our initMultiSample window
	// if we return a valid context, we want to destroy our current window 
	// and create a new one using the MultiSample Interface.
	// this if statement is true the first time CreateGLWindow is called 
	// the second time it is called within the second if statement it is false

	if (!arbMultisampleSupported && CHECK_FOR_MULTISAMPLE)
	{
		if (InitMultisample(hInstance, hWnd, pfd))
		{
			MessageBox(NULL, TEXT("Multisampling"), TEXT("Supported"), MB_OK);
			KillGLWindow();		// Kill the Window
			
			return CreateGLWindow(title, fullscreenflag);
		}
		else
			MessageBox(NULL, TEXT("returning false"), TEXT("InitMultisample()"), MB_OK);

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




