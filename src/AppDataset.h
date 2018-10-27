#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "RecordedData.h"

class AppDataset
{

public:

	void saveSession(const RecordedData& session);
	void updateScores(const RecordedData& session);

	void saveLibrary(string url);
	void loadLibrary(string url);

	vector<string> selectNextRound(string forcedId = "", string excludeSessionId = "");

	

private:

	typedef map<string, RecordedData> DataSet;
	DataSet dataset; // the whole shablang
	ofxJSONElement datasetJson;

	// on startup, find dirs (xml?)
	// load recordings


};