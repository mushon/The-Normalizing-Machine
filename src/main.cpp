#include "testApp.h"
#include "ofMain.h"

#ifdef TARGET_WIN32
	#include "ofAppGlutWindow.h"
#endif

//========================================================================
int main( ){


	ofWindowMode windowMode;
#ifdef DEBUG
	windowMode = OF_WINDOW;
#else
	windowMode = OF_FULLSCREEN;
#endif

#ifdef TARGET_WIN32
	//ofAppGlutWindow window;
	ofSetupOpenGL(1920,1080, windowMode);	// <-------- setup the GL context
#endif
#ifdef TARGET_OSX
	ofSetupOpenGL(1024,768, windowMode);			// <-------- setup the GL context
#endif
#ifdef TARGET_LINUX
	// TODO
#endif
#ifndef DEBUG
	ofHideCursor();
#endif

	try
	{
		testApp* app = new testApp();
		ofRunApp(app);
	}
	catch(exception e)
	{
		cout << e.what();
	}
}
