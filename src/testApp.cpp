#include "testApp.h"
#define PROFILE
#include "ofxProfile.h"



//--------------------------------------------------------------
void testApp::setup() {

	state = IDLE;

	isRecording		= false;

	n_players = 0;

	setupRecording();
	//setupPlayback("e:\\t0.oni");
	//setupPlayback("E:\\t2.oni");
	//setupPlayback("e:\\t3.oni");
	for (int i=0; i<25; ++i)
	{ 
		string filename = "records/t1.oni";
		setupPlayback(ofToDataPath(filename));
	}


	drawDepth=false;
	drawGui=false;
	drawProfiler=false;
	drawVideo=true;

	lastSeenUser.setTimeout(5000);

	yesIcon.loadImage("assets/i-yes-40.png");
	noIcon.loadImage("assets/i-no-40.png");

	txt_pointing.loadImage("assets/txt_pointing.png");
	txt_position.loadImage("assets/txt_position.png");
	txt_prompt.loadImage("assets/txt_prompt.png");


	//ofTrueTypeFont::setGlobalDpi(72);
	verdana.loadFont("fonts/verdana.ttf", 50, true, true);
	verdana.setLineHeight(54.0f);
	//verdana.setLetterSpacing(1.037);

	ofEnableAlphaBlending();

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
	ofVec2f minDist = ofVec2f(99999, 99999);
	float minId = SelectedUser::NO_USER;

	// HACK: nite internally counts down 10 seconds, even if user is not visible
	if(openNIRecorder.trackedUsers.size() > 0)
	{	
		for(map<int, ofxOpenNIUser>::iterator it = openNIRecorder.trackedUsers.begin(); it != openNIRecorder.trackedUsers.end(); ++it)
		{
			ofxOpenNIUser& u = it->second;
			if (u.isVisible())				
			{
				ofxOpenNIJoint j = u.getJoints().at(nite::JointType::JOINT_HEAD);

				if (j.positionConfidence < 0.5) continue;

				ofPoint headPoint = j.positionReal;
				nVisibleUsers++;


				userMessage << it->first << ":" << headPoint << endl;
				ofVec2f dist = ofVec2f(headPoint.x - spot.x, headPoint.z - spot.z); // discard height(y)    <<<--------------------------might be a hang here, consider other way of choosing
				if (dist.length() < minDist.length())
				{
					minId = it->first;
					minDist = dist;
				}
			}
		} // end for map

		if (minId != SelectedUser::NO_USER)
		{
			// keep track uf id (if changes in the middle)
			selectedUser.id = minId;
			selectedUser.dist = minDist;
			ofxOpenNIUser& u = openNIRecorder.trackedUsers.at(minId);
					
			selectedUser.headPoint = u.getJoints().at(nite::JointType::JOINT_HEAD).positionReal;

			ofxOpenNIJoint rhj = u.getJoints().at(nite::JointType::JOINT_RIGHT_HAND);
			ofxOpenNIJoint rsj = u.getJoints().at(nite::JointType::JOINT_RIGHT_SHOULDER);
			if (rhj.positionConfidence < 0.5 || rsj.positionConfidence < 0.5)
			{
				selectedUser.resetHandPoints();
			}
			else
			{
				ofPoint rightHand = rhj.positionReal;
				ofPoint rightShoulder = rsj.positionReal;
				selectedUser.updatePoints(rightHand, rightShoulder);
			}
		}
		else
		{
			selectedUser = SelectedUser(); //reset
		}
	}



	if (nVisibleUsers == 0)
	{
		if (state == IDLE)
		{
			//TODO: idle animations
		}
		else  //user left in the middle of interacion - stop instructions / show warning with countdown
		{
			// stop recording?
			if (lastSeenUser.getCountDown() < 0)
			{
				state = IDLE;
			}
		}		
	}
	else if (nVisibleUsers > 1 || simulateMoreThanOne)
	{
		state = MORE_THAN_ONE;
	}
	else 
	{
		lastSeenUser.reset();

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
				if (selectedUser.dist.length() > spotRadius + spotRadiusHysteresis)
				{
					state = GOTO_SPOT;
				}
				else
				{
					if (selectedUser.rightHand.z < selectedUser.rightShoulder.z + handShoulderDistance)
					{
						userMessage << "waiting for hand Raise" << endl;
					}
					else
					{
						if (selectedUser.isSteady())
						{
							state = SELECTION;
							selectedUser.hovered = -1;
						}
						else
						{
							userMessage << "Hold Steady" << endl;
						}
					}
				}

				break;
			}
		case SELECTION:
			{
				if (selectedUser.dist.length() > spotRadius + spotRadiusHysteresis)
				{
					//give timeout?
					state = GOTO_SPOT;
				}
				else
				{
					ofPoint p = selectedUser.getPointingDir();

					// TODO: sanity check if hand is +- at shoulder level
					ofVec2f v(p.x, p.y);
					v /= (touchScreenSize / 2); // <<<< There's alot of UI tweaking here, where the window sits (width = shoulders width?)

					//v.x = powf(fabs(v.x), 1.5) * (v.x > 0 ? 1 : -1); // should do some non linear function, 
					//v.y = powf(fabs(v.y), 1.5) * (v.y > 0 ? 1 : -1); // should do some non linear function, 
					//v.y = powf(v.y, 3); // only on x

					ofVec2f screenCenter(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
					selectedUser.screenPoint = v.getMapped(screenCenter, ofVec2f(ofGetScreenWidth()/2, 0), ofVec2f(0, -1 * ofGetScreenHeight()/2)); // reverse y, assume -1 < v.x, v.y < 1

					float progress = selectedUser.getProgress();
					cursor.update(selectedUser.screenPoint, progress);

					int hover = 0;
					if (v.x > 0) hover+=1;
					if (v.y < 0) hover+=2;

					float w = (ofGetScreenWidth() - margin) / 2; //380
					float height = (ofGetScreenHeight() - margin - bottomMargin) / 2; //480

					if (abs(selectedUser.screenPoint.x - (ofGetScreenWidth()/2)) < w/4 && abs(selectedUser.screenPoint.y - (ofGetScreenHeight()/2)) < height/4) //inside middle frame
					{
						hover = SelectedUser::NO_HOVER;
					}
					if (v.y < minBottomScreen) // hand down
					{
						hover = SelectedUser::NO_HOVER;
					}

					if (hover == SelectedUser::NO_HOVER || selectedUser.hovered != hover) //changed selection
					{
						selectedUser.hovered = hover;
						selectedUser.steady.reset();
						selectedUser.selectTimer.reset();
						selectedUser.waitForSteady = true;
					}


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

		case MORE_THAN_ONE:
			{
				state = IDLE;
			}
		}

	}

	int t = sqrt(n_players);
	for (int i=0; i<t; i++)
	{
		int j = rand() % MAX_PLAYERS;

		ofxProfileSectionPush(string("openni update ").append(ofToString(j)));
		openNIPlayers[j].update();
		ofxProfileSectionPop();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
/*	int w = ofGetScreenWidth() / s;
	int h = ofGetScreenHeight() / s;
	float iw = openNIRecorder.imageWidth;					//130
	float ih = openNIRecorder.imageHeight;				//0

	for (int i=0; i<n_players; i++)
	{
		ofPushMatrix();
		int x = i%s;
		int y = i/s;
		ofTranslate(x*w, y*h);
		ofScale(w / iw, h / ih);
		openNIPlayers[i].drawImage();
		ofPopMatrix();
	}
	return;
	*/
	
	//ofSetRectMode(OF_RECTMODE_CENTER);

	ofxProfileThisFunction();

	if (drawVideo) {

		int s = 5;

		//numbers in comments relate to screen size of width:768, height:1024 (Portrait mode!) 
		float w = (ofGetScreenWidth() - (s-1) * margin) / s;					//380
		float h = (ofGetScreenHeight() -(s-1) * margin - bottomMargin) / s;	//480
		float sx = (openNIRecorder.imageWidth - w) / 2;					//130
		float sy = (openNIRecorder.imageHeight - h) / 2;				//0

		float iw = openNIRecorder.imageWidth;					//130
		float ih = openNIRecorder.imageHeight;				//0


		for (int i=0; i<n_players; i++)
		{

			ofPushMatrix();
			// video order:
			// 0 1
			// 2 3
			int x = i%s;
			int y = i/s;

			ofTranslate(x*(w+margin), y*(h+margin));
			ofScale(2 * w / iw, 2 * h / ih);
			openNIPlayers[i].drawImage();
//			openNIPlayers[i].drawImageSubsection(w, h, sx, sy);

			ofPopMatrix();
			continue;

			int dx = 2*x - 1; // map 0,1 to -1,1
			int dy = 2*y - 1;

			playbackScale = 1.0f;
			if (state == MORE_THAN_ONE)
			{
				playbackScale = 0.66;

			}

			
			if (state == SELECTION && selectedUser.hovered != SelectedUser::NO_HOVER)
			{
				ofVec2f p = selectedUser.screenPoint;
				p -= ofVec2f(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);

				float x = ofMap(fabs(p.x), w/4, ofGetScreenWidth()/4, 0.0f, 1.0f, true);
				float y = ofMap(fabs(p.y), h/4, ofGetScreenHeight()/4, 0.0f, 1.0f, true);

				if (p.y < -1)
				{
					y = ofMap(p.y, -1, minBottomScreen, 1.0f, 0.0f, true); // fix jitter when hand is too low
				}

				float s01 = (x*y); // score
				float maxExpand = 0.3;
				float s = maxExpand * s01; 
				playbackScale = (i==selectedUser.hovered) ? 1.0f+s : 1.0f-s;

			}

			ofTranslate(ofGetScreenWidth()/2 + (dx * w/2 * playbackScale), (ofGetScreenHeight() - bottomMargin)/2 + (dy * h/2 * playbackScale));
			ofScale(playbackScale, playbackScale);

			openNIPlayers[i].drawImageSubsection(w, h, sx, sy);


			if (state == MORE_THAN_ONE)
			{
				// cover with 50% black
				int alphaImage = 128;
				ofSetColor(ofColor::black, alphaImage);
				ofFill();
				ofRect(0,0, w, h);
			}

			if (state == SELECTION && selectedUser.hovered != SelectedUser::NO_HOVER)
			{
				int alphaIcon = 255 * ofMap(selectedUser.getProgress(), 0.3, 0.7, 1, 0, true);
				
				ofSetColor(255, 255, 255, alphaIcon);
				ofImage& icon = (i==selectedUser.hovered) ? yesIcon : noIcon;
				icon.draw(0, -h/3);
			}

			ofPopMatrix();
		}

		ofPushMatrix();
		ofTranslate(ofGetScreenWidth() / 2, (ofGetScreenHeight() - bottomMargin) / 2);
		ofxProfileSectionPush("draw live");


		ofPushMatrix();
		float sc2 = 0.5;
		if (state == MORE_THAN_ONE)
		{
			sc2 = 1;
		}
		ofScale(sc2, sc2);
		if (state == MORE_THAN_ONE)
		{
			ofSetColor(ofColor::black);
			ofFill();
			ofRect(0,0, w + 2*margin, h + 2*margin);
		}		
		openNIRecorder.drawImageSubsection(w, h, sx, sy);
		ofPopMatrix();

		if (state == GOTO_SPOT)
		{	

			userMessage << "go to the spot. Please move " 
				<< (abs(selectedUser.dist.x - spotRadius < 0) ? "Right" : "Left")
				<< (abs(selectedUser.dist.x - spotRadius < 0) && abs(selectedUser.dist.y - spotRadius < 0) ? " and " : "")
				<< (abs(selectedUser.dist.y - spotRadius < 0)? "Forward" : "Back") << endl
				<< endl;

			ofSetLineWidth(3);
			ofSetColor(ofColor::green, userMapAlpha);
			ofLine(0, -h/2*sc2, 0, h/2*sc2);

			drawOverheadText(txt_position, h*sc2);

			//TODO: instruct user to step into spot (visualy? top view)
			//draw user map
			float maxZ = 4000.0f;		
			float factor = h*sc2/maxZ;

			ofPushMatrix();
			ofPushStyle();
			ofTranslate(0 , -h/2*sc2);

			ofScale(factor, factor);
			ofVec2f spot2d(spot.x, spot.z);

			ofSetColor(ofColor::green, userMapAlpha);
			ofFill();
			ofCircle(spot2d, spotRadius);
			ofNoFill();
			ofCircle(spot2d,spotRadius + spotRadiusHysteresis);



			ofLine(spot2d.x, spot2d.y, -spotRadius, spotRadius);

			ofNoFill();
			ofSetLineWidth(5);
			ofSetColor(ofColor::white, userMapAlpha);

			ofVec2f v(selectedUser.headPoint.x, selectedUser.headPoint.z);
			ofCircle(v, 200);


			ofPopStyle();
			ofPopMatrix();

			//draw arrow
		}



		if (state == RAISE_HAND)
		{
			drawOverheadText(txt_prompt, h*sc2);
		}

		if (state == SELECTION)
		{
			int alphaIcon = 255 * ofMap(selectedUser.getProgress(), 0.3, 0.7, 1, 0, true);

			ofSetColor(255, 255, 255, alphaIcon);
			drawOverheadText(txt_pointing, h*sc2);
			
		}





		ofxProfileSectionPop();
		ofPopMatrix();

	}

	if (state == SELECTION)
	{
		//userMessage << "waiting for selection... TODO: instructions how to select" << endl;
		//userMessage << "pointing dir: " << selectedUser.getPointingDir() << endl;
		cursor.draw();
	}


	if (drawDepth)
	{
		ofSetRectMode(OF_RECTMODE_CORNER);
		//openNIRecorder.drawDepth();
		openNIRecorder.draw();
	}

	drawDebugText();

	if (drawProfiler)
	{
		ofDrawBitmapString(ofxProfile::describe(), profilerPos);
	}



}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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

	case 'F':
		ofToggleFullscreen();
		break;

	case 'g':
		gui->toggleVisible();
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

	simulateMoreThanOne = false;
	gui->addToggle("simulate (m)ore 1", &simulateMoreThanOne)->bindToKey('m');




	gui->addSpacer();

	profilerPos = ofxUIVec3f(220, 0);
	//gui->add2DPad("profilerPos", ofxUIVec3f(0, ofGetScreenWidth()), ofxUIVec3f(0, ofGetScreenHeight()), &profilerPos);

	spotRadius = 400;
	gui->addSlider("spot radius", 0, 1000, &spotRadius);
	spotRadiusHysteresis = 100;
	gui->addSlider("sr Hysteresis", 0, 300, &spotRadiusHysteresis);

	spot.z = 1600; // distance from sensor [mm]
	gui->addSlider("spot Z", 500, 3000, &spot.z);

	userMapAlpha = 60;
	gui->addIntSlider("userMapAlpha", 0, 255, &userMapAlpha);


	handShoulderDistance = 200;
	gui->addIntSlider("handShoulderDistance", 100, 500, &handShoulderDistance);
	
	minBottomScreen = -1.2;
	gui->addSlider("minBottomScreen", -2, -1, &minBottomScreen);



	margin = 8;
	gui->addIntSlider("margin", 0, 24, &margin);

	bottomMargin = 56;
	gui->addIntSlider("margin", 0, 100, &bottomMargin);

	touchScreenSize = 600;		// virtual screen 
	gui->addSlider("touchScreenSize", 100, 1000, &touchScreenSize);


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

void testApp::drawOverheadText(ofImage& txt, int h)
{
	ofPoint pos(0, -h/2 + txt.getHeight()/2);
	ofSetColor(ofColor::black, 128);
	ofFill();
	//ofRect(pos, txt.getWidth(), txt.getHeight()); // text background

	ofSetColor(ofColor::white);
	txt.draw(pos);
}

void testApp::drawDebugText()
{

	ofSetColor(255, 255, 0);

	stringstream msg;
	msg
		<< "F: Fullscreen" << endl
		<< "s : start/stop recording: " << (isRecording ? "RECORDING":"READY") << endl
		<< endl
		//XXX << "File  : " << openNIRecorder.getDevice(). g_Recorder.getCurrentFileName() << endl
		<< "State : " << stateToString(state) << endl
		<< "Height: " << openNIRecorder.imageHeight << endl
		<< "Width : " << openNIRecorder.imageWidth << endl
		<< lastSeenUser.getCountDown() << endl
		<< "User Message: " << userMessage.str() << endl
		;

	ofDrawBitmapString(msg.str(), 220, 200);

}