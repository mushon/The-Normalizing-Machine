#pragma once
#include "ofMain.h"
#include "ofxHttpUtils.h"

class HttpFaceApi
{
public:
	HttpFaceApi();
	~HttpFaceApi();
	void newResponse(ofxHttpResponse & response);
	void setup(string imgFile);

	ofxHttpUtils httpUtils;
	int counter;
	string responseStr;
	string requestStr;
	static const string action_url;
};



