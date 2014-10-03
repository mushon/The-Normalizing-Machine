#pragma once

#include "ofMain.h"

class AppCursor
{
public:
	AppCursor(void);
	void update(ofPoint pos, float progress);
	void draw();
	void setPosition(ofVec2f pos) {position = pos;}
private:
	ofPath path;

	ofVec2f position;

	int minRadius;
	int maxRadius;

	float lowAngle; //12 oclock, 0 degrees is 3 oclock
	float highAngle;

	int crossSize;
};

