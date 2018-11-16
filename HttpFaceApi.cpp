#include "HttpFaceApi.h"

const string HttpFaceApi::action_url = "http://localhost/of-test/upload.php";

HttpFaceApi::HttpFaceApi()
{
	ofAddListener(httpUtils.newResponseEvent, this, &HttpFaceApi::newResponse);
	httpUtils.start();
}


HttpFaceApi::~HttpFaceApi()
{
}



//--------------------------------------------------------------
void HttpFaceApi::newResponse(ofxHttpResponse & response) {
	responseStr = ofToString(response.status) + ": " + (string)response.responseBody;
}

//--------------------------------------------------------------
void HttpFaceApi::setup(string imgFile) {
	ofxHttpForm form;
	form.action = action_url;
	form.method = OFX_HTTP_POST;
	form.addFormField("number", ofToString(counter));
	form.addFile("file", "ofw-logo.gif");
	httpUtils.addForm(form);
	requestStr = "message sent: " + ofToString(counter);
	counter++;
}

