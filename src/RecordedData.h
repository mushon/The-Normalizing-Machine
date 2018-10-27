#pragma once

//Json::Value

struct RecordedData
{

	static const int N_OTHERS = 2;
	RecordedData()
	{
		sessionId = "";
		id = "";

		for (int i = 0; i < N_OTHERS; i++)
		{
			othersId[i] = "";
			othersSelection[i] = false;
			othersPtr[i] = NULL;
		}

		vScore = 0;
		xScore = 0;

		totalHeight = 0;
		headHeight = 0;
		torsoLength = 0;
		shouldersWidth = 0;
	}

	RecordedData(Json::Value v)
	{
		id = v["id"].asString();
		sessionId = v["sessionId"].asString();

		for (int i = 0; i < N_OTHERS; i++)
		{
			othersId[i] = v["others"][i]["id"].asString();
			othersSelection[i] = v["others"][i]["selection"].asBool();
		}
		vScore = v["vScore"].asInt();
		xScore = v["xScore"].asInt();

		totalHeight = v["totalHeight"].asFloat();
		headHeight = v["headHeight"].asFloat();
		torsoLength = v["torsoLength"].asFloat();
		shouldersWidth = v["shouldersWidth"].asFloat();

	}


	void makeSelection(string _sessionId, string recordingFilename, int selection,
		float _totalHeight, float _headHeight, float _torsoLength, float _shouldersWidth)
	{
		id = recordingFilename;
		sessionId = _sessionId;
		othersSelection[selection] = true;

		totalHeight = _totalHeight;
		headHeight = _headHeight;	
		torsoLength = _torsoLength;
		shouldersWidth = _shouldersWidth;
	}

	Json::Value toJson()
	{
		Json::Value v;
		v["id"] = id;
		v["sessionId"] = sessionId;

		for (int i = 0; i < N_OTHERS; i++)
		{
			Json::Value other;
			other["id"] = othersId[i];
			other["selection"] = othersSelection[i];
			v["others"].append(other);
		}

		v["vScore"] = vScore;
		v["xScore"] = xScore;

		v["totalHeight"] = totalHeight;
		v["headHeight"] = headHeight;
		v["torsoLength"] = torsoLength;
		v["shouldersWidth"] = shouldersWidth;

		return v;
	}


	string id; //timestamp: ofGetTimestampString returns in this format: 2011-01-15-18-29-35-299
	string sessionId; // id is for one selection. this points to the first id
	string othersId[N_OTHERS];
	bool othersSelection[N_OTHERS];
	RecordedData* othersPtr[N_OTHERS];

	int scoreCount() const { return vScore + xScore; }
	int vScore; //how many scored you. to be updated globally
	int xScore;

	float totalHeight;		// distance(head, feet)
	float headHeight;		// distance(head, neck)
	float torsoLength;		// distance(neck, torso)
	float shouldersWidth;	// distance(shoulders)

	// time, file, location, selection v/x
	// string location;

};
