#pragma once
#include "ofMain.h"
// #include "ofxKinectCommonBridge.h"

class KinectUtil
{
public:
	KinectUtil();
	~KinectUtil();
	// static int countVisibleUsers(ofxKinectCommonBridge& kinect);
	// static bool checkMainJointsConfidence(Skeleton& skeleton);
	static int countVisibleUsers() { return 1; }
	static bool checkMainJointsConfidence() { return true; }

};

