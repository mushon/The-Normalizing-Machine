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

struct RecordedData
{
	static const int N_OTHERS = 4;
	RecordedData()
	{
		id = "";

		for (int i=0;i<N_OTHERS;i++)
		{
			othersId[i] = "";
			othersSelection[i] = false;
			othersPtr[i] = NULL;
		}

		vScore = 0;
		xScore = 0;

		// time, file, location, selection v/x
		location = ""; 

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
	RecordedData* othersPtr[N_OTHERS];

	int scoreCount() const {return vScore + xScore;}
	int vScore; //how many scored you. to be updated globally
	int xScore;

	// time, file, location, selection v/x
	string location; 
};

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



	AppTimer lastSeenUser;
	SelectedUser selectedUser;



private:
	void	setupRecording(string _filename = "");
	void	setupPlayback(string _filename);
	string	generateFileName();

	bool isRecording;

	static const unsigned int MAX_PLAYERS = 4;
	ofxOpenNI openNIRecorder;

	ofDirectory dir;
	ofxOpenNI players[MAX_PLAYERS];

	int playersRowSize;
	int n_players;

	float playbackScale;

	string lastRecordingFilename;

	string lastDump;

	State state;

	int nVisibleUsers;
	stringstream userMessage;

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

	string recDir;

	void startRecording();
	void saveRecording();
	void stopRecording();
	void abortRecording();

	AppCursor cursor;
	SelectedUser getClosestUser();
	int updateSelectedUser();
	int getVisibleUsers();
	

	bool simulateMoreThanOne; // for debugging purposes

	string getRecDirString(string url);
	bool testLoadLibrary;
	void loadLibrary();
	void saveLibrary();

	void saveSessionToDataSet();

	void updateScores();
	void select25();
	void select4();

};


#endif
