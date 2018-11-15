#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxUI.h"

#include "SelectedUser.h"
#include "AppCursor.h"
#include "AppTimer.h"
#include "State.h"

#include "RecordedData.h"
#include "FfmpegRecorder.h"
#include "AppDataset.h"
#include "ofxKinectCommonBridge.h"


//#define DO_WATCHDOG


#ifdef DO_WATCHDOG

#ifdef TARGET_WIN32
#include "Watchdog_Responder.h"
#endif

#endif




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
	ofxKinectCommonBridge kinect;
	static const string imageDir;

private:

	State state;

	// session
	RecordedData session;


	// DB records
	AppDataset dataset;
	string datasetJsonFilename;

	// Video records
	FfmpegRecorder recorder;
	string recDir;
	string generateFileName();

	// Video player
	void loadImages(string path, vector<ofImage*>& images);
	void setupPlayback(string path);
	static const unsigned int MAX_PLAYERS = 2;
	static const unsigned int MAX_IMAGES = 5;
	vector<ofImage*> players[MAX_PLAYERS];
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

	ofImage img_rounds[RecordedData::MAX_ROUND_COUNT]; // r#.png;
	ofImage img_rounds_active[RecordedData::MAX_ROUND_COUNT]; // r#_active.png;

	ofImage img_rounds_star;
	ofImage img_rounds_star_active;

	ofImage img_r_left;
	ofImage img_r_right;
	ofImage img_record;

	ofImage img_gapmarker;
	ofImage img_placemark;
	ofImage img_placemarker_body;
	ofImage img_placemarker_head;
	ofImage img_placemark_0_2_position;
	ofImage img_prompt_0_1_idle;
	ofImage img_prompt_0_2_position;
	ofImage img_prompt_0_3_onebyone;
	ofImage img_prompt_1_1_point;
	ofImage img_prompt_10_goodbye;
	ofImage img_prompt_2_1_moreNormal;
	ofImage img_prompt_9_turnLeft;

	ofImage img_goodbye;
	ofImage img_one_by_one;
	ofImage img_position_yourself;
	ofImage img_step_in;
	ofImage img_wellcome_msg;
	string roundsUsers [RecordedData::MAX_ROUND_COUNT+1];
	void puploateRoundUsers();



	void drawOverheadText(ofImage&, int x, int y, int w);

	void setupGui();
	ofxUIScrollableCanvas* gui;

	AppCursor cursor;
	bool lockCursorY;

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
	void drawPlayers();
	void drawSplitScreen(ofFbo& fbo);

	float progressSmooth = 1.0;
	float progressSmoothFactor = 0.0f;

	float playerFrameScale = 0.0f;
	float playerFrameScaleSmooth = 0.0f;
	float playerFrameScaleSmoothFactor = 0.8f;

	float getPlayerWidth() {
		return 480; // (ofGetScreenWidth() - margin) / 2; //380
	}
	float getPlayerHeight() {
		return 640; // (ofGetScreenHeight() - margin - bottomMargin) / 2; //480
	}

	float textY;

	AppTimer postSelectionTimer;
	//void setupNextRound(bool lastUser, string forcedId = "", string excludeSessionId = "");
	void testApp::setupNextRound(int round, string firstId = "", string secondId = "");

	void drawRoundSelections();
	float roundSelectionsScale = 0;
	float roundSelectionsScaleSmooth = 0;
	float roundSelectionsSmoothFactor = 0.8f;

	AppTimer resultTimer;
	AppTimer imgSeqTimer;

	ofFbo fbo;
	bool drawProjection = true;
	int wallAngle = 0;

	int selectionTimeout;		// ms
	int postSelectionTimeout;	// ms
	int recordingDuration;		// ms
	int welcomeDuration;	    //ms
	int resultTimeout;			// ms
	int imgSeqTimeout;			// ms
	
	bool faceRecorded = false;

	float selectionBufferWidth;
	void drawKinect();
	void drawFbo();
	ofPath frame;
	//static const ofRectangle cropImage;
	int welcomeTime = 0;
	//void setUpResult(string id);
	//int collapse = 0;
	//int collapseNum = 0;
	ofImage* resultImage;
	float kinectYPos = 0.0;
	int cropW = 3840;
	int cropH = 2160;
	int cropX = 0;
	int cropY = 0;
	int imgId;


#ifdef DO_WATCHDOG
	
#ifdef TARGET_WIN32
		unique_ptr<WatchDog_Responder> wdr;
#endif
	
#endif
};


#endif
