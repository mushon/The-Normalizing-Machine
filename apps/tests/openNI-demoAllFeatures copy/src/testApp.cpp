#include "testApp.h"

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
    
	nearThreshold = 500;
	farThreshold  = 1000;
    
	filterFactor = 0.1f;
	
	interactionState = 0;
	selectedVid = -1;
	userAccumulatedTime = 0;
	
	wdth = 380;//640;
    hght = 480;
	pointerCoords.set(wdth/2,hght/2,0);
	
	yAxisDiff = 300;
	selectionThreshold = 10;
    
	setupRecording();
	intState1 = false;
    
	ofBackground(0, 0, 0);
    
    //tracker.setup();
    
    tmpTracker.setup();
    //recorder = new ofxQTKitAVScreenRecorder();
    recorder.setup();//wdth,hght,generateFileName());
    fboSaver.allocate(wdth, hght, GL_RGB);
    fboPixels.allocate(wdth, hght, OF_IMAGE_COLOR);
    
    fboSaver.begin();
    ofClear(255,255,255,0);
    fboSaver.end();
    ofHideCursor();
	//yoni - video save size
    
    
	chooseVideos();
    
    faceTrackStarted = false;
    recordingFace = true;
    faceState = "wait";
	sessionRecorded = false;
    
    userTrackStart = 0;
    recordStart = ofGetElapsedTimeMillis();
    
    trackWait = 0; 
    trackWaitThresh = 3000; // 5 seconds in milis
    
    userTrackThresh = 7000; // 10 seconds in milis
    recordThresh = 7000; // 15 seconds in milis
    
    vidStartTime  = 0;
    vidPlayTime = 7000;
    
    
    zDist = 400;
	
	
	ofBackground(0); //black background
    
    tilted=1;
    
    if(tilted==1){
        tiltedWidth = ofGetWindowHeight();
        tiltedHeight = ofGetWindowWidth();
    }else{
        tiltedWidth = ofGetWindowWidth();
        tiltedHeight = ofGetWindowHeight();
    }
    
    vidWidth = 380;
    vidHeight = 480;
	
	// setting up font file:
    courierNew.loadFont("Courier_New_Bold.ttf", 14);
    courierSml.loadFont("Courier_New_Bold.ttf", 10);
    
    lastCursorBlink = ofGetElapsedTimeMillis();
    
    rec.loadImage("rec.png");
    pointer.loadImage("pointer.png");
    placeholder.loadImage("placeholder.png");
    c_empty.loadImage("c_empty.png");
    v_empty.loadImage("v_empty.png");
    v_full.loadImage("v_full.png");
    x_empty.loadImage("x_empty.png");
    x_full.loadImage("x_full.png");
    vidPlaceholder.loadImage("vidPlaceholder.png");
    prompt_scr0_h.loadImage("prompt_scr0_h.png");
    prompt_scr0_a.loadImage("prompt_scr0_a.png");
    prompt_instructions.loadImage("prompt_instructions.png");
    normal_h.loadImage("prompt_h_normal.png");
    normal_a.loadImage("prompt_a_normal.png");
    thanks_h.loadImage("thanks_h.png");
    promptTimer = ofGetElapsedTimeMillis();
    blinkTimer = ofGetElapsedTimeMillis();
	
    
    ofEnableAlphaBlending();
    
    // grid setup
    cols = 8;
    rows = 7; // saving first row for "terminal" messages
    gutter = 8;
    unitWidth = (tiltedWidth - gutter*(cols-1))/cols;
    unitHeight = unitWidth * vidHeight/vidWidth;
    //unitHeight = (tiltedHeight - gutter*(rows-1))/rows;
    chooseMiniVideos();
    
    topBanner = 0;
    bottomBanner = -54;
    selected = -1; //no selection
	
	recording = true;
	pointerRot = 0;
	pointerCount = 0;
	promptLang = 0;
	blink = 0;
	screen = 1;
	
	pointing = 0;
	transition = 0;
	line = 0;
    instructions_timeout = 600;
	firstline = 874;
    lineheight = 25;
	
	stateStartTime = ofGetElapsedTimeMillis();
	lastUserTime = ofGetElapsedTimeMillis();
	lastVideoStartTime = ofGetElapsedTimeMillis();
	state0Time = ofGetElapsedTimeMillis();
    
    
	miniVid = 0;
	isTrackingFace = false;
	
	
    
}

//-------------------------------------
void testApp::chooseVideos(){
	vidToPlay = 0;
	
    relistFiles();
    // grabber.initGrabber(wdth, hght);
    // grabber.setDeviceID(1);
    
    vector <string> vidFilesTemp = videoFiles;
	while (videoPlayers.size() > 0){
		videoPlayers.erase(videoPlayers.begin());
	}
    
    for (int i = 0; i<4; i++){
        ofVideoPlayer vidPl;
        vidPl.setLoopState(OF_LOOP_NORMAL);
		int size = vidFilesTemp.size();
        int new_pick = (int)ofRandom(size); 
        string fileNamePath =  vidFilesTemp.at(new_pick);//ofRandom(vidFilesTemp.size()));
		if (size > 2){
			fileNamePath =  vidFilesTemp.at(new_pick);//ofRandom(vidFilesTemp.size()));
		}
        vidFilesTemp.erase(vidFilesTemp.begin() + new_pick);
        vidPl.loadMovie(fileNamePath);
        vidPl.play();
        videoPlayers.push_back(vidPl);
    }
	
}
//-------------------------------------
//new update
void testApp::chooseMiniVideos(){
	relistFiles();
    
    vector <string> vidFilesTemp = videoFiles;
	
	while (miniVideoPlayers.size() > 0){
		miniVideoPlayers.erase(miniVideoPlayers.begin());
	}
	while (miniVideoPlayers.size() > 0){
		miniVideoNames.erase(miniVideoNames.begin());
	}
	
    int n = cols * rows;
	
    for (int i = 0; i<n; i++){
        ofVideoPlayer vidPl;
        vidPl.setLoopState(OF_LOOP_NORMAL);
		int size = vidFilesTemp.size();
        int new_pick = (int)ofRandom(size);
        string fileNamePath =  vidFilesTemp.at(new_pick);//ofRandom(vidFilesTemp.size()));
		if (size > 2){
			fileNamePath =  vidFilesTemp.at(new_pick);//ofRandom(vidFilesTemp.size()));
		}
		if (size < 2){
			relistFiles();
			vidFilesTemp = videoFiles;
		}
        vidFilesTemp.erase(vidFilesTemp.begin() + new_pick);
        vidPl.loadMovie(fileNamePath);
        vidPl.play();
        miniVideoPlayers.push_back(vidPl);
		miniVideoNames.push_back(fileNamePath.substr(1,12));
    }
}
//new update


//----------------------------------------------------------------------------------------------------------------------------------

