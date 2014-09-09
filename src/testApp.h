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
	bool drawProfiler;

	enum State {
		IDLE, RECOGNITION, GOTO_SPOT, RAISE_HAND, SELECTION, CONFIRMATION
	};
	State state;

	const string stateToString(const testApp::State& state )
	{
		string str;
#define X(state) case testApp::State::state: str = #state; break;
		switch(state)
		{
			X(IDLE);
			X(RECOGNITION);
			X(GOTO_SPOT);
			X(RAISE_HAND);
			X(SELECTION);
			X(CONFIRMATION);
		}
#undef X

		return str;
	}

	static const unsigned long long stateResetTimeout = 5 * 1000; // 5 seconds
	unsigned long long lastTimeSeenUser;

	struct SelectedUser
	{
		static const int NO_USER = -1;
		
		float smoothingFactor;

		int id;
		ofVec3f pointingDir;
		ofVec3f pointingVelocity;
		ofVec2f screenPoint;

		SelectedUser() : 
			id(NO_USER), smoothingFactor(0.2f)
		{			
		}

		void updatePointingDir(ofPoint p)
		{
			if (pointingDir == ofVec3f())
			{
				pointingDir = p;
				pointingVelocity = ofVec3f();
			}
			else
			{
				pointingVelocity = p - pointingDir;
				pointingDir.interpolate(p, smoothingFactor);
				
			}
		}

	} selectedUser;
	



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
