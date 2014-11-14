#include "testApp.h"
#define PROFILE
#include "ofxProfile.h"



//--------------------------------------------------------------
void testApp::setup() {

	ofSetFrameRate(30);
	ofxOpenNI::shutdown();
	openni::Status rc = OpenNI::initialize();

	isRecording		= false;
	setupRecording();

	drawDepth=false;
	drawGui=false;
	drawProfiler=false;
	drawVideo=true;
	drawText=false;
	testLoadLibrary = true;

	lastSeenUser.setTimeout(3000);

	yesIcon20.loadImage("assets/i-yes-20.png");
	noIcon20.loadImage("assets/i-no-20.png");

	yesIcon.loadImage("assets/i-yes-40.png");
	noIcon.loadImage("assets/i-no-40.png");

	txt_pointing.loadImage("assets/txt_pointing.png");
	txt_position.loadImage("assets/txt_position.png");
	txt_prompt.loadImage("assets/txt_prompt.png");
	txt_toomany.loadImage("assets/txt_toomany.png");

	ofEnableAlphaBlending();

	setupGui();

	screenCenter = ofVec2f(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);

	ofBackground(0, 0, 0);

	loadLibrary();
	begin();
}

void testApp::setupRecording(string _filename)
{
	openNIRecorder.setup();
	openNIRecorder.addDepthStream();
	openNIRecorder.addImageStream();
	openNIRecorder.addUserTracker();
	//	openNIRecorder.addHandsTracker();
	openNIRecorder.start();
}

void testApp::setupPlayback(string _filename) {
	//player.stop();
	ofLogNotice("setupPlayback:") << _filename << endl;

	ofxOpenNI& player = players[n_players];
	n_players++;

	player.setup(_filename.c_str());
	//player.addDepthStream();
	player.addImageStream();
	player.start();

}

