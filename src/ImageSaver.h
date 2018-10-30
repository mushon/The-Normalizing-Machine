#pragma once

#include "ofMain.h"

class ImageSaver
{
public:
	ImageSaver();
	~ImageSaver();

	void setup(const string& parentDir, string format);
	void save(const string& dir);
	static const int CAPTURE_W = 1920;
	static const int CAPTURE_H = 1080;
	string parentDir;
	string format;
	ofVideoGrabber grabber;
};

