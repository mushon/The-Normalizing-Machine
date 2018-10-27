#include "testApp.h"
#define PROFILE
#include "ofxProfile.h"



//--------------------------------------------------------------
void testApp::setup() {

	ofSetFrameRate(30);
	ofxOpenNI::shutdown();
	openni::Status rc = OpenNI::initialize();

	appRecorder.setup();

	drawDepth=false;
	drawGui=false;
	drawProfiler=false;
	drawVideo=true;
	drawText=false;

	selectedUser.lastSeen.setTimeout(3000);

	yesIcon20.loadImage("assets/i-yes-20.png");
	noIcon20.loadImage("assets/i-no-20.png");

	yesIcon.loadImage("assets/i-yes-40.png");
	noIcon.loadImage("assets/i-no-40.png");

	txt_pointing.loadImage("assets/txt_pointing.png");
	txt_position.loadImage("assets/txt_position.png");
	txt_prompt.loadImage("assets/txt_prompt.png");
	txt_toomany.loadImage("assets/txt_toomany.png");

	img_arrow_left.loadImage("assets/arrow_left.png");
	img_face_left.loadImage("assets/face_left.png");
	img_placemark_body.loadImage("assets/placemark_body.png");
	img_placemark_head.loadImage("assets/placemark_head.png");

	for (int i = 0; i < MAX_ROUND_COUNT + 1; i++){
		img_rounds[i].loadImage("assets/r" + to_string(i+1) + ".png");
		img_rounds_active[i].loadImage("assets/r" + to_string(i+1) + "_active.png");
	} 

	img_r_left.loadImage("assets/r_left.png");
	img_r_right.loadImage("assets/r_right.png");
	img_record.loadImage("assets/record.png");

	ofEnableAlphaBlending();

	setupGui();

	ofBackground(0, 0, 0);

	dataset.loadLibrary(recDir + datasetJsonFilename);
	state = IDLE;
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

void testApp::setupNextRound(string forcedId, string excludeSessionId) {
	for (int i = 0; i<n_players; i++)
	{
		players[i].stop();
	}
	n_players = 0;

	currData = dataset.selectNextRound(forcedId, excludeSessionId); // better name?

	for (int i = 0; i<currData.N_OTHERS; i++)
	{
		setupPlayback(recDir + currData.othersId[i]);
	}
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
	appRecorder.update();
	ofxProfileSectionPop();

	int nVisibleUsers = appRecorder.countVisibleUsers(); // vector from sensor
	userMessage << nVisibleUsers << endl;

	updateSelectedUser();

	if (simulateMoreThanOne)
	{
		nVisibleUsers = 99;
	}

	if (nVisibleUsers == 0)
	{
		if (state != IDLE)
		{
			// stop recording?
			if (selectedUser.lastSeen.getCountDown() == 0)
			{
				// 1 -> 0
				state = IDLE;
			}
		}		
	}
	
	if (nVisibleUsers == 1)
	{
		selectedUser.lastSeen.reset();

		switch (state)
		{
		case IDLE:
			{
				userMessage << selectedUser.distance << endl;
				if (selectedUser.distance < idleThreshold)
				{
					state = STEP_IN;
				}
				break;
			}
		case STEP_IN:
			{
				if (selectedUser.id == SelectedUser::NO_USER || 
				   selectedUser.distance > idleThreshold + idleThresholdHysteresis)
				{
					state = IDLE;
				}

				if (selectedUser.distance < stepInThreshold)
				{
					setupNextRound(); // first round
					sessionId = appRecorder.getLastFilename();
					roundSelections.clear();
					state = GOTO_SPOT;
				}
				break;
			}

		case GOTO_SPOT:
			{
				if (selectedUser.id == SelectedUser::NO_USER ||
					selectedUser.distance > stepInThreshold + stepInThresholdHysteresis)
				{
					state = STEP_IN;
				}

				if (selectedUser.distance < spotRadius)
				{
					state = RAISE_HAND;
				}
				break;
			}

		case RAISE_HAND:
			{
				if (selectedUser.distance > spotRadius + spotRadiusHysteresis)
				{
					state = GOTO_SPOT;
				}
				else
				{
					if (selectedUser.getSelectedArm().hand.z < selectedUser.getSelectedArm().shoulder.z - handShoulderDistance)
					{
						if (selectedUser.isSteady())
						{
							selectedUser.reset();
							cursor = AppCursor();

							cursor.setPosition(ofVec2f(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2));

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
				if (selectedUser.getSelectedArm().hand.z > selectedUser.getSelectedArm().shoulder.z - handShoulderDistance)
				{
					appRecorder.abort();
					state = RAISE_HAND;
				}
				if (selectedUser.distance > spotRadius + spotRadiusHysteresis)
				{
					appRecorder.abort();
					//give timeout?
					state = GOTO_SPOT;
				}
				else
				{
					ofPoint p = selectedUser.getPointingDir();

					float x = -(selectedUser.getSelectedArm().shoulder.z - screenZ) * p.x / p.z - screenL;
					float y = -(selectedUser.getSelectedArm().shoulder.z - screenZ) * p.y / p.z - screenB;

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

					float cx = ofGetScreenWidth() / 2;
					float cy = ofGetScreenHeight() / 2;

					selectedUser.screenPoint = v.getMapped(ofVec2f(cx, cy), ofVec2f(cx, 0), ofVec2f(0, -cy)); // reverse y, assume -1 < v.x, v.y < 1

					selectedUser.screenPoint.y = ofLerp(ofGetScreenHeight() / 2, selectedUser.screenPoint.y, 0.1);  // force to center // 2-player hack 

					float progress = selectedUser.getProgress();
					cursor.update(selectedUser.screenPoint, progressSmooth);

					int hover = 0;
					if (v.x > 0) hover += 1;
					// if (v.y < 0) hover+=2; 2 players hack

					float w = getPlayerWidth(); 
					float h = getPlayerHeight();

					if (abs(selectedUser.screenPoint.x - cx) < w/4) // && abs(selectedUser.screenPoint.y - (ofGetScreenHeight()/2)) < h/4) //inside middle frame
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
						selectedUser.getSelectedArm().steady.reset();
						selectedUser.selectTimer.reset();
						selectedUser.waitForSteady = true;
						appRecorder.abort();
					}


					if (selectedUser.selectTimer.getCountDown() < 2000)
					{
						appRecorder.start(recDir);
					}

					//TODO select mechanism (click/timeout)
					
					for (int i = 0; i < roundSelections.size(); i++) {
						userMessage << "roundCount: " << i << ": " << roundSelections[i] << endl;
					}

					bool selected = (selectedUser.selectTimer.getCountDown() == 0);
					if(selected)
					{
						appRecorder.stop();
						ofSleepMillis(100); // seems like it's fixed

						currData.makeSelection(sessionId, sessionId, selectedUser.hovered,
							selectedUser.totalHeight, selectedUser.headHeight, selectedUser.torsoLength, selectedUser.shouldersWidth);

						// info: ALL dataset is saved everytime
						dataset.saveSession(currData);
						dataset.saveLibrary(recDir + datasetJsonFilename);

						ofSleepMillis(100); // seems like it's fixed

						roundSelections.push_back(selectedUser.hovered);

						postSelectionTimer.setTimeout(1000); 
						postSelectionTimer.reset();
						state = SELECTION_POST;
					}

				}
				break;

			}
		case SELECTION_POST:
		{
			if (postSelectionTimer.getCountDown() <= 0) {
				if (roundSelections.size() < MAX_ROUND_COUNT) {
					if (roundSelections.size() == MAX_ROUND_COUNT - 1) {
						setupNextRound(currData.othersId[selectedUser.hovered]); // show self
					}
					else {
						setupNextRound(currData.othersId[selectedUser.hovered], sessionId); // keep winner
					}
					selectedUser.reset();
					state = SELECTION;
				}
				else {
					state = RESULT;
				}
			}

			break;
		}

		case RESULT:
			{
				// show prompt - look sideways
				bool b = true; // isFaceLookingSideWays(); // get from camera
				if (b) {
					state = PROFILE_CONFIRMED;
				}
				break;

			}
		case PROFILE_CONFIRMED:
			{
			// TODO: save video with sessionId (front, side)
			// TODO: show thank you and goodbye
				// animate back to idle
				// change from live to recording
				if (selectedUser.distance > stepInThreshold + stepInThresholdHysteresis)
				{
					ofLogNotice("RESULT -> IDLE");
					state = IDLE;
				}				
				break;
			}

		case MORE_THAN_ONE:
			{
				state = IDLE;
				break;
			}
		}
	}
	
	if (nVisibleUsers > 1)
	{
		state = MORE_THAN_ONE;
	}

	// set drawing parameters (before smoothing)
	if (state == IDLE) {
		liveFrameScale = 0;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0;
	}
	if (state == STEP_IN) {
		liveFrameScale = 0;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0;
	}
	if (state == GOTO_SPOT) {
		liveFrameScale = 1;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0;
	}
	if (state == RAISE_HAND) {
		liveFrameScale = 0.75;
		playerFrameScale = 1.0f;
		roundSelectionsScale = 1.0f;
	}
	if (state == SELECTION) {
		liveFrameScale = 0.5;
		playerFrameScale = 1.0f;
		roundSelectionsScale = 1.0f;
	}
	if (state == SELECTION_POST) {
		//liveFrameScale = 0.5;
		playerFrameScale = 0;
		roundSelectionsScale = 1.0f;
	}
	if (state == RESULT) {
		liveFrameScale = 0;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 1.0f;
	}
	if (state == PROFILE_CONFIRMED) {
		liveFrameScale = 0;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0.0f;
	}
	if (state == MORE_THAN_ONE) {
		//liveFrameScale = 0; // do nothing
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0.0f;
	}

	
	// smoothing
	liveFrameScaleSmooth *= liveFrameScaleSmoothFactor;
	liveFrameScaleSmooth += (1 - liveFrameScaleSmoothFactor) * liveFrameScale;

	playerFrameScaleSmooth *= playerFrameScaleSmoothFactor;
	playerFrameScaleSmooth += (1 - playerFrameScaleSmoothFactor) * playerFrameScale;

	float progress = 1.0;
	if (state == SELECTION && selectedUser.hovered != SelectedUser::NO_HOVER)
	{
		progress = selectedUser.getProgress();
	}

	progressSmooth *= progressSmoothFactor;
	progressSmooth += (1 - progressSmoothFactor) * progress;

	roundSelectionsScaleSmooth *= roundSelectionsSmoothFactor;
	roundSelectionsScaleSmooth += (1 - roundSelectionsSmoothFactor) * roundSelectionsScale;


	for (int i = 0; i < n_players; i++)
	{
		// draw player
		float s = selectionMaxExpand * (1 - progress);

		float selectionScale = 1;
		if (state == SELECTION) {
			selectionScale = (i == selectedUser.hovered) ? (1.0f + s) : (1.0f - s);
		}

		selectionScaleSmoothed[i] *= selectionScaleSmoothFactor;
		selectionScaleSmoothed[i] += (1 - selectionScaleSmoothFactor) * selectionScale;

		playbackScales[i] = playerFrameScaleSmooth * selectionScaleSmoothed[i];
	}

	for (int i=0; i<n_players; i++)
	{
		ofxProfileSectionPush(string("openni update ").append(ofToString(i)));
		players[i].update();
		ofxProfileSectionPop();
	}
}


void testApp::drawGotoSpot() {

	// // Depracated debug info:
	// ofVec2f dist(selectedUser.headPoint.x - spot.x, selectedUser.headPoint.z - spot.z);
	// userMessage << "go to the spot. Please move " 
	// << (abs(dist.x - spotRadius < 0) ? "Right" : "Left")
	// << (abs(dist.x - spotRadius < 0) && abs(dist.y - spotRadius < 0) ? " and " : "")
	// << (abs(dist.y - spotRadius < 0)? "Forward" : "Back") << endl
	// << endl;
	
	float sc2 = liveFrameScaleSmooth; // legacy
	float w = getPlayerWidth();
	float h = getPlayerHeight();

	ofSetLineWidth(2);
	ofSetColor(ofColor::green, userMapAlpha);
	ofLine(0, -h / 2 * sc2, 0, h / 2 * sc2);

	drawOverheadText(txt_position, -sc2*w / 2 + txt_position.getWidth() / 2, sc2*h / 2 - txt_position.getHeight() / 2, w * sc2);

	//TODO: instruct user to step into spot (visualy? top view)
	//draw user map
	float maxZ = 4000.0f;
	float factor = h*sc2 / maxZ;

	ofPushMatrix();
	ofPushStyle();
	ofTranslate(0, -h / 2 * sc2);

	ofScale(factor, factor);
	ofVec2f spot2d(spot.x, spot.z);

	ofSetColor(ofColor::green, userMapAlpha);
	ofFill();
	ofCircle(spot2d, spotRadius);
	ofNoFill();
	ofCircle(spot2d, (spotRadius + spotRadiusHysteresis));

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

void testApp::drawOverlay() {

	float w = getPlayerWidth();
	float h = getPlayerHeight();
	// cover with 50% black
	int alphaImage = 128;
	ofSetColor(ofColor::black, alphaImage);
	ofFill();
	ofRect(0, 0, w, h);
}


void testApp::drawLiveFrame() {
	float w = getPlayerWidth();
	float h = getPlayerHeight();
	float scale = liveFrameScaleSmooth;
	ofPushMatrix();
	ofScale(scale, scale);
	
	//border
	ofSetColor(ofColor::black);
	ofFill();
	ofRect(0, 0, w + 2 * margin / scale, h + 2 * margin / scale);

	appRecorder.drawImageSubsection(w, h, 0, 0);
	ofPopMatrix();
}

void testApp::drawIconAnimations(int i) {
	float w = getPlayerWidth();
	float h = getPlayerHeight();

	int dx = i % 2;
	dx = 2 * dx - 1; // map 0,1 to -1,1

	ofImage& icon = (i == selectedUser.hovered) ? yesIcon : noIcon;

	float transitionLength = 0.05;
	float transitionBegin = (i == selectedUser.hovered) ? 0.4 : 0.5 + 0.05 * i;
	int alphaIcon = ofMap(1 - selectedUser.getProgress(), transitionBegin, transitionBegin + transitionLength, 0, 255, true);

	//float iconScale = ofMap(selectedUser.getProgress(), transitionBegin, transitionBegin + transitionLength, 1.0f, 0.0f, true);
	float iconTrans = ofMap(1 - selectedUser.getProgress(), transitionBegin, transitionBegin + transitionLength, 0, 1, true);
	iconTrans = sqrt(iconTrans);
	iconTrans = ofMap(iconTrans, 0, 1, h * 0.75 + icon.getHeight() * 2, 0.0f, true);

	//ofSetColor(255, 255, 255, alphaIcon);
	ofPushMatrix();
	ofTranslate(0, iconTrans);
	//ofScale(iconScale, iconScale);
	ofSetColor(255, 255, 255, alphaIcon);
	icon.draw(0, 0);
	ofPopMatrix();

	if (i == selectedUser.hovered)
	{
		drawOverheadText(txt_pointing, -dx * (-w + txt_pointing.getWidth()) / 2, (h - txt_pointing.getHeight()) / 2, w);
	}
}

void testApp::drawTotalScore(int i) {
	RecordedData* other = currData.othersPtr[i];

	for (int j = 0; j<other->scoreCount(); j++)
	{
		int iconSpacing = 5;
		ofImage& icon = (j < other->vScore) ? yesIcon20 : noIcon20;
		icon.draw(
			(-(other->scoreCount() - 1) / 2 + j) * (icon.getWidth() + iconSpacing), 0);
	}
}

void testApp::drawPlayers() {
	ofPushMatrix();
	//numbers in comments relate to screen size of width:768, height:1024 (Portrait mode!) 
	float w = getPlayerWidth();
	float h = getPlayerHeight();

	for (int i = 0; i < n_players; i++)
	{
		// draw player
		float playbackScale = playbackScales[i]; // refac

		ofPushMatrix();
		// video order:
		// 0 1
		// 2 3
		int dx = i % 2;
		int dy = i / 2;

		dx = 2 * dx - 1; // map 0,1 to -1,1
		dy = 2 * dy - 1;

		dy = 0; // force // 2-player hack 

		ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
		ofTranslate(dx * (w + margin) / 2 * playbackScale, dy * (h + margin) / 2 * playbackScale);

		ofScale(playbackScale, playbackScale);

		// draw player
		ofRectangle border(0, 0, w + margin, h + margin);
		ofFill();
		ofSetColor(ofColor::black);
		ofRect(border);

		//			players[i].drawImage();
		players[i].drawImageSubsection(w, h, 0, 0);


		if (state == SELECTION && selectedUser.hovered != SelectedUser::NO_HOVER)
		{
			drawIconAnimations(i);
		}

		ofPopMatrix();
	}
	ofPopMatrix();	
}
//--------------------------------------------------------------
void testApp::drawRoundSelections(){
	ofPushMatrix();
	ofPushStyle();
	auto prevRectMode = ofGetRectMode();
	ofSetRectMode(OF_RECTMODE_CORNER);

	int border = 4;
	int iconWidth = img_rounds[0].getWidth();
	int totalWidth = (MAX_ROUND_COUNT + 1) * iconWidth + MAX_ROUND_COUNT * border;

	ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
	ofTranslate(0, getPlayerHeight() / 2 + iconWidth * 1.5);
	ofScale(roundSelectionsScaleSmooth, roundSelectionsScaleSmooth);
	ofTranslate(-totalWidth / 2, 0);
	
	ofSetColor(255 * roundSelectionsScaleSmooth);

	int currentRound = roundSelections.size();
	
	for (int i = 0; i < MAX_ROUND_COUNT + 1; i++) {
		const ofImage* img;
		if (i < currentRound) { // previously selected
			img = (roundSelections[i] == 0) ? &img_r_left : &img_r_right;
		}
		else if (i == currentRound) { // current round
			img = &img_rounds_active[i];
		}
		else {
			img = &img_rounds[i];
		}
		img->draw((border + iconWidth) * i, 0);
	}
	
	ofSetRectMode(prevRectMode);
	ofPopStyle();
	ofPopMatrix();
}


void testApp::draw(){
	ofSetRectMode(OF_RECTMODE_CENTER);

	ofxProfileThisFunction();

	if (drawVideo) {

		if (playerFrameScaleSmooth > 0.01) {
			drawPlayers();
		}
		
		if (roundSelectionsScaleSmooth > 0.01) {
			drawRoundSelections();
		}
		
		//draw live frame
		if (liveFrameScaleSmooth > 0.01)
		{
			float w = getPlayerWidth();
			float h = getPlayerHeight();

			userMessage << "w" << w << endl;
			userMessage << "h" << h << endl;

			ofPushMatrix();
			ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
			
			float sc2 = liveFrameScaleSmooth;

			ofxProfileSectionPush("draw live");
			drawLiveFrame();

			//draw overlays	
			if (state == GOTO_SPOT) {
				drawGotoSpot();
			}

			if (state == MORE_THAN_ONE)
			{
				drawOverheadText(txt_toomany, -sc2*w/2 + txt_toomany.getWidth()/2, 0, w * sc2);
				drawOverlay(); //?
			}

			if (state == RAISE_HAND)
			{
				drawOverheadText(txt_prompt, -sc2*w/2 + txt_prompt.getWidth()/2, sc2*h/2 - txt_prompt.getHeight()/2, w * sc2);
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

		if (state == RESULT)
		{
			//drawTotalScore(i);
		}
		
		if (state == PROFILE_CONFIRMED)
		{
			userMessage << "thank you and goodbye";
		}

	}

	if (drawDepth)
	{
		ofSetRectMode(OF_RECTMODE_CORNER);
		appRecorder.draw();
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
		appRecorder.stop();
		break;
	case 'S':
		appRecorder.start(recDir);
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


//	ofxOpenNI::shutdown();

	ofLogNotice("testApp exit OK");
}

void testApp::setupGui(){
	float dim = 16;

	gui = new ofxUISuperCanvas("Turing Normalizing Machine", 2);

	gui->addLabelButton("Save XML", false);

	gui->addRadio("State", AppState::getStates());
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

	idleThreshold = 1000;
	gui->addSlider("idleThreshold", 500, 2000, &idleThreshold);

	idleThresholdHysteresis = 100;
	gui->addSlider("idle.Thr.Hyst", 10, 200, &idleThresholdHysteresis);

	stepInThreshold = 700;
	gui->addSlider("stepInThreshold", 200, 1000, &stepInThreshold);

	stepInThresholdHysteresis = 100;
	gui->addSlider("stepIn.Thr.Hyst", 10, 200, &stepInThresholdHysteresis);

	spotRadius = 200;
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


	// screen-sensor distance. ir-sensor=0, depth values are z positive
	// +z = front
	// +y is up

	screenZ = -1700;
	gui->addSlider("screenZ", -3000, 0, &screenZ);

	screenB = 1500;// screen bottom
	gui->addSlider("screenB", -5000, 5000, &screenB);

	screenT = 3000;// screen top
	gui->addSlider("screenT", -5000, 5000, &screenT);

	screenL = -600;// screen left
	gui->addSlider("screenL", -1000, 1000, &screenL);

	screenR = 600;// screen right
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
	ofLogNotice("guiEvent") << "got event from: " << name << endl;

	if(name == "State")
	{
		ofxUIRadio *radio = (ofxUIRadio *) e.widget;
		ofLogNotice("guiEvent") << "value" << radio->getValue()<<
			" active name: " << radio->getActiveName() << endl;
		// appState.set(value, force=true) 
	}

	if(name == "Save XML" && e.getButton()->getValue())
	{
		gui->saveSettings(ofToDataPath("gui/settings.xml"));
	}



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
		<< "s : start/stop recording: " << (appRecorder.IsRecording() ? "RECORDING" : "READY") << endl
		<< endl
		//XXX << "File  : " << openNIRecorder.getDevice(). g_Recorder.getCurrentFileName() << endl
		<< "State : " << AppState::toString(state) << endl
		<< "User Last seen: " << selectedUser.lastSeen.getCountDown() << endl
		<< "User Message: " << userMessage.str() << endl
		;

	for (int i = 0; i < currData.N_OTHERS; i++) {
		msg << "#" << i << ": " << currData.othersId[i] << endl;
	}

	ofDrawBitmapString(msg.str(), 220, 200);

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

// note: adds id, distance and headpoint to SelectedUser
SelectedUser testApp::getClosestUser()
{
	SelectedUser user;

	if (appRecorder.openNIRecorder.trackedUsers.size() > 0)
	{
		for (map<int, ofxOpenNIUser>::iterator it = appRecorder.openNIRecorder.trackedUsers.begin(); it != appRecorder.openNIRecorder.trackedUsers.end(); ++it)
		{
			ofxOpenNIUser& u = it->second;
			ofxOpenNIJoint jh = u.getJoints().at(nite::JointType::JOINT_HEAD);
			user.headPoint = jh.positionReal;

			ofVec2f dist = ofVec2f(user.headPoint.x - spot.x, user.headPoint.z - spot.z); // discard height(y)    <<<--------------------------might be a hang here, consider other way of choosing

			float distance = dist.length();
			if (distance < user.distance)
			{
				user.id = it->first;
				user.distance = distance;
			}

			userMessage << user.id << ":" << user.headPoint << endl;
		}
	} // end for map
	
	return user;
}

void testApp::updateSelectedUser()
{
	SelectedUser user = getClosestUser();

	if (user.id == SelectedUser::NO_USER)
	{
		selectedUser = SelectedUser(); //reset
	}
	else
	{
		// CHANGED USER (state)

		// keep track of id (if changes in the middle)
		selectedUser.id = user.id;
		selectedUser.distance = user.distance;
		selectedUser.headPoint = user.headPoint;
		
		ofxOpenNIUser& u = appRecorder.openNIRecorder.trackedUsers.at(user.id);

		ofxOpenNIJoint rhj = u.getJoints().at(nite::JointType::JOINT_RIGHT_HAND);
		ofxOpenNIJoint rsj = u.getJoints().at(nite::JointType::JOINT_RIGHT_SHOULDER);
		ofxOpenNIJoint lhj = u.getJoints().at(nite::JointType::JOINT_LEFT_HAND);
		ofxOpenNIJoint lsj = u.getJoints().at(nite::JointType::JOINT_LEFT_SHOULDER);


		ofxOpenNIJoint neck = u.getJoints().at(nite::JointType::JOINT_NECK);
		ofxOpenNIJoint lhip= u.getJoints().at(nite::JointType::JOINT_LEFT_HIP);
		ofxOpenNIJoint rhip = u.getJoints().at(nite::JointType::JOINT_RIGHT_HIP);
		ofxOpenNIJoint lfoot = u.getJoints().at(nite::JointType::JOINT_LEFT_FOOT);
		ofxOpenNIJoint rfoot = u.getJoints().at(nite::JointType::JOINT_RIGHT_FOOT);

		// update body measurements
		float userHeight = 0;
		if (rfoot.positionConfidence >= 0.5) {
			userHeight = selectedUser.headPoint.distance(rfoot.positionReal);
		}
		else if (lfoot.positionConfidence >= 0.5) {
			userHeight = selectedUser.headPoint.distance(lfoot.positionReal);
		}
		if (userHeight > selectedUser.totalHeight) {
			selectedUser.totalHeight = userHeight;
		}

		float headHeight = 0;
		if (neck.positionConfidence >= 0.5) {
			headHeight = selectedUser.headPoint.distance(neck.positionReal);
		}
		if (headHeight > selectedUser.headHeight) {
			selectedUser.headHeight = headHeight;
		}

		float torsoLength = 0;
		if (neck.positionConfidence >= 0.5 && lhip.positionConfidence >= 0.5) {
			torsoLength = neck.positionReal.distance(lhip.positionReal);
		}
		else if (neck.positionConfidence >= 0.5 && rhip.positionConfidence >= 0.5) {
			torsoLength = neck.positionReal.distance(rhip.positionReal);
		}
		if (torsoLength > selectedUser.torsoLength) {
			selectedUser.torsoLength = torsoLength;
		}

		float shouldersWidth = 0;
		if (rsj.positionConfidence >= 0.5 && lsj.positionConfidence >= 0.5) {
			shouldersWidth = rsj.positionReal.distance(lsj.positionReal);
		}
		if (shouldersWidth > selectedUser.shouldersWidth) {
			selectedUser.shouldersWidth = shouldersWidth;
		}

		userMessage << "user.totalHeight: " << selectedUser.totalHeight << endl;
		userMessage << "user.headHeight: " << selectedUser.headHeight << endl;
		userMessage << "user.torsoLength: " << selectedUser.torsoLength << endl;
		userMessage << "user.shouldersWidth: " << selectedUser.shouldersWidth << endl;



		bool updateLeftArm = lhj.positionConfidence >= 0.5 && lsj.positionConfidence >= 0.5;
		bool updateRightArm = rhj.positionConfidence >= 0.5 && rsj.positionConfidence >= 0.5;
		
		if (updateLeftArm)
		{
			ofPoint leftHand = lhj.positionReal;
			ofPoint leftShoulder = lsj.positionReal;
			selectedUser.leftArm.update(leftHand, leftShoulder);
		}
		if (updateRightArm)
		{
			ofPoint rightHand = rhj.positionReal;
			ofPoint rightShoulder = rsj.positionReal;
			selectedUser.rightArm.update(rightHand, rightShoulder);
		}
		
		if (updateRightArm || updateLeftArm)
		{
			selectedUser.update();
		}
		else
		{
			selectedUser.reset();
		}
		
	}
}
