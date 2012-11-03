//
//  ofxBox2dSnowFlake.h
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 7/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ofxBox2dExample_ofxBox2dSnowFlake_h
#define ofxBox2dExample_ofxBox2dSnowFlake_h

#pragma once
#include "ofMain.h"
#include "ofxBox2dPolygonUtils.h"
#include "ofxBox2dCircle.h"


class ofxBox2dSnowFlake : public ofxBox2dCircle, public ofPolyline {
	
private:
	
	float radius;
	
public:
	ofxBox2dSnowFlake();
    
	ofColor color;
    float var;
    float rot;
    float meltingRate;
    float lineWidth;

    
	virtual void draw(); 

};


#endif