void testApp::begin()
{
	for (int i=0; i<n_players; i++)
	{
		players[i].stop();
	}
	n_players = 0;
	currData = RecordedData();
	select4();
	state = IDLE;
}
//--------------------------------------------------------------
void testApp::update(){
	
	static int lastMin=0;
	
	int mins = ofGetElapsedTimeMillis() / (1000 * 60); // (ofGetElapsedTimef());
	if (mins > lastMin)
	{
		lastMin = mins;
		ofLogNotice("Minutes Passed") << mins;
	}
	
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
				ofxOpenNIJoint j1 = u.getJoints().at(nite::JointType::JOINT_HEAD);
				ofxOpenNIJoint j2 = u.getJoints().at(nite::JointType::JOINT_RIGHT_SHOULDER);
				ofxOpenNIJoint j3 = u.getJoints().at(nite::JointType::JOINT_TORSO);


				if (j1.positionConfidence < 0.5 &&
					j2.positionConfidence < 0.5 &&
					j3.positionConfidence < 0.5)
					continue;

				ofPoint headPoint = j1.positionReal;
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
				selectedUser.reset();
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
			if (lastSeenUser.getCountDown() == 0)
			{
				state = IDLE;
			}
			if (state==RESULT)
			{
				begin();
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
				userMessage << selectedUser.dist.length() << endl;
				if (selectedUser.dist.length() < idleThreshold)
				{
					state = GOTO_SPOT;
					userMessage << "TODO: begin to show instructions";
				}
				break;
			}

		case GOTO_SPOT:
			{
				if (selectedUser.id == SelectedUser::NO_USER ||
					selectedUser.dist.length() > idleThreshold + idleThresholdHysteresis)
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
					if (selectedUser.hand.z < selectedUser.shoulder.z - handShoulderDistance)
					{
						if (selectedUser.isSteady())
						{
							selectedUser.reset();
							cursor = AppCursor();
							cursor.setPosition(screenCenter);
							state = SELECTION;
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
				if (selectedUser.hand.z > selectedUser.shoulder.z - handShoulderDistance)
				{
					if (isRecording) abortRecording();
					state = RAISE_HAND;
				}
				if (selectedUser.dist.length() > spotRadius + spotRadiusHysteresis)
				{
					if (isRecording) abortRecording();
					//give timeout?
					state = GOTO_SPOT;
				}
				else
				{
					ofPoint p = selectedUser.getPointingDir();

					float x = -(selectedUser.shoulder.z - screenZ) * p.x / p.z - screenL;
					float y = -(selectedUser.shoulder.z - screenZ) * p.y / p.z - screenB;

					float kx = (x-screenL) / (screenR - screenL);
					float ky = (y-screenB) / (screenT - screenB);


					// TODO: sanity check if hand is +- at shoulder level
					ofVec2f v(2*kx-1, 2*ky-1);
					//ofVec2f v(2*kx-1, 2*ky-1);

					//	v /= (touchScreenSize / 2); // <<<< There's alot of UI tweaking here, where the window sits (width = shoulders width?)
					selectedUser.screenPoint01 = v;

					//v.x = powf(fabs(v.x), 1.5) * (v.x > 0 ? 1 : -1); // should do some non linear function, 
					//v.y = powf(fabs(v.y), 1.5) * (v.y > 0 ? 1 : -1); // should do some non linear function, 
					//v.y = powf(v.y, 3); // only on x


					selectedUser.screenPoint = v.getMapped(screenCenter, ofVec2f(ofGetScreenWidth()/2, 0), ofVec2f(0, -ofGetScreenHeight()/2)); // reverse y, assume -1 < v.x, v.y < 1

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
					if (abs(v.x) > outsideScreenFactor || abs(v.y) > outsideScreenFactor) // hand down
					{
						hover = SelectedUser::NO_HOVER;
					}

					if (hover == SelectedUser::NO_HOVER || selectedUser.hovered != hover) //changed selection
					{
						selectedUser.hovered = hover;
						selectedUser.steady.reset();
						selectedUser.selectTimer.reset();
						selectedUser.waitForSteady = true;
						if (isRecording) abortRecording();
					}


					if (!isRecording && selectedUser.selectTimer.getCountDown() < 6000)
					{
						startRecording();
					}

					//TODO select mechanism (click/timeout)
					bool selected = (selectedUser.selectTimer.getCountDown() == 0);
					if(selected)
					{
						saveRecording();
						state = RESULT;
					}

				}
				break;

			}
		case RESULT:
			{
				//draw score
				//animate back to idle
				//change from live to recording
				if (selectedUser.dist.length() > spotRadius + spotRadiusHysteresis)
				{
					ofLogNotice("RESULT -> IDLE");
					begin();
				}				
				break;
			}

		case MORE_THAN_ONE:
			{
				state = IDLE;
			}
		}

	}

	for (int i=0; i<n_players; i++)
	{
		ofxProfileSectionPush(string("openni update ").append(ofToString(i)));
		players[i].update();
		ofxProfileSectionPop();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetRectMode(OF_RECTMODE_CENTER);

	ofxProfileThisFunction();

	if (drawVideo) {

		ofPushMatrix();
		//numbers in comments relate to screen size of width:768, height:1024 (Portrait mode!) 
		float w = (ofGetScreenWidth() - margin) / 2;					//380
		float h = (ofGetScreenHeight() - margin - bottomMargin) / 2;	//480

		userMessage << "w" << w << endl;
		userMessage << "h" << h << endl;

		ofPoint globalTranslation;
		float maxExpand = 0.2;
		float s = 0;

		if (state == SELECTION && selectedUser.hovered != SelectedUser::NO_HOVER)
		{
			ofVec2f p = selectedUser.screenPoint;
			p -= ofVec2f(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);

			float x = ofMap(fabs(p.x), w/4, ofGetScreenWidth()/4, 0.0f, 1.0f, true);
			float y = ofMap(fabs(p.y), h/4, ofGetScreenHeight()/4, 0.0f, 1.0f, true);

			if (abs(selectedUser.screenPoint01.x) > 1)
			{
				x = ofMap(abs(selectedUser.screenPoint01.x), 1, outsideScreenFactor, 1.0f, 0.0f, true); // fix jitter when hand is too low
			}
			if (abs(selectedUser.screenPoint01.y) > 1)
			{
				y = ofMap(abs(selectedUser.screenPoint01.y), 1, outsideScreenFactor, 1.0f, 0.0f, true); // fix jitter when hand is too low
			}

			float s01 = (x*y); // score
			s = maxExpand * s01; 

			//translate into screen (avoid spill)
			int tx = selectedUser.hovered % 2;
			int ty = selectedUser.hovered / 2;
			tx = 2*tx - 1; // map 0,1 to -1,1
			ty = 2*ty - 1;
			globalTranslation = ofPoint(-tx * s * w, -ty * s * h);
			ofTranslate(globalTranslation);
		}

		for (int i=0; i < 4; i++)
		{

			ofPushMatrix();
			// video order:
			// 0 1
			// 2 3
			int dx = i%2;
			int dy = i/2;

			dx = 2*dx - 1; // map 0,1 to -1,1
			dy = 2*dy - 1;

			playbackScale = 1.0f;
			if (state == MORE_THAN_ONE)
			{
				playbackScale = 0.66;
			}


			if (state == SELECTION && selectedUser.hovered != SelectedUser::NO_HOVER)
			{
				playbackScale = (i==selectedUser.hovered) ? 1.0f+s : 1.0f-s;
			}

			ofTranslate(ofGetScreenWidth()/2, ofGetScreenHeight()/2);
			ofTranslate(dx * (w+margin)/2 * playbackScale, dy * (h+margin)/2 * playbackScale);

			ofScale(playbackScale, playbackScale);

			ofRectangle border(0, 0, w+margin, h+margin);
			ofFill();
			ofSetColor(ofColor::black);
			ofRect(border);

			//			players[i].drawImage();
			players[i].drawImageSubsection(w, h, 0, 0);


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
				ofImage& icon = (i==selectedUser.hovered) ? yesIcon : noIcon;

				float transitionLength = 0.05;		
				float transitionBegin = (i==selectedUser.hovered) ? 0.4 : 0.5 + 0.05 * i;				
				int alphaIcon = ofMap(1 - selectedUser.getProgress(), transitionBegin, transitionBegin + transitionLength, 0, 255, true);

				//float iconScale = ofMap(selectedUser.getProgress(), transitionBegin, transitionBegin + transitionLength, 1.0f, 0.0f, true);
				float iconTrans = ofMap(1 - selectedUser.getProgress(), transitionBegin, transitionBegin + transitionLength, 0, 1, true);
				iconTrans = sqrt(iconTrans);
				iconTrans = ofMap(iconTrans, 0, 1, h * 0.75 + icon.height * 2, 0.0f, true);


				//ofSetColor(255, 255, 255, alphaIcon);
				ofPushMatrix();
				ofTranslate(0, iconTrans);
				//ofScale(iconScale, iconScale);
				ofSetColor(255, 255, 255, alphaIcon);
				icon.draw(0, 0);
				ofPopMatrix();

				if (i==selectedUser.hovered)
				{
					drawOverheadText(txt_pointing, -dx * (-w + txt_pointing.getWidth()) / 2, (h - txt_pointing.getHeight()) / 2, w);
				}
			}

			if (state == RESULT)
			{
				RecordedData* other = currData.othersPtr[i];

				for (int j=0;j<other->scoreCount();j++)
				{
					int iconSpacing = 5;
					ofImage& icon = (j < other->vScore) ? yesIcon20 : noIcon20;
					icon.draw(
						(-(other->scoreCount() - 1) / 2 + j) * (icon.width + iconSpacing), 0);
				}
			}

			ofPopMatrix();
		}


		if (state == GOTO_SPOT || state == RAISE_HAND || state == SELECTION || state == MORE_THAN_ONE) // not on IDLE or RESULT
		{
			//draw live frame
			ofPushMatrix();
			ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
			ofxProfileSectionPush("draw live");


			ofPushMatrix();
			float sc2 = 0.5;
			if (state == MORE_THAN_ONE || state == GOTO_SPOT)
			{
				sc2 = 1;
			}
			ofScale(sc2, sc2);

			//border
			ofSetColor(ofColor::black);
			ofFill();
			ofRect(0,0, w + 2*margin/sc2, h + 2*margin/sc2);


			openNIRecorder.drawImageSubsection(w, h, 0, 0);
			ofPopMatrix();

			//draw overlays

			if (state == GOTO_SPOT)
			{	

				userMessage << "go to the spot. Please move " 
					<< (abs(selectedUser.dist.x - spotRadius < 0) ? "Right" : "Left")
					<< (abs(selectedUser.dist.x - spotRadius < 0) && abs(selectedUser.dist.y - spotRadius < 0) ? " and " : "")
					<< (abs(selectedUser.dist.y - spotRadius < 0)? "Forward" : "Back") << endl
					<< endl;

				ofSetLineWidth(2);
				ofSetColor(ofColor::green, userMapAlpha);
				ofLine(0, -h/2*sc2, 0, h/2*sc2);

				drawOverheadText(txt_position, -sc2*w/2 + txt_position.getWidth()/2, sc2*h/2 - txt_position.getHeight()/2, w * sc2);

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
				ofCircle(spot2d,(spotRadius + spotRadiusHysteresis));



				//ofLine(spot2d.x - spotRadius, spot2d.y,spot2d.x + spotRadius, spot2d.y);

				ofNoFill();
				ofSetLineWidth(5);
				ofSetColor(ofColor::white, userMapAlpha);

				ofVec2f v(selectedUser.headPoint.x, selectedUser.headPoint.z);
				ofCircle(v, 200);


				ofPopStyle();
				ofPopMatrix();

				//draw arrow
			}

			if (state == MORE_THAN_ONE)
			{
				drawOverheadText(txt_toomany, -sc2*w/2 + txt_toomany.getWidth()/2, 0, w * sc2);
			}

			if (state == RAISE_HAND)
			{
				drawOverheadText(txt_prompt, -sc2*w/2 + txt_prompt.getWidth()/2, sc2*h/2 - txt_prompt.getHeight()/2, w * sc2);
			}

			ofxProfileSectionPop();
			ofPopMatrix();
		}

		ofPopMatrix();

		if (state == SELECTION)
		{
			//userMessage << "waiting for selection... TODO: instructions how to select" << endl;
			//userMessage << "pointing dir: " << selectedUser.getPointingDir() << endl;
			cursor.draw();
		}
	}




	if (drawDepth)
	{
		ofSetRectMode(OF_RECTMODE_CORNER);
		//openNIRecorder.drawDepth();
		openNIRecorder.draw();
	}

	if (drawText)
	{
		drawDebugText();
	}

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
	string timeFormat = "%Y_%m_%d_%H_%M_%S_%i";
	string name = ofGetTimestampString(timeFormat);
	string filename = (name + ".oni");
	return filename;
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

	for (int i=0; i<n_players; i++)
	{
		players[i].stop();
	}
	openNIRecorder.stop(); 

	ofxOpenNI::shutdown();

	ofLogNotice("testApp exit OK");
}