void testApp::setupRecording(string _filename) {
    
#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	hardware.setup();				// libusb direct control of motor, LED and accelerometers
	hardware.setLedOption(LED_OFF); // turn off the led just for yacks (or for live installation/performances ;-)
#endif
    
	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	//recordContext.setupUsingXMLFile();
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);
    
	recordUser.setup(&recordContext, true);
	recordUser.setSmoothing(filterFactor);				// built in openni skeleton smoothing...
	recordUser.setUseMaskPixels(isMasking);
	recordUser.setUseCloudPoints(isCloud);
	recordUser.setMaxNumberOfUsers(1);					// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)
    
    
    
	recordHandTracker.setup(&recordContext, 4);
	recordHandTracker.setSmoothing(filterFactor);		// built in openni hand track smoothing...
	recordHandTracker.setFilterFactors(filterFactor);	// custom smoothing/filtering (can also set per hand with setFilterFactor)...set them all to 0.1f to    begin with
    
    recordHandTracker.setMaxNumHands(2);
    
	recordContext.toggleRegisterViewport();
	recordContext.toggleMirror();
    
    
}

void testApp::setupPlayback(string _filename) {
    
	playContext.shutdown();
	playContext.setupUsingRecording(ofToDataPath(_filename));
	playDepth.setup(&playContext);
	playImage.setup(&playContext);
    
	playUser.setup(&playContext, true);
	playUser.setSmoothing(filterFactor);				// built in openni skeleton smoothing...
	playUser.setUseMaskPixels(isMasking);
	playUser.setUseCloudPoints(isCloud);
    
	playHandTracker.setup(&playContext, 4);
	playHandTracker.setSmoothing(filterFactor);			// built in openni hand track smoothing...
	playHandTracker.setFilterFactors(filterFactor);		// custom smoothing/filtering (can also set per hand with setFilterFactor)...set them all to 0.1f to begin with
    
	playContext.toggleRegisterViewport();
	playContext.toggleMirror();
    
}

