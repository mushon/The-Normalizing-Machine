#pragma once

#include "ofMain.h"
#include "ofxBlackmagic.h"


class testApp : public ofBaseApp{
	public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
	void startRecording(int tracked_id);
	void stopRecording(void);
    
    ofxBlackmagic   mVidGrabber;
    ofImage         mGrayscaleImage;
    
    bool            mZoom;

};