void testApp::setupGui(){
	float dim = 16;

	gui = new ofxUISuperCanvas("Turing Normalizing Machine");

	gui->addLabelButton("Save XML", false);

	//	recDir = "e:/records/";
	//	recDir = ofToDataPath("/records/");
	//  recDir = "C:/Users/SE_Shenkar/Dropbox/records/";
	
	recDir = getRecDirString(ofToDataPath("recDir.json"));

	gui->addLabel("Rec Dir", recDir);

	datasetJsonFilename = "dataset.json";
	gui->addLabel("datasetJsonFilename", datasetJsonFilename);


	// add FPS
	gui->addFPSSlider("FPS", 30)->setDrawOutline(true);
	gui->addToggle("draw (g)ui", &drawGui)->bindToKey('g');
	gui->addToggle("draw (v)ideo", &drawVideo)->bindToKey('v');
	gui->addToggle("draw (p)rofiler", &drawProfiler)->bindToKey('p');
	gui->addToggle("draw (d)epth", &drawDepth)->bindToKey('d');
	gui->addToggle("draw (t)ext", &drawText)->bindToKey('t');


	simulateMoreThanOne = false;
	gui->addToggle("simulate (m)ore 1", &simulateMoreThanOne)->bindToKey('m');

	gui->addSpacer();

	profilerPos = ofxUIVec3f(220, 0);
	//gui->add2DPad("profilerPos", ofxUIVec3f(0, ofGetScreenWidth()), ofxUIVec3f(0, ofGetScreenHeight()), &profilerPos);

	idleThreshold = 500;
	gui->addSlider("idleThreshold", 200, 2000, &idleThreshold);

	idleThresholdHysteresis = 100;
	gui->addSlider("idle.Thr.Hyst", 10, 200, &idleThresholdHysteresis);

	spotRadius = 400;
	gui->addSlider("spot radius", 0, 1000, &spotRadius);
	spotRadiusHysteresis = 100;
	gui->addSlider("sr Hysteresis", 0, 300, &spotRadiusHysteresis);

	spot.z = 1600; // distance from sensor [mm]
	gui->addSlider("spot Z", 500, 3000, &spot.z);

	userMapAlpha = 60;
	gui->addIntSlider("userMapAlpha", 0, 255, &userMapAlpha);
	textAlpha = 150;
	gui->addIntSlider("textAlpha", 0, 255, &textAlpha);


	handShoulderDistance = 200;
	gui->addIntSlider("handShoulderDistance", 100, 500, &handShoulderDistance);

	outsideScreenFactor = 1.2;
	gui->addSlider("outsideScreenFactor", 1, 2, &outsideScreenFactor);



	margin = 8;
	gui->addIntSlider("margin", 0, 24, &margin);

	bottomMargin = 56;
	gui->addIntSlider("bottomMargin", 0, 100, &bottomMargin);

	touchScreenSize = 600;		// virtual screen 
	gui->addSlider("touchScreenSize", 100, 1000, &touchScreenSize);


	screenZ = -1700;
	gui->addSlider("screenZ", -3000, 0, &screenZ);

	screenB = 1500;// screen bottom
	gui->addSlider("screenB", -1000, 2000, &screenB);

	screenT = 3000;// screen bottom
	gui->addSlider("screenT", 1000, 5000, &screenT);

	screenL = -600;// screen bottom
	gui->addSlider("screenL", -1000, 1000, &screenL);

	screenR = 600;// screen bottom
	gui->addSlider("screenR", -1000, 1000, &screenR);

	gui->autoSizeToFitWidgets();
	ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);   

	gui->saveSettings(ofToDataPath("gui/default_settings.xml"));
	gui->loadSettings(ofToDataPath("gui/settings.xml"));

	gui->setVisible(drawGui);
}


