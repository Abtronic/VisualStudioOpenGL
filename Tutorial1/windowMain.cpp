// Creating a Windows Class or 'wrapper' to make creating a window easier
// so that we can get on with programming rather than thinking about 
// setting up the environment

//#pragma comment(lib, "glew32.lib")

#include <windows.h>
//#include <GL\glew.h>
//#include <gl\gl.h>
//#include <gl\glu.h>
//#include <gl\glext.h>
//#include <gl\wglext.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>



#include "GLWindowsWrapper.h"
//#include "BitmapLoader.h"

//#define GLEW_STATIC
#define PROJECTION		// If PROJECTION is defined then we have perspective projection else orthographic
#define DEBUG			// If DEBUG is defined a debug console window will also be created.
// Global Variables
CGLWindows	OpenGLWindow;		// Initialising a CGLWindows class
//CBitmap *bitmap;
GLuint texture;
//bool matrixFlag = 0;
//GLfloat modelView[16];
GLuint VertexArray1D;

//void printMatrix(GLfloat []);
double angle = 0.0f;


//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  OpenGL Functions  /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


/*int LoadGLTextures()
{
	int status = false;
	CBitmap *TextureImage;

	TCHAR *filename = TEXT("NewExample32.bmp");

	TextureImage = NULL;
	if (!(TextureImage = new CBitmap))
	{
		return status;
	}

	if(TextureImage->LoadBMP(filename))
	{
		status = true;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear filtering
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY, 0, TextureImage->pixelformat,
			GL_UNSIGNED_BYTE, TextureImage->data);
	}

	if (TextureImage)
	{
		delete TextureImage;
	}

	return status;
}*/


GLvoid ReSizeGLScene(GLsizei width, GLsizei height)	// Function to resize and initialise the GL window
{
	if (height == 0)								// Prevent a divide by zero by 
	{
		height = 1;									// making the height equal to one
	}

	glViewport(0, 0, width, height);				// Reset the Current Viewport
	glMatrixMode(GL_PROJECTION);					// Select the Projection Matrix
	glLoadIdentity();								// Reset the Projection Matrix

#ifdef PROJECTION
	// Calculate the aspect ratio of the window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 2000.0f);
#else
	glOrtho(-4.0f, 4.0f, -4.0f * ((GLfloat)height / (GLfloat)width), 
		4.0f * ((GLfloat)height / (GLfloat)width), 0.1f, 100.0f);
#endif

	glMatrixMode(GL_MODELVIEW);						// Select the Modelview Matrix
	glLoadIdentity();								// Reset the Modelview Matrix
}

int InitGL(GLvoid)									// Initialisation of a new OpenGL Window
{
	/*if (GLEW_OK != glewInit())			// Initialize the extension entry points.
	{
		// GLEW failed
		return false;
	}
	if(!GLEW_ARB_vertex_array_object)
	{
		std::cout << "ARB_vertex_array_object is not supported" << std::endl;
	}//*/


	glEnable(GL_TEXTURE_2D);			// Enables 2D texturing
	glShadeModel(GL_SMOOTH);			// Enables smooth shading (gourad shading)

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);			// Black Background

	glClearDepth(1.0f);					// The range of the depth buffer is clamped from 0 to 1
	glEnable(GL_DEPTH_TEST);			// Enables Depth-Testing
	glDepthFunc(GL_LEQUAL);				// Sets that only values less than or equal to the clear-depth 
										// value should be drawn (e.g. z value of 1 and 2 are 1/1 and 2/2
										// respectively in the depth buffer, both in the region 0 to 1.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really nice perspective calculation

	glEnable(GL_CULL_FACE);				// Enables face culling
	glCullFace(GL_BACK);				// Sets the back faces to be culled, i.e. triangles that are wound CW

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

/*	if(!LoadGLTextures())
	{
		return false;
	}*/
	/*glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(-5.0f, 1.0f, 1.0f, 0.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	printMatrix(modelView);*/


//	glGenVertexArrays(1, &VertexArrayID);

	return true;
}


int DrawGLScene(GLvoid)					// Function that handles all of the OpenGL drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the screen and the depth buffer
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);


