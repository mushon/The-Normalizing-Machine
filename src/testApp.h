#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxUI.h"
#include "AppCursor.h"

using namespace ofxCv;
using namespace cv;

class Timer 
{
	unsigned long long timeout;
	unsigned long long last;

public:

	Timer(unsigned long long _timeout = 0) : timeout(_timeout)
	{
		reset();
	}

	void setTimeout(unsigned long long _timeout)
	{
		timeout = _timeout;
	}

	int getCountDown()
	{
		int countdown = last + timeout - ofGetSystemTime();
		return max(0, countdown);
	}

	void reset()
	{
		last = ofGetSystemTime();
	}		
};

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

	ofPoint profilerPos;
	bool drawProfiler;
	bool drawDepth;


	enum State {
		IDLE, GOTO_SPOT, RAISE_HAND, SELECTION, CONFIRMATION
	};
	State state;

	const string stateToString(const testApp::State& state )
	{
		string str;
#define X(state) case testApp::State::state: str = #state; break;
		switch(state)
		{
			X(IDLE);
			X(GOTO_SPOT);
			X(RAISE_HAND);
			X(SELECTION);
			X(CONFIRMATION);
		}
#undef X

		return str;
	}

	Timer lastSeenUser;

	struct SelectedUser
	{
		static const int NO_USER = -1;

		float handSmoothingFactor;
		float shoulderSmoothingFactor;
		int hovered;

		int id;

		ofPoint rightHand;
		ofPoint rightShoulder;
		ofPoint headPoint;
		ofVec2f dist;

		ofPoint hand; //filtered
		ofPoint shoulder; //filtered

		ofVec2f screenPoint;

		SelectedUser() : 
			id(NO_USER), handSmoothingFactor(0.1f), shoulderSmoothingFactor(0.05f)
		{			
		}


		void updatePoints(ofPoint h, ofPoint s)
		{
			if (hand == ofVec3f() && shoulder == ofVec3f())
			{
				hand = h;
				shoulder = s;
			}
			else
			{
				hand.interpolate(h, handSmoothingFactor);
				shoulder.interpolate(s, shoulderSmoothingFactor);
			}
		}

		ofVec3f getPointingDir()
		{
			return hand - shoulder;
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

	int nVisibleUsers;
	stringstream userMessage;

	ofPoint spot;
	float spotRadius;



	void setupGui();
	ofxUISuperCanvas* gui;

	void startRecording();
	void stopRecording();

	AppCursor cursor;
};

#endif
