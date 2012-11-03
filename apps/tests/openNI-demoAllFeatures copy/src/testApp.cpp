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

	setupRecording();

	ofBackground(0, 0, 0);

    tracker.setup();
    
    recorder.setup();
    fboSaver.allocate(320, 240, GL_RGB);
    fboPixels.allocate(320, 240, OF_IMAGE_COLOR);
//    ofEnableAlphaBlending();
//    alphaPNG.loadImage("1.png"); //test image
    fboSaver.begin();
    ofClear(255,255,255,0);
    fboSaver.end();

    
    
    //    cam.listDevices();
    //    
    //    cam.setDeviceID(3);
    // 	cam.initGrabber(640, 480);


}

void testApp::setupRecording(string _filename) {

#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	hardware.setup();				// libusb direct control of motor, LED and accelerometers
	hardware.setLedOption(LED_OFF); // turn off the led just for yacks (or for live installation/performances ;-)
#endif

	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	//recordContext.setupUsingXMLFile();
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);

	recordUser.setup(&recordContext);
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

	oniRecorder.setup(&recordContext, ONI_STREAMING);
	//oniRecorder.setup(&recordContext, ONI_CYCLIC, 60); // KINECT RECORDER
	//read the warning in ofxOpenNIRecorder about memory usage with ONI_CYCLIC recording!!!

}

