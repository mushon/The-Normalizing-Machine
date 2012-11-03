//
//  ofxBox2dLogo.h
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 7/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ofxBox2dExample_ofxBox2dLogo_h
#define ofxBox2dExample_ofxBox2dLogo_h

#pragma once
#include "ofMain.h"
#include "ofxBox2dPolygonUtils.h"
#include "ofxBox2dCircle.h"


class ofxBox2dLogo : public ofxBox2dCircle, public ofPolyline {
	
private:
	
	float radius;
	
public:
	ofxBox2dLogo();
        
	ofColor color;
    float var;
    float xPos;
    float yPos;
    float rot;
    
    float sizeMulitplier;
    float   starAlpha;
    
    ofImage     logo;
    
    virtual void imageSetup();
    virtual void draw();
    GLUquadricObj *quadric;
    
};


#endif
