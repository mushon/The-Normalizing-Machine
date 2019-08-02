#include "testApp.h"

#define RECDIR "records/"
const string testApp::imageDir = "SeqImg/";
const int playersYOffset = -100;  // make slider ui

//--------------------------------------------------------------
void testApp::setup() {
	ofSeedRandom();
	setupDisplay();
	setupGui();
	setupInput();
	setupAssets();
	dataset.loadLibrary(recDir + datasetJsonFilename);

	setupWatchdog();
	state = IDLE;
}

void testApp::loadImages(string path, vector<ofImage*>& images) {
	ofDirectory dir(path);
	
	dir.allowExt("jpeg"); // only show ? files
	dir.listDir(); 	// populate the directory object
	dir.sortByDate();

	//go through and print out all the paths
	for (int i = 0; i < dir.size() && i < MAX_IMAGES; i++) {
		ofLogNotice(dir.getPath(i));
		ofImage* image = new ofImage();
		image->load(dir.getPath(i));
		image->update();
		images.push_back(image);
	}
}

void testApp::setupPlayback(string path) {
	loadImages(recDir + path, players[n_players]);
	n_players++;
}

void testApp::setupNextRound(int round, string firstId, string secondId) {
	for (int i = 0; i < n_players; i++)
	{
		for (auto& img : players[i]) {
			delete img;
		}
		players[i].clear();
	}
	n_players = 0;

	vector<string> nextIds;
	switch(round) {
	case 0:
		nextIds.push_back(roundsUsers[0]);
		nextIds.push_back(roundsUsers[1]);
		break;
	case 1:
		nextIds.push_back(roundsUsers[2]);
		nextIds.push_back(firstId);
		break;
	case 2:
		nextIds.push_back(firstId);
		nextIds.push_back(roundsUsers[3]);
		break;
	case 3:
		nextIds.push_back(roundsUsers[4]);
		nextIds.push_back(firstId);
		break;
	case 4:
		nextIds.push_back(firstId);
		nextIds.push_back(secondId);
		break;
	}

	session.setupNextRound(nextIds);

	for (int i = 0; i < session.N_OTHERS; i++)
	{
		int r = session.currentRound();
		setupPlayback(session.othersId[r][i]);
	}

	imgSeqTimer.setTimeout(imgSeqTimeout);
	imgSeqTimer.reset();
	imgId = 0;
}