void testApp::guiEvent(ofxUIEventArgs &e)
{

	string name = e.getName();
	int kind = e.getKind();
	ofLogNotice("") << "got event from: " << name << endl;

	if(name == "State")
	{
		ofxUIRadio *radio = (ofxUIRadio *) e.widget;
		ofLogNotice("") << "value" << radio->getValue() << endl;
		ofLogNotice("") << " active name: " << radio->getActiveName() << endl;
	}

	if(name == "Save XML" && e.getButton()->getValue())
	{
		gui->saveSettings(ofToDataPath("gui/settings.xml"));
	}



}

void testApp::startRecording()
{
	lastRecordingFilename = generateFileName();
	ofLogNotice("startRecording") << recDir + lastRecordingFilename;	
	openNIRecorder.startRecording(recDir + lastRecordingFilename);
	isRecording = true;
	ofLogNotice("startRecording") << "OK";
}

void testApp::saveRecording()
{
	ofLogNotice("saveRecording");
	stopRecording();
	ofSleepMillis(100);
	saveSessionToDataSet();	
	updateScores();
	saveLibrary();
	ofSleepMillis(100);
	ofLogNotice("saveRecording") << "OK";

	// when recording is complete, save his selection data, process face frames and save to data,
	// update other selected x/v in db
	// select 25 :current25
	// meanwhile in 'position yourself'
	// select 4 :current4
	// when to start recording? (each hoverChange abort and start over) 
	// void abortRecording() // delete file
	// 


}


