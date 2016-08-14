#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"

// facade for record functions
class AppRecorder
{

	bool isRecording;
	ofxOpenNI openNIRecorder;
	string lastFilename;

public:

	bool isOn() { return isRecording; }

	void setup(string _filename = "");
	void start();
	void save();
	void stop();
	void abort();

	void update();

	void draw() {
		openNIRecorder.draw();
	};

	void drawImageSubsection(float w, float h, float sx, float sy) {
		openNIRecorder.drawImageSubsection(w, h, sx, sy);
	}


	string getLastFilename() { return lastFilename; }
};