/*
void testApp::setupNextRound(bool lastUser, string forcedId, string excludeSessionId) {
	for (int i = 0; i<n_players; i++)
	{
		players[i].stop();
	}
	n_players = 0;

	vector<string> nextIds = dataset.selectNextRound(lastUser, forcedId, excludeSessionId); // better name?
	session.setupNextRound(nextIds);

	for (int i = 0; i<session.N_OTHERS; i++)
	{
		int r = session.currentRound();
		while (!setupPlayback(RECDIR + session.othersId[r][i])) {
			dataset.dataset.erase(session.othersId[r][i]);
		}
	}
}
*/
/*
void testApp::setUpResult(string id) {
	for (int i = 0; i < n_players; i++)
	{
		players[i].stop();
	}
	n_players = 0;
	setupPlayback(recDir + id);
}
*/
//--------------------------------------------------------------
void testApp::update(){
	recorder.update();

	static int lastMin=0;

	int mins = ofGetElapsedTimeMillis() / (1000 * 60); // (ofGetElapsedTimef());
	if (mins > lastMin)
	{
		lastMin = mins;
		ofLogNotice("Minutes Passed") << mins;
	}

	//ofxProfileSectionPush("openni update live");
	// kinect.update();
	inputDevice.update();
	//ofxProfileSectionPop();

	// int nVisibleUsers = KinectUtil::countVisibleUsers(kinect); // vector from sensor
	int nVisibleUsers = inputDevice.countVisibleUsers();
	
	if (nVisibleUsers == 0)
	{
		if (state != IDLE)
		{
			// stop recording?
			if (selectedUser.lastSeen.done())
			{
				// 1 -> 0
				state = IDLE;
			}
		}
	}

	if (nVisibleUsers == 1)
	{
		selectedUser = inputDevice.getClosestUser();
		if (selectedUser.id == SelectedUser::NO_USER) 
		{ // user lost
			state = IDLE;
		}

		switch (state)
		{
		case IDLE:
			{
				if (selectedUser.distance < idleThreshold)
				{
					state = STEP_IN;
				}
				break;
			}
		case STEP_IN:
			{
				if(selectedUser.distance > idleThreshold + idleThresholdHysteresis) {
					state = IDLE;
				}

				if (selectedUser.distance < stepInThreshold)
				{
					state = GOTO_SPOT;
				}
				break;
			}
		case GOTO_SPOT:
			{
				if (selectedUser.distance > stepInThreshold + stepInThresholdHysteresis)
				{ // user stepped out of interaction zone
					state = STEP_IN;
				}
				else if (selectedUser.distance < spotRadius)
				{ // user in spot
					state = RAISE_HAND;
				}
				break;
			}
			// // enter
			// welcomeTimer.setTimeout(welcomeDuration);
			// welcomeTimer.reset();
			// //EW mac: // recorder.capture(ofToDataPath(imageDir), session.id, ofRectangle(cropX, cropY, cropW, cropH));
			// state = WELCOME_MSG;
			// ---
			// case WELCOME_MSG:
			// 	if (welcomeTimer.done()) {
			// 		// TODO: when user is already in spot
			// 		// this creates one frame flash when changing states:
			// 		// -> GOTO_SPOT -> RAISE_HAND
			// 		// possibly, just show welcome overlay, regardless (no welcome state)
			// 	}
			// 	break;
		case RAISE_HAND:
			{
				if (selectedUser.distance > spotRadius + spotRadiusHysteresis)
				{
					state = GOTO_SPOT;
				}
				else
				{
					if (selectedUser.handRaised)
					{
						// if (selectedUser.isSteady())
						// {
						state = SELECTION;
						// }
					}
				}

				break;
			}
		case SELECTION:
			{
				if (prev_state != state) {
					// was in step in !?
					if (prev_state == RAISE_HAND){
						// First time only
						session = RecordedData();
						session.id = generateFileName();

						// # populate
						roundsUsers[0] = dataset.getLatestUser();
						int i = 1;
						while (i < RecordedData::MAX_ROUND_COUNT) {
							bool dup = false;
							string selected =  dataset.getRandomUser();
							for (int j = 0; j < i; j++) {
								if (roundsUsers[j] == selected) {
									dup = true;
									continue;
								}
							}
							if (!dup) {
								roundsUsers[i] = selected;
								i++;
							}
						}
	
						setupNextRound(0);
					}

					if (prev_state == SELECTION_POST) {
						int r = session.currentRound();
						if (r < RecordedData::MAX_ROUND_COUNT) {
						string lastWinnerId = session.othersId[r-1][hovered];
						if (r == RecordedData::MAX_ROUND_COUNT - 1) {
							setupNextRound(r, lastWinnerId, session.id); // keep winner + show self
						}
						else {
							setupNextRound(r, lastWinnerId); // keep winner, exclude self
						}
					}

					// was in RAISE_HAND
					hovered = NO_HOVER;
					selectedUser.selectionTimer.setTimeout(selectionTimeout);
					selectedUser.selectionTimer.reset();
					cursor = AppCursor();
					cursor.setPosition(ofVec2f(ofGetScreenWidth() / 2 + cursorWidthOffset , ofGetScreenHeight() / 2 + cursorHightOffset));
				}

				// EW: disabled this, since user already raised hand
				// if (selectedUser.getSelectedArm().hand.z > selectedUser.getSelectedArm().shoulder.z - handShoulderDistance/* ||
				// 	selectedUser.getSelectedArm().hand.x - selectedUser.getSelectedArm().shoulder.x > abs(handShoulderDistance)*/)
				// {
				// 	//recorder.abort();
				// state = RAISE_HAND;
				// }
				if (selectedUser.distance > spotRadius + spotRadiusHysteresis)
				{
					//recorder.abort();
					//give timeout?
					state = GOTO_SPOT;
				}
				else
				{
					ofPoint screenPoint = inputDevice.getScreenPoint();
					if (lockCursorY) { screenPoint.y = ofGetScreenHeight() / 2; }
					cursor.update(screenPoint, progressSmooth);

					int hover = NO_HOVER;

					if (screenPoint.x < ofGetScreenWidth() / 2) //- cursorWidthOffset - selectionBufferWidth) {
					{
						hover = 0;  // left
					}
					if (screenPoint.x > ofGetScreenWidth() / 2)
					// if (screenPoint.x > ofGetScreenWidth() / 2 + cursorWidthOffset + selectionBufferWidth) {
					{
						hover = 1;  // right
					}

					if (hover == NO_HOVER || hovered != hover)
					{ 
						// changed selection
						// selectedUser.getSelectedArm().steady.reset();
						selectedUser.selectionTimer.reset();
						// selectedUser.waitForSteady = true;
						//recorder.abort();
					}
					hovered = hover;
					
					//TODO select mechanism (click/timeout)
					bool selected = (selectedUser.selectionTimer.done());
					if(selected)
					{
						// add vote
						session.makeSelection(hovered);
						/*
						if (appRecorder.IsRecording()) {
							appRecorder.stop();
							ofSleepMillis(100); // seems like it's fixed
						}
						*/
						postSelectionTimer.setTimeout(postSelectionTimeout);
						postSelectionTimer.reset();
						/*
						if (session.currentRound() == RecordedData::MAX_ROUND_COUNT - 2) { // one before last round
							recorder.start(recDir, session.id, recordingDuration);
						}
						*/
						if (session.currentRound() == RecordedData::MAX_ROUND_COUNT - 3) { // two before last round
							//if (selectedUser.selectionTimer.getCountDown() < recordingDuration)
							//{
								//EW mac: // recorder.capture(recDir, session.id, ofRectangle(cropX, cropY, cropW, cropH));
							//}
						}
						state = SELECTION_POST;
					}

				}
				break;

			}
		case SELECTION_POST:
		{
			if (postSelectionTimer.done()) {
					state = SELECTION;
				}
				else {
					//players[hovered]->getCurrentFrame();
					resultImage = yesIcon; // HACK EW// *players[hovered].back();
					//resultImage.allocate(players[hovered]->getWidth(), players[hovered]->getHeight(), OF_IMAGE_COLOR);
					//resultImage.setFromPixels(players[hovered]->getPixels());
					session.saveUserMeasurements(selectedUser.totalHeight + 0.25 * selectedUser.headHeight, selectedUser.headHeight, selectedUser.torsoLength, selectedUser.shouldersWidth, selectedUser.armLength);

					// info: ALL dataset is saved every time
					/*
					EW mac_fix
					dataset.saveSession(session);
					dataset.updateScores(session);
					dataset.saveLibrary(recDir + datasetJsonFilename);
					 */

					resultTimer.setTimeout(resultTimeout);
					resultTimer.reset();
					// ofSleepMillis(100); // seems like it's fixed
					//recorder.capture(imageDir, session.id, ofRectangle(cropX, cropY, cropW, cropH), false);
					state = RESULT;
				}
			}
			break;
		}

		case RESULT:
			{
				// show prompt - look sideways
				if (resultTimer.done()) { // isFaceLookingSideWays(); // get from camera
					state = PROFILE_CONFIRMED;
				}
				break;
			}

		case PROFILE_CONFIRMED:
			{
				// TODO: save video with sessionId (front, side). you cant save front it here too late
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

	prev_state = state;
	// // // 

	// set drawing parameters (before smoothing)
	if (state == IDLE) {
		liveFrameScale = 1;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0;
	}
	if (state == STEP_IN) {
		liveFrameScale = 1;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0;
	}
	if (state == WELCOME_MSG) {
		liveFrameScale = 1;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0;
	}
	if (state == GOTO_SPOT) {
		liveFrameScale = 1;
		playerFrameScale = 0.0f;
		roundSelectionsScale = 0;
	}
	if (state == RAISE_HAND) {
		liveFrameScale = 0.5;
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
		 liveFrameScale = 0.5;
		// playerFrameScale = 0.0f;
		roundSelectionsScale = 0;
	}
	if (state == PROFILE_CONFIRMED) {
		liveFrameScale = 1;
		// playerFrameScale = 0.0f;
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
	if (state == SELECTION && hovered != NO_HOVER)
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
			selectionScale = (i == hovered) ? (1) : (1.0f - s);
		}
		playbackScales[i] = selectionScale;

		//selectionScaleSmoothed[i] *= selectionScaleSmoothFactor;
		//selectionScaleSmoothed[i] += (1 - selectionScaleSmoothFactor) * selectionScale;

		//playbackScales[i] = playerFrameScaleSmooth * selectionScaleSmoothed[i];
	}

	drawFbo();
}


void testApp::drawGotoSpot() {

	// // Depracated debug info:
	// ofVec2f dist(selectedUser.headPoint.x - spot.x, selectedUser.headPoint.z - spot.z);
	// // stst = stringstream()
	// stst << "go to the spot. Please move "
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
	//ofVec2f spot2d(spot.x, spot.z);

	ofSetColor(ofColor::green, userMapAlpha);
	ofFill();
	ofCircle(spot.x, spot.z, spotRadius);
	ofNoFill();
	ofCircle(spot.x, spot.z, (spotRadius + spotRadiusHysteresis));

	//ofLine(spot2d.x - spotRadius, spot2d.y,spot2d.x + spotRadius, spot2d.y);

	ofNoFill();
	ofSetLineWidth(5);
	ofSetColor(ofColor::white, userMapAlpha);

	//ofVec2f v(selectedUser.headPoint.x, selectedUser.headPoint.z);
	ofCircle(selectedUser.headPoint.x, selectedUser.headPoint.z, 200);

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

	float w = 340;// getPlayerWidth();
	float h = 480;// getPlayerHeight();

	float scale = liveFrameScaleSmooth;
	ofPushMatrix();
	ofScale(scale, scale);

	//border
	//ofSetColor(ofColor::black);
	//ofFill();
	//ofNoFill();
	//ofDrawRectRounded(0, 0, w + 2 * margin / scale, h + 2 * margin / scale, 6);
	//ofRect(0, 0, w + 2 * margin / scale, h + 2 * margin / scale);

	// draw cropped area in center of frame
	// float imageWidth = KINECT_WIDTH;
	// float imageHeight = KINECT_HIGHT;

	
	// float offsetW = (imageWidth - w) / 2;
	// float offsetH = (imageHeight - h) / 2;

	inputDevice.draw();
	img_record.draw((img_record.getWidth() + margin - w) / 2 , (img_record.getHeight() + margin - h) / 2); // top left

	frame.clear();
	frame.rectangle(-w / 2, -h / 2, w, h);
	frame.draw();

	ofPopMatrix();
}

void testApp::drawIconAnimations(int i) {
	float w = getPlayerWidth();
	float h = getPlayerHeight();

	int dx = i % 2;
	dx = 2 * dx - 1; // map 0,1 to -1,1

	ofImage& icon = (i == hovered) ? yesIcon : noIcon;

	float transitionLength = 0.05;
	float transitionBegin = (i == hovered) ? 0.4 : 0.5 + 0.05 * i;
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

	if (i == hovered)
	{
		drawOverheadText(txt_pointing, -dx * (-w + txt_pointing.getWidth()) / 2, (h - txt_pointing.getHeight()) / 2, w);
	}
}


void testApp::drawPlayers() {

	//numbers in comments relate to screen size of width:768, height:1024 (Portrait mode!)
	float w = getPlayerWidth();
	float h = getPlayerHeight();

	ofPushMatrix();
	// move everything down
	ofTranslate(0, playersYOffset);

	for (int i = 0; i < n_players; i++)
	{
		// draw player
		float playbackScale = 1;  playbackScales[i]; // refac
		float fade = playbackScales[i];
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
		//ofScale(playbackScale, playbackScale);
		//ofSetColor(255, 255, 255, 255 * fade);

		// draw player
		//ofRectangle border(0, 0, w + margin, h + margin);
		//ofFill();
		//ofSetColor(ofColor::black);
		//ofRect(border);

		// draw cropped area in center of frame
		//float imageWidth = players[i].getWidth();
		//float imageHeight = players[i].getHeight();

		//float offsetW = (imageWidth - w) / 2;
		//float offsetH = (imageHeight - h) / 2;
		if (imgId >= players[i].size()) {
			imgId = 0;
		}
		if (imgSeqTimer.done()) {
			imgId++;
			imgSeqTimer.reset();
		}
	//	ofEnableAlphaBlending();
		frame.clear();
		frame.rectangle(-w/2, -h/2, w, h);
		frame.draw();

		//players[i].getTextureReference().drawSubsection(0, 0, w, h, offsetW, offsetH, w, h);
		if (players[i].size() > imgId && players[i].at(imgId) != NULL) {
			players[i].at(imgId)->draw(0, 0,  w, h);
		}
	//	ofDisableAlphaBlending();

		if (state == SELECTION && hovered != NO_HOVER)
		{
			if (i == hovered) {
				img_prompt_2_1_moreNormal.draw(0, textY);
			}
			// drawIconAnimations(i);
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
	int totalWidth = (RecordedData::MAX_ROUND_COUNT + 1) * iconWidth + RecordedData::MAX_ROUND_COUNT * border;

	ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
	ofTranslate(0, getPlayerHeight() / 2 + iconWidth * 1.5);
	ofScale(roundSelectionsScaleSmooth, roundSelectionsScaleSmooth);
	ofTranslate(-totalWidth / 2, 0);

	ofSetColor(255 * roundSelectionsScaleSmooth);

	int currentRound = session.currentRound();

	for (int i = 0; i < RecordedData::MAX_ROUND_COUNT; i++) {
		const ofImage* img;
		if (i < currentRound) { // previously selected
			img = (session.roundSelections[i] == 0) ? &img_r_left : &img_r_right;
		}
		else if (i == currentRound) { // current round
			img = &img_rounds_active[i];
		}
		else {
			img = &img_rounds[i];
		}
		img->draw((border + iconWidth) * i, 0);
	}

	// draw star
	const ofImage* img;
	if (state == State::PROFILE_CONFIRMED) { //. || State::RESULT
		img = &img_rounds_star_active;
	}
	else {
		img = &img_rounds_star;
	}
	img->draw((border + iconWidth) * RecordedData::MAX_ROUND_COUNT, 0);


	ofSetRectMode(prevRectMode);
	ofPopStyle();
	ofPopMatrix();
}

void testApp::drawFbo() {
	fbo.begin();
	ofClear(0, 0, 0, 0);

	ofSetRectMode(OF_RECTMODE_CENTER);

	if (drawVideo) {

		if (playerFrameScaleSmooth > 0.01) {
			drawPlayers();
		}

		if (roundSelectionsScaleSmooth > 0.01) {
			drawRoundSelections();
		}

		//draw live frame
		ofPushMatrix();
		{
			ofTranslate(0, playersYOffset);

			if (liveFrameScaleSmooth > 0.01)
			{
				float w = getPlayerWidth();
				float h = getPlayerHeight();

				ofPushMatrix();
				ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);

				float sc2 = liveFrameScaleSmooth;

				drawLiveFrame();

				//draw overlays
				if (state == IDLE) {
					ofEnableAlphaBlending();
					img_step_in.draw(0, 0);
					ofDisableAlphaBlending();
					img_prompt_0_1_idle.draw(0, textY);
				}

				if (state == STEP_IN) {
					ofEnableAlphaBlending();
					img_step_in.draw(0, 0);
					ofDisableAlphaBlending();
					img_prompt_0_1_idle.draw(0, textY);
				}

				if (state == WELCOME_MSG) {
					ofEnableAlphaBlending();
					img_prompt_0_3_intro.draw(0, 0);
					ofDisableAlphaBlending();
				}

				if (state == GOTO_SPOT) {
					ofEnableAlphaBlending();
					img_position_yourself.draw(0, 0);
					ofDisableAlphaBlending();
					//drawGotoSpot(); // todo draw red shadow
					img_prompt_0_2_position.draw(0, textY);
				}

				if (state == MORE_THAN_ONE)
				{
					ofEnableAlphaBlending();
					img_one_by_one.draw(0, 0);
					ofDisableAlphaBlending();
					img_prompt_0_3_onebyone.draw(0, textY);
				}

				if (state == RAISE_HAND)
				{
					img_prompt_1_1_point.draw(0, textY);
				}

				if (state == RESULT)
				{

					ofEnableAlphaBlending();

					resultImage.draw(0, 0, w, h);
					frame.clear();
					frame.rectangle(-w / 2, -h / 2, w, h);
					frame.draw();
					ofDisableAlphaBlending();
					img_prompt_2_1_moreNormal.draw(0, textY);
					//drawTotalScore(i);
				}

				if (state == PROFILE_CONFIRMED)
				{
					ofEnableAlphaBlending();
					img_goodbye.draw(0, 0);
					ofDisableAlphaBlending();
					img_prompt_10_goodbye.draw(0, textY);
				}

				//ofxProfileSectionPop();
				ofPopMatrix();
			}


			if (state == SELECTION)
			{
				if (drawCursor) {
					cursor.draw();
				}
			}
		}
		ofPopMatrix();


	}
	fbo.end();
}

void testApp::draw(){
	//ofxProfileThisFunction();
	//ofSetColor(ofColor::white);

	if (drawProjection)
	{
		ofSetRectMode(OF_RECTMODE_CORNER);
		drawSplitScreen(fbo);
	}
	else
	{
		fbo.draw(0, 0);
	}

	// debugging draw
	if (drawDebugInput)
	{
		ofSetRectMode(OF_RECTMODE_CORNER);
		inputDevice.draw_debug();
	}

	if (drawText)
	{
		drawDebugText();
	}

	if (drawProfiler)
	{
		//ofDrawBitmapString(ofxProfile::describe(), profilerPos);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key) {

	case 'c':
		recorder.capture(ofToDataPath(imageDir), generateFileName(), ofRectangle(cropX, cropY, cropW, cropH));
		break;

	case 's':
		//recorder.abort();
		break;
	case 'S':
		//recorder.start(recDir, generateFileName() + ofToString(ofGetElapsedTimeMillis()), recordingDuration);
		break;

	case 'F':
		ofToggleFullscreen();
		break;

	case 'g':
		gui->toggleVisible();
		// gui->isVisible() ? ofShowCursor() : ofHideCursor; // not working, of bug
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

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		for (auto& img : players[i]) {
			delete img;
		}
	}

	//kinect.stop();

	ofLogNotice("testApp exit OK");
}

void testApp::setupGui(){
	float dim = 16;

	gui = new ofxUIScrollableCanvas();
	gui->setScrollAreaToScreen();
	gui->setScrollableDirections(false, true);
	gui->setDamping(0); // no acceleration

	gui->addLabel("The Normalizing Machine");

	gui->addLabelButton("Load XML", false);
	gui->addLabelButton("Save XML", false);
	gui->addLabelButton("RESET XML", false);

	// gui->addRadio("State", AppState::getStates());
	//	recDir = "e:/records/";
	//	recDir = ofToDataPath("/records/");
	//  recDir = "C:/Users/SE_Shenkar/Dropbox/records/";

	gui->addTextInput("ImageDir", imageDir);

	recDir = getRecDirString(ofToDataPath("recDir.json"));

	gui->addLabel("Rec Dir", recDir);

	datasetJsonFilename = "dataset.json";
	gui->addLabel("datasetJsonFilename", datasetJsonFilename);
	gui->addSpacer();

	gui->addFPSSlider("FPS", 60)->setDrawOutline(true);
	gui->addToggle("draw (g)ui", &drawGui)->bindToKey('g');
	gui->addToggle("draw (v)ideo", &drawVideo)->bindToKey('v');
	gui->addToggle("draw (d)ebug input", &drawDebugInput)->bindToKey('d');
	gui->addToggle("draw (t)ext", &drawText)->bindToKey('t');
	gui->addToggle("draw (p)rojection", &drawProjection)->bindToKey('p');
	gui->addToggle("draw (c)ursor", &drawCursor)->bindToKey('c');
	// gui->addToggle("draw (K)Kinect", &drawProfiler)->bindToKey('K');

	gui->addSpacer();

	profilerPos = ofxUIVec3f(220, 0);
	//gui->add2DPad("profilerPos", ofxUIVec3f(0, ofGetScreenWidth()), ofxUIVec3f(0, ofGetScreenHeight()), &profilerPos);

	gui->addLabel("Screen Position");

	// screen-sensor distance. ir-sensor=0, depth values are z positive
	// +y is up
	// +z = front

	screenZ = -1700;
	gui->addSlider("screenZ", -3000, 0, &screenZ);

	screenB = 1500;// screen bottom
	gui->addSlider("screenB", -5000, 5000, &screenB);

	screenT = 3000;// screen top
	gui->addSlider("screenT", -5000, 5000, &screenT);

	screenL = -2000;// screen left
	gui->addSlider("screenL", -3000, 3000, &screenL);

	screenR = 2000;// screen right
	gui->addSlider("screenR", -3000, 3000, &screenR);

	outsideScreenFactor = 5;
	gui->addSlider("outsideScreenFactor", 1, 10, &outsideScreenFactor);

	gui->addSpacer();
	///
	gui->addLabel("Spot & Thresholds");
	depthClip.x = 500;
	gui->addSlider("Near Clip", 500, 3000, &depthClip.x);
	depthClip.y = 4000;
	gui->addSlider("Far Clip", 2000, 6000, &depthClip.y);
	cursorHightOffset = 0;
	gui->addIntSlider("Cursor Hight offset Pix", -ofGetScreenHeight() / 2, ofGetScreenHeight()/2, &cursorHightOffset);
	cursorWidthOffset = 0;
	gui->addIntSlider("Cursor Width offset Pix", -ofGetScreenWidth() / 2, ofGetScreenWidth() / 2, &cursorWidthOffset);
	spot.x = 0;
	gui->addSlider("spot X", -500, 500, &spot.x);

	spot.z = 1600; // distance from sensor [mm]
	gui->addSlider("spot Z", 500, 3000, &spot.z);

	spotRadius = 300;
	gui->addSlider("spot radius", 0, 1000, &spotRadius);
	spotRadiusHysteresis = 300;
	gui->addSlider("spot Hysteresis", 0, 400, &spotRadiusHysteresis);

	stepInThreshold = 700;
	gui->addSlider("stepIn Threshold", 200, 1000, &stepInThreshold);
	stepInThresholdHysteresis = 200;
	gui->addSlider("stepIn Hysteresis", 10, 400, &stepInThresholdHysteresis);

	idleThreshold = 1000;
	gui->addSlider("idle Threshold", 500, 2000, &idleThreshold);
	idleThresholdHysteresis = 200;
	gui->addSlider("idle Hysteresis", 10, 400, &idleThresholdHysteresis);

	gui->addSpacer();
	///
	gui->addLabel("Selection");

	selectionBufferWidth = 20;
	gui->addSlider("selectionBufferWidth in Pix", 0, 1000, &selectionBufferWidth);

	handShoulderDistance = 200;
	gui->addIntSlider("handShoulderDistance", 100, 500, &handShoulderDistance);

	selectionTimeout = 3000;
	gui->addIntSlider("selectionTimeout", 100, 10000, &selectionTimeout);

	postSelectionTimeout = 1000;
	gui->addIntSlider("postSelectionTimeout", 100, 10000, &postSelectionTimeout);

	recordingDuration = 2000;
	gui->addIntSlider("recordingDuration", 100, 10000, &recordingDuration);

	welcomeDuration = 5000;
	gui->addIntSlider("welcome msg", 100, 10000, &welcomeDuration);

	resultTimeout = 3000;
	gui->addIntSlider("resultTimeout", 0, 10000, &resultTimeout);

	imgSeqTimeout = 200;
	gui->addIntSlider("imgSeqTimeout", 100, 1000, &imgSeqTimeout);

	gui->addSpacer();
	///
	gui->addLabel("Drawing");
	userMapAlpha = 60;
	gui->addIntSlider("userMapAlpha", 0, 255, &userMapAlpha);
	textAlpha = 150;
	gui->addIntSlider("textAlpha", 0, 255, &textAlpha);
	margin = 8;
	gui->addIntSlider("margin", 0, 24, &margin);

	textY = getPlayerHeight() / 2;
	gui->addSlider("textY", 0, 1000, &textY);

	lockCursorY = true;
	gui->addToggle("(l)ock cursor Y", &lockCursorY)->bindToKey('l');

	gui->addSpacer();

	gui->addLabel("Misc");
	kinectYPos = 0.0;
	gui->addSlider("kinect y pos in Meter", 0.0, 4.0, &kinectYPos);

	cropW = 3840;
	gui->addIntSlider("Crop Width", 100, 3840, &cropW);

	cropH = 2160;
	gui->addIntSlider("Crop Hight", 100, 2160, &cropH);

	cropX = 0;
	gui->addIntSlider("Crop X", 100, 3840, &cropX);

	cropY = 0;
	gui->addIntSlider("Crop Y", 100, 2160, &cropY);

	gui->addSpacer();
	gui->addSpacer();
	gui->addSpacer();


	gui->autoSizeToFitWidgets();
	ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);

	gui->saveSettings(ofToDataPath("gui/default_settings.xml"));

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

	if (name == "Load XML" && e.getButton()->getValue())
	{
		gui->loadSettings(ofToDataPath("gui/settings.xml"));
	}

	if (name == "Save XML" && e.getButton()->getValue())
	{
		gui->saveSettings(ofToDataPath("gui/settings.xml"));
	}

	if (name == "RESET XML" && e.getButton()->getValue())
	{
		gui->loadSettings(ofToDataPath("gui/default_settings.xml"));
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

	ofPushStyle();
	ofSetColor(255, 255, 0);

	stringstream msg;
	msg
		<< "F: Fullscreen" << endl
		<< "s : start/stop recording: " << (recorder.isRecording() ? "RECORDING" : "READY") << endl
		<< endl
		//XXX << "File  : " << openNIRecorder.getDevice(). g_Recorder.getCurrentFileName() << endl
		<< "inputDevice: " << endl
		<< "	Visible Users : " << inputDevice.countVisibleUsers() << endl
		<< "	Screen Point : " << inputDevice.getScreenPoint() << endl
		<< " hovered: " << hovered << endl
		<< "State : " << AppState::toString(state) << endl
		<< "Timers: " << endl
		<< "	welcome Timer: " << welcomeTimer.getCountDown() << endl
		<< "	result Timer: " << resultTimer.getCountDown() << endl
		<< "	postSelection Timer: " << postSelectionTimer.getCountDown() << endl
		<< "	imgSeq Timer: " << imgSeqTimer.getCountDown() << endl
		<< "User: " << endl
		<< "	distance: " << selectedUser.distance << endl
		<< "	Last seen: " << selectedUser.lastSeen.getCountDown() << endl
		<< "	PointingDir: " << selectedUser.getPointingDir() << endl
		<< "	isSteady: " << selectedUser.isSteady() << endl
		<< "	is Hand Raised : " << selectedUser.handRaised << endl
		<< "	selectedUser.selectionTimer: " << selectedUser.selectionTimer.getCountDown() << endl
		;


	msg << "session.currentRound: " << session.currentRound() <<"/"<< session.MAX_ROUND_COUNT << endl;
	for (int i = 0; i < session.currentRound(); i++) {
		msg << "round #" << i << ": " << session.roundSelections[i] << endl;
	}
	for (int i = 0; i < session.N_OTHERS; i++) {
		//if (session.othersId[session.currentRound()][i]) {
		msg << "othersId [" << i << "]: " << session.othersId[session.currentRound()][i] << endl;
		//}
	}


	ofDrawBitmapString(msg.str(), 220, 200);
	ofPopStyle();
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


string testApp::generateFileName() {
	string timeFormat = "%Y_%m_%d_%H_%M_%S_%i";
	string name = ofGetTimestampString(timeFormat);
	return name;
}


void testApp::drawSplitScreen(ofFbo& fbo) {
	ofPushMatrix();
	//float z = tan(angle)* fboW / 2;
	ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2 - fbo.getHeight() / 2);
	ofRotateY(-wallAngle);
	fbo.getTextureReference().drawSubsection(-(fbo.getWidth() / 2), 0, 0, fbo.getWidth() / 2, fbo.getHeight(), 0, 0, fbo.getWidth() / 2, fbo.getHeight());
	ofRotateY(wallAngle * 2);
	fbo.getTextureReference().drawSubsection(0, 0, 0, fbo.getWidth() / 2, fbo.getHeight(), fbo.getWidth() / 2, 0, fbo.getWidth() / 2, fbo.getHeight());
	ofPopMatrix();
}

void testApp::setupAssets() {

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

	for (int i = 0; i < RecordedData::MAX_ROUND_COUNT; i++){
		img_rounds[i].loadImage("assets/r" + to_string(i+1) + ".png");
		img_rounds_active[i].loadImage("assets/r" + to_string(i+1) + "_active.png");
	}

	img_rounds_star.loadImage("assets/r_star.png");
	img_rounds_star_active.loadImage("assets/r_star_active.png");

	img_r_left.loadImage("assets/r_left.png");
	img_r_right.loadImage("assets/r_right.png");
	img_record.loadImage("assets/record.png");

	img_gapmarker.loadImage("assets/gapmarker.png");
	img_placemark.loadImage("assets/placemark.png");
	img_placemarker_body.loadImage("assets/placemarker_body.png");
	img_placemarker_head.loadImage("assets/placemarker_head.png");
	img_placemark_0_2_position.loadImage("assets/placemark_0.2_position.png");

	img_prompt_0_1_idle.loadImage("assets/prompt_0.1_idle.png");
	img_prompt_0_2_position.loadImage("assets/prompt_0.2_position.png");
	img_prompt_0_3_onebyone.loadImage("assets/prompt_0.3_onebyone.png");
	img_prompt_1_1_point.loadImage("assets/prompt_1.1_point.png");
	img_prompt_10_goodbye.loadImage("assets/prompt_10_goodbye.png");
	img_prompt_2_1_moreNormal.loadImage("assets/prompt_2.1_moreNormal.png");
	img_prompt_9_turnLeft.loadImage("assets/prompt_9_turnLeft.png");

	img_goodbye.loadImage("assets/goodbye.png");
	img_one_by_one.loadImage("assets/one_by_one.png");
	img_position_yourself.loadImage("assets/position_yourself.png");
	img_step_in.loadImage("assets/step_in.png");
	// img_wellcome_msg.loadImage("assets/welcome_msg.png");
	img_prompt_0_3_intro.loadImage("assets/prompt_0.3_intro.png");
}


void testApp::setupInput() {
	inputDevice.setup();
	selectedUser.lastSeen.setTimeout(3000);
}
	

void testApp::setupDisplay() {
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	drawCursor = true;
	drawDebugInput = false;
	drawGui = false;
	drawProfiler = false;
	drawVideo = true;
	drawText = false;

	ofEnableAntiAliasing();
	ofDisableAlphaBlending();

	ofBackground(0, 0, 0, 0);

	fbo.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA);
	//ofDirectShowPlayer* dPlayer = new ofDirectShowPlayer();
	//ofPtr <ofBaseVideoPlayer> ptr(dPlayer);
	//players[0].setPlayer(ptr);
	//players[1].setPlayer(ptr);
	//players[0].setLoopState(ofLoopType::OF_LOOP_PALINDROME);
	//players[1].setLoopState(ofLoopType::OF_LOOP_PALINDROME);

	// video border frame
	frame.setStrokeColor(ofColor::white);
	frame.setStrokeWidth(4);
	frame.setFilled(false);
}


void testApp::setupWatchdog() {
#ifdef DO_WATCHDOG

#ifdef TARGET_WIN32
	wdr = make_unique<WatchDog_Responder>(true, 10000, "../../watchdog");
#endif

#endif
}