void testApp::stopRecording()
{
	ofLogNotice("") << "stopRecording: " << lastRecordingFilename << endl;
	openNIRecorder.stopRecording();
	isRecording = false;
	//	ofLogNotice("") << "stopRecording: " << "OK" << endl;

	//HACKHACK !!!
	//setupPlayback(lastRecordingFilename);
}

void testApp::abortRecording()
{
	ofLogNotice("") << "abortRecording: " << lastRecordingFilename << endl;
	stopRecording();
	//delete file?
}

void testApp::drawOverheadText(ofImage& txt, int x, int y, int w)
{
	ofSetColor(ofColor::black, textAlpha);
	ofFill();
	ofRect(0, y, w, txt.getHeight()); // text background

	ofSetColor(ofColor::white, 255);
	txt.draw(x, y);
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
		<< lastSeenUser.getCountDown() << endl
		<< "User Message: " << userMessage.str() << endl
		;

	ofDrawBitmapString(msg.str(), 220, 200);

}

void testApp::saveSessionToDataSet()
{
	currData.id = lastRecordingFilename;
	for (int i=0; i<RecordedData::N_OTHERS; i++)
	{
		currData.othersSelection[i] = false;
	}
	currData.othersSelection[selectedUser.hovered] = true;
	currData.location = "Kiev"; //TODO add textEdit w/load save
	currData.vScore = 0;
	currData.xScore = 0;

	dataset.push_back(currData);
}

void testApp::saveLibrary()
{
	ofLogNotice("saveLibrary");

	ofxJSONElement json;
	for (DataSet::iterator it = dataset.begin(); it != dataset.end(); it++)
	{
		if (it->id != "")
		{
			json.append(it->toJson());
		}
	}

	bool success = json.save(recDir + datasetJsonFilename, true);
	ofLogNotice("saveLibrary") << (success ? "OK":"FAIL");
}

