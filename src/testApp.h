#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxUI.h"

#include "SelectedUser.h"
#include "AppCursor.h"
#include "AppTimer.h"
#include "State.h"

#include "RecordedData.h"
#include "AppRecorder.h"

class testApp : public ofBaseApp{

	typedef list<RecordedData> DataSet;
	DataSet dataset; // the whole shablang
	ofxJSONElement datasetJson;
	string datasetJsonFilename;

	RecordedData currData;
	// on startup, find dirs (xml?)
	// load recordings

public:
	void setup();
	void begin();
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

	SelectedUser selectedUser;

private:

	State state;

	AppRecorder appRecorder;
	string recDir;

	void setupPlayback(string _filename);
	static const unsigned int MAX_PLAYERS = 4;
	ofxOpenNI players[MAX_PLAYERS];
	int n_players;


	ofPoint spot;
	float spotRadius;
	float spotRadiusHysteresis;
	float idleThreshold;
	float idleThresholdHysteresis;

	int userMapAlpha;
	int textAlpha;

	int handShoulderDistance;
	float outsideScreenFactor;

	float screenZ;
	float screenB;
	float screenT;
	float screenL;
	float screenR;


	bool drawText;
	void drawDebugText();
	stringstream userMessage;

	int margin;
	int bottomMargin;

	float touchScreenSize; // virtual screen 

	ofImage yesIcon;
	ofImage noIcon;
	ofImage yesIcon20;
	ofImage noIcon20;

	ofImage txt_pointing;
	ofImage txt_position;
	ofImage txt_prompt;
	ofImage txt_toomany;

	void drawOverheadText(ofImage&, int x, int y, int w);

	void setupGui();
	ofxUISuperCanvas* gui;

	AppCursor cursor;
	SelectedUser getClosestUser();
	int updateSelectedUser();

	bool simulateMoreThanOne; // for debugging purposes

	string getRecDirString(string url);
	bool testLoadLibrary;
	void loadLibrary();
	void saveLibrary();

	void saveSession();
	void saveSessionToDataSet();

	void updateScores();
	void select25();
	void select4();

};


#endif
