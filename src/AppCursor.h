#pragma once

#include "ofMain.h"

class AppCursor
{
public:
	AppCursor(void);
	void update(ofPoint pos, float progress);
	void draw();

private:
	ofPath path;

	ofPoint center;

	int minRadius;
	int maxRadius;

	float lowAngle; //12 oclock, 0 degrees is 3 oclock
	float highAngle;

	int crossSize;
};

