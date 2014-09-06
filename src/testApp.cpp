#include "testApp.h"
#define PROFILE
#include "ofxProfile.h"



//--------------------------------------------------------------
void testApp::setup() {

	state = State::Idle;
	spot = ofPoint(0, 0, 2000); // two meter from sensor

	isTracking		= true;
	isTrackingHands	= true;
	isRecording		= false;

	n_players = 0;

	setupRecording();
	setupPlayback("e:\\t111.oni");
	setupPlayback("E:\\t2.oni");
	setupPlayback("e:\\t3.oni");
	setupPlayback("e:\\t4.oni");


	drawVideo=false;

	setupGui();
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
	userMessage = stringstream();

	ofxProfileSectionPush("openni update live");
	openNIRecorder.update();
	ofxProfileSectionPop();

	int nVisibleUsers = 0;
	// HACK: nite internally counts down 10 seconds, even if user is not visible
	if(openNIRecorder.trackedUsers.size() > 0)
	{	
		for(map<int, ofxOpenNIUser>::iterator it = openNIRecorder.trackedUsers.begin(); it != openNIRecorder.trackedUsers.end(); ++it){
			ofxOpenNIUser& u = it->second;
			if (u.isVisible()) nVisibleUsers++;
		}
	}



	if (nVisibleUsers == 0)
	{
		if (state == Idle)
		{
			userMessage << "Idle";
		}
		else  //stop instructions / show warning with countdown
		{
			unsigned long long timeout = ofGetSystemTime() - lastTimeSeenUser; // counting up
			int countdown = stateResetTimeout - timeout;

			userMessage << "Countdown: " << countdown << endl;
			if (countdown < 0)
			{
				state = Idle;
			}
		}		
	}
	else 
	{
		lastTimeSeenUser = ofGetSystemTime();

		switch (state)
		{
		case Idle: //this happens only once in the transition
			state = Recognition;
			userMessage << "TODO: begin to show instructions";
			break;

		case Recognition:
			userMessage << "TODO: show instructions" << endl;
			userMessage << "waiting for hand Raise" << endl;


			//select closest user to the spot
			for(map<int, ofxOpenNIUser>::iterator it = openNIRecorder.trackedUsers.begin(); it != openNIRecorder.trackedUsers.end(); ++it)
			{
				ofxOpenNIUser& u = it->second;
				if (u.isVisible())
				{
					ofPoint headPoint = u.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;
					userMessage << headPoint << endl;


				}
				
				
			}

			break;

		}

	}
	



	for (int i=0; i<n_players; i++)
	{
		stringstream ss;
		ss << "openni update " << i;

		ofxProfileSectionPush(ss.str());
		openNIPlayers[i].update();
		ofxProfileSectionPop();
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

	if (drawVideo) {

		for (int i=0; i<n_players; i++)
		{

			ofPushMatrix();
			int dx = i%2 + 1;
			int dy = i/2 + 1;

			float sc = 0.5;

			ofTranslate(dx * ofGetScreenWidth() / 3 - openNIPlayers[i].imageWidth * sc / 2,
				dy * ofGetScreenHeight() / 3 - openNIPlayers[i].imageHeight * sc / 2);

			ofScale(sc, sc);
			openNIPlayers[i].drawImage();

			ofPopMatrix();
		}

		ofPushMatrix();
		//ofTranslate(ofGetScreenWidth() / 2 - openNIRecorder.imageWidth / 2,
		//	ofGetScreenHeight() / 2 - openNIRecorder.imageHeight / 2);
		ofxProfileSectionPush("draw live");
		openNIRecorder.draw();
		ofxProfileSectionPop();
		ofPopMatrix();
	}

	ofSetColor(255, 255, 0);

	string statusRec		= (string)(!isRecording ? "READY" : "RECORDING");

	stringstream msg;

	msg
		<< "User Message: " << userMessage.str() << endl
		<< "F: Fullscreen" << endl
		<< "    s : start/stop recording  : " << statusRec << endl
		<< endl
		//XXX << "File  : " << openNIRecorder.getDevice(). g_Recorder.getCurrentFileName() << endl
		<< "FPS   : " << ofToString(ofGetFrameRate()) << endl
		<< "State : " << state << endl
		<< "Height: " << openNIRecorder.imageHeight << endl
		<< "Width : " << openNIRecorder.imageWidth << endl;


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

	case 'F':
		ofToggleFullscreen();
		break;

	case 'g':
		gui->toggleVisible();
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

	//	Sleep(5000);
	ofxOpenNI::shutdown();
}

void testApp::setupGui(){
	float dim = 16;

	gui = new ofxUISuperCanvas("Turing Normalizing Machine");

	gui->addToggle("draw (v)ideo", &drawVideo)->bindToKey('v');
	gui->addToggle("draw (g)ui", &drawGui)->bindToKey('g');

	gui->addSpacer();

	vector<string> states;
	states.push_back("Idle"); //video grid
	states.push_back("Recognition"); //instructions
	states.push_back("Selection"); //add face layer
	// start recording around raise hand gesture
	states.push_back("Confirmation"); //add face layer
	// present selection
	// record data:	
	// time, file, location, selction v/x
	//	gui->addRadio("State", states, OFX_UI_ORIENTATION_VERTICAL, dim, dim)->activateToggle(State.Idle); 


	gui->autoSizeToFitWidgets();
	ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);   
}


void testApp::guiEvent(ofxUIEventArgs &e)
{

	string name = e.getName();
	int kind = e.getKind();
	cout << "got event from: " << name << endl;

	if(name == "State")
	{
		ofxUIRadio *radio = (ofxUIRadio *) e.widget;
		cout << "value" << radio->getValue() << endl;
		cout << " active name: " << radio->getActiveName() << endl;
	}

}


std::ostream& operator<<( std::ostream& os, const testApp::State& state )
{
#define X(state) case testApp::State::state: os << #state; break;
	switch(state)
	{
		X(Idle);
		X(Recognition);
		X(Selection);
		X(Confirmation);
	}
#undef X

	return os;
}