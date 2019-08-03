#pragma once

#include "ofMain.h"

class AppInputDevice
{
public:

	virtual void setup() { };
	virtual void update() {};
	virtual void draw() { };
	virtual void draw_debug() { };
	virtual void exit() { };

	virtual int countVisibleUsers() = 0;
	virtual const SelectedUser& getClosestUser() = 0;  // --> activeUser
	virtual ofPoint getScreenPoint() = 0;

};

class MouseInputDevice : public AppInputDevice {
// This class emulates both:
// -  a 'kinect' sensor (# people),
// -  a User (id, position, hand-raised)
// -  Video Input (recording)

public:
	virtual void setup() { 
		user.id = 99;
		user.lastSeen.setTimeout(10000);
		user.selectionTimer.setTimeout(1000);
		user.selectionTimer.reset();

		// Grabber
		camWidth = 640;  // try to grab at this size.
		camHeight = 480;
		

		//get back a list of devices.
		vector<ofVideoDevice> devices = vidGrabber.listDevices();

		for(size_t i = 0; i < devices.size(); i++){
			if(devices[i].bAvailable){
				//log the device
				ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
			} else {
				//log the device and note it as unavailable
				ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
			}
		}

		vidGrabber.setDeviceID(0);
		vidGrabber.setDesiredFrameRate(30);
		vidGrabber.initGrabber(camWidth, camHeight);

		videoPixels.allocate(camWidth, camHeight, OF_PIXELS_RGB);
		videoTexture.allocate(videoPixels);
	};

	virtual void update() {

		mousePosition.x = ofGetMouseX();
		mousePosition.y = ofGetMouseY();
		
		user.update();
		user.handRaised = false;
		if (ofGetMousePressed(0)) {
			user.selectionTimer.reset();
			user.handRaised = true;
		}

		cursor.update(mousePosition, user.selectionTimer.getProgress());
		
		user.headPoint = ofPoint(mousePosition.x - ofGetScreenWidth() / 2, 
			0, mousePosition.y);

		user.distance = mousePosition.distance(ofPoint(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2));

		vidGrabber.update();

	};
	
	virtual void draw() { 
		// show fake image?
		ofPushMatrix();
		ofPushStyle();

		ofSetColor(ofColor::white);
    	vidGrabber.draw(0, 0, 340, 480); // 340? 380?


		ofPopStyle();
		ofPopMatrix();

	};
	
	virtual void draw_debug() { 
		cursor.draw();
	};
	// virtual void exit() { };

	virtual int countVisibleUsers() {
		if (mousePosition.x > ofGetScreenWidth() - 100 && mousePosition.y < 100)
		{
			return 2;
		}
		else if (user.distance < 1000) {
			return 1;
		}
		else {
		 	return 0;
		}
	}
	virtual const SelectedUser& getClosestUser() {
		if (countVisibleUsers() > 0) {
			return user;
		}
		else {
			return SelectedUser();
		}
	}

	virtual ofPoint getScreenPoint() {
		return mousePosition;
	}


private:
	SelectedUser user;
	ofPoint mousePosition;
	AppCursor cursor;

	ofVideoGrabber vidGrabber;
	ofPixels videoPixels;
	ofTexture videoTexture;
	int camWidth;
	int camHeight;

};

class KinectInputDevice : public AppInputDevice {
	
public:
	virtual void setup() {
	/*
		kinect.initSensor();
		//kinect.initIRStream(640, 480);
		kinect.initColorStream(KINECT_WIDTH, KINECT_HIGHT, true);
		kinect.initDepthStream(KINECT_WIDTH, KINECT_HIGHT, true);
		kinect.initSkeletonStream(false);
		//kinect.setUseStreams(true);
		//kinect.createColorPixels(KINECT_WIDTH, KINECT_HIGHT);
		kinect.setUseTexture(true);
		kinect.setDepthClipping(depthClip.x, depthClip.y); // TODO:: export to settings

		//simple start
		kinect.start();
	*/	
	}


