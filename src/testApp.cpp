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

	nVisibleUsers = 0;
	float minDist = 99999;
	// HACK: nite internally counts down 10 seconds, even if user is not visible
	if(openNIRecorder.trackedUsers.size() > 0)
	{	
		for(map<int, ofxOpenNIUser>::iterator it = openNIRecorder.trackedUsers.begin(); it != openNIRecorder.trackedUsers.end(); ++it){
			ofxOpenNIUser& u = it->second;
			if (u.isVisible())				
			{
				nVisibleUsers++;

				ofPoint headPoint = u.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;
				ofVec2f dist = ofVec2f(headPoint.x - spot.x, headPoint.z - spot.z); // discard height(y)    <<<--------------------------might be a hang here, consider other way of choosing
				if (dist.length() < minDist)
				{
					selectedUser.id = it->first;
					selectedUser.headPoint = u.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;

					selectedUser.rightHand = u.getJoints().at(nite::JointType::JOINT_RIGHT_HAND).positionReal;
					selectedUser.rightShoulder = u.getJoints().at(nite::JointType::JOINT_RIGHT_SHOULDER).positionReal;
					selectedUser.dist = dist;

					minDist = dist.length();
				}

			}
		} // end for map
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
			// stop recording?
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


		switch (state)
		{
		case IDLE: //this happens only once in the transition
			{
				state = GOTO_SPOT;
				userMessage << "TODO: begin to show instructions";
				break;
			}


		case GOTO_SPOT:
			{
				if (selectedUser.id == SelectedUser::NO_USER)
				{
					state = IDLE;
				}

				if (selectedUser.dist.length() < spotRadius)
				{
					state = RAISE_HAND;
				}
				break;
			}
		case RAISE_HAND:
			{
				if (selectedUser.dist.length() > spotRadius)
				{
					state = GOTO_SPOT;
				}
				else
				{
					if (selectedUser.rightHand.y > selectedUser.rightShoulder.y)
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
				if (selectedUser.dist.length() > spotRadius)
				{
					//give timeout?
					state = GOTO_SPOT;
				}
				else
				{
					selectedUser.updatePoints(selectedUser.rightHand, selectedUser.rightShoulder);
					ofPoint p = selectedUser.getPointingDir();

					// TODO: sanity check if hand is +- at shoulder level
					ofVec2f v(p.x, p.y);

					float halfTouchScreenSize = 300;																	// <<<< There's alot of UI tweaking here, where the window sits (width = shoulders width?)
					v /= halfTouchScreenSize; // virtual screen with size of 2 * halfTouchScreenSize 

					v.x = powf(fabs(v.x), 1.5) * (v.x > 0 ? 1 : -1); // should do some non linear function, 
					v.y = powf(fabs(v.y), 1.5) * (v.y > 0 ? 1 : -1); // should do some non linear function, 
					//v.y = powf(v.y, 3); // only on x

					ofVec2f screenCenter(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
					selectedUser.screenPoint = v.getMapped(screenCenter, ofVec2f(ofGetScreenWidth()/2, 0), ofVec2f(0, -1 * ofGetScreenHeight()/2)); // reverse y, assume -1 < v.x, v.y < 1

					float progress = (ofGetSystemTime() % 1000) / 1000.0;
					cursor.update(selectedUser.screenPoint, progress);
					// TODO check if outside center video (from imageSubsection)
					// TODO count time for selection
					int h = 0;
					if (v.x > 0) h+=1;
					if (v.y < 0) h+=2;

					selectedUser.hovered = h;



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
		ofxProfileSectionPush(string("openni update ").append(ofToString(i)));
		openNIPlayers[i].update();
		ofxProfileSectionPop();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofxProfileThisFunction();

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




			ofTranslate(dx * (ofGetScreenWidth()) / 4, dy * (ofGetScreenHeight() - bottomMargin) / 4);

			if (state == SELECTION)
			{
				float sc = (i==selectedUser.hovered) ? 1.2 : 0.8; //or uncrop?
				ofScale(sc, sc);
			}

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

		ofScale(0.5, 0.5);
		openNIRecorder.drawImageSubsection(w, h, sx, sy);

		if (drawDepth)
		{
			//openNIRecorder.drawDepth();
			openNIRecorder.draw();

		}

		ofxProfileSectionPop();
		ofPopMatrix();
	}
	if (state == GOTO_SPOT)
	{

		userMessage << "go to the spot. Please move " 
			<< (selectedUser.dist.x < 0 ? "Right" : "Left")
			<< " and "
			<< (selectedUser.dist.y > 0 ? "Forward" : "Back") 
			<< endl;
		//TODO: instruct user to step into spot (visualy? top view)


		//draw user map

		ofPushMatrix();
		ofPushStyle();
		ofSetColor(ofColor::white);

		ofTranslate(OFX_UI_GLOBAL_CANVAS_WIDTH / 2, 580);

		float w = OFX_UI_GLOBAL_CANVAS_WIDTH;
		float h = 240;

		float xFactor = w/4000;
		float zFactor = h/4000;

		ofNoFill();
		ofRect(0, h/2, w, h);
		ofDrawBitmapString("Sensor", 0, 0);

		ofScale(xFactor, zFactor);
		ofVec2f spot2d(spot.x, spot.z);

		ofSetColor(ofColor::red);
		ofFill();

		ofCircle(spot2d, spotRadius);
		ofSetColor(ofColor::white);

		for(map<int, ofxOpenNIUser>::iterator it = openNIRecorder.trackedUsers.begin(); it != openNIRecorder.trackedUsers.end(); ++it)
		{
			ofxOpenNIUser& u = it->second;
			if (u.isVisible())
			{
				ofPoint headPoint = u.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;
				ofVec2f v(headPoint.x, headPoint.z);
				ofCircle(v, 100);
			}
		}

		ofPopStyle();
		ofPopMatrix();
	}

	if (state == SELECTION)
	{
		userMessage << "waiting for selection... TODO: instructions how to select" << endl;
		userMessage << "pointing dir: " << selectedUser.getPointingDir() << endl;

		cursor.draw();
	}


	ofSetColor(255, 255, 0);

	stringstream msg;
	msg
		<< "User Message: " << userMessage.str() << endl
		<< "F: Fullscreen" << endl
		<< "s : start/stop recording: " << (isRecording ? "RECORDING":"READY") << endl
		<< endl
		//XXX << "File  : " << openNIRecorder.getDevice(). g_Recorder.getCurrentFileName() << endl
		<< "State : " << stateToString(state) << endl
		<< "Height: " << openNIRecorder.imageHeight << endl
		<< "Width : " << openNIRecorder.imageWidth << endl;
	ofDrawBitmapStringHighlight(msg.str(), 220, 200);

	if (drawProfiler)
	{
		ofDrawBitmapStringHighlight(ofxProfile::describe(), profilerPos);
	}



}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	float smooth;

	switch (key) {

	case 'c':
		ofxProfile::clear();
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
	ofLogNotice("testApp exit");

	openNIRecorder.stop(); 
	for (int i=0; i<n_players; i++)
	{
		openNIPlayers[i].stop();
	}
	ofxOpenNI::shutdown();

	ofLogNotice("testApp exit OK");
}

void testApp::setupGui(){
	float dim = 16;

	gui = new ofxUISuperCanvas("Turing Normalizing Machine");
	// add FPS
	gui->addFPSSlider("FPS", 30)->setDrawOutline(true);
	gui->addToggle("draw (g)ui", &drawGui)->bindToKey('g');
	gui->addToggle("draw (v)ideo", &drawVideo)->bindToKey('v');
	gui->addToggle("draw (p)rofiler", &drawProfiler)->bindToKey('p');
	gui->addToggle("draw (d)epth", &drawDepth)->bindToKey('d');


	gui->addSpacer();

	profilerPos = ofxUIVec3f(220, 0);
	gui->add2DPad("profilerPos", ofxUIVec3f(0, ofGetScreenWidth()), ofxUIVec3f(0, ofGetScreenHeight()), &profilerPos);

	spotRadius = 200;
	gui->addSlider("spot radius", 0, 1000, &spotRadius);

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
