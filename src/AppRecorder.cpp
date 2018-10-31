#include "AppRecorder.h"

void AppRecorder::setup()
{
	isRecording = false;

	openNIRecorder.setup();
	openNIRecorder.addDepthStream();
	openNIRecorder.addImageStream();
	openNIRecorder.addUserTracker();
	//	openNIRecorder.addHandsTracker();
	openNIRecorder.start();
}

void AppRecorder::start(string recDir, string filename, string ext)
{
	if (!isRecording)
	{
		isRecording = true;
		ofLogNotice("startRecording") << recDir + filename + ext;
		openNIRecorder.startRecording(recDir + filename + ext);
		ofLogNotice("startRecording") << "OK";
	}
}


void AppRecorder::update()
{
	openNIRecorder.update();
}


void AppRecorder::stop()
{
	if (isRecording)
	{
		ofLogNotice("") << "stopRecording: " << endl;
		openNIRecorder.stopRecording();
		ofLogNotice("") << "stopRecording: " << "OK" << endl;
		isRecording = false;
	}
	//HACKHACK !!!
	//setupPlayback(lastFilename);
}

void AppRecorder::abort()
{
	if (isRecording)
	{
		ofLogNotice("") << "abortRecording: " << endl;
		stop();
		//delete file?
	}
}


bool checkMainJointsConfidence(ofxOpenNIUser& u)
{
	ofxOpenNIJoint jh = u.getJoints().at(nite::JointType::JOINT_HEAD);
	ofxOpenNIJoint jrs = u.getJoints().at(nite::JointType::JOINT_RIGHT_SHOULDER);
	ofxOpenNIJoint jls = u.getJoints().at(nite::JointType::JOINT_LEFT_SHOULDER);
	ofxOpenNIJoint jt = u.getJoints().at(nite::JointType::JOINT_TORSO);

	// bool headOK = jh.positionConfidence > 0.5; 
	// NOTE: not checking head since hand can hide it sometimes
	bool oneShoulderOK = (jrs.positionConfidence > 0.5 || jls.positionConfidence > 0.5);
	bool torsoOK = jt.positionConfidence > 0.5;
	return (oneShoulderOK && torsoOK);

}

int AppRecorder::countVisibleUsers()
{
	int nVisibleUsers = 0;
	// HACK: nite internally counts down 10 seconds, even if user is not visible
	if (openNIRecorder.trackedUsers.size() > 0)
	{
		for (map<int, ofxOpenNIUser>::iterator it = openNIRecorder.trackedUsers.begin(); it != openNIRecorder.trackedUsers.end(); ++it)
		{
			ofxOpenNIUser& u = it->second;
			if (u.isVisible() && checkMainJointsConfidence(u))
			{
				nVisibleUsers++;
			}
		}
	}

	return nVisibleUsers;
}
