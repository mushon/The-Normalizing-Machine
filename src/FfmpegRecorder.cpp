#include "FfmpegRecorder.h"

const string FfmpegRecorder::args = " -y -an -t " + ofToString(RECORDING_TIME) + " -f dshow -framerate 30 -video_size 1920x1080 -i video=\"Logitech BRIO\" -preset faster ";
const string FfmpegRecorder::ffmpeg = "C:\\Users\\PROTECH\\TNM\\ffmpeg-4.0.2-win64-static\\bin\\ffmpeg.exe";

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


void FfmpegRecorder::start(string recDir, string filename, string ext /*= ".mp4"*/)
{
	string cmd(ffmpeg + args + recDir + filename + "." + ext);
	ffmpegThread.setup(cmd);
	recording = true;
}

void FfmpegRecorder::update()
{
	if (recording) {
		if ((ofGetElapsedTimef() - time) > (RECORDING_TIME + RECORDING_TIME_EXTRA)) {
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
