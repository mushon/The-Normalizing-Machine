#include "testApp.h"

using namespace ofxCv;

void testApp::setup() {
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	
//    cam.listDevices();
//    
//    cam.setDeviceID(3);

	cam.initGrabber(640, 480);
	
    
    
    //Openni
    
    isLive			= true;
    
    recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	//recordContext.setupUsingXMLFile();
//	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);
    
//	recordUser.setup(&recordContext);

    
    recordContext.toggleRegisterViewport();
	recordContext.toggleMirror();
    oniRecorder.setup(&recordContext, ONI_STREAMING);

    
    tracker.setup();


}

void testApp::update() {
//	cam.update();
    
    
    if (isLive){  
//	if(cam.isFrameNew()) {
        recordContext.update();
		recordDepth.update();
//        recordImage.update();

//      recordUser.update();

        ofPixels pix;

        unsigned char *pixels = recordImage.getPixels();
        
        pix.set(*pixels);
        
        tracker.update(toCv(pix));
//        
//        
//		position = tracker.getPosition();
//		scale = tracker.getScale();
//		orientation = tracker.getOrientation();
//		rotationMatrix = tracker.getRotationMatrix();
	}
}

void testApp::draw() {
	ofSetColor(255);
//	cam.draw(0, 0);
  //  recordImage.draw(0,0);
    recordDepth.draw(0,0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
	
//	if(tracker.getFound()) {
//		ofSetLineWidth(1);
//		tracker.draw();
//		
//		easyCam.begin();
//		ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
//		ofTranslate(640 / 2, 480 / 2);
//		applyMatrix(rotationMatrix);
//		ofScale(5,5,5);
//		ofDrawAxis(scale);
//		tracker.getObjectMesh().drawWireframe();
//		easyCam.end();
//	}
}

void testApp::keyPressed(int key) {
	if(key == 'r') {
		tracker.reset();
	}
}