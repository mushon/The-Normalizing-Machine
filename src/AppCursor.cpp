#include "AppCursor.h"


AppCursor::AppCursor(void)
{
	ring.setArcResolution(360);
	
	minRadius = 40;
	maxRadius = 50;

	lowAngle = -90; //12 oclock, 0 degrees is 3 oclock
	highAngle = -89;

	crossSize = 8;
}


void AppCursor::update(ofPoint pos, float progress)
{
	float smoothingFactor = 0.1;
	position.interpolate(pos, smoothingFactor);
	highAngle = ofMap(progress, 0.0f, 1.0f, -89, 270, true);
}

void AppCursor::draw()
{
	ofPushStyle();
	ofSetColor(ofColor::white);
	ofSetLineWidth(2);

	ofNoFill();
	ofCircle(position, minRadius);
	ring.clear();
	ring.arc(position, minRadius, minRadius, lowAngle, highAngle);  
	ring.moveTo(position);
	ring.arc(position, maxRadius, maxRadius, lowAngle ,highAngle);
	ring.draw();

	ofPushMatrix();
	ofTranslate(position);
	ofLine(-crossSize, 0, crossSize,0); // x line
	ofLine(0, -crossSize, 0, crossSize); // y line
	ofPopMatrix();
	ofPopStyle();
}