//--------------------------------------------------------------
void testApp::update(){
    
#ifdef TARGET_OSX // only working on Mac at the moment
	hardware.update();
    
#endif
	//yoni!!//
	//flush trackwait in all facestates
	
	// update all nodes
	recordContext.update();
	recordDepth.update();
	recordImage.update();
	recordUser.update();
	
	if (interactionState == 0){ //standby
		screen = 0;
		//display welcome & instructions message (Hello.jpg)
		//if user identified switch to interactionState = 1 
		//interactionState = 1;
		//new update
		if(transition == 0){
			if (ofGetElapsedTimeMillis() - stateStartTime < 1000){
				for (int i = 0; i< miniVideoPlayers.size(); i++){
					miniVideoPlayers[i].setPosition(0);
					miniVideoPlayers[i].update();
					miniVideoPlayers[i].play();
				}
			}
			else{
				for (int i = 0; i< miniVideoPlayers.size(); i++){
					if (i != miniVid){
						miniVideoPlayers[i].stop();
					}
				}
			}
			if (ofGetElapsedTimeMillis() - state0Time >= 5000){
				miniVid = (miniVid+1)%miniVideoPlayers.size();
				miniVideoPlayers[miniVid].setPosition(0);
				state0Time = ofGetElapsedTimeMillis();
			}
			miniVideoPlayers[miniVid].update();
			miniVideoPlayers[miniVid].play();
			
			//new update
			
			//if (recordUser.getNumberOfTrackedUsers() > 0){
			if (userInFrame2(wdth,hght)){
				if (ofGetElapsedTimeMillis() - stateStartTime > 5000){
					cout << " accepted, elapsed time: " << ofToString(ofGetElapsedTimeMillis() - stateStartTime )<< endl;
					
					transition = 1;
					
					//chooseVideos();
					userAccumulatedTime = 0;
					trackWait = ofGetElapsedTimeMillis();
					userTrackStart = ofGetElapsedTimeMillis();
					lastVideoStartTime = ofGetElapsedTimeMillis();
					lastVideoSelectedTime = ofGetElapsedTimeMillis();
					selectedVid = -1;
					/*
					 //interactionState = 1; 
					 sessionRecorded = false;
					 faceState = "wait";
					 //screen = 1;
					 transition = 1;
					 stateStartTime =ofGetElapsedTimeMillis();
					 //setupRecording();
					 if(recorder.getIsRecording()){
					 recorder.finishMovie();
					 cout << "STOPPPED (interaction 0) RECORDING" << endl;
					 }
					 //recorder.finishMovie();
					 //recorder = new ofxQTKitAVScreenRecorder();
					 
					 recorder.setup(); //wdth,hght,generateFileName());
					 fboSaver.allocate(wdth, hght, GL_RGB);
					 fboPixels.allocate(wdth, hght, OF_IMAGE_COLOR);
					 
					 fboSaver.begin();
					 ofClear(255,255,255,0);
					 fboSaver.end();
					 */
				}
				else{
					//cout << " user, elapsed time: " << ofToString(ofGetElapsedTimeMillis() - stateStartTime )<< endl;
					
				}
			}
			else{
				//cout << " no user, elapsed time: " << ofToString(ofGetElapsedTimeMillis() - stateStartTime )<< endl;
				//no user in frame
				//stateStartTime = ofGetElapsedTimeMillis();
				
			}
		}
		else{
			//cout << " transition = 1, " << ofToString(transition)<< endl;
			//stateStartTime = ofGetElapsedTimeMillis();
			//cout << ofToString(recordUser.getNumberOfTrackedUsers()) << " getNumberOfTrackedUsers, time: " << ofToString(ofGetElapsedTimeMillis() - stateStartTime)<< endl;
		}
		
	}
	if (interactionState == 1){ //identified
		//do all below actions (start recording, playing, chosing videos
		//if user is lost switch to interactionState = 3 
		//if user made selection switch to interactionState = 2
		//stateStartTime = now (for either 2/3 states)
		
		
		unsigned char *pixels = recordImage.getPixels();
		if (recordUser.getNumberOfTrackedUsers() > 0){ //tracker.getFound()) { //instead use recordUser.getNumberOfTrackedUsers() > 0
			//cout << ofToString(userAccumulatedTime) << " userAccumulatedTime " << endl << faceState << ", facestate, " << ofToString(sessionRecorded) << " <<< sessionRecorded"  << endl;
			//cout << ofToString(ofGetElapsedTimeMillis() - recordStart) << " recorded time, " << endl << ofToString(recorder.getIsRecording()) << ", isRecording, "  << endl;
			
			
			if(faceState == "wait" && (!sessionRecorded)){
				if (ofGetElapsedTimeMillis() - userTrackStart > 2000) {
					cout << "RECORDING A NEW ONE!" << endl;
					// LETS SEE IF 2 SECONDS HAVE PASSED (CURRENT TIME - STARTTIME FOR TRACKING > THRESHOLD)                            
					recorder.startNewRecording();
					recordStart = ofGetElapsedTimeMillis();
					faceState = "recording";                            
				}
			}
			
			if (faceState == "recording"){
				
				int   rw = 380;//out of 640
				int   rh = 480;
				int     x = (640-rw)/2;
				int     y = (480-rh)/2;//full height
				unsigned char face[ rw * rh * 3 ];  
				
				for (int i = 0; i < rw; i++){  
					for (int j = 0; j < rh; j++){  
						
						int mainPixelPos = ((j + y) * 640 + (i + x)) * 3;  
						int subPixlPos = (j * (rw) + i) * 3;  
						
						face[subPixlPos] = pixels[mainPixelPos];   // R  
						face[subPixlPos + 1] = pixels[mainPixelPos + 1];  // G  
						face[subPixlPos + 2] = pixels[mainPixelPos + 2];  // B  
						
						
					}
				}
				faceImg.clear();
				faceImg.setFromPixels(face, rw, rh, OF_IMAGE_COLOR); 
				// RECORDING:::                
				if(recorder.getIsRecording()) {
					recorder.update(); 
					fboSaver.begin();
					ofClear(0,0,0,0);
					fboSaver.end();
					fboSaver.begin();        
					faceImg.draw(0,0);
					fboSaver.end();
					fboSaver.readToPixels(fboPixels);
					recorder.addFrame(fboPixels);
					// CHECK IF WE RECORDED EVERYTHING:                    
					
				}
				if (ofGetElapsedTimeMillis() - recordStart > 5000 && faceState=="recording"){
					sessionRecorded = true;
					faceState = "wait";
					trackWait = ofGetElapsedTimeMillis();
					
					cout << "STOPPPED RECORDING!" << endl;
					
					recorder.finishMovie();
				}
			} 			
		}
		
		if (recordUser.getNumberOfTrackedUsers() > 0){
			recordUser.startTracking(0);
			recordUser.setUseCloudPoints(true);
			user = recordUser.getTrackedUser(0);
		}
		else{
			//cout << " <<< User getNumberOfTrackedUsers == 0"  << endl;
		}
		if (ofGetElapsedTimeMillis() - stateStartTime > 500){
			for (int i = 0; i< videoPlayers.size(); i++){
				if (i != vidToPlay)
				{
					videoPlayers[i].stop();
				}
				else {
					if (ofGetElapsedTimeMillis() -lastVideoStartTime >= vidPlayTime){
						videoPlayers[i].setPosition(0);
						lastVideoStartTime = ofGetElapsedTimeMillis();
					}
					videoPlayers[i].update();                
					videoPlayers[i].play();
				}
			}
		}else {
			for (int i = 0; i< videoPlayers.size(); i++){
                videoPlayers[i].setPosition(0);
				videoPlayers[i].update();
				videoPlayers[i].play();
			}
		}    
		//if lost user
		
		if (recordUser.getNumberOfTrackedUsers() == 0) { //recordUser.getNumberOfTrackedUsers() > 0
			//cout << ofToString(recordUser.getNumberOfTrackedUsers()) << " getNumberOfTrackedUsers, " << ofToString(ofGetElapsedTimeMillis() - lastUserTime) << " lastUserTime diff " << endl;
			if (ofGetElapsedTimeMillis() - lastUserTime > 3000){
				intState1 = false;
				interactionState = 3; 
				screen = 2;
			}
		}
		else{
			lastUserTime = ofGetElapsedTimeMillis();
		}
		//if user made selection
		if (selectedVid >= 0 && sessionRecorded){
			//cout << ofToString(selectedVid) << " selectedVid, " << ofToString(userAccumulatedTime) << " userAccumulatedTime " << endl;
			if (userAccumulatedTime > userTrackThresh) { 
				interactionState = 2; //here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				screen = 2;
				intState1 = false;
				stateStartTime = ofGetElapsedTimeMillis();
				userAccumulatedTime = 0;
			}
		}
		else{
			//cout << ofToString(selectedVid) << " selectedVid, not playing. " << ofToString(userAccumulatedTime) << " userAccumulatedTime "  << endl;
		}
	}
	
	if (interactionState == 2){ //completed
		//display thank you message (ThankYou.jpg)
		//after 15 secs switch to interactionState = 4 
		//tmpTracker.reset();
		screen = 2;
        if (ofGetElapsedTimeMillis() -lastVideoStartTime >= vidPlayTime){
            videoPlayers[selectedVid].setPosition(0);
            lastVideoStartTime = ofGetElapsedTimeMillis();
        }
        videoPlayers[selectedVid].update();
        videoPlayers[selectedVid].play();
		if (ofGetElapsedTimeMillis() - stateStartTime > 1000){ 
			
			sessionRecorded = false;
			faceState = "wait";
		}
		if (ofGetElapsedTimeMillis() - stateStartTime > 15000){ interactionState = 4; }
		
	}
	
	if (interactionState == 3){ //lost
		//display session distrupted message (Distrupted.jpg)
		//after 3 secs switch to interactionState = 4 
		if (ofGetElapsedTimeMillis() - stateStartTime > 3000){ interactionState = 4; }
	}
	if (interactionState == 4){ //end: session ended (move to 0); make sure user is clear, videos closed, etc.
		//make sure user is clear, videos closed
		//switch to interactionState = 0 
		//stateStartTime = now (for state 0)
		stateStartTime = ofGetElapsedTimeMillis();
		interactionState = 0;
		selectedVid = -1;
		transition = 0;
		
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	if(tilted==1){
        //Rotate 90 deg
        ofPushMatrix();
        ofTranslate(ofGetWindowWidth()/2, ofGetWindowHeight()/2, 0);
        ofRotate(-90);
        ofTranslate(-ofGetWindowHeight()/2, -ofGetWindowWidth()/2, 0);
    }
    
    if(screen > 0 ){                                                    
        if(screen == 2){
            selected = selectedVid;
			
            selectedX = tiltedWidth/2 - (selected%2)*(vidWidth+gutter) - vidWidth/2;
            selectedY = tiltedHeight/2 - floor(selected/2)*(vidHeight+gutter) - vidHeight/2;
            
            if (curSelectedX != selectedX){
                curSelectedX = (5*curSelectedX+selectedX)/6;
            }
            if (curSelectedY != selectedY){
                curSelectedY = (5*curSelectedY+selectedY)/6;
            }
            
            ofPushMatrix();
            ofTranslate( curSelectedX, curSelectedY, 0);
        }
        
        //Video Frames
        
        for (int i=0; i<4; i++){
            if (screen == 2){
                if(i==selectedVid){
                    ofSetColor(255);
				} else {
					int clr = ABS(curSelectedX-selectedX);
					if (ABS(clr > 255)){
						clr = 255;
					}
                    ofSetColor(clr);      //frame color
				}
            } else if (screen == 1){
                ofSetColor(255);
            }
            // Insert inspected video here (out of 4)
			//ofSetColor(200);
			videoPlayers[i].draw((i%2)*(vidWidth+gutter),floor(i/2)*(vidHeight+gutter)+topBanner,vidWidth, vidHeight);
			/*ofRect((i%2)*(vidWidth+gutter),
			 floor(i/2)*(vidHeight+gutter)+topBanner,
			 vidWidth,
			 vidHeight);*/
			//ofSetColor(255);
            c_empty.draw((i%2)*(tiltedWidth-81)+(1-2*(i%2))*10, floor(i/2)*(2*vidHeight+gutter-81)+(1-2*(floor(i/2)))*10);
			
			if (pointing==1){ //should be replaced with hover conditioning
				
                if(i==selectedVid){
                    v_empty.draw((i%2)*(tiltedWidth-81)+(1-2*(i%2))*10, floor(i/2)*(2*vidHeight+gutter-81)+(1-2*(floor(i/2)))*10);
                } else {
                    x_empty.draw((i%2)*(tiltedWidth-81)+(1-2*(i%2))*10, floor(i/2)*(2*vidHeight+gutter-81)+(1-2*(floor(i/2)))*10);
                }
            } else if(screen == 2 && i==selectedVid){
				v_empty.draw((i%2)*(tiltedWidth-81)+(1-2*(i%2))*10, floor(i/2)*(2*vidHeight+gutter-81)+(1-2*(floor(i/2)))*10);
			} 
        }
		//reset pointing to 0:
		pointing=0;
        //user frame (including outlines)
        ofSetColor(0);
        ofRect(285,417,206,256);
        
        ofPushMatrix();                 // push the current coordinate position
        ofTranslate(293, 425, 0);
        ofScale(0.5, 0.5, 1);
        
        ofSetColor(255);
		recordImagePixels = recordImage.getPixels();
		drawCropped(380, 480);			
		//recordImage.draw(0, 0, 380, 480);
        //cam.draw(0,0);              //----------------------<<<< CAMERA GOES HERE!!!
        ofPopMatrix();                  // pop the tracking camera
        
        //facetracking drawing
        if(screen == 2 && abs(curSelectedX-selectedX + curSelectedY-selectedY)<1){// && tmpTracker.getFound()) { //-------------<<<<< SHOULD GO ON PLAYING VIDEO
            //yoni - notice copy
			cout << ofToString(selectedVid) << " selectedVid, waiting for facetracking. " << endl; //ofToString(userAccumulatedTime) << " userAccumulatedTime "  << endl;
			
			unsigned char *tmpPixels = videoPlayers[selectedVid].getPixels();
			ofImage tmpFaceImg;
			tmpFaceImg.setFromPixels(tmpPixels, 320, 240, OF_IMAGE_COLOR);
			tmpFaceImg.setFromPixels(tmpPixels, wdth, hght, OF_IMAGE_COLOR);
			Mat tmpImgMat = toCv(tmpFaceImg);
			tmpTracker.update(tmpImgMat);
			
			ofSetColor(255,0,255); // changing to purple
			ofSetLineWidth(0.5);
			ofPushMatrix();				// push playing video with face detection
			
			int transX = (vidToPlay%2)*(vidWidth+gutter);
			int transY = floor(vidToPlay/2)*(vidHeight+gutter);
			
			ofTranslate(transX, transY);
			
			
            tmpTracker.draw();
            
            /// START NOSE OVERLAY ------------------------------- ///
            
            ofPolyline NoseLine;
            
            NoseLine = tmpTracker.getImageFeature(tmpTracker.NOSE_BRIDGE);
            
            float noseX;
            
            ofPoint noseTop, noseBottom;
            noseTop.set(NoseLine.getVertices()[0]);
            noseBottom.set(NoseLine.getVertices()[3]);
            
            float noseDist = ofDist(noseTop.x,
                                    noseTop.y,
                                    noseBottom.x,
                                    noseBottom.y);
            
            float offset = noseDist*3;
            
            ofSetColor(255); // changing to purple
            
            //define the position of the bar
            if (noseTop.x > noseBottom.x){
                noseX = noseTop.x + offset;
            } else {
                noseX = noseBottom.x + offset;
            }
            
            //draw vertical lines from eyes
            ofLine(noseTop.x, noseTop.y,
                   noseX, noseTop.y);
            
            ofLine(noseBottom.x, noseBottom.y,
                   noseX, noseBottom.y);
            
            //draw the distance number
            courierSml.drawString(
								  "Nose: " + ofToString(noseDist),
								  noseX + 5,
								  noseTop.y);
            
            ofSetColor(0,0,255); // changing to blue
            
            //draw the connecting lineÉ
            ofLine(noseX, noseTop.y,
                   noseX, noseBottom.y);
            
            int rectSize = 5;
            int rectOffset = 2;
            
            //and the rectangles at its corners
            ofRect(noseX-rectOffset,noseTop.y-rectOffset,rectSize,rectSize);
            ofRect(noseX-rectOffset,noseBottom.y-rectOffset,rectSize,rectSize);
            
            // END NOSE OVERLAY //
            
            /// START MOUTH OVERLAY ------------------------------- ///
            
            ofPolyline mouthLine;
            
            mouthLine = tmpTracker.getImageFeature(tmpTracker.INNER_MOUTH);
            
            float mouthX;
            
            ofPoint mouthTop, mouthBottom;
            mouthTop.set(mouthLine.getVertices()[2]);
            mouthBottom.set(mouthLine.getVertices()[6]);
            
            float mouthDist = ofDist(mouthTop.x,
									 mouthTop.y,
									 mouthBottom.x,
									 mouthBottom.y);
            
            ofSetColor(255); // changing to white
            
            //define the position of the bar
            if (mouthTop.x > mouthBottom.x){
                mouthX = mouthTop.x + offset*0.75;
            } else {
                mouthX = mouthBottom.x + offset*0.75;
            }
            
            //draw vertical lines from mouth
            ofLine(mouthTop.x, mouthTop.y,
                   mouthX, mouthTop.y);
            
            ofLine(mouthBottom.x, mouthBottom.y,
                   mouthX, mouthBottom.y);
            
            //draw the distance number
            courierSml.drawString(
								  "Lips: " + ofToString(mouthDist),
								  mouthX + 5,
								  mouthTop.y);
            
            ofSetColor(0,0,255); // changing to blue
            
            //draw the connecting lineÉ
            ofLine(mouthX, mouthTop.y,
                   mouthX, mouthBottom.y);
            
            //and the rectangles at its corners
            ofRect(mouthX-rectOffset,mouthTop.y-rectOffset,rectSize,rectSize);
            ofRect(mouthX-rectOffset,mouthBottom.y-rectOffset,rectSize,rectSize);
            
            // END MOUTH OVERLAY //
            
            /// START EYES OVERLAY ------------------------------- ///
            
            ofPolyline leftEyeLine, rightEyeLine, rightEyeLineCenter, leftEyeLineCenter;
            
            leftEyeLine = tmpTracker.getImageFeature(tmpTracker.LEFT_EYE);
            rightEyeLine = tmpTracker.getImageFeature(tmpTracker.RIGHT_EYE);
            
            
            //float eyesOffset = -50;
            float eyesY;
            
            ofPoint leftEyeCenter, rightEyeCenter;
            leftEyeCenter.set(leftEyeLine.getBoundingBox().getCenter());
            rightEyeCenter.set(rightEyeLine.getBoundingBox().getCenter());
            
            ofSetColor(255); // changing to white
            
            //define the position of the bar
            if (leftEyeCenter.y > rightEyeCenter.y){
                eyesY = rightEyeCenter.y - offset;
            } else {
                eyesY = leftEyeCenter.y - offset;
            }
            
            //draw vertical lines from eyes
            ofLine(leftEyeCenter.x, leftEyeCenter.y,
                   leftEyeCenter.x, eyesY);
            
            ofLine(rightEyeCenter.x, rightEyeCenter.y,
                   rightEyeCenter.x, eyesY);
            
            //draw the distance number
            courierSml.drawString(
								  "Eye distance: " + ofToString(ofDist(leftEyeCenter.x,
																	   leftEyeCenter.y,
																	   rightEyeCenter.x,
																	   rightEyeCenter.y)),
								  leftEyeCenter.x,
								  eyesY - 5);
            
            ofSetColor(0,0,255); // changing to blue
            
            //draw the connecting lineÉ
            ofLine(leftEyeCenter.x, eyesY,
                   rightEyeCenter.x, eyesY);
            
            //and the rectangles at its corners
            ofRect(leftEyeCenter.x-rectOffset,eyesY-rectOffset,rectSize,rectSize);
            ofRect(rightEyeCenter.x-rectOffset,eyesY-rectOffset,rectSize,rectSize);
			
            // END EYES OVERLAY //
            
            // DRAW THE CONTOURS OF THE FIGURE ------------------------------------//
            
            // Not sure how to do this but I think ofxCvContourFinder() has something to do with it.
            
            // Maybe that's another direction:
            // ofPolyline allPoints;
            // allPoints = tracker.getImagePoints(i);
            
            // Two more relevant links (with code):
            // http://forum.openframeworks.cc/index.php?topic=1138.0
            // http://susemiessner.org/projects_mma_ex13.html
            
            //easyCam.begin();
			//		ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
			//		ofTranslate(640 / 2, 480 / 2);
			//		applyMatrix(rotationMatrix);
			//		ofScale(5,5,5);
			//		ofDrawAxis(scale);
			//		tracker.getObjectMesh().drawWireframe();
            //easyCam.end();
			ofPopMatrix();				// pop playing video with face detection 
        }
        
		
        
        if(screen==2){
            ofPopMatrix();                  // pop selection screen
        }
        
        // GRAPHICS ---------------------------------------------------------- //
        
        
        
        // POINTER:
        
        if(screen==1){
            ofPushMatrix();                 // push the current coordinate position
			if (recordUser.getNumberOfTrackedUsers() > 0){
				pointerCoords = getPointerCoordinates();
				if (pointerCoords.z > 0){
					pointing = 1;
				}
				else{
					pointing = 0;
					vidToPlay = -1;
				}
			}
			if (pointing == 1){
				ofTranslate(pointerCoords.x, pointerCoords.y, 0);
				//ofTranslate(mouseX, mouseY, 0);
				
				ofSetColor(255);                // changing to white
				
				pointerCount = floor(userTrackThresh/1000) - floor((userAccumulatedTime)/1000);
				
				if (pointerCount < 10 && pointerCount >= 0){
					courierNew.drawString(ofToString((int) pointerCount), -5, 5);
				} else if (pointerCount >= 10){
					ofLine(0,5,0,-5);
					ofLine(5,0,-5,0);
				} else {
					// Selection made
					ofLine(0,50,0,-50);
					ofLine(50,0,-50,0);
				}
				
				ofRotate(pointerRot*3);         // change the coordinate system
				pointerRot++;
				pointer.draw(-38,-38);          // draw the pointer
				ofPopMatrix();                  // recall the pushed coordinate position
			}
        }
        
        
        
    } 
	else if (screen == 0){
        
        ofBackground(0); //black background
		
		// idle mode
        // presented between when user leaves & new user steps in
        // draw video grid
        
        //Video Frames
        
        if(transition == 1){
            if (linesY > -1.05*tiltedHeight){
                if (line%3 == 0){                   // %number sets the speed of the transition
                    linesY -= 25;                   // push the current coordinate position
                }
            } else{
                if (line > instructions_timeout){   // time left to read the instructions
                    //screen++;
					if (recordUser.getNumberOfTrackedUsers() > 0){
						
						////yoni - notice from state = 1
						chooseVideos();
						userAccumulatedTime = 0;
						trackWait = ofGetElapsedTimeMillis();
						userTrackStart = ofGetElapsedTimeMillis();
						lastVideoStartTime = ofGetElapsedTimeMillis();
						lastVideoSelectedTime = ofGetElapsedTimeMillis();
						stateStartTime =ofGetElapsedTimeMillis();
						sessionRecorded = false;
						faceState = "wait";
						selectedVid = -1;
						interactionState = 1; 
						screen = 1;
						line = linesY = transition = 0; // reset everything
						//interactionState = 1; 
						//screen = 1;
						//transition = 1;
						
						//setupRecording();
						if(recorder.getIsRecording()){
							recorder.finishMovie();
							cout << "STOPPPED (interaction 0) RECORDING" << endl;
						}
						//recorder.finishMovie();
						//recorder = new ofxQTKitAVScreenRecorder();
						
						recorder.setup(); //wdth,hght,generateFileName());
						fboSaver.allocate(wdth, hght, GL_RGB);
						fboPixels.allocate(wdth, hght, OF_IMAGE_COLOR);
						
						fboSaver.begin();
						ofClear(255,255,255,0);
						fboSaver.end();
					}
					else{
						//todo:yoni - go back to 0
						stateStartTime = ofGetElapsedTimeMillis();
						selectedVid = -1;interactionState = 0; 
						line = linesY = transition = 0;
						screen = 0;
						
					}
                }
            }
            //
            line++;
            
            //courierSml.drawString(ofToString((int) line), 748, 50);    //draw the instructions countdown
            ofPushMatrix();
            ofTranslate(0, linesY, 0);
            
            
        }
		
        
        // Begin loop for columns
        for (int i = 0; i < cols; i++) {
            // Begin loop for rows
            for (int j = 0; j < rows; j++) {
                
                // Scaling up to draw a rectangle at (x,y)
				int x = i*(unitWidth + gutter);
				int y = j*(unitHeight + gutter); // added 1 to offset first row
				// For every column and row, a rectangle is drawn at an (x,y) location scaled and sized by videoScale.
				
				ofSetColor(200);            //just as a marker
				
				//new update
				int pos = i*rows+j;
				//miniVideoPlayers[pos].play();
				if (pos == miniVid){
					miniVideoPlayers[pos].play();
				}
				
				miniVideoPlayers[pos].draw(x,y,unitWidth, unitHeight);
				//new update
				
                //ofRect(x,y,unitWidth,unitHeight); // <<<<--------------------Replace with videos
				
                x += (unitWidth - 60)/2; // using x for the banner
                y += unitHeight - 13; // using y for the banner
                ofSetColor(0);
				
                ofRect(x,y,60,13);
                ofSetColor(255);
                userID = "#0000"+ofToString((int) i+j*cols); //<<<<----------Video ID
                courierSml.drawString(userID, x+2, y+13); // draw the framerate
            }
        }
        
	}
	
	
    // PROMPT:
    ofSetColor(0);                // changing to black
    ofRect(0,0,tiltedWidth,topBanner);
    
    ofSetColor(122);                // changing to gray
    // in draw:
    ofSetColor(122);                // changing to gray
    // in draw:
	
    if (screen == 0){
        for(int i=0;i<46;i++){
            courierNew.drawString("TNM$", 10, 874+i*25);
        }
    }
    courierNew.drawString("TNM$", 10, 999);
    courierNew.drawString("TNM$", 10, 1024);
    
    ofSetColor(255);                // changing to white
    
    // in draw:
    if (ofGetElapsedTimeMillis() - promptTimer > 5000){
        promptLang ++;
        promptLang = promptLang % 3;
        promptTimer = ofGetElapsedTimeMillis();
    }
    
    if (screen == 1){
        if (promptLang == 0){
            courierNew.drawString("Who's the most normal?", 60, 999);
        } else if (promptLang == 1) {
            normal_h.draw(60, 984);
        } else {
            normal_a.draw(60, 984);
        }
    } 
	else if (screen == 0){
		if (promptLang == 0){ // English
            courierNew.drawString("this is a computer science experiment", 60, firstline);
            courierNew.drawString("using social pattern recognition", 60, firstline+lineheight);
            courierNew.drawString("to finally decode: ", 60, firstline+lineheight*2);
            ofSetColor(255,0,0);
            courierNew.drawString("who is normal?", 60, firstline+lineheight*3);
            ofSetColor(255);
            courierNew.drawString("awaiting next subject", 60, firstline+lineheight*5);
        }else if(promptLang == 1){ // Hebrew
            prompt_scr0_h.draw(60, firstline-15);
        }else if(promptLang == 2){ // Arabic
            prompt_scr0_h.draw(60, firstline-15);
        }
        if(transition==1){
            courierNew.drawString("subject found", 60, firstline+lineheight*6);
            courierNew.drawString("setting upÉ", 60, firstline+lineheight*7);
            
            // English
            courierNew.drawString("Welcome.", 60, firstline+lineheight*12);
            courierNew.drawString("This software aims at automatically identifying normal people.", 60, firstline+lineheight*13);
            courierNew.drawString("Please assist the machine in learning the task.", 60, firstline+lineheight*14);
            courierNew.drawString("Select the MOST NORMAL person of the 4 people presented at", 60, firstline+lineheight*15);
            courierNew.drawString("the next screen by pointing at it for several seconds.", 60, firstline+lineheight*16);
            
            courierNew.drawString("Thank you.", 60, firstline+lineheight*18);
            
            // Arabic & Hebrew
            prompt_instructions.draw(57, firstline+lineheight*21-15);
            
            courierNew.drawString("........................................................................", 60, firstline+lineheight*45);
            ofSetColor(0);
            ofRect(60+tiltedWidth*(instructions_timeout-line)/instructions_timeout, firstline+lineheight*45-5, tiltedWidth, 5);
            ofSetColor(255);
        }
    } else if (screen == 2){
        if (promptLang == 0){ // placeholder for more language support
            courierNew.drawString("thank you for your contribution", 60, firstline+lineheight*5);
            courierNew.drawString("you're now added to the database", 60, firstline+lineheight*6);
        }else if(promptLang == 1){ // Hebrew
            thanks_h.draw(60, firstline+lineheight*5-15);
        }else if(promptLang == 2){ // Arabic
            thanks_h.draw(60, firstline+lineheight*5-15); /////////////////////
        }
    }
	
    
    // TEXT DOT & REC BLINKING
    
    if (ofGetElapsedTimeMillis() - blinkTimer > 500){
        blink ++;
        blink = blink % 2;
        blinkTimer = ofGetElapsedTimeMillis();
    }
    
    if(screen == 1){ // recording screen
        if (blink == 0){
            ofRect(60,firstline+lineheight*6-12,10,14);
            
        } else {
            rec.draw(296,428); // record mark
        }
    } else if (screen == 0 && transition==0){
        if (blink == 0){
            ofRect(60,firstline+lineheight*6-12,10,14);
        }
    }
    
    if(transition == 1){
        ofPopMatrix();
    }
    
    ofSetColor(0,255,0);
    //courierSml.drawString(ofToString((int) ofGetFrameRate()), 748, 15);    //draw the framerate
    ofSetColor(255);
	
    if(tilted==1){
        ofPopMatrix(); // out of 90 degrees
    }    
}

//--------------------------------------------------------------
bool testApp::userInFrame(int croppedwidth, int croppedheight, int userID){
	bool userInRange = false;
	ofxUserGenerator * user_generator = &recordUser;
	if (recordUser.getNumberOfTrackedUsers() > 0){
		int x = (640 - croppedwidth)/2;
		int y = (480 - croppedheight)/2;
		for (int i = 0; i < croppedwidth; i++){
			for (int j = 0; j < croppedheight; j++){
				ofPoint pos = user_generator->getWorldCoordinateAt(x+i, y+j, userID);
				if (pos.z == 0 && isCPBkgnd) continue;	// gets rid of background -> still a bit weird if userID > 0...
				else if (pos.z > 0) {
					userInRange = true;
					return userInRange;
				}
			}
		}
		return userInRange;
	}
	return userInRange;
}

bool testApp::userInFrame2(int croppedwidth, int croppedheight){
	bool userInRange = false;
	ofxUserGenerator * user_generator = &recordUser;
	if (recordUser.getNumberOfTrackedUsers() > 0){
		if (recordHandTracker.getNumTrackedHands()>0){
			ofPoint handPos;
			int x = (640 - croppedwidth)/2;
			int y = (480 - croppedheight)/2;
			int handIndex = 0;
			ofxTrackedHand* handOne =  recordHandTracker.getHand(0);
			handPos = handOne->projectPos;
			int maxhands = MAX(4,recordHandTracker.getNumTrackedHands());
			for (int i = 1; i < maxhands; i++){
				ofxTrackedHand* handtmp =  recordHandTracker.getHand(i);
				if (handtmp != NULL){
					if (handPos.z > (handtmp->projectPos).z || handPos.z < 1 ){
						if ((handtmp->projectPos).z > 1){
							handPos = handtmp->projectPos;
							handOne = handtmp;
							handIndex = i;
						}
					}
				}
			}
			if (handPos.x > x && handPos.x < 640-x){
				if (handPos.y > y && handPos.y < 480 - y){
					userInRange = true;
				}
			}
		}
	}
	return userInRange;
}


//--------------------

//--------------------
ofPoint testApp::getPointerCoordinates() {
	
	/////////////////////////////////////
	// Calc user tracked hands time
	// to identify userAccumulatedTime
	// on specific video
	/////////////////////////////////////
	
    float pcx = 0;
    float pcy = 0;
    float pcz = 0;
	
	if (recordUser.getNumberOfTrackedUsers() > 0){
		ofPoint handPos;
		if (intState1){
			centerHead = getCenterHead(&recordUser);
			if (centerHead.x > 0){
				intState1 = true;
			}
			
		}
		if (recordHandTracker.getNumTrackedHands() == 0){
			userAccumulatedTime = 0;
			lastVideoStartTime = ofGetElapsedTimeMillis();
			lastVideoSelectedTime = ofGetElapsedTimeMillis();
		}
		else if (recordHandTracker.getNumTrackedHands()>0){
			int handIndex = 0;
			ofxTrackedHand* handOne =  recordHandTracker.getHand(0);
			handPos = handOne->projectPos;
			int maxhands = MAX(4,recordHandTracker.getNumTrackedHands());
			for (int i = 1; i < maxhands; i++){
				ofxTrackedHand* handtmp =  recordHandTracker.getHand(i);				
				if (handtmp != NULL){
					if (handPos.z > (handtmp->projectPos).z || handPos.z < 1 ){
						if ((handtmp->projectPos).z > 1){
							handPos = handtmp->projectPos;
							handOne = handtmp;
							handIndex = i;
						}
					}
				}
			}
			int xgutter = (640-380)/2;
			int ycenter = 1024/2;
			int xcenter = 768/2;
			pcx = ofMap(handPos.x, xgutter, 640-xgutter, 0, 768);
			pcy = ofMap(handPos.y, MAX(centerHead.y - 300,0), MIN(centerHead.y+300,hght),0,1024, true);
			pcz = handPos.z;
			//if ((abs(rightHand.position[1].Z - neck.position[1].Z) > zDist) && ( ofGetElapsedTimeMillis() -  vidStartTime > vidPlayTime ) ){
			if ((abs(centerHead.z - pcz) > zDist)){// && ( ofGetElapsedTimeMillis() -  vidStartTime > vidPlayTime ) ){    
				int oldVidToPlay = vidToPlay;
				
				// !! LITTLE MINDFUCK: Y-AXIS IS INVERTED HERE, DUNNO WHY, THAT'S HOW IT GOES I GUESS      
				
				//if (rightHand.position[1].Y < neck.position[1].Y){
				if (pcy < ycenter){
					if (pcx > xcenter){
						vidToPlay = 1;
						selectedVid =1;
						if (vidToPlay != oldVidToPlay){
							lastVideoStartTime = ofGetElapsedTimeMillis();
							lastVideoSelectedTime = ofGetElapsedTimeMillis();
						}
						else{
							userAccumulatedTime = ofGetElapsedTimeMillis() - lastVideoSelectedTime;
						}
						
					}else if (pcx < xcenter) {
						vidToPlay = 0;
						selectedVid = 0;
						if (vidToPlay != oldVidToPlay){
							lastVideoStartTime = ofGetElapsedTimeMillis();
							lastVideoSelectedTime = ofGetElapsedTimeMillis();
						}
						else{
							userAccumulatedTime = ofGetElapsedTimeMillis() - lastVideoSelectedTime;
						}
					}
					
					
				}
				else if (pcy > ycenter){
					if (pcx > xcenter){
						vidToPlay = 3;
						selectedVid = 3;
						if (vidToPlay != oldVidToPlay){
							lastVideoStartTime = ofGetElapsedTimeMillis();
							lastVideoSelectedTime = ofGetElapsedTimeMillis();
						}
						else{
							userAccumulatedTime = ofGetElapsedTimeMillis() - lastVideoSelectedTime;
						}
					}
					else if (pcx < xcenter) {
						vidToPlay = 2;
						selectedVid = 2;
						if (vidToPlay != oldVidToPlay){
							lastVideoStartTime = ofGetElapsedTimeMillis();
							lastVideoSelectedTime = ofGetElapsedTimeMillis();
						}
						else{
							userAccumulatedTime = ofGetElapsedTimeMillis() - lastVideoSelectedTime;
						}
					}
				}
				if (oldVidToPlay != -1){
					videoPlayers[oldVidToPlay].stop();
				}
				if (vidToPlay != -1){
					videoPlayers[vidToPlay].play();
				}
				vidStartTime = ofGetElapsedTimeMillis();
			}
		}
	}
    ofPoint result(pcx, pcy, pcz);
    return result;
}


//--------------------
void testApp::drawCropped(int croppedwidth, int croppedheight){
	//*recordImagePixels = recordImage.getPixels();
    ofImage croppedImg, originalImg;
	originalImg.setFromPixels(recordImagePixels, 640, 480, OF_IMAGE_COLOR);
	int x = (640 - croppedwidth)/2;
	int y = (480 - croppedheight)/2;
	
	unsigned char tmpimg[ croppedwidth * croppedheight * 3 ];  
	
	for (int i = 0; i < croppedwidth; i++){  
		for (int j = 0; j < croppedheight; j++){  
			
			int mainPixelPos = ((j + y) * 640 + (i + x)) * 3;  
			int subPixlPos = (j * (croppedwidth) + i) * 3;  
			
			tmpimg[subPixlPos] = recordImagePixels[mainPixelPos];   // R  
			tmpimg[subPixlPos + 1] = recordImagePixels[mainPixelPos + 1];  // G  
			tmpimg[subPixlPos + 2] = recordImagePixels[mainPixelPos + 2];  // B  
			
			
		}
	}
	
	
	
	croppedImg.setFromPixels(tmpimg, croppedwidth, croppedheight, OF_IMAGE_COLOR);                
	croppedImg.draw(0,0);
	
	
}



//--------------------------------------------------------------

void testApp:: drawMasks() {
    
	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	allUserMasks.draw(640, 0, 640, 480);
	glDisable(GL_BLEND);
    glPopMatrix();
	user1Mask.draw(320, 480, 320, 240);
	user2Mask.draw(640, 480, 320, 240);
	
}
//--------------------------------------------------------------

void testApp::drawPointCloud(ofxUserGenerator * user_generator, int userID) {
    
	glPushMatrix();
    
	int w = user_generator->getWidth();
	int h = user_generator->getHeight();
    
	glTranslatef(w, h/2, -500);
	ofRotateY(pointCloudRotationY);
    
	glBegin(GL_POINTS);
    
	int step = 1;
    
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			ofPoint pos = user_generator->getWorldCoordinateAt(x, y, userID);
			if (pos.z == 0 && isCPBkgnd) continue;	// gets rid of background -> still a bit weird if userID > 0...
			//yoni - test//
			//ofColor color = user_generator->getWorldColorAt(x,y, userID);
			//glColor4ub((unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a);
			glVertex3f(pos.x, pos.y, pos.z);
		}
	}
    
	glEnd();
    
	glColor3f(1.0f, 1.0f, 1.0f);
    
	glPopMatrix();
}
//--------------------------------------------------------------

