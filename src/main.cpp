#include "testApp.h"
#include "ofMain.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_FULLSCREEN);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:

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
