#ifndef _TEST_APP
#define _TEST_APP

//#define USE_IR // Uncomment this to use infra red instead of RGB cam...

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxQTKitAVScreenRecorder.h" 
using namespace ofxCv;
using namespace cv;


class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void exit();
    
    void updateState1();
	void chooseVideos();
	void chooseMiniVideos();
	void drawCropped(int croppedwidth, int croppedheight);
	ofPoint getPointerCoordinates();
	bool userInFrame(int croppedwidth, int croppedheight, int userID);
	bool userInFrame2(int croppedwidth, int croppedheight);
    
	void	setupRecording(string _filename = "");
	void	setupPlayback(string _filename);
	string	generateFileName();

	bool				isLive, isTracking, isRecording, isCloud, isCPBkgnd, isMasking;
	bool				isTrackingHands, isFiltering;

	ofxOpenNIContext	recordContext, playContext;
	ofxDepthGenerator	recordDepth, playDepth;

#ifdef USE_IR
	ofxIRGenerator		recordImage, playImage;
#else
	ofxImageGenerator	recordImage, playImage;
#endif

	ofxHandGenerator	recordHandTracker, playHandTracker;

	ofxUserGenerator	recordUser, playUser;
	ofxOpenNIRecorder	oniRecorder;
    

        

    

#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	ofxHardwareDriver	hardware;
#endif


	void				drawMasks();
	void				drawPointCloud(ofxUserGenerator * user_generator, int userID);
    ofPoint             centermass(ofxUserGenerator * user_generator, int userID);
	ofPoint				getCenterHead(ofxUserGenerator * user_generator); 
	int					nearThreshold, farThreshold;
	int					pointCloudRotationY;

	ofImage				allUserMasks, user1Mask, user2Mask, depthRangeMask;

	float				filterFactor;

    
    
    
    ofImage img;
    
///
    
    
    ofxFaceTracker tracker, tmpTracker;
	ofVec2f position;
	float scale;
	ofVec3f orientation;
	ofMatrix4x4 rotationMatrix;
	
	Mat translation, rotation;
	ofMatrix4x4 pose;
    
    ofPolyline leftEye;
        ofPolyline fullFace;

    ofRectangle faceBox;
// CAMERA:
//    ofVideoGrabber cam;

    
//RECORDING VIDEO:
    
    ofFbo fboSaver;
    ofPixels fboPixels;
    ofxQTKitAVScreenRecorder recorder; 

    //WEBCAM
    ofxQTKitVideoGrabber grabber;

    
    
    int wdth, hght;
    
//STATE MACHINE:
    
    bool kinectState;
    
// TIME TRACKERS:
    
    bool faceTrackStarted;
    
    float userTrackStart, recordStart;
    
    float userTrackThresh, recordThresh, trackWait, trackWaitThresh;
    
    
    int vidToPlay;
    

 // FACE"
    
    ofImage faceImg;
    bool recordingFace;
    
// VIDEO PLAYING:
    
    vector <ofVideoPlayer> videoPlayers, miniVideoPlayers;
	vector <string> videoNames, miniVideoNames;
	int miniVid;
	bool isTrackingFace;

    
// FILES:
    void				relistFiles();
    vector <string> videoFiles; 
    
    float vidPlayTime;
    float vidStartTime;
    
    string faceState; 
    
    
    
// KINECT TRACKER:
    
    ofxLimb             leftHand, rightHand, neck, leftshoulder, rightshoulder;
    
    bool skelReady;
    
    
    float zDist;
    ofxTrackedUser*     user;
	
	
// interaction states:
	// 0 - standby: ready for user (move to 1); display welcome slide; min duration 5 sec
	// 1 - identified: user identified (move to 2 or 3); record user video; select 4 videos for display; interaction; consider - notify idelness
	// 2 - completed: user made selection (move to 4); display thank you slide; duration 3 sec
	// 3 - lost: user tracking lost (move to 4); display session distrupted slide; duration 3 sec
	// 4 - end: session ended (move to 0); make sure user is clear, videos closed, etc.
	int interactionState;
	float stateStartTime;
	
	float lastUserTime;
	float lastVideoStartTime;
	float lastVideoSelectedTime;
	float userAccumulatedTime;
	float state0Time;
	
	int selectedVid;
	int tmpMaxHeight;
	bool intState1;
	int yAxisDiff;
	int selectionThreshold;
	ofPoint centerHead;
	unsigned char *recordImagePixels;
	ofPoint pointerCoords;
	bool sessionRecorded;
	
	int tilted, tiltedWidth, tiltedHeight;
    
    // Capture size
    int vidWidth, vidHeight;
    
    int topBanner;
	int bottomBanner;
    
    // Setup are we recording now?
    bool recording;// = true;
    
    // setup font
    ofTrueTypeFont courierNew, courierSml;
	int firstline; //first line to draw on
    int lineheight; // height & spacing of the text lines
    
	
    
    // setup time buffer for coursor
    int lastCursorBlink;
    
    // setup images
    //ofImage rec, normal_h, normal_a, pointer; //allocate space for variable
	ofImage rec, prompt_scr0_h, prompt_scr0_a, prompt_instructions, normal_h, normal_a, pointer, c_empty, v_empty, v_full, x_empty, x_full, placeholder, vidPlaceholder, thanks_h, thanks_a; //allocate space for variable
    
    // define whether we are pointing now or not:
    bool pointing;
	
    // setup pointer rotation
    int pointerRot;// = 0;
    
    // setup pointer countdown
    int pointerCount;// = 0;
    
    // setup prompt languages loop
    int promptTimer, promptLang;// = 0;
    
    // blink loop
    int blinkTimer, blink;// = 0;
    
    // screens
    int screen;// = 1; // 0 = grid; 1 = tracking; 2 = chosen
	
	// screens
    //int screen = 0; // 0 = grid; 1 = tracking; 2 = chosen
    int transition;
    int linesY;
    int line;
    int instructions_timeout;
    
    // for the grid
    int cols, rows, gutter, unitWidth, unitHeight;
    
    // setup userID:
    string userID;
    
    // setup selection:
    int selected, curSelectedX, curSelectedY, selectedX, selectedY;
	

};



#endif