ofPoint testApp::centermass(ofxUserGenerator * user_generator, int userID) {
    
    
    
    int w = user_generator->getWidth();
	int h = user_generator->getHeight();
	//cout << ofToString(h) << " h " << ofToString(w) << " w " << ofToString(userID) << " userID "  << endl;
    float sumx = 0;
    float sumy = 0;
    float sumz = 0;
    float tot = 1;
    int step = 1;
    
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			ofPoint pos = user_generator->getWorldCoordinateAt(x, y, userID);
			if (pos.z == 0 && isCPBkgnd) continue;	// gets rid of background -> still a bit weird if userID > 0...
            sumx += pos.x;
            sumy += pos.y;
            sumz += pos.z;
            tot +=1;
		}
	}
    sumx = sumx/tot;
    sumy = sumy/tot;
    sumz = sumz/tot;    
    
	
    ofPoint result(sumx, sumy, sumz);
    
    return result;
}



//--------------------------------------------------------------
ofPoint testApp::getCenterHead(ofxUserGenerator * user_generator) {
	
	
    int w = user_generator->getWidth();
    int h = user_generator->getHeight();
    int sumx = 0;
    int topy = 0;
    int sumz = 0;
	int tot = 1;
	int step = 1;
	int userID = 0;
	
	while(sumz == 0 && userID < 9){
		sumx = 0;
		topy = 0;
		sumz = 0;
		tot = 1;		
		for(int y = 0; y < h-step; y += step) {
			for(int x = 0; x < w-step; x += step) {
				ofPoint pos = user_generator->getWorldCoordinateAt(x, y, userID);
				if (pos.z == 0 && isCPBkgnd) continue;	// gets rid of background -> still a bit weird if userID > 0...
				if (pos.z > 0) {
					//sumx += pos.x;
					sumz += pos.z;
					topy = pos.y;
					tot +=1;
				}
			}
		}
		userID++;
	}
	
	
    //sumx = sumx/tot;
    sumz = sumz/tot;
	sumx = w/2;
	
	topy = (int)(0.95*topy);
    ofPoint result(sumx, topy, sumz);    
    return result;
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
	float smooth;
    
	switch (key) {
#ifdef TARGET_OSX // only working on Mac at the moment
		case 357: // up key
			hardware.setTiltAngle(hardware.tilt_angle++);
			break;
		case 359: // down key
			hardware.setTiltAngle(hardware.tilt_angle--);
			break;
#endif
		case 's':
		case 'S':
			if (isRecording) {
				oniRecorder.stopRecord();
				isRecording = false;
				break;
			} else {
				oniRecorder.startRecord(generateFileName());
				isRecording = true;
				break;
			}
			break;
		case 'p':
		case 'P':
			if (oniRecorder.getCurrentFileName() != "" && !isRecording && isLive) {
				setupPlayback(oniRecorder.getCurrentFileName());
				isLive = false;
			} else {
				isLive = true;
			}
			break;
		case 't':
		case 'T':
			isTracking = !isTracking;
			break;
		case 'h':
		case 'H':
			isTrackingHands = !isTrackingHands;
			if(isLive) recordHandTracker.toggleTrackHands();
			if(!isLive) playHandTracker.toggleTrackHands();
			break;
		case 'f':
		case 'F':
			isFiltering = !isFiltering;
			recordHandTracker.isFiltering = isFiltering;
			playHandTracker.isFiltering = isFiltering;
			break;
		case 'm':
		case 'M':
			isMasking = !isMasking;
			recordUser.setUseMaskPixels(isMasking);
			playUser.setUseMaskPixels(isMasking);
			break;
		case 'c':
		case 'C':
			isCloud = !isCloud;
			recordUser.setUseCloudPoints(isCloud);
			playUser.setUseCloudPoints(isCloud);
			break;
		case 'b':
		case 'B':
			isCPBkgnd = !isCPBkgnd;
			break;
		case '9':
		case '(':
			smooth = recordUser.getSmoothing();
			if (smooth - 0.1f > 0.0f) {
				recordUser.setSmoothing(smooth - 0.1f);
				playUser.setSmoothing(smooth - 0.1f);
			}
			break;
		case '0':
		case ')':
			smooth = recordUser.getSmoothing();
			if (smooth + 0.1f <= 1.0f) {
				recordUser.setSmoothing(smooth + 0.1f);
				playUser.setSmoothing(smooth + 0.1f);
			}
			break;
		case '[':
            //case '{':
			if (filterFactor - 0.1f > 0.0f) {
				filterFactor = filterFactor - 0.1f;
				recordHandTracker.setFilterFactors(filterFactor);
				if (oniRecorder.getCurrentFileName() != "") playHandTracker.setFilterFactors(filterFactor);
			}
			break;
		case ']':
            //case '}':
			if (filterFactor + 0.1f <= 1.0f) {
				filterFactor = filterFactor + 0.1f;
				recordHandTracker.setFilterFactors(filterFactor);
				if (oniRecorder.getCurrentFileName() != "") playHandTracker.setFilterFactors(filterFactor);
			}
			break;
		case ';':
		case ':':
			smooth = recordHandTracker.getSmoothing();
			if (smooth - 0.1f > 0.0f) {
				recordHandTracker.setSmoothing(smooth -  0.1f);
				playHandTracker.setSmoothing(smooth -  0.1f);
			}
			break;
		case '\'':
		case '\"':
            
			smooth = recordHandTracker.getSmoothing();
			if (smooth + 0.1f <= 1.0f) {
				recordHandTracker.setSmoothing(smooth +  0.1f);
				playHandTracker.setSmoothing(smooth +  0.1f);
			}
			break;
		case '>':
		case '.':
			farThreshold += 50;
			if (farThreshold > recordDepth.getMaxDepth()) farThreshold = recordDepth.getMaxDepth();
			break;
		case '<':
		case ',':
			farThreshold -= 50;
			if (farThreshold < 0) farThreshold = 0;
			break;
            
		case '+':
		case '=':
			nearThreshold += 50;
			if (nearThreshold > recordDepth.getMaxDepth()) nearThreshold = recordDepth.getMaxDepth();
			break;
            
		case '-':
		case '_':
			nearThreshold -= 50;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
		case 'R':
			recordContext.toggleRegisterViewport();
			break;
            
        case '2':
            //recorder.finishMovie(); 
            break;
            
        case '1':
            //recorder.startNewRecording(); 
            break; 
			
		case 'r':
			//tracker.reset();
			break;
            
        case ' ':
            screen++;
			screen %= 3;
            break;
            
        case '/':
            selected++;
			selected %= 4;
            break; 
			
        default:
            break;
	}
}

//yoni - notice//
string testApp::generateFileName() {
    
	string _root = "movie/";
    
	string _timestamp = ofToString(ofGetDay()) +
	ofToString(ofGetMonth()) +
	ofToString(ofGetYear()) +
	ofToString(ofGetHours()) +
	ofToString(ofGetMinutes()) +
	ofToString(ofGetSeconds());
    
	string _filename = (_root + _timestamp + ".mov");
    
	return _filename;
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
	if (isCloud) pointCloudRotationY = x;
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------


void testApp::relistFiles(){
	//yoni - change paths//
    //remove videos to save in data
	//or change at addon ofQTkitAvCreenRecorder.mm
	string videoPath = ofToDataPath("videos");
    
    ofDirectory videoDir(videoPath);
    
    //take movs
    videoDir.allowExt("mov");
    
    //populate the directory object
    videoDir.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < videoDir.numFiles(); i++){
        videoFiles.push_back(videoDir.getPath(i));
    }
}

//--------------------------------------------------------------

void testApp::exit(){ 
    recorder.exit(); 
}


