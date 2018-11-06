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
	void start(string recDir, string filename, string ext = ".mp4");
	void update();
	//void stop();
	void abort();
	bool isRecording() {
		return recording;
	};
	static const int  RECORDING_TIME = 20;// 20s
	static const int  RECORDING_TIME_EXTRA = 0.5;// 20s
	static const string args;
	static const string ffmpeg;
	execThread ffmpegThread;
	float time;
	bool recording;

};

