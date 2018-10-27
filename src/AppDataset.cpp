#include "AppDataSet.h"


void AppDataset::saveSession(const RecordedData& session)
{
	dataset[session.id] = session;
}

void AppDataset::saveLibrary(string url)
{
	ofLogNotice("saveLibrary");

	ofxJSONElement json;
	for (DataSet::iterator it = dataset.begin(); it != dataset.end(); it++)
	{
		if (it->first != "")
		{
			json.append(it->second.toJson());
		}
	}

	bool success = json.save(url, true);
	ofLogNotice("saveLibrary") << (success ? "OK" : "FAIL");
}


void AppDataset::loadLibrary(string url)
{

	// Now parse the JSON
	bool parsingSuccessful = datasetJson.open(url);

	if (parsingSuccessful)
	{
		ofLogNotice("loadLibrary") << datasetJson.getRawString(true);
	}
	else {
		ofLogNotice("loadLibrary") << "Failed to parse JSON.";
	}

	for (unsigned int i = 0; i < datasetJson.size(); ++i)
	{
		Json::Value v = datasetJson[i];
		string id = v["id"].asString();
		dataset[id] = (RecordedData(v));
	}
}


bool sortById(const RecordedData& lhs, const RecordedData& rhs)
{
	return lhs.id < rhs.id;
}
bool sortByScoreCount(const RecordedData& lhs, const RecordedData& rhs)
{
	return lhs.scoreCount() < rhs.scoreCount();
}


vector<string> AppDataset::selectNextRound(string forcedId, string excludeSessionId)
{
	vector<string> newRound;
	
	if (!forcedId.empty()) 
	{
		// pick forcedId
		newRound.push_back(forcedId);
	}

	
	// pick last selection
	DataSet::iterator maxIt = dataset.begin();
	for (auto it = dataset.begin(); it != dataset.end(); it++) {
		if (it->first > maxIt->first &&
			it->first != excludeSessionId) {
			maxIt = it;
		}
	}
	newRound.push_back(maxIt->first);

	if (newRound.size() < RecordedData::N_OTHERS) {
		// pick least scored
		DataSet::iterator leastScoredIt = dataset.begin(); // = std::min_element(dataset.begin(), dataset.end(), sortByScoreCount);
		for (auto it = dataset.begin(); it != dataset.end(); it++) {
			if (it->first != newRound[0] &&
				it->first != excludeSessionId &&
				it->second.scoreCount() < leastScoredIt->second.scoreCount()) {
				leastScoredIt = it;
			}
		}
		newRound.push_back(leastScoredIt->first);
	}

	if (rand() % 2 == 0) {
		swap(newRound[0], newRound[1]);
	}

	return newRound;
}


void AppDataset::updateScores(const RecordedData& session)
{
	for (int r = 0; r < RecordedData::MAX_ROUND_COUNT; r++)  // TODO FIXME
	{
		for (int i = 0; i < RecordedData::N_OTHERS; i++)  // TODO FIXME
		{
			RecordedData& other = dataset[session.othersId[r][i]];

			if (session.othersSelection[r][i])
			{
				other.vScore++;
			}
			else
			{
				other.xScore++;
			}
		}
	}
}