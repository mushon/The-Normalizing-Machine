//
//  ofxBox2dAdvert.h
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 7/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ofxBox2dExample_ofxBox2dAdvert_h
#define ofxBox2dExample_ofxBox2dAdvert_h

#pragma once
#include "ofMain.h"
#include "ofxBox2dPolygonUtils.h"
#include "ofxBox2dRect.h"


class ofxBox2dAdvert : public ofxBox2dRect, public ofPolyline {
	
private:
	
	float radius;
	
public:
	ofxBox2dAdvert();
    
    ofxBox2dRect    boundingBox;
    
	ofColor color;
    float var;
    float xPos;
    float yPos;
    float rot;
    
    float sizeMulitplier;
    float   starAlpha;
    
    ofImage     advert;
    
    virtual void imageSetup();
    virtual void draw();
    GLUquadricObj *quadric;
    
};


#endif
