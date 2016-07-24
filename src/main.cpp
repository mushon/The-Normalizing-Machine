#include "testApp.h"
#include "ofMain.h"

#ifdef TARGET_WIN32
	#include "ofAppGlutWindow.h"
#endif

//========================================================================
int main( ){


#ifdef TARGET_WIN32
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_FULLSCREEN);			// <-------- setup the GL context
#endif
#ifdef TARGET_OSX
	ofSetupOpenGL(1024,768, OF_WINDOW);			// <-------- setup the GL context
#endif
#ifdef TARGET_LINUX
	// TODO
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
