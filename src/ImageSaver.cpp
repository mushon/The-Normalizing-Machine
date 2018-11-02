#include "ImageSaver.h"



ImageSaver::ImageSaver()
{
}


ImageSaver::~ImageSaver()
{
}


void ImageSaver::setup(const string& parentDir, string format) {
	this->parentDir = parentDir;
	this->format = format;
	
	auto devs = grabber.listDevices();

	for (auto & d : devs) {
		std::cout << d.id << ": " << d.deviceName << " // " <<
			d.formats.size() << " formats" << std::endl;
		for (auto & f : d.formats) {
			std::cout << "  " << f.width << "x" << f.height << std::endl;
			for (auto & fps : f.framerates) {
				std::cout << "    " << fps << std::endl;
			}
		}
	}
	// Setup grabber
	grabber.setDeviceID(0);
	grabber.setPixelFormat(OF_PIXELS_NATIVE);
	grabber.setDesiredFrameRate(30);
	grabber.setup(CAPTURE_W, CAPTURE_H);
	grabber.setUseTexture(false);
	grabber.update();
}

void ImageSaver::save(const string& dir) {
	string out  = parentDir + "/" + dir + "/frame_" + ofToString(ofGetElapsedTimeMillis()) +"." + format;

	grabber.update();
	if (grabber.isFrameNew()) {
		//ofPixels pixels = grabber.getPixels();
		ofImage image;
		image.setUseTexture(false);
		image.setFromPixels(grabber.getPixelsRef());
		image.crop(600, 100, 720, 680);
		image.update();
 		image.save(out);
	}
}