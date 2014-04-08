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

	nearThreshold = 500;
	farThreshold  = 1000;

	filterFactor = 0.1f;

	setupRecording();

	ofBackground(0, 0, 0);

}

void testApp::setupRecording(string _filename) {

	openNIRecorder.setup(true);
    openNIRecorder.addDepthGenerator();
    openNIRecorder.addImageGenerator();
    
	openNIRecorder.addUserGenerator();
	openNIRecorder.setUserSmoothing(filterFactor);				// built in openni skeleton smoothing...
	openNIRecorder.setUseMaskPixelsAllUsers(isMasking);
	openNIRecorder.setUsePointCloudsAllUsers(isCloud);
	openNIRecorder.setMaxNumUsers(1);					// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)
    

	openNIRecorder.addHandsGenerator();
	openNIRecorder.addAllHandFocusGestures();    
	openNIRecorder.setHandSmoothing(filterFactor);
    
    openNIRecorder.setMaxNumHands(2);
	
	openNIRecorder.setRegister(true);
    openNIRecorder.setMirror(true);

	openNIRecorder.start(); //
}

void testApp::setupPlayback(string _filename) {
	openNIPlayer.stop();
	openNIPlayer.startPlayer(ofToDataPath(_filename));
}

//--------------------------------------------------------------
void testApp::update(){


#ifdef TARGET_OSX // only working on Mac at the moment
	hardware.update();
#endif

	if (isLive) {

		// update all nodes

		ofxProfileSectionPush("openni update");
		openNIRecorder.update();
		ofxProfileSectionPop();

        // demo getting depth pixels directly from depth gen
		
		
		// TODO: addDepthThreshold(int _nearThreshold,

		//XXX depthRangeMask.setFromPixels( openNIRecorder.getDepthPixels(nearThreshold, farThreshold), recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);

		// update tracking/recording nodes
		//XXX recordUser.update();
		//XXX if (isRecording) oniRecorder.update();

	} else {

		// update all nodes
		openNIPlayer.update();

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
	ofDrawBitmapString( lastDump, ofPoint( 640, 500 ) );
	//ofDrawBitmapString( " 'c' to clear profile data", ofPoint( 10, ofGetHeight()-20 ) );
	

	ofSetColor(255, 255, 255);

	glPushMatrix();
	glScalef(0.75, 0.75, 0.75);

	if (isLive) {


		ofxProfileSectionPush("drawDepth");
		openNIRecorder.drawDepth(0,0,640,480);
		ofxProfileSectionPop();

		ofxProfileSectionPush("drawImage");
		openNIRecorder.drawImage(640,0,640,480);
        ofxProfileSectionPop();


		if (isTracking) {
			openNIRecorder.drawSkeletons();
		}

		if (isTracking) {
				openNIRecorder.drawHands();
		}



        if (0 && openNIRecorder.getNumTrackedUsers() > 0){
            
			XnPoint3D com;
			openNIRecorder.getUserGenerator().GetCoM(1, com);
            //ofPoint center = centermass(&recordUser, 1);
			ofPoint center(com.X, com.Y, com.Z);

            
            ofCircle(center.x, center.y,  10);

            /*
			 ofxOpenNIHand & handOne = openNIRecorder.getTrackedHand(1);
            
                ofPoint handPos = handOne.getPosition();
               
            
                ofSetColor(255, 0, 0);
                ofLine(handPos, center);
                string distHand	= ofToString(sqrt(pow(center.x-handPos.x,2) + pow(center.y-handPos.y,2)));
            
                cout << handPos.z - center.z << " DISTANCE TO CENTER" << endl;
            
                ofDrawBitmapString(distHand, handPos.x, handPos.y);
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

		openNIPlayer.drawDepth(0,0,640,480);
		openNIPlayer.drawImage(640, 0, 640, 480);

		depthRangeMask.draw(0, 480, 320, 240);	// can use this with openCV to make masks, find contours etc when not dealing with openNI 'User' like objects

		if (isTracking) {
			openNIPlayer.drawSkeletons();

//			if (isMasking) drawMasks();
			//XXX if (isCloud) drawPointCloud(&playUser, 0);	// 0 gives you all point clouds; use userID to see point clouds for specific users

		}
		if (isTrackingHands)
			openNIPlayer.drawHands();
	}

	glPopMatrix();

	ofSetColor(255, 255, 0);

	string statusPlay		= (string)(isLive ? "LIVE STREAM" : "PLAY STREAM");
	string statusRec		= (string)(!isRecording ? "READY" : "RECORDING");
	string statusSkeleton	= (string)(isTracking ? "TRACKING USERS: " + (string)(isLive ? ofToString(openNIRecorder.getNumTrackedUsers()) : ofToString(openNIPlayer.getNumTrackedUsers())) + "" : "NOT TRACKING USERS");
	string statusSmoothSkel = (string)(isLive ? ofToString(openNIRecorder.getUserSmoothing()) : ofToString(openNIPlayer.getUserSmoothing()));
	string statusHands		= (string)(isTrackingHands ? "TRACKING HANDS: " + (string)(isLive ? ofToString(openNIRecorder.getNumTrackedHands()) : ofToString(openNIPlayer.getNumTrackedHands())) + ""  : "NOT TRACKING");
	string statusFilter		= (string)(isFiltering ? "FILTERING" : "NOT FILTERING");
	string statusFilterLvl	= ofToString(filterFactor);
	string statusSmoothHand = (string)(isLive ? ofToString(openNIRecorder.getHandsSmoothing()) : ofToString(openNIPlayer.getHandsSmoothing()));
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
		<< "F: Fullscreen" << endl
	<< "    s : start/stop recording  : " << statusRec << endl
	<< "    p : playback/live streams : " << statusPlay << endl
	<< "    t : skeleton tracking     : " << statusSkeleton << endl
	<< "( / ) : smooth skely (openni) : " << statusSmoothSkel << endl
	<< "    h : hand tracking         : " << statusHands << endl
	<< "    f : filter hands (custom) : " << statusFilter << endl
	<< "[ / ] : filter hands factor   : " << statusFilterLvl << endl
	
	<< "    m : drawing masks         : " << statusMask << endl
	<< "    c : draw cloud points     : " << statusCloud << endl
	<< "    b : cloud user data       : " << statusCloudData << endl
	<< "- / + : nearThreshold         : " << ofToString(nearThreshold) << endl
	<< "< / > : farThreshold          : " << ofToString(farThreshold) << endl
	<< endl
	//XXX << "File  : " << openNIRecorder.getDevice(). g_Recorder.getCurrentFileName() << endl
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

/* XXX

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
*/


/* XXX

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

*/

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

		
		
		case'c':
			ofxProfile::clear();
			lastDump = "";
		break;
	


		case 's':
		case 'S':
			if (isRecording) {
				openNIRecorder.stopRecording();
				isRecording = false;
				break;
			} else {
				openNIRecorder.startRecording(generateFileName());
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
				if(isLive) openNIRecorder.getHandsGenerator().StopTrackingAll();
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


		case 'm':
		case 'M':
			isMasking = !isMasking;
//XXX		recordUser.setUseMaskPixels(isMasking);
//XXX		playUser.setUseMaskPixels(isMasking);
			break;
		//case 'c':
		case 'C':
			isCloud = !isCloud;
//XXX			recordUser.setUsePointCloud(isCloud);
//XXX			playUser.setUsePointCloud(isCloud);
			break;
		case 'b':
		case 'B':
			isCPBkgnd = !isCPBkgnd;
			break;
		case '9':
		case '(':
			smooth = openNIRecorder.getUserSmoothing();
			if (smooth - 0.1f > 0.0f) {
				openNIRecorder.setUserSmoothing(smooth - 0.1f);
			}
			break;
		case '0':
		case ')':
		smooth = openNIRecorder.getUserSmoothing();	
			if (smooth + 0.1f <= 1.0f) {
				openNIRecorder.setUserSmoothing(smooth + 0.1f);
			}
			break;

		case '>':
		case '.':
			farThreshold += 50;
			//XXX if (farThreshold > openNIRecorder.maxDepth()) farThreshold = recordDepth.getMaxDepth();
			break;
		case '<':
		case ',':
			farThreshold -= 50;
			if (farThreshold < 0) farThreshold = 0;
			break;

		case '+':
		case '=':
			nearThreshold += 50;
			//XXX if (nearThreshold > recordDepth.getMaxDepth()) nearThreshold = recordDepth.getMaxDepth();
			break;

		case '-':
		case '_':
			nearThreshold -= 50;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
		case 'r':
			//XXX recordContext.toggleRegisterViewport();
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
	openNIRecorder.stop(); 
}


