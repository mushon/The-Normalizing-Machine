#pragma once

//Json::Value

struct RecordedData
{

	static const int N_OTHERS = 2;
	static const int MAX_ROUND_COUNT = 5;

	RecordedData()
	{
		id = "";
		
		for (int r = 0; r < MAX_ROUND_COUNT; r++) {	
			for (int i = 0; i < N_OTHERS; i++)
			{
				othersId[r][i] = "";
				othersSelection[r][i] = false;
			}
		}

		totalHeight = 0;
		headHeight = 0;
		torsoLength = 0;
		shouldersWidth = 0;
		armLength = 0;

		vScore = 0;
		xScore = 0;
	}

	RecordedData(Json::Value v)
	{
		id = v["id"].asString();
		
		for (int r = 0; r < MAX_ROUND_COUNT; r++) {
			for (int i = 0; i < N_OTHERS; i++)
			{
				othersId[r][i] = v["others"][r][i]["id"].asString();
				othersSelection[r][i] = v["others"][r][i]["selection"].asBool();
			}
		}

		vScore = v["vScore"].asInt();
		xScore = v["xScore"].asInt();

		totalHeight = v["totalHeight"].asFloat();
		headHeight = v["headHeight"].asFloat();
		torsoLength = v["torsoLength"].asFloat();
		shouldersWidth = v["shouldersWidth"].asFloat();
		armLength = v["armLength"].asFloat();
	}

	Json::Value toJson()
	{
		Json::Value v;
		v["id"] = id;

		for (int r = 0; r < MAX_ROUND_COUNT; r++) {
			Json::Value round;
			for (int i = 0; i < N_OTHERS; i++) {
				Json::Value other;
				other["id"] = othersId[r][i];
				other["selection"] = othersSelection[r][i];
				round.append(other);
			}
			v["others"].append(round);
		}

		v["vScore"] = vScore;
		v["xScore"] = xScore;

		v["totalHeight"] = totalHeight;
		v["headHeight"] = headHeight;
		v["torsoLength"] = torsoLength;
		v["shouldersWidth"] = shouldersWidth;
		armLength = v["armLength"].asFloat();

		return v;
	}


	void makeSelection(int selection)
	{
		int r = currentRound();
		othersSelection[r][selection] = true;
		roundSelections.push_back(selection);
	}


	void saveUserMeasurements(float _totalHeight, float _headHeight, float _torsoLength, float _shouldersWidth, float _armLength)
	{
		totalHeight = _totalHeight;
		headHeight = _headHeight;
		torsoLength = _torsoLength;
		shouldersWidth = _shouldersWidth;
		armLength = _armLength;
	}

	void setupNextRound(vector<string> nextIds) {
		int r = currentRound();
		for (int i = 0; i < nextIds.size(); i++) {
			othersId[r][i] = nextIds[i];
		}
	}

	int currentRound() {
		return roundSelections.size();
	}

	string id; //timestamp: ofGetTimestampString returns in this format: 2011-01-15-18-29-35-299
	string othersId[MAX_ROUND_COUNT][N_OTHERS];
	bool othersSelection[MAX_ROUND_COUNT][N_OTHERS];
	vector<int> roundSelections;

	float getScore() const { 
		return vScore / (vScore + xScore + 1);
	}
	int scoreCount() const { return vScore + xScore; }
	int vScore; //how many scored you. to be updated globally
	int xScore;

	float totalHeight;		// distance(head, feet)
	float headHeight;		// distance(head, neck)
	float torsoLength;		// distance(neck, torso)
	float shouldersWidth;	// distance(shoulders)
	float armLength;        // arm from elbow to wrist


	// time, file, location, selection v/x
	// string location;

};
