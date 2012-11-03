//
//  ofxBox2dSnowFlake.cpp
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 7/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxBox2dSnowFlake.h"

//------------------------------------------------
ofxBox2dSnowFlake::ofxBox2dSnowFlake(){
}

//------------------------------------------------
void ofxBox2dSnowFlake::draw() {
	
	if(!isBody()) return;
	
	radius = getRadius();
    rot += 0.7;
    //float var = ofRandom(10);
    glPushMatrix();
    glTranslatef(getPosition().x, getPosition().y, 0);
  //  ofRotateY(rot);
    ofRotateZ(rot);
    
    ofSetLineWidth(lineWidth);
    ofSetColor(color.r, color.g, color.b);
    for(int i=0; i < 180; i+= 90){
        ofRotate(i);
        ofLine(-1 * radius,0,radius,0);
        ofLine(0.66 * radius, 0, radius, 0.33 * radius); 
        ofLine(0.66 * radius, 0, radius, -0.33 * radius);
        ofLine(-0.66 * radius, 0, -1 * radius, 0.33 * radius);
        ofLine(-0.66 * radius, 0, -1 * radius, -0.33 * radius);
        
    }
    ofRotate(45);
    radius = radius + var;
    for(int i=0; i < 180; i += 90){
        ofRotate(i);
        ofLine(-1 * radius,0,radius,0);
        ofLine(0.66 * radius, 0, radius, 0.33 * radius); 
        ofLine(0.66 * radius, 0, radius, -0.33 * radius);
        ofLine(-0.66 * radius, 0, -1 * radius, 0.33 * radius);
        ofLine(-0.66 * radius, 0, -1 * radius, -0.33 * radius);
        
    }
    
    ofPopStyle();

    glPopMatrix();
	
}