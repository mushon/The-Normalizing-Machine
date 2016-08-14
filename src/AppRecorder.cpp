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

void AppRecorder::start()
{
	lastFilename = generateFileName();
	ofLogNotice("startRecording") << recDir + lastFilename;
	openNIRecorder.startRecording(recDir + lastFilename);
	isRecording = true;
	ofLogNotice("startRecording") << "OK";
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

