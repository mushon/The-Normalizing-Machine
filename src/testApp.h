#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxUI.h"
#include "AppCursor.h"

#include "SelectedUser.h"
#include "AppTimer.h"


using namespace ofxCv;
using namespace cv;

struct RecordedData
{
	RecordedData()
	{
		time = ofGetUnixTime();
	}
	// record data:	
	unsigned int time; //primary key : id
	unsigned int v;
	unsigned int x[3];

	unsigned int vScore;
	unsigned int xScore;
	
	// time, file, location, selection v/x

};

class testApp : public ofBaseApp{

	vector<RecordedData> dataset; // the whole shablang
	// on startup, find dirs (xml?)
	// load recordings

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
		IDLE, GOTO_SPOT, RAISE_HAND, SELECTION, CONFIRMATION, MORE_THAN_ONE
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
			X(MORE_THAN_ONE);
		default:
			str = "undefined";
		}
#undef X

		return str;
	}

	AppTimer lastSeenUser;
	SelectedUser selectedUser;
	
	

private:
	void	setupRecording(string _filename = "");
	void	setupPlayback(string _filename);
	string	generateFileName();

	bool isRecording;

	static const int MAX_PLAYERS = 25;
	ofxOpenNI openNIRecorder;
	ofxOpenNI openNIPlayers[MAX_PLAYERS];
	int n_players;
	
	float playbackScale;

	string lastRecordingFilename;

	string lastDump;

	int nVisibleUsers;
	stringstream userMessage;

	ofPoint spot;
	float spotRadius;
	float spotRadiusHysteresis;

	int userMapAlpha;
	int handShoulderDistance;
	float minBottomScreen;

	void drawDebugText();


	int margin;
	int bottomMargin;

	float touchScreenSize; // virtual screen 

	ofImage yesIcon;
	ofImage noIcon;

	ofImage txt_pointing;
	ofImage txt_position;
	ofImage txt_prompt;
	void drawOverheadText(ofImage&, int h);


	ofTrueTypeFont	verdana;

	void setupGui();
	ofxUISuperCanvas* gui;

	void startRecording();
	void stopRecording();

	AppCursor cursor;

	bool simulateMoreThanOne; // for debugging purposes
};


#endif
