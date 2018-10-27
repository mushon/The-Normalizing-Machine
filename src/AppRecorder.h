#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"

// facade for record functions
class AppRecorder
{

public:

	ofxOpenNI openNIRecorder;

	void setup();
	void start(string recDir, string filename, string ext = ".oni");
	void update();
	void stop();
	void abort();


	void draw() {
		openNIRecorder.draw();
	};

	void drawImageSubsection(float w, float h, float sx, float sy) {
		openNIRecorder.drawImageSubsection(w, h, sx, sy);
	}

	int countVisibleUsers();

	bool IsRecording() { return isRecording; }
	string getLastFilename() { return lastFilename; }

private:
	bool isRecording;
	string lastFilename;

};
