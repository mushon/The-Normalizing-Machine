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
#include "ofxJSON\src\ofxJSON.h"


using namespace ofxCv;
using namespace cv;

struct RecordedData
{
	static const int N_OTHERS = 4;
	RecordedData()
	{
	}

	RecordedData(Json::Value v)
	{
		id = v["id"].asString();

		for (int i=0; i<N_OTHERS; i++)
		{
			othersId[i] = v["others"][i]["id"].asString();
			othersSelection[i] = v["others"][i]["selection"].asBool();
		}
		vScore = v["vScore"].asInt();
		xScore = v["xScore"].asInt();
	}

	Json::Value toJson()
	{
		Json::Value v;
		v["id"] = id;

		for (int i=0; i<N_OTHERS; i++)
		{
			Json::Value other;
			other["id"] = othersId[i];
			other["selection"] = othersSelection[i];
			v["others"].append(other);
		}

		v["vScore"] = vScore;
		v["xScore"] = xScore;

		return v;
	}


	string id; //timestamp: ofGetTimestampString returns in this format: 2011-01-15-18-29-35-299
	string othersId[N_OTHERS];
	bool othersSelection[N_OTHERS];

	int scoreCount() const {return vScore + xScore;}
	int vScore; //how many scored you. to be updated globally
	int xScore;

	// time, file, location, selection v/x
	string location; 
};

class testApp : public ofBaseApp{

	typedef vector<RecordedData> DataSet;
	DataSet dataset; // the whole shablang
	ofxJSONElement datasetJson;
	string datasetJsonFilename;

	RecordedData currData;
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
		IDLE, GOTO_SPOT, RAISE_HAND, SELECTION, RESULT, MORE_THAN_ONE
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
			X(RESULT);
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

	static const unsigned int MAX_PLAYERS = 25;
	ofxOpenNI openNIRecorder;

	ofDirectory dir;
	ofxOpenNI players[MAX_PLAYERS];

	int playersRowSize;
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

	string recDir;
	
	void startRecording();
	void saveRecording();
	void stopRecording();
	void abortRecording();

	AppCursor cursor;

	bool simulateMoreThanOne; // for debugging purposes


	bool testLoadLibrary;
	void loadLibrary();
	void saveLibrary();

	void saveSessionToDataSet();

	void updateScores();
	void select25();
	void select4();

};


#endif
