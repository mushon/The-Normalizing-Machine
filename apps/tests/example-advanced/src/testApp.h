#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenNI.h"

using namespace ofxCv;
using namespace cv;

#include "ofxFaceTracker.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	ofVec2f position;
	float scale;
	ofVec3f orientation;
	ofMatrix4x4 rotationMatrix;
	
	Mat translation, rotation;
	ofMatrix4x4 pose;

    ofEasyCam easyCam;

//OFXOPENNI:
    
    bool isLive;
    
    ofxUserGenerator	recordUser, playUser;
    ofxOpenNIContext	recordContext, playContext;
	ofxDepthGenerator	recordDepth, playDepth;
    ofxImageGenerator	recordImage, playImage;
	ofxOpenNIRecorder	oniRecorder;

	
};
