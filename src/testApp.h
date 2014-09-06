#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxUI.h"

using namespace ofxCv;
using namespace cv;


class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void exit();

	void guiEvent(ofxUIEventArgs &e);
	bool drawVideo;
	bool drawGui;
	
	struct StateString
	{
		StateString() :
			Idle("Idle"),
			Recognition("Recognition"),
			Selection("Selection"),
			Confirmation("Confirmation")
		{
		}
		const string Idle, Recognition, Selection, Confirmation;
	} stateString;

	enum State {
		Idle, Recognition, Selection, Confirmation
	};
	State state;
	static const unsigned long long stateResetTimeout = 5 * 1000; // 5 seconds
	unsigned long long lastTimeSeenUser;

	

private:
	void	setupRecording(string _filename = "");
	void	setupPlayback(string _filename);
	string	generateFileName();

	bool isTracking;
	bool isRecording;
	bool isTrackingHands;

	ofxOpenNI openNIRecorder;
    ofxOpenNI openNIPlayers[4];
	int n_players;
	string lastRecordingFilename;

	string lastDump;

	stringstream userMessage;

	ofPoint spot;

	
	void setupGui();
	ofxUISuperCanvas* gui;
	
};

#endif