void testApp::setupPlayback(string _filename) {

	playContext.shutdown();
	playContext.setupUsingRecording(ofToDataPath(_filename));
	playDepth.setup(&playContext);
	playImage.setup(&playContext);

	playUser.setup(&playContext);
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

	if (isLive) {

		// update all nodes
		recordContext.update();
		recordDepth.update();
		recordImage.update();
        
   //FACETRACKER:      
        unsigned char *pixels = recordImage.getPixels();
    
        //   Mat imgMat = Mat(640, 480, CV_8UC1, pixels, 0);  
        img.setFromPixels(pixels, 640, 480, OF_IMAGE_COLOR);
        //    pix.set(*pixels);
        Mat imgMat = toCv(img);
        
        tracker.update(imgMat);

  
        
        

//CLEAR FBO:
        
        fboSaver.begin();
        ofClear(0,0,0,0);
        fboSaver.end();
        
// RECORDING:::                
        if(recorder.getIsRecording()) {

        
            if (tracker.getFound()){
//            tracker.draw();

//TRACKING FACE:
    

            fullFace = tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
            faceBox = fullFace.getBoundingBox();
                int      x = faceBox.x-25;
                int     y = faceBox.y-50;
                int    rw = faceBox.width+50;
                int   rh = faceBox.height+50;
                ofImage faceImg;

                         
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

                
                fboSaver.begin();
                faceImg.setFromPixels(face, rw, rh, OF_IMAGE_COLOR);                
                faceImg.draw(0,0);
                fboSaver.end();
                
                fboSaver.readToPixels(fboPixels);
                recorder.addFrame(fboPixels);
                
            }
            
            

            
                                
        }
        
        
        
        //       TRACKER
        if(tracker.getFound()) {
//            
//            ofMesh mesh = tracker.getObjectMesh();
//            ofVec3f center =  mesh.getCentroid();
//            
//            ofNoFill();
            
            
            
            
            
            //EYE            
            //        leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
            //        ofRectangle eyeBox = leftEye.getBoundingBox();
            //        ofRect(eyeBox.x, eyeBox.y, eyeBox.width, eyeBox.height);
            
            
            //            tracker.draw();
        }
        

        
        recorder.update(); 
//////////         
        

		
        // demo getting depth pixels directly from depth gen
		depthRangeMask.setFromPixels(recordDepth.getDepthPixels(nearThreshold, farThreshold),
									 recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);

		// update tracking/recording nodes
		recordUser.update();
		if (isRecording) oniRecorder.update();

		// demo getting pixels from user gen
		if (isTracking && isMasking) {
			allUserMasks.setFromPixels(recordUser.getUserPixels(), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user1Mask.setFromPixels(recordUser.getUserPixels(1), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user2Mask.setFromPixels(recordUser.getUserPixels(2), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
		}
        

        
        
        
        
        


	} else {

		// update all nodes
		playContext.update();
		playDepth.update();
		playImage.update();

		// demo getting depth pixels directly from depth gen
		depthRangeMask.setFromPixels(playDepth.getDepthPixels(nearThreshold, farThreshold),
									 playDepth.getWidth(), playDepth.getHeight(), OF_IMAGE_GRAYSCALE);

		// update tracking/recording nodes
		if (isTracking) playUser.update();

		// demo getting pixels from user gen
		if (isTracking && isMasking) {
			allUserMasks.setFromPixels(playUser.getUserPixels(), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user1Mask.setFromPixels(playUser.getUserPixels(1), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user2Mask.setFromPixels(playUser.getUserPixels(2), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(255, 255, 255);

	glPushMatrix();
	glScalef(0.75, 0.75, 0.75);

	if (isLive) {

//		recordDepth.draw(0,0,640,480);
		recordImage.draw(0, 0, 640, 480);
        
  /*      
        //       TRACKER
        if(tracker.getFound()) {
            
            ofMesh mesh = tracker.getObjectMesh();
            ofVec3f center =  mesh.getCentroid();
            
            ofNoFill();
            ofRect(faceBox.x-25, faceBox.y-50, faceBox.width+50, faceBox.height+50);
            
            ofImage faceImg;        
            faceImg.allocate(faceBox.width+50, faceBox.height+50, OF_IMAGE_COLOR);
            
            
            
            //        tracker.draw();
            ofPushMatrix();
            ofTranslate(640, 480);
            //    ofSetColor(255,0,0);
            
            ofTranslate(100, 100);
            //   ofSetColor(0,255,0);
            
            ofPopMatrix();
            
            
            //EYE            
            //        leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
            //        ofRectangle eyeBox = leftEye.getBoundingBox();
            //        ofRect(eyeBox.x, eyeBox.y, eyeBox.width, eyeBox.height);
            
            
            //            tracker.draw();
        }
        
*/
        

        if (recordUser.getNumberOfTrackedUsers() > 0){
            recordUser.startTracking(1);
            
            recordUser.setUseCloudPoints(true);

            
            ofPoint center = centermass(&recordUser, 1);
            
            ofCircle(center.x, center.y,  10);

			
            
            
                ofxTrackedHand* handOne =  recordHandTracker.getHand(1);
            
                ofPoint handPos = handOne->projectPos;
               
            
                ofSetColor(255, 0, 0);
                ofLine(handPos, center);
                string distHand	= ofToString(sqrt(pow(center.x-handPos.x,2) + pow(center.y-handPos.y,2)));
            
                cout << handPos.z - center.z << " DISTANCE TO CENTER" << endl;
            
                ofDrawBitmapString(distHand, handPos.x, handPos.y);

                recordHandTracker.drawHands();

/* THIS IS FOR SKELETON CALIBRATION:            
            
//            if (!user.skeletonCalibrated){
//            recordUser.requestCalibration(1);
//                if (user.skeletonCalibrating){
//            recordUser.startPoseDetection(1);
//            
//                }
//            }
//
//            leftHand = user.left_lower_arm;
//            rightHand = user.right_lower_arm;
//            neck = user.neck;
//            
//            ofSetColor(255, 255, 0);
//            ofCircle(rightHand.position[1].X,rightHand.position[1].Y, 30);
            
*/           


            

        }

        
   //		depthRangeMask.draw(0, 480, 320, 240);	// can use this with openCV to make masks, find contours etc when not dealing with openNI 'User' like objects

            
        /* DRAWING PIXELS OF USER: 
        
		if (isTracking) {
			recordUser.draw();

			if (isMasking) drawMasks();
			if (isCloud) drawPointCloud(&recordUser, 1);	// 0 gives you all point clouds; use userID to see point clouds for specific users

		}
    */


        


	} else {

		playDepth.draw(0,0,640,480);
		playImage.draw(640, 0, 640, 480);

		depthRangeMask.draw(0, 480, 320, 240);	// can use this with openCV to make masks, find contours etc when not dealing with openNI 'User' like objects

		if (isTracking) {
			playUser.draw();

//			if (isMasking) drawMasks();
			if (isCloud) drawPointCloud(&playUser, 0);	// 0 gives you all point clouds; use userID to see point clouds for specific users

		}
		if (isTrackingHands)
			playHandTracker.drawHands();
	}

	glPopMatrix();

	ofSetColor(255, 255, 0);

	string statusPlay		= (string)(isLive ? "LIVE STREAM" : "PLAY STREAM");
	string statusRec		= (string)(!isRecording ? "READY" : "RECORDING");
	string statusSkeleton	= (string)(isTracking ? "TRACKING USERS: " + (string)(isLive ? ofToString(recordUser.getNumberOfTrackedUsers()) : ofToString(playUser.getNumberOfTrackedUsers())) + "" : "NOT TRACKING USERS");
	string statusSmoothSkel = (string)(isLive ? ofToString(recordUser.getSmoothing()) : ofToString(playUser.getSmoothing()));
	string statusHands		= (string)(isTrackingHands ? "TRACKING HANDS: " + (string)(isLive ? ofToString(recordHandTracker.getNumTrackedHands()) : ofToString(playHandTracker.getNumTrackedHands())) + ""  : "NOT TRACKING");
	string statusFilter		= (string)(isFiltering ? "FILTERING" : "NOT FILTERING");
	string statusFilterLvl	= ofToString(filterFactor);
	string statusSmoothHand = (string)(isLive ? ofToString(recordHandTracker.getSmoothing()) : ofToString(playHandTracker.getSmoothing()));
	string statusMask		= (string)(!isMasking ? "HIDE" : (isTracking ? "SHOW" : "YOU NEED TO TURN ON TRACKING!!"));
	string statusCloud		= (string)(isCloud ? "ON" : "OFF");
	string statusCloudData	= (string)(isCPBkgnd ? "SHOW BACKGROUND" : (isTracking ? "SHOW USER" : "YOU NEED TO TURN ON TRACKING!!"));

	string statusHardware;

#ifdef TARGET_OSX // only working on Mac at the moment
	ofPoint statusAccelerometers = hardware.getAccelerometers();
	stringstream	statusHardwareStream;

	statusHardwareStream
	<< "ACCELEROMETERS:"
	<< " TILT: " << hardware.getTiltAngle() << "/" << hardware.tilt_angle
	<< " x - " << statusAccelerometers.x
	<< " y - " << statusAccelerometers.y
	<< " z - " << statusAccelerometers.z;

	statusHardware = statusHardwareStream.str();
#endif

	stringstream msg;

	msg
	<< "    s : start/stop recording  : " << statusRec << endl
	<< "    p : playback/live streams : " << statusPlay << endl
	<< "    t : skeleton tracking     : " << statusSkeleton << endl
	<< "( / ) : smooth skely (openni) : " << statusSmoothSkel << endl
	<< "    h : hand tracking         : " << statusHands << endl
	<< "    f : filter hands (custom) : " << statusFilter << endl
	<< "[ / ] : filter hands factor   : " << statusFilterLvl << endl
	<< "; / ' : smooth hands (openni) : " << statusSmoothHand << endl
	<< "    m : drawing masks         : " << statusMask << endl
	<< "    c : draw cloud points     : " << statusCloud << endl
	<< "    b : cloud user data       : " << statusCloudData << endl
	<< "- / + : nearThreshold         : " << ofToString(nearThreshold) << endl
	<< "< / > : farThreshold          : " << ofToString(farThreshold) << endl
	<< endl
	<< "File  : " << oniRecorder.getCurrentFileName() << endl
	<< "FPS   : " << ofToString(ofGetFrameRate()) << "  " << statusHardware << endl;

	ofDrawBitmapString(msg.str(), 20, 560);

}

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
			ofColor color = user_generator->getWorldColorAt(x,y, userID);
			glColor4ub((unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a);
			glVertex3f(pos.x, pos.y, pos.z);
		}
	}

	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
}

ofPoint testApp::centermass(ofxUserGenerator * user_generator, int userID) {
 


    int w = user_generator->getWidth();
	int h = user_generator->getHeight();
    int sumx = 0;
    int sumy = 0;
    int sumz = 0;
    int tot = 1;
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

    //    
//    int sumxs = 0;
//    int sumys = 0;
//    int sumzs = 0;
//    //sumz = sumz/tot;
//    
//    for(int y = 0; y < h; y += step) {
//		for(int x = 0; x < w; x += step) {
//			ofPoint pos = user_generator->getWorldCoordinateAt(x, y, userID);
//			if (pos.z == 0 && isCPBkgnd) continue;	// gets rid of background -> still a bit weird if userID > 0...
//            sumxs += pow(pos.x-sumx,2);
//            sumys += pow(pos.y-sumy,2);
//            //sumzs += pow(pos.z-sumz,2);
//        }
//	}
//    
//    int sumxn = 0;
//    int sumyn = 0;
//    int sumzn = 0;
//    int totn = 1;
//    
//    sumxs = sqrt(sumxs/tot)/2;
//    sumys = sqrt(sumys/tot)/2;
//    //sumzs = sqrt(sumzs/tot);
//    
//    for(int y = 0; y < h; y += step) {
//		for(int x = 0; x < w; x += step) {
//			ofPoint pos = user_generator->getWorldCoordinateAt(x, y, userID);
//			if (pos.z > 0 && (pow(pos.x-sumx,2)) < sumxs && (pow(pos.y-sumy,2)) < sumys) {            
//                sumxn += pos.x;
//                sumyn += pos.y;
//                //sumzn += pow(pos.z-sumz,2);
//                totn +=1;
//            }
//		}
//	}
//    
//    
// 
//    cout << sumx << " " << sumy << " " << sumz << " " << tot << " SUMX SUMY SUMZ TOT" << endl;


//    ofRect(sumxn/totn, sumyn/totn, 10, 10);
    ofPoint result(sumx, sumy, sumz);

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
		case 'r':
			recordContext.toggleRegisterViewport();
			break;
            
        case '2':
            recorder.finishMovie(); 
            break;
            
        case '1':
            recorder.startNewRecording(); 
            break; 
            
        default:
            break;
	}
}

string testApp::generateFileName() {

	string _root = "kinectRecord";

	string _timestamp = ofToString(ofGetDay()) +
	ofToString(ofGetMonth()) +
	ofToString(ofGetYear()) +
	ofToString(ofGetHours()) +
	ofToString(ofGetMinutes()) +
	ofToString(ofGetSeconds());

	string _filename = (_root + _timestamp + ".oni");

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

void testApp::exit(){ 
    recorder.exit(); 
}


