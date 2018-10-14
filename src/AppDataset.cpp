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
		string id = v["id"].asString();
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


RecordedData AppDataset::selectNextRound()
{
	return select2();
}


RecordedData AppDataset::select2()
{
	// 1 last one
	// 1 least scored

	RecordedData newRecord;

	DataSet::iterator maxit;

	maxit = std::max_element(dataset.begin(), dataset.end(), sortById); //latest
	newRecord.othersId[0] = maxit->id;
	newRecord.othersPtr[0] = &(*maxit);

	DataSet::iterator leastScored1 = std::max_element(dataset.begin(), dataset.end(), sortByScoreCount);
	DataSet::iterator first = dataset.begin();
	DataSet::iterator last = dataset.end();

	if (first != last)
	{
		while (++first != last)
		{
			if (first != maxit && sortByScoreCount(*first, *leastScored1))    // or: if (comp(*first,*smallest)) for version (2)
				leastScored1 = first;
		}
	}

	newRecord.othersId[1] = leastScored1->id;
	newRecord.othersPtr[1] = &(*leastScored1);

	return newRecord;
}

RecordedData AppDataset::select4() // legacy
{
		// 1 last one
	// 2 least times scored
	// 1 random

	RecordedData newRecord;

	DataSet::iterator maxit;

	maxit = std::max_element(dataset.begin(), dataset.end(), sortById); //latest
	newRecord.othersId[0] = maxit->id;
	newRecord.othersPtr[0] = &(*maxit);

	DataSet::iterator leastScored1 = std::max_element(dataset.begin(), dataset.end(), sortByScoreCount);
	DataSet::iterator first = dataset.begin();
	DataSet::iterator last = dataset.end();

	if (first != last)
	{
		while (++first != last)
		{
			if (first != maxit && sortByScoreCount(*first, *leastScored1))    // or: if (comp(*first,*smallest)) for version (2)
				leastScored1 = first;
		}
	}


	DataSet::iterator leastScored2;
	leastScored2 = std::max_element(dataset.begin(), dataset.end(), sortByScoreCount);

	for (DataSet::iterator it = dataset.begin(); it != dataset.end(); it++)
	{
		if (it == maxit)
			continue;

		if (it == leastScored1)
			continue;

		if (it->scoreCount() < leastScored2->scoreCount())
		{
			leastScored2 = it;
		}
	}


	newRecord.othersId[1] = leastScored1->id;
	newRecord.othersId[2] = leastScored2->id;

	newRecord.othersPtr[1] = &(*leastScored1);
	newRecord.othersPtr[2] = &(*leastScored2);

	//random
	DataSet::iterator randit = std::min_element(dataset.begin(), dataset.end(), sortById); //first, just as a fallback

	int r = rand() % dataset.size();
	for (int j = 0; j<r; j++)
	{
		randit++;
	}
	for (int i = 0; i<dataset.size(); i++)
	{
		if (randit == dataset.end())
			randit = dataset.begin();

		if (randit != maxit && randit != leastScored1 && randit != leastScored2)
		{
			break;
		}
		randit++;
	}
	newRecord.othersId[3] = randit->id;
	newRecord.othersPtr[3] = &(*randit);

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
