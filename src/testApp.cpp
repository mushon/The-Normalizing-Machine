#include "testApp.h"
#define PROFILE
#include "ofxProfile.h"



//--------------------------------------------------------------
void testApp::setup() {

	isLive			= true;
	isTracking		= true;
	isTrackingHands	= true;
	isFiltering		= false;
	isRecording		= false;
	isCloud			= false;
	isCPBkgnd		= true;
	isMasking		= true;

	n_players = 0;

	setupRecording();
	setupPlayback("e:\\t1.oni");
	setupPlayback("E:\\t2.oni");
	setupPlayback("e:\\t3.oni");
	setupPlayback("e:\\t4.oni");


	ofBackground(0, 0, 0);

}

void testApp::setupRecording(string _filename) {

	openNIRecorder.setup();
    openNIRecorder.addDepthStream();
    openNIRecorder.addImageStream();
    
	openNIRecorder.addUserTracker();
	
	//openNIRecorder.setUserSmoothing(filterFactor);				// built in openni skeleton smoothing...
	//openNIRecorder.setMaxNumUsers(1);					// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)

	openNIRecorder.addHandsTracker();
	/*
	openNIRecorder.addAllHandFocusGestures();    
	openNIRecorder.setHandSmoothing(filterFactor);
    
    openNIRecorder.setMaxNumHands(2);
	
	openNIRecorder.setRegister(true);
    openNIRecorder.setMirror(true);
	*/

	openNIRecorder.start(); //
}

void testApp::setupPlayback(string _filename) {
	//openNIPlayer.stop();

	//openNIPlayers.push_back(ofxOpenNI());
	//ofxOpenNI& player = openNIPlayers.back();
	
	openNIPlayers[n_players].setup(_filename.c_str());
    openNIPlayers[n_players].addDepthStream();
    openNIPlayers[n_players].addImageStream();
    openNIPlayers[n_players].start();
	n_players++;
	
	//if (n_players >= 4) n_players=0;

}

