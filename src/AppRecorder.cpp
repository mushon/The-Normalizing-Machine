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
	lastFilename = generateFileName();
	ofLogNotice("startRecording") << recDir + lastFilename;
	openNIRecorder.startRecording(recDir + lastFilename);
	ofLogNotice("startRecording") << "OK";
	isRecording = true;
}


void AppRecorder::update()
{
	openNIRecorder.update();
}

void AppRecorder::save()
{
	ofLogNotice("saveRecording");

	ofLogNotice("saveRecording") << "OK";

	// when recording is complete, save his selection data, process face frames and save to data,
	// update other selected x/v in db
	// select 25 :current25
	// meanwhile in 'position yourself'
	// select 4 :current4
	// when to start recording? (each hoverChange abort and start over) 
	// void abortRecording() // delete file
	// 


}


void AppRecorder::stop()
{
	ofLogNotice("") << "stopRecording: " << lastFilename << endl;
	openNIRecorder.stopRecording();
	isRecording = false;
	ofLogNotice("") << "stopRecording: " << "OK" << endl;

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
