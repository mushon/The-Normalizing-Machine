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
#include "AppDataset.h"
#include "ImageSaver.h"


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
	bool drawCursor;

	SelectedUser selectedUser;

private:

	State state;

	// session
	RecordedData session;


	// DB records
	AppDataset dataset;
	string datasetJsonFilename;

	// Video records
	AppRecorder appRecorder;
	string recDir;
	string generateFileName();

	// Video player
	void setupPlayback(string _filename);
	static const unsigned int MAX_PLAYERS = 2;
	ofxOpenNI players[MAX_PLAYERS];
	float playbackScales[MAX_PLAYERS];
	int n_players;
	float selectionScaleSmoothed[MAX_PLAYERS] = { 1.0f, 1.0f };
	float selectionScaleSmoothFactor = 0.7f;
	float selectionMaxExpand = 0.2;

	ofPoint spot;
	float spotRadius;
	float spotRadiusHysteresis;
	float idleThreshold;
	float idleThresholdHysteresis;
	float stepInThreshold;
	float stepInThresholdHysteresis;

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


	ofImage yesIcon;
	ofImage noIcon;
	ofImage yesIcon20;
	ofImage noIcon20;

	ofImage txt_pointing;
	ofImage txt_position;
	ofImage txt_prompt;
	ofImage txt_toomany;


	ofImage img_arrow_left;
	ofImage img_face_left;
	ofImage img_placemark_body;
	ofImage img_placemark_head;

	ofImage img_rounds[RecordedData::MAX_ROUND_COUNT + 1]; // r#.png;
	ofImage img_rounds_active[RecordedData::MAX_ROUND_COUNT + 1]; // r#_active.png;

	ofImage img_r_left;
	ofImage img_r_right;
	ofImage img_record;

	void drawOverheadText(ofImage&, int x, int y, int w);

	void setupGui();
	ofxUISuperCanvas* gui;

	AppCursor cursor;
	SelectedUser getClosestUser();
	void updateSelectedUser();

	bool simulateMoreThanOne; // for debugging purposes

	string getRecDirString(string url);
	bool testLoadLibrary;

	void drawGotoSpot();
	void drawLiveFrame();
	float liveFrameScale = 0.0f;
	float liveFrameScaleSmooth = 0.0f;
	float liveFrameScaleSmoothFactor = 0.8f;

	void drawOverlay();
	void drawIconAnimations(int i);
	void drawTotalScore(int i);
	void drawPlayers();
	void drawSplitScreen(ofFbo& fbo);

	float progressSmooth = 1.0;
	float progressSmoothFactor = 0.6f;

	float playerFrameScale = 0.0f;
	float playerFrameScaleSmooth = 0.0f;
	float playerFrameScaleSmoothFactor = 0.8f;

	float getPlayerWidth() {
		return 380; // (ofGetScreenWidth() - margin) / 2; //380
	}
	float getPlayerHeight() {
		return 480; // (ofGetScreenHeight() - margin - bottomMargin) / 2; //480
	}

	AppTimer postSelectionTimer;
	void setupNextRound(string forcedId = "", string excludeSessionId = "");

	void drawRoundSelections();
	float roundSelectionsScale = 0;
	float roundSelectionsScaleSmooth = 0;
	float roundSelectionsSmoothFactor = 0.8f;

	AppTimer resultTimer;

	ofFbo fbo;
	bool drawProjection = true;
	int wallAngle = -20;

	int selectionTimeout;		// ms
	int postSelectionTimeout;	// ms
	int recordingDuration;		// ms
	int resultTimeout;			// ms
	ImageSaver imageSaver;
	bool faceRecorded = false;
	static const string DATA_PATH_ROOT;
	static const string IMAGE_DIR;
};


#endif