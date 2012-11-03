//
//  ofxBox2dFITLogo.h
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 9/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ofxBox2dExample_ofxBox2dFITLogo_h
#define ofxBox2dExample_ofxBox2dFITLogo_h

#include "ofMain.h"
#include "ofxBox2dPolygonUtils.h"
#include "ofxBox2dCircle.h"

class ofxBox2dFITLogo : public ofxBox2dCircle, public ofPolyline {

private:

float radius;

public:
ofxBox2dFITLogo();

ofColor color;
ofTrueTypeFont fitFont;
float var;
float xPos;
float yPos;
float rot;
float meltingRate;
float lineWidth;

float sizeMulitplier;
float starAlpha;

ofImage     logo;

virtual void imageSetup();
virtual void draw();
GLUquadricObj *quadric;

};


#endif