//	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
//	printMatrix(modelView);


	//glRasterPos2f(-1,-1);
	
	//glDrawPixels(bitmap->sizeX, bitmap->sizeY, bitmap->pixelformat, GL_UNSIGNED_BYTE, bitmap->data);
	
/*	glBindTexture(GL_TEXTURE_2D, texture);		// Select our texture

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  0.0f);	// Bottom Left
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  0.0f);	// Bottom Right
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  0.0f);	// Top Right
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  0.0f);	// Top Left
	} glEnd(); // */
	angle += 0.1f;
	glRotatef(angle, 1.0f, 0.0f, 1.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE_ARB);

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	{
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 1.0f); glVertex3f(-0.1f, 0.0f, 0.9f);
		glVertex3f(0.0f, 0.0f, 1.0f); glVertex3f(0.1f, 0.0f, 0.9f);
		
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f); glVertex3f(-0.1f, 0.9f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f); glVertex3f(0.1f, 0.9f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f); glVertex3f(0.9f, -0.1f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f); glVertex3f(0.9f, 0.1f, 0.0f);

	} glEnd(); // */

	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_MULTISAMPLE_ARB);




	/*glBegin(GL_TRIANGLES);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
	}
	glEnd();*/


	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


// wndproc handles window messages
LRESULT CALLBACK WndProc(	HWND hWnd,		// Handle for this Window
												UINT uMsg,		// Message  for this Window
												WPARAM wParam,	// Additional message information
												LPARAM lParam)	// Additional message information
{
	if (!OpenGLWindow.WindowsMessages(uMsg, wParam, lParam))	// If Message is processed i.e. 
	{
		return 0;
	}
	
	//Pass all unhandled messages to DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Function where certain keys can be tested to see if they were pressed
void KeyboardInput(void)
{

}


int WINAPI WinMain( HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					LPSTR		lpCmdLine,
					int			nCmdShow)
{

// The following code creates a console window along with the graphical window.
// The console will be used to display debug information by using the following commands:
// std::cout << variable string << std::endl;
#ifdef DEBUG
	if(AllocConsole())
	{
		SetConsoleTitleA("Debug Console");
		freopen("CONOUT$", "wb", stdout);
		freopen("CONOUT$", "wb", stderr);
	}
#endif

//	bitmap = new CBitmap;

	//bitmap->LoadBMP(TEXT("Tim.bmp"));

	//MessageBoxPrintf(TEXT("Simple"), TEXT("Width:%d, Height: %d"), bitmap->sizeX, bitmap->sizeY);

	bool fullscreen = false;
	// Ask the user if they would like to run fullscreen or in a window
	if (MessageBox(NULL, TEXT("Would you like to run in Fullscreen Mode?"), 
		TEXT("Start Fullscreen?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		fullscreen = true;
	}
	else				// else we are using windowed mode
	{
		fullscreen = false;
	}
	// Note that the window is created with 'fullscreen' specifications and 
	// with a request for an anti-aliased (8 passes) graphical window
	if (!OpenGLWindow.CreateGLWindow(TEXT("C++ OpenGL Window Class"), fullscreen, 
		MSV::GL_MULTISAMPLE_EIGHT))	// If we recieve an error code
	{
		return 0;			// Quit 
	}

	int QuitMessage = OpenGLWindow.MessageLoop();

//	delete bitmap;
	FreeConsole();

	return (QuitMessage);
}



int CDECL MessageBoxPrintf (TCHAR * szCaption, TCHAR * szFormat, ...)
{
	TCHAR szBuffer [1024];
	va_list pArgList;

	// The va_start macro (defined in STDARG.H) is usually equivalent to:
	// pArgList = (char *) &szFormat + sizeof (szFormat);

	va_start (pArgList, szFormat);
	
	// The last argument to wvsprintf points to the arguments

	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);

	// The va_end macro just zeroes out the pArgList for no good reason

	va_end(pArgList);

	return MessageBox(NULL, szBuffer, szCaption, 0);
}

/*void printMatrix(GLfloat matrix[])
{

	for (int i = 0; i < 16; i+= 4)
	{
		std::cout << matrix[i] << "\t" << matrix[i+1] << "\t" 
			<< matrix[i+2] << "\t" << matrix[i+3] << std::endl;
	}

}*/