//--------------------------------------------------------------
void testApp::update(){


#ifdef TARGET_OSX // only working on Mac at the moment
	hardware.update();
#endif

	if (isLive) {

		// update all nodes

		ofxProfileSectionPush("openni update live");
		openNIRecorder.update();
		ofxProfileSectionPop();

		
		for (int i=0; i<n_players; i++)
		{
			stringstream ss;
			ss << "openni update ";
			ss << i;

			ofxProfile::sectionPush(ss.str());
			openNIPlayers[i].update();
			ofxProfileSectionPop();
	
		}

        // demo getting depth pixels directly from depth gen
		
		
		// TODO: addDepthThreshold(int _nearThreshold,

		//XXX depthRangeMask.setFromPixels( openNIRecorder.getDepthPixels(nearThreshold, farThreshold), recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);

		// update tracking/recording nodes
		//XXX recordUser.update();
		//XXX if (isRecording) oniRecorder.update();

	} else {

		// update all nodes
		//openNIPlayer.update();

		// demo getting depth pixels directly from depth gen
		//XXX depthRangeMask.setFromPixels(playDepth.getDepthPixels(nearThreshold, farThreshold),									 playDepth.getWidth(), playDepth.getHeight(), OF_IMAGE_GRAYSCALE);

		// update tracking/recording nodes
		//XXX if (isTracking) playUser.update();

		// demo getting pixels from user gen
		//XXX if (isTracking && isMasking) {
		//XXX allUserMasks.setFromPixels(playUser.getUserPixels(), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
		//XXX user1Mask.setFromPixels(playUser.getUserPixels(1), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
		//XXX user2Mask.setFromPixels(playUser.getUserPixels(2), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
		//XXX }
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	lastDump = ofxProfile::describe();
	
	ofxProfileThisFunction();

	ofSetColor(0);
	ofDrawBitmapString( lastDump, ofPoint( 640, 500 ) );
	ofSetColor(255);
	ofDrawBitmapString( lastDump, ofPoint( 641, 501 ) );

	//ofDrawBitmapString( " 'c' to clear profile data", ofPoint( 10, ofGetHeight()-20 ) );
	

	ofSetColor(255, 255, 255);

	glPushMatrix();
	glScalef(0.75, 0.75, 0.75);

	if (isLive) {


		ofxProfileSectionPush("drawDepth");
		openNIRecorder.draw();
		ofxProfileSectionPop();

		
		ofTranslate(320,10);
		for (int i=0; i<n_players; i++)
		{
			ofPushMatrix();
			ofScale(0.5, 0.5);
			ofTranslate(0, i * 480);
			openNIPlayers[i].draw();
			ofPopMatrix();
		}
		//openNIRecorder.drawImage(640,0,640,480);



	}
	glPopMatrix();

	ofSetColor(255, 255, 0);

	string statusPlay		= (string)(isLive ? "LIVE STREAM" : "PLAY STREAM");
	string statusRec		= (string)(!isRecording ? "READY" : "RECORDING");
//	string statusSkeleton	= (string)(isTracking ? "TRACKING USERS: " + (string)(isLive ? ofToString(openNIRecorder.getNumTrackedUsers()) : ofToString(openNIPlayer.getNumTrackedUsers())) + "" : "NOT TRACKING USERS");
//	string statusSmoothSkel = (string)(isLive ? ofToString(openNIRecorder.getUserSmoothing()) : ofToString(openNIPlayer.getUserSmoothing()));
//	string statusHands		= (string)(isTrackingHands ? "TRACKING HANDS: " + (string)(isLive ? ofToString(openNIRecorder.getNumTrackedHands()) : ofToString(openNIPlayer.getNumTrackedHands())) + ""  : "NOT TRACKING");
	string statusFilter		= (string)(isFiltering ? "FILTERING" : "NOT FILTERING");
//	string statusSmoothHand = (string)(isLive ? ofToString(openNIRecorder.getHandsSmoothing()) : ofToString(openNIPlayer.getHandsSmoothing()));
	string statusMask		= (string)(!isMasking ? "HIDE" : (isTracking ? "SHOW" : "YOU NEED TO TURN ON TRACKING!!"));
	string statusCloud		= (string)(isCloud ? "ON" : "OFF");
	string statusCloudData	= (string)(isCPBkgnd ? "SHOW BACKGROUND" : (isTracking ? "SHOW USER" : "YOU NEED TO TURN ON TRACKING!!"));

	string statusHardware;

	stringstream msg;

	msg
		<< "F: Fullscreen" << endl
	<< "    s : start/stop recording  : " << statusRec << endl
	<< "    p : playback/live streams : " << statusPlay << endl
//	<< "    t : skeleton tracking     : " << statusSkeleton << endl
//	<< "( / ) : smooth skely (openni) : " << statusSmoothSkel << endl
//	<< "    h : hand tracking         : " << statusHands << endl
	
	<< "    m : drawing masks         : " << statusMask << endl
	<< "    c : draw cloud points     : " << statusCloud << endl
	<< "    b : cloud user data       : " << statusCloudData << endl
	<< endl
	//XXX << "File  : " << openNIRecorder.getDevice(). g_Recorder.getCurrentFileName() << endl
	<< "FPS   : " << ofToString(ofGetFrameRate()) << "  " << statusHardware << endl;

	ofDrawBitmapString(msg.str(), 20, 560);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	float smooth;

	switch (key) {
		
		case'c':
			ofxProfile::clear();
			lastDump = "";
		break;
	


		case 's':
		case 'S':
			if (isRecording) {
				openNIRecorder.stopRecording();
				isRecording = false;

				//HACKHACK !!!
				//setupPlayback(lastRecordingFilename);
				break;


			} else {
				lastRecordingFilename = generateFileName();
				openNIRecorder.startRecording(lastRecordingFilename);
				isRecording = true;
				break;
			}
			break;
		case 'p':
		case 'P':

			//! XXX
			//! XXX if (openNIRecorder.getCurrentFileName() != "" && !isRecording && isLive) {
			//! XXX setupPlayback(oniRecorder.getCurrentFileName());
			//! XXX	isLive = false;
			//! XXX } else {
			//! XXX isLive = true;
			//! XXX }
			break;
		case 't':
		case 'T':
			isTracking = !isTracking;
			break;
		case 'h':
		case 'H':
			isTrackingHands = !isTrackingHands;
			if (!isTrackingHands)
			{
				//TODO: start tracking
			}
			else
			{
//				if(isLive) openNIRecorder.getHandsGenerator().StopTrackingAll();
			}
			break;
		case 'f':
			isFiltering = !isFiltering;
//XXX			recordHandTracker.isFiltering = isFiltering;
//XXX			playHandTracker.isFiltering = isFiltering;
			break;
		
		case 'F':
			ofToggleFullscreen();
			break;

		case 'b':
		case 'B':
			isCPBkgnd = !isCPBkgnd;
			break;
		case '9':
		case '(':
//			smooth = openNIRecorder.getUserSmoothing();
			if (smooth - 0.1f > 0.0f) {
//				openNIRecorder.setUserSmoothing(smooth - 0.1f);
			}
			break;
		case '0':
		case ')':
//		smooth = openNIRecorder.getUserSmoothing();	
			if (smooth + 0.1f <= 1.0f) {
//				openNIRecorder.setUserSmoothing(smooth + 0.1f);
			}
			break;
            
        case '2':
            //XXX recorder.finishMovie(); 
            break;
            
        case '1':
            //XXX recorder.startNewRecording(); 
            break; 
            
        default:
            break;
	}
}

string testApp::generateFileName() {

	string _root = "e:\\kinectRecord";

	string _timestamp = ofToString(ofGetDay()) +
	ofToString(ofGetMonth()) +
	ofToString(ofGetYear()) +
	ofToString(ofGetHours()) +
	ofToString(ofGetMinutes()) +
	ofToString(ofGetSeconds());

	string _filename = (_root + _timestamp + ".oni");

	return _filename;

}

void testApp::keyReleased(int key){
}

void testApp::mouseMoved(int x, int y ){
}

void testApp::mouseDragged(int x, int y, int button){
}

void testApp::mousePressed(int x, int y, int button){
}

void testApp::mouseReleased(int x, int y, int button){
}

void testApp::windowResized(int w, int h){
}

void testApp::exit(){ 

	openNIRecorder.stop(); 
	for (int i=0; i<n_players; i++)
	{
		openNIPlayers[i].stop();
	}

	Sleep(5000);
	ofxOpenNI::shutdown();
}