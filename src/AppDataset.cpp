#include "AppDataSet.h"


void AppDataset::saveSession(const RecordedData& record)
{
	dataset.push_back(record);
	updateScores(record);
}

void AppDataset::saveLibrary(string url)
{
	ofLogNotice("saveLibrary");

	ofxJSONElement json;
	for (DataSet::iterator it = dataset.begin(); it != dataset.end(); it++)
	{
		if (it->id != "")
		{
			json.append(it->toJson());
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
		// string id = v["id"].asString();
		dataset.push_back(RecordedData(v));
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


RecordedData AppDataset::selectNextRound(string forcedId, string excludeSessionId)
{
	RecordedData newRecord;
	int i = 0;
	
	if (!forcedId.empty()) 
	{
		// pick forcedId
		DataSet::iterator forceIt;
		for (auto it = dataset.begin(); it != dataset.end(); it++) {
			if (it->id == forcedId) {
				forceIt = it;
				break;
			}
		}
		newRecord.othersId[i] = forceIt->id;
		newRecord.othersPtr[i] = &(*forceIt);
		i++;
	}

	
	// pick last selection
	DataSet::iterator maxIt = dataset.begin();
	for (auto it = dataset.begin(); it != dataset.end(); it++) {
		if (it->id > maxIt->id &&
			it->sessionId != excludeSessionId) {
			maxIt = it;
		}
	}
	newRecord.othersId[i] = maxIt->id;
	newRecord.othersPtr[i] = &(*maxIt);
	i++;

	if (i < 2) {
		// pick least scored
		DataSet::iterator leastScoredIt = dataset.begin(); // = std::min_element(dataset.begin(), dataset.end(), sortByScoreCount);
		for (auto it = dataset.begin(); it != dataset.end(); it++) {
			if (it->id != newRecord.othersId[0] &&
				it->sessionId != excludeSessionId &&
				it->scoreCount() < leastScoredIt->scoreCount()) {
				leastScoredIt = it;
			}
		}
		newRecord.othersId[i] = leastScoredIt->id;
		newRecord.othersPtr[i] = &(*leastScoredIt);
		i++;
	}

	if (rand() % 2 == 0) {
		swap(newRecord.othersId[0], newRecord.othersId[1]);
		swap(newRecord.othersPtr[0], newRecord.othersPtr[1]);
	}

	return newRecord;
}



void AppDataset::updateScores(const RecordedData& data)
{
	for (int i = 0; i < 2; i++)  // TODO FIXME
	{
		RecordedData* other = data.othersPtr[i];

		if (data.othersSelection[i])
		{
			other->vScore++;
		}
		else
		{
			other->xScore++;
		}
	}
}
