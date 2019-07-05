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
	virtual SelectedUser getClosestUser() = 0;  // --> activeUser

};

class AppMouse : public AppInputDevice {

public:
	virtual void setup() { 
		user.id = 99;
	};

	virtual void update() {
		mousePosition.x = ofGetPreviousMouseX();
		mousePosition.y = ofGetPreviousMouseY();

		user.distance = mousePosition.distance(ofPoint(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2));;
	};
	// virtual void draw() { };
	// virtual void draw_debug() { };
	// virtual void exit() { };

	virtual int countVisibleUsers() {
		if (mousePosition.x < 100 && mousePosition.y < 100)
		{
			return 0;
		}
		else if (mousePosition.x > 1000 && mousePosition.y > 1000) {
			return 2;
		}
		else {
			return 1;
		}
	}
	virtual SelectedUser getClosestUser() {
		if (countVisibleUsers() > 0) {
			return user;
		}
		else {
			return SelectedUser();
		}
	}

private:
	SelectedUser user;
	ofPoint mousePosition;
};

class AppKinect : public AppInputDevice {
	
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

		userMessage << "user.totalHeight: " << selectedUser.totalHeight << endl;
		userMessage << "user.headHeight: " << selectedUser.headHeight << endl;
		userMessage << "user.torsoLength: " << selectedUser.torsoLength << endl;
		userMessage << "user.shouldersWidth: " << selectedUser.shouldersWidth << endl;
		userMessage << "user.armLength: " << selectedUser.armLength << endl;



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
	virtual SelectedUser getClosestUser()
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

};