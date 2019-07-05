#pragma once
#include "ofMain.h"

class execThread : public ofThread {
public:
	execThread();
	void setup(string command);
	void threadedFunction();
	void close();
private:
	string execCommand;
};

class FfmpegRecorder
{
public:
	FfmpegRecorder();
	~FfmpegRecorder();
	//void setup();
	bool start(string recDir, string filename, int recordingDuration, string ext = ".mp4");
	bool capture(string recDir, string sessionDir, ofRectangle cropRect, string ext = ".jpeg");
	void update();
	//void stop();
	void abort();
	bool isRecording() {
		return recording;
	};
	static const int  RECORDING_TIME = 20;// 20s
	static const int  CAPTURE_TIME = 9;// 20s
	static const int RECORDING_TIME_EXTRA = 1;// 20s
	static const string V_ARGS;
	static const string CAPTURE_ARGS;
	static const string FFMPEG;
	execThread ffmpegThread;
	float startTime;
	bool recording;
	float recordingTime;

};