string testApp::getRecDirString(string url)
{

	ofxJSONElement json;
	// Now parse the JSON
	bool parsingSuccessful = json.open(url);
	string dir;
	if (parsingSuccessful) 
	{
		ofLogNotice("load Recdir") << json.getRawString(true);
		dir = json["recDir"].asString();
	}
	else
	{
		ofLogNotice("load Recdir") << "Failed to parse JSON.";
		dir = ofToDataPath("records/");
	}

	return dir;
}

void testApp::loadLibrary()
{
	std::string url = recDir + datasetJsonFilename;

	// Now parse the JSON
	bool parsingSuccessful = datasetJson.open(url);

	if (parsingSuccessful) 
	{
		ofLogNotice("loadLibrary") << datasetJson.getRawString(true);
	} else {
		ofLogNotice("loadLibrary") << "Failed to parse JSON.";
	}

	for(unsigned int i = 0; i < datasetJson.size(); ++i)
	{
		Json::Value v = datasetJson[i];
		string id = v["id"].asString(); 
		dataset.push_back(RecordedData(v));
	}
}

void testApp::select25()
{
	//sort by rank
	//5: take last 5
	//2: take highest and lowest score
	//18: random
}


bool sortById(const RecordedData& lhs, const RecordedData& rhs)
{
	return lhs.id < rhs.id;
}
bool sortByScoreCount(const RecordedData& lhs, const RecordedData& rhs)
{
	return lhs.scoreCount() < rhs.scoreCount();
}


void testApp::select4()
{
	// 1 last one
	// 2 least times scored
	// 1 random

	DataSet::iterator maxit;

	maxit = std::max_element(dataset.begin(), dataset.end(), sortById); //latest
	currData.othersId[0] = maxit->id;
	currData.othersPtr[0] = &(*maxit);

	DataSet::iterator leastScored1 = std::max_element(dataset.begin(), dataset.end(), sortByScoreCount);
	DataSet::iterator first = dataset.begin();
	DataSet::iterator last = dataset.end();

	if (first!=last)
	{
		while (++first!=last)
		{
			if (first != maxit && sortByScoreCount(*first,*leastScored1))    // or: if (comp(*first,*smallest)) for version (2)
				leastScored1=first;
		}
	}


	DataSet::iterator leastScored2;
	leastScored2 = std::max_element(dataset.begin(), dataset.end(), sortByScoreCount);

	for (DataSet::iterator it = dataset.begin(); it != dataset.end(); it++)
	{
		if (it == maxit)
			continue;

		if (it == leastScored1)
			continue;

		if (it->scoreCount() < leastScored2->scoreCount())
		{
			leastScored2 = it;
		}
	}


	currData.othersId[1] = leastScored1->id;
	currData.othersId[2] = leastScored2->id;

	currData.othersPtr[1] = &(*leastScored1);
	currData.othersPtr[2] = &(*leastScored2);

	//random
	DataSet::iterator randit = std::min_element(dataset.begin(), dataset.end(), sortById); //first, just as a fallback

	int r = rand() % dataset.size();
	for (int j=0; j<r; j++)
	{
		randit++;
	}
	for (int i=0;i<dataset.size();i++)
	{
		if (randit == dataset.end())
			randit = dataset.begin(); 

		if (randit != maxit && randit != leastScored1 && randit != leastScored2)
		{
			break;
		}
		randit++;
	}
	currData.othersId[3] = randit->id;
	currData.othersPtr[3] = &(*randit);

	for (int i=0; i<4; i++)
	{
		setupPlayback(recDir + currData.othersId[i]);
	}
}

/*
{
dir.listDir("records/");
dir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order

//allocate the vector to have as many ofImages as files
int n = min(MAX_PLAYERS, dir.size());

if (testLoadLibrary) n = MAX_PLAYERS;

// iterate through the files and load them
for(int i = 0; i < n; i++)
{
string filename;
if (testLoadLibrary) 
{
filename = "records/t2.oni";
}
else
{
filename = dir.getPath(i);
}

setupPlayback(ofToDataPath(filename));
}

// TODO: load face data
}
*/

void testApp::updateScores()
{
	for (int i=0; i<4; i++)
	{
		RecordedData* other = currData.othersPtr[i];

		if (currData.othersSelection[i])
		{
			other->vScore++;
		}
		else
		{
			other->xScore++;
		}
	}
}
