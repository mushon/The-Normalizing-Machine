#include "FfmpegRecorder.h"

const string FfmpegRecorder::V_ARGS = " -y -an -f dshow -framerate 10 -video_size 640x480 -i video=\"Logitech BRIO\" -an -vcodec h264_nvenc -preset fast -filter:v transpose=1 ";
//-c:v h264_nvenc - qp 0 -filter:v transpose=1

const string FfmpegRecorder::CAPTURE_ARGS = " -f dshow -s 1920x1080 -i video=\"Logitech BRIO\" -vframes 10 -r 0.2 -an -vf transpose=1,crop=";
//-s uhd2160
const string FfmpegRecorder::FFMPEG = "C:\\ffmpeg-4.0.2\\bin\\ffmpeg.exe";

//--------------------------------------------------------------
//--------------------------------------------------------------
execThread::execThread() {
	execCommand = "";
}

//--------------------------------------------------------------
void execThread::setup(string command) {
	execCommand = command;
	startThread(false);
}

//--------------------------------------------------------------
void execThread::threadedFunction() {
	if (isThreadRunning()) {
		system(execCommand.c_str());
		while (isThreadRunning()) {
			yield();
		}
	}
}

void execThread::close()
{
	if (isThreadRunning()) {
		waitForThread();
	}
	system("TASKKILL /IM ffmpeg.exe /F");
}


FfmpegRecorder::FfmpegRecorder()
{
}


FfmpegRecorder::~FfmpegRecorder()
{
	ffmpegThread.close();
}


bool FfmpegRecorder::start(string recDir, string filename, int recordingDuration, string ext /*= ".mp4"*/)
{
	if (!recording) {
		string cmd(FFMPEG + " -t " + ofToString(recordingDuration / 1000) + V_ARGS + recDir + filename + ext);
		ffmpegThread.setup(cmd);
		recordingTime = recordingDuration / 1000;
		startTime = ofGetElapsedTimef();
		recording = true;
		return true;
	}
	return false;
}

bool FfmpegRecorder::capture(string recDir, string sessionDir, ofRectangle cropRect, string ext /*= ".jpeg"*/)
{
	ofDirectory::createDirectory(recDir + sessionDir, true, true);
	if (!recording) {
		string cmd(FFMPEG + CAPTURE_ARGS +
			ofToString(cropRect.width) + ":" +
			ofToString(cropRect.height) + ":" +
			ofToString(cropRect.x) + ":" +
			ofToString(cropRect.y) + " Data/" +
			recDir + sessionDir + "/frame_" +
			ofToString(ofGetElapsedTimeMillis()) +
			+"_%02d" + ext);
		ffmpegThread.setup(cmd);
		recordingTime = CAPTURE_TIME;
		startTime = ofGetElapsedTimef();
		recording = true;
		return true;
	}
	return false;
}

void FfmpegRecorder::update()
{
	if (recording) {
		if ((ofGetElapsedTimef() - startTime) > (recordingTime + RECORDING_TIME_EXTRA)) {
			ffmpegThread.close();
			recording = false;
		}
	}
}

void FfmpegRecorder::abort()
{
	if (recording) {
		ffmpegThread.close();
	}
	recording = false;
}
