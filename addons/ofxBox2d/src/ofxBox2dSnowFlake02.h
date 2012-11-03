//
//  ofxBox2dSnowFlake02.h
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 8/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ofxBox2dExample_ofxBox2dSnowFlake02_h
#define ofxBox2dExample_ofxBox2dSnowFlake02_h

#pragma once
#include "ofMain.h"
#include "ofxBox2dPolygonUtils.h"
#include "ofxBox2dCircle.h"


class ofxBox2dSnowFlake02 : public ofxBox2dCircle, public ofPolyline {
	
private:
	
	float radius;
	
public:
	ofxBox2dSnowFlake02();
    
	ofColor color;
    float var;
    float rot;
    float meltingRate;
    float lineWidth;
    
    
	virtual void draw(); 


};



#endif