	virtual void update() {
	// was void testApp::updateSelectedUser()

	/*
	SelectedUser user = getClosestUser();
	if (user.id == SelectedUser::NO_USER)
	{
		selectedUser = SelectedUser(); //reset
	}
	else
	{
	 /
		// CHANGED USER (state)

		// keep track of id (if changes in the middle)
		selectedUser.id = user.id;
		selectedUser.distance = user.distance;
		selectedUser.headPoint = user.headPoint;

		Skeleton& skeleton = kinect.getSkeletons().at(user.id);

		auto & head = skeleton.at(NUI_SKELETON_POSITION_HEAD);

		auto & rhj = skeleton.at(NUI_SKELETON_POSITION_WRIST_RIGHT);
		auto & rsj = skeleton.at(NUI_SKELETON_POSITION_SHOULDER_RIGHT);
		auto & lhj = skeleton.at(NUI_SKELETON_POSITION_WRIST_LEFT);
		auto & lsj = skeleton.at(NUI_SKELETON_POSITION_SHOULDER_LEFT);

		auto & neck = skeleton.at(NUI_SKELETON_POSITION_SHOULDER_CENTER);
		auto & hip = skeleton.at(NUI_SKELETON_POSITION_HIP_CENTER);

		auto & relbow = skeleton.at(NUI_SKELETON_POSITION_ELBOW_RIGHT);
		auto & rwrist = skeleton.at(NUI_SKELETON_POSITION_WRIST_RIGHT);


		// update body measurements
		float userHeight = 0;
		if (head.getTrackingState() == SkeletonBone::Tracked) {
			userHeight = head.getStartPosition().y + kinectYPos;
		}

		if (userHeight > selectedUser.totalHeight) {
			selectedUser.totalHeight = userHeight;
		}

		float headHeight = 0;
		if (neck.getTrackingState() == SkeletonBone::Tracked && head.getTrackingState() == SkeletonBone::Tracked) {
			headHeight = head.getStartPosition().distance(neck.getStartPosition());
		}
		if (headHeight > selectedUser.headHeight) {
			selectedUser.headHeight = headHeight;
		}

		float torsoLength = 0;
		if (neck.getTrackingState() == SkeletonBone::Tracked && hip.getTrackingState() == SkeletonBone::Tracked) {
			torsoLength = neck.getStartPosition().distance(hip.getStartPosition());
		}

		if (torsoLength > selectedUser.torsoLength) {
			selectedUser.torsoLength = torsoLength;
		}

		float shouldersWidth = 0;
		if (rsj.getTrackingState() == SkeletonBone::Tracked && lsj.getTrackingState() == SkeletonBone::Tracked) {
			shouldersWidth = rsj.getScreenPosition().distance(lsj.getScreenPosition());
		}
		if (shouldersWidth > selectedUser.shouldersWidth) {
			selectedUser.shouldersWidth = shouldersWidth;
		}

		float armLength = 0;
		if (relbow.getTrackingState() == SkeletonBone::Tracked && rwrist.getTrackingState() == SkeletonBone::Tracked) {
			armLength = relbow.getStartPosition().distance(rwrist.getStartPosition());
		}

		if (armLength > selectedUser.armLength) {
			selectedUser.armLength = armLength;
		}

		bool updateLeftArm = lhj.getTrackingState() == SkeletonBone::Tracked && lsj.getTrackingState() == SkeletonBone::Tracked;
		bool updateRightArm = rhj.getTrackingState() == SkeletonBone::Tracked && rsj.getTrackingState() == SkeletonBone::Tracked;

		if (updateLeftArm)
		{
			ofPoint leftHand = lhj.getStartPosition();
			ofPoint leftShoulder = lsj.getStartPosition();
			selectedUser.leftArm.update(leftHand, leftShoulder);
		}
		if (updateRightArm)
		{
			ofPoint rightHand = rhj.getStartPosition();
			ofPoint rightShoulder = rsj.getStartPosition();
			selectedUser.rightArm.update(rightHand, rightShoulder);
		}

		if (updateRightArm || updateLeftArm)
		{
			selectedUser.update();
		}
		else
		{
			selectedUser.reset(selectionTimeout);
		}

	}
	*/
	}


