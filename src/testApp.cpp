#include "testApp.h"
#define PROFILE
#include "ofxProfile.h"



//--------------------------------------------------------------
void testApp::setup() {

	ofSetRectMode(OF_RECTMODE_CENTER);

	state = IDLE;

	spot = ofPoint(0, 0, 1600); // two meter from sensor

	isTracking		= true;
	isTrackingHands	= true;
	isRecording		= false;

	n_players = 0;

	setupRecording();
	setupPlayback("e:\\t0.oni");
	setupPlayback("E:\\t2.oni");
	setupPlayback("e:\\t3.oni");
	setupPlayback("e:\\t4.oni");


	drawDepth=false;
	drawGui=false;
	drawProfiler=false;
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
		if (state == IDLE)
		{
			userMessage << "Idle";
			selectedUser = SelectedUser(); //reset
		}

		else  //stop instructions / show warning with countdown
		{
			unsigned long long timeout = ofGetSystemTime() - lastTimeSeenUser; // counting up
			int countdown = stateResetTimeout - timeout;

			userMessage << "Countdown: " << countdown << endl;
			if (countdown < 0)
			{
				state = IDLE;
			}
		}		
	}
	else 
	{
		lastTimeSeenUser = ofGetSystemTime();
		float minDist = 99999;
		const float SPOT_RADIUS = 300.0f;

		switch (state)
		{
		case IDLE: //this happens only once in the transition
			{
				state = RECOGNITION;
				userMessage << "TODO: begin to show instructions";
				break;
			}

		case RECOGNITION:
			{
				userMessage << "TODO: show instructions" << endl;

				//select closest user to the spot (if there's more than one user in the scene)
				for(map<int, ofxOpenNIUser>::iterator it = openNIRecorder.trackedUsers.begin(); it != openNIRecorder.trackedUsers.end(); ++it)
				{
					ofxOpenNIUser& u = it->second;
					if (u.isVisible())
					{
						ofPoint headPoint = u.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;
						userMessage << headPoint << endl;

						float dist = ofVec2f(headPoint.x - spot.x, headPoint.z - spot.z).length(); // discard height(y)
						if (dist < minDist)
						{
							selectedUser.id = it->first;
							minDist = dist;
						}

					}
				}

				if (selectedUser.id != SelectedUser::NO_USER)
				{
					state = GOTO_SPOT;
				}
			}
		case GOTO_SPOT:
			{
				ofxOpenNIUser& u = openNIRecorder.trackedUsers[selectedUser.id];
				ofPoint headPoint = u.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;
				ofVec2f dist = ofVec2f(headPoint.x - spot.x, headPoint.z - spot.z); // discard height(y)


				if (dist.length() > SPOT_RADIUS) //TODO: extern var (gui)
				{
					userMessage << "go to the spot. Please move " 
						<< (dist.x < 0 ? "Right" : "Left")
						<< " and "
						<< (dist.y > 0 ? "Forward" : "Back") 
						<< endl;
					//TODO: instruct user to step into spot (visualy? top view)
				}
				else
				{
					state = RAISE_HAND;
				}
				break;
			}
		case RAISE_HAND:
			{

				ofxOpenNIUser& closestUser = openNIRecorder.trackedUsers[selectedUser.id];
				ofPoint headPoint = closestUser.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;
				ofPoint rightHandPoint = closestUser.getJoints().at(nite::JointType::JOINT_RIGHT_HAND).positionReal;

				ofVec2f dist = ofVec2f(headPoint.x - spot.x, headPoint.z - spot.z); // discard height(y)


				if (dist.length() > SPOT_RADIUS) //TODO: extern var (gui)
				{
					state = GOTO_SPOT;
				}
				else
				{
					ofPoint rightShoulder = closestUser.getJoints().at(nite::JointType::JOINT_RIGHT_SHOULDER).positionReal;
					if (rightHandPoint.y < rightShoulder.y)
					{
						userMessage << "waiting for hand Raise" << endl;
					}
					else
					{
						state = SELECTION;
					}
				}

				break;
			}
		case SELECTION:
			{
				ofxOpenNIUser& closestUser = openNIRecorder.trackedUsers[selectedUser.id];
				ofPoint headPoint = closestUser.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;
				ofVec2f dist = ofVec2f(headPoint.x - spot.x, headPoint.z - spot.z); // discard height(y)
				if (dist.length() > SPOT_RADIUS) //TODO: extern var (gui)
				{
					//give timeout?
					state = GOTO_SPOT;
				}
				else
				{
					userMessage << "TODO: instructions how to select" << endl;
					userMessage << "waiting for selection" << endl;

					//compute trajectory (shoulder/hand)
					ofPoint rightHandPoint = closestUser.getJoints().at(nite::JointType::JOINT_RIGHT_HAND).positionReal;
					ofPoint rightShoulder = closestUser.getJoints().at(nite::JointType::JOINT_RIGHT_SHOULDER).positionReal;

					selectedUser.updatePointingDir(rightHandPoint - rightShoulder);

					ofPoint p = selectedUser.pointingDir;
					userMessage << "pointing dir: " << p << endl;

					// TODO: sanity check if hand is +- at shoulder level
					ofVec2f v(p.x, p.y);

					float halfTouchScreenSize = 500;
					v /= halfTouchScreenSize; // virtual screen with size of 2 * halfTouchScreenSize 

					ofVec2f s(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
					v.map(s, ofVec2f(ofGetScreenWidth(), 0), ofVec2f(0, -1 * ofGetScreenHeight())); // reverse y, assume -1 < v.x, v.y < 1
					selectedUser.screenPoint = v;


					//TODO select mechanism (click/timeout)
					bool selected = false;
					if(selected)
					{
						//selected item (x out of 4)
						state = CONFIRMATION;

					}

				}


				break;

			}
		case CONFIRMATION:
			{
				break;
			}
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

	if (drawProfiler)
	{
		lastDump = ofxProfile::describe();

		ofxProfileThisFunction();

		ofSetColor(0);
		ofDrawBitmapString( lastDump, ofPoint( 640, 500 ) );
		ofSetColor(255);
		ofDrawBitmapString( lastDump, ofPoint( 641, 501 ) );
	}

	if (drawVideo) {

		int margin = 8; // TODO extern to gui
		int bottomMargin = 56;


		for (int i=0; i<n_players; i++)
		{

			ofPushMatrix();
			int dx = i%2;
			int dy = i/2;

			dx = 2*dx + 1; // map 0,1 to 1,3
			dy = 2*dy + 1;

			float sc = 0.5;


			ofTranslate(dx * (ofGetScreenWidth()) / 4, dy * (ofGetScreenHeight() - bottomMargin) / 4);

			//ofScale(sc, sc);

			//numbers in comments relate to screen size of width:768, height:1024 (Portrait mode!) 
			float w = (ofGetScreenWidth() - margin) / 2; //380
			float h = (ofGetScreenHeight() - margin - bottomMargin) / 2; //480
			float sx = (openNIPlayers[i].imageWidth - w) / 2; //130
			float sy = (openNIPlayers[i].imageHeight - h) / 2; //0

			openNIPlayers[i].drawImageSubsection(w, h, sx, sy);

			ofPopMatrix();
		}

		ofPushMatrix();
		ofTranslate(ofGetScreenWidth() / 2, (ofGetScreenHeight() - bottomMargin) / 2);
		ofxProfileSectionPush("draw live");

		float w = (ofGetScreenWidth() - margin) / 2; //380
		float h = (ofGetScreenHeight() - margin - bottomMargin) / 2; //480
		float sx = (openNIRecorder.imageWidth - w) / 2; //130
		float sy = (openNIRecorder.imageHeight - h) / 2; //0

		openNIRecorder.drawImageSubsection(w, h, sx, sy);
		ofxProfileSectionPop();
		ofPopMatrix();
	}
	if (state == RECOGNITION || state == GOTO_SPOT)
	{
		//draw user map

		ofPushMatrix();
		ofPushStyle();
		ofSetColor(ofColor::white);

		ofTranslate(10, 480);

		float w = 320;
		float h = 240;

		float xFactor = w/4000;
		float zFactor = h/4000;

		ofNoFill();
		ofRect(0, 0, w, h);
		ofDrawBitmapString("Sensor", w/2, 0);

		ofVec2f spot2d(w/2 + spot.x * xFactor, spot.z * zFactor);

		ofSetColor(ofColor::red);
		ofFill();

		ofCircle(spot2d, 18);
		ofSetColor(ofColor::white);

		for(map<int, ofxOpenNIUser>::iterator it = openNIRecorder.trackedUsers.begin(); it != openNIRecorder.trackedUsers.end(); ++it)
		{
			ofxOpenNIUser& u = it->second;
			if (u.isVisible())
			{
				ofPoint headPoint = u.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;
				ofVec2f v(w/2 + headPoint.x * xFactor, headPoint.z * zFactor);
				ofCircle(v, 5);
			}
		}

		ofPopStyle();
		ofPopMatrix();
	}

	if (state == SELECTION)
	{
		ofPushStyle();

		ofVec2f v = selectedUser.screenPoint;

		float zVal = -selectedUser.pointingDir.z; //reverse z
		float minHandShoulderDistance = 100; //10cm
		float maxHandShoulderDistance = 300; //30cm

		float minCursorSize = 3;
		float maxCursorSize = 30;

		ofNoFill();
		ofSetLineWidth(3);
		ofCircle(v, maxCursorSize);

		float zp = ofMap(zVal, minHandShoulderDistance, maxHandShoulderDistance , 0, 1, true);

		ofColor markerColor(ofColor::red);
		markerColor.lerp(ofColor::green, zp);
		ofSetColor(markerColor);

		float zl = ofMap(zVal, minHandShoulderDistance, maxHandShoulderDistance , minCursorSize, maxCursorSize, true);

		ofFill();
		ofCircle(v, zl);

		ofPopStyle();
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
		<< "State : " << stateToString(state) << endl
		<< "Height: " << openNIRecorder.imageHeight << endl
		<< "Width : " << openNIRecorder.imageWidth << endl;


	ofDrawBitmapString(msg.str(), 20, 560);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	float smooth;

	switch (key) {

	case 'c':
		ofxProfile::clear();
		lastDump = "";
		break;



	case 's':
	case 'S':
		if (isRecording) {
			stopRecording();
		}
		else
		{
			startRecording();
		}
		break;

	case 't':
	case 'T':
		isTracking = !isTracking;
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

	gui->addToggle("draw (g)ui", &drawGui)->bindToKey('g');
	gui->addToggle("draw (v)ideo", &drawVideo)->bindToKey('v');
	gui->addToggle("draw (p)rofiler", &drawProfiler)->bindToKey('p');
	gui->addToggle("draw (d)epth", &drawDepth)->bindToKey('d');


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

void testApp::startRecording()
{
	lastRecordingFilename = generateFileName();
	openNIRecorder.startRecording(lastRecordingFilename);
	isRecording = true;

	cout << "startRecording: " << lastRecordingFilename << endl;
}

void testApp::stopRecording()
{
	openNIRecorder.stopRecording();
	isRecording = false;

	//HACKHACK !!!
	//setupPlayback(lastRecordingFilename);
}
