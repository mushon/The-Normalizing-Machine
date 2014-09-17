#include "AppCursor.h"


AppCursor::AppCursor(void)
{
	path.setArcResolution(360);
	
	minRadius = 40;
	maxRadius = 50;

	lowAngle = -90; //12 oclock, 0 degrees is 3 oclock
	highAngle = -89;

	crossSize = 8;
}


void AppCursor::update(ofPoint pos, float progress)
{
	center = pos;
	highAngle = ofMap(progress, 0.0f, 1.0f, -89, 270, true);
}

void AppCursor::draw()
{
	ofPushStyle();
	ofSetColor(ofColor::white);
	ofSetLineWidth(2);

	ofNoFill();
	ofCircle(center, minRadius);
	path.clear();
	path.arc(center, minRadius, minRadius, lowAngle, highAngle);  
	path.moveTo(center);
	path.arc(center, maxRadius, maxRadius, lowAngle ,highAngle);
	path.draw();

	ofPushMatrix();
	ofTranslate(center);
	ofLine(-crossSize, 0, crossSize,0); // x line
	ofLine(0, -crossSize, 0, crossSize); // y line
	ofPopMatrix();
	ofPopStyle();
}