	// note: adds id, distance and headpoint to SelectedUser
	virtual const SelectedUser& getClosestUser()
	{
		SelectedUser user;
		/*
		auto skeletons = kinect.getSkeletons();
		for (int i = 0; i != skeletons.size(); i++) {
			Skeleton skeleton = skeletons[i];
			if (!skeleton.empty()) {
				if (KinectUtil::checkMainJointsConfidence(skeleton)) {
					user.headPoint = skeleton.at(NUI_SKELETON_POSITION_HEAD).getStartPosition();
					user.headPoint = user.headPoint * 1000;
					ofVec2f dist = ofVec2f(user.headPoint.x - spot.x, user.headPoint.z - spot.z); // discard height(y)    <<<--------------------------might be a hang here, consider other way of choosing

					float distance = dist.length();
					if (distance < user.distance)
					{
						user.id = i;
						user.distance = distance;
					}

					userMessage << user.id << ":" << user.headPoint << endl;
				}
			}
		} // end for map
		*/

		return user;
	}

	virtual void draw() {
		// kinect.getColorTexture().drawSubsection(0, 0, w, h, offsetW, offsetH, w, h);
		//kinect.draw(0, 0);
		//appRecorder.drawImageSubsection(w, h, offsetW, offsetH);
	}

	virtual void draw_debug() {
	// was: void testApp::drawKinect()
	/*
	kinect.draw(640, 0);
	kinect.drawDepth(0, 0);

	ofPushStyle();
	ofSetColor(255, 0, 0);
	ofSetLineWidth(3.0f);
	auto skeletons = kinect.getSkeletons();
	for (auto & skeleton : skeletons) {
		for (auto & bone : skeleton) {
			switch (bone.second.getTrackingState()) {
			case SkeletonBone::Inferred:
				ofSetColor(0, 0, 255);
				break;
			case SkeletonBone::Tracked:
				ofSetColor(0, 255, 0);
				break;
			case SkeletonBone::NotTracked:
				ofSetColor(255, 0, 0);
				break;
			}

			auto index = bone.second.getStartJoint();
			auto connectedTo = skeleton.find((_NUI_SKELETON_POSITION_INDEX)index);
			if (connectedTo != skeleton.end()) {
				ofLine(connectedTo->second.getScreenPosition(), bone.second.getScreenPosition());
			}

			ofCircle(bone.second.getScreenPosition(), 10.0f);
		}
	}
	ofPopStyle();
	 */
	}


	virtual ofPoint getScreenPoint() {

		//ofPoint p = selectedUser.getPointingDir();

		//float x = -(selectedUser.getSelectedArm().shoulder.z - screenZ) * p.x / p.z - screenL;
		//float y = -(selectedUser.getSelectedArm().shoulder.z - screenZ) * p.y / p.z - screenB;

		//float kx = (x-screenL) / (screenR - screenL);
		//float ky = (y-screenB) / (screenT - screenB);


		// TODO: sanity check if hand is +- at shoulder level
		//ofVec2f v(2*kx-1, 2*ky-1);
		//ofVec2f v(2*kx-1, 2*ky-1);
		

		//v.x = powf(fabs(v.x), 1.5) * (v.x > 0 ? 1 : -1); // should do some non linear function,
		//v.y = powf(fabs(v.y), 1.5) * (v.y > 0 ? 1 : -1); // should do some non linear function,
		//v.y = powf(v.y, 3); // only on x

		//float cx = ofGetScreenWidth() / 2;
		//float cy = ofGetScreenHeight() / 2;

		//selectedUser.screenPoint = v.getMapped(ofVec2f(cx, cy), ofVec2f(cx, 0), ofVec2f(0, -cy)); // reverse y, assume -1 < v.x, v.y < 1

		//selectedUser.screenPoint.x = ofLerp(ofGetScreenWidth() / 2, selectedUser.screenPoint.x, 0.1);  // force to center // 2-player hack
		//selectedUser.screenPoint.y = ofLerp(ofGetScreenHeight() / 2, selectedUser.screenPoint.y, 0.1);  // force to center // 2-player hack
		 
		 /*
		 EW: this was the latest running code, all the above was commented
		ofVec2f v;
		v.x = ofMap(selectedUser.getSelectedArm().hand.x, screenL, screenR, 0, ofGetWidth() , true);
		v.y = ofGetHeight() / 2  + cursorHightOffset; // // fix to 
		  */
	}

};