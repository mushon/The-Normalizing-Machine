#include "AppRecorder.h"

string generateFileName() {
	string timeFormat = "%Y_%m_%d_%H_%M_%S_%i";
	string name = ofGetTimestampString(timeFormat);
	string filename = (name + ".oni");
	return filename;
}

void AppRecorder::setup(string _filename)
{
	isRecording = false;

	openNIRecorder.setup();
	openNIRecorder.addDepthStream();
	openNIRecorder.addImageStream();
	openNIRecorder.addUserTracker();
	//	openNIRecorder.addHandsTracker();
	openNIRecorder.start();
}

void AppRecorder::start(string recDir)
{
	if (!isRecording)
	{
		isRecording = true;
		lastFilename = generateFileName();
		ofLogNotice("startRecording") << recDir + lastFilename;
		openNIRecorder.startRecording(recDir + lastFilename);
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
		ofLogNotice("") << "stopRecording: " << lastFilename << endl;
		openNIRecorder.stopRecording();
		ofLogNotice("") << "stopRecording: " << "OK" << endl;
		isRecording = false;
	}
	//HACKHACK !!!
	//setupPlayback(lastFilename);
}

void AppRecorder::abort()
{
	ofLogNotice("") << "abortRecording: " << lastFilename << endl;
	stop();
	//delete file?
}


bool checkMainJointsConfidence(ofxOpenNIUser& u)
{
	ofxOpenNIJoint jh = u.getJoints().at(nite::JointType::JOINT_HEAD);
	ofxOpenNIJoint jrs = u.getJoints().at(nite::JointType::JOINT_RIGHT_SHOULDER);
	ofxOpenNIJoint jls = u.getJoints().at(nite::JointType::JOINT_LEFT_SHOULDER);
	ofxOpenNIJoint jt = u.getJoints().at(nite::JointType::JOINT_TORSO);

	return (jh.positionConfidence > 0.5 &&
		(jrs.positionConfidence > 0.5 || jls.positionConfidence > 0.5) &&
		jt.positionConfidence > 0.5);

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
