#include "FfmpegRecorder.h"

const string FfmpegRecorder::V_ARGS = " -y -an -t " + ofToString(RECORDING_TIME) + " -f dshow -framerate 25 -video_size 1920x1080 -i video=\"Logitech BRIO\" -an -preset faster ";
//-c:v h264_nvenc - qp 0

const string FfmpegRecorder::CAPTURE_ARGS = " -f dshow -s uhd2160 -i video=\"Logitech BRIO\" -vframes 10 -r 0.5 -an -vf crop=";
const string FfmpegRecorder::FFMPEG = "C:\\Users\\toga\\Documents\\ffmpeg-20180925-a7429d8-win64-static\\ffmpeg-20180925-a7429d8-win64-static\\bin\\ffmpeg.exeffmpeg.exe";

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
}


bool FfmpegRecorder::start(string recDir, string filename, string ext /*= ".mp4"*/)
{
	if (!recording) {
		string cmd(FFMPEG + V_ARGS + recDir + filename + "." + ext);
		ffmpegThread.setup(cmd);
		recordingTime = RECORDING_TIME;
		startTime = ofGetElapsedTimef();
		recording = true;
		return true;
	}
	return false;
}

bool FfmpegRecorder::capture(string recDir, string sessionDir, ofRectangle cropRect, bool profile, string ext /*= ".jpeg"*/)
{
	if (!recording) {
		string cmd(FFMPEG + CAPTURE_ARGS + ofToString(cropRect.x) + ":" +
			ofToString(cropRect.y) + ":" +
			ofToString(cropRect.width) + ":" +
			ofToString(cropRect.height) + ":" + " " +
			recDir + sessionDir + "/frame_" + ofToString(ofGetElapsedTimeMillis()) +
	"_" + ofToString(profile) + "_%d." + ext);
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
