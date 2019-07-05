#include "testApp.h"
#include "ofMain.h"

#ifdef TARGET_WIN32
	#include "ofAppGlutWindow.h"
#endif

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;

	settings.setSize(1920, 1080);

#ifdef TARGET_WIN32
	settings.decorated = false;
	settings.setPosition(glm::vec2(1920*3, 0));
#endif


#ifdef TARGET_WIN32
	//ofAppGlutWindow window;
	//ofSetupOpenGL(1920,1080, windowMode);	// <-------- setup the GL context
#endif
#ifdef TARGET_OSX
	auto windowMode = OF_WINDOW;
	ofSetupOpenGL(1920,1080, windowMode);			// <-------- setup the GL context
#endif
#ifdef TARGET_LINUX
	// TODO
#endif

	ofCreateWindow(settings);

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
