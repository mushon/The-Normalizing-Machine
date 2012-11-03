//
//  ofxBox2dSnowFlake02.cpp
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 8/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxBox2dSnowFlake02.h"
#include "ofxBox2dBaseShape.h"


//------------------------------------------------
ofxBox2dSnowFlake02::ofxBox2dSnowFlake02(){
}

//------------------------------------------------
void ofxBox2dSnowFlake02::draw() {
	
	if(!isBody()) return;
	
	radius = getRadius();
    rot += 0.7;
    //float var = ofRandom(10);
    glPushMatrix();
    glTranslatef(getPosition().x, getPosition().y, 0);
    ofRotateY(rot);
    ofRotateZ(rot);
    
    ofSetLineWidth(1.0f);
    ofSetColor(255, 255, 255);
//    ofSetColor(color.r, color.g, color.b);
    for(int i=0; i < 4; i++){
        ofRotate(60);
        ofSetLineWidth(lineWidth);
        
        
        ofLine(-1 * radius,0,radius,0);
        
        ofSetLineWidth(2);
        
        //Right Side of the Line
        ofLine(ofPoint(0.7*radius, 0), ofPoint(0.95 * radius, 0.05 * radius));
        ofLine(ofPoint(0.7*radius, 0), ofPoint(0.95 * radius, -0.05 * radius));
        
        ofLine(ofPoint(0.61*radius, 0), ofPoint(0.783*radius, 0.166*radius));
        ofLine(ofPoint(0.61*radius, 0), ofPoint(0.783*radius, -0.166*radius));
        
        ofLine(ofPoint(0.45*radius, 0), ofPoint(0.7*radius, 0.25*radius));
        ofLine(ofPoint(0.45*radius, 0), ofPoint(0.7*radius, -0.25*radius));
        
        
        //Left Side of the Line
        ofLine(ofPoint(-0.7*radius, 0), ofPoint(-0.95 * radius, 0.05 * radius));
        ofLine(ofPoint(-0.7*radius, 0), ofPoint(-0.95 * radius, -0.05 * radius));
        
        ofLine(ofPoint(-0.61*radius, 0), ofPoint(-0.783*radius, 0.166*radius));
        ofLine(ofPoint(-0.61*radius, 0), ofPoint(-0.783*radius, -0.166*radius));
        
        ofLine(ofPoint(-0.45*radius, 0), ofPoint(-0.7*radius, 0.25*radius));
        ofLine(ofPoint(-0.45*radius, 0), ofPoint(-0.7*radius, -0.25*radius));
//        ofLine(0.66 * radius, 0, radius, 0.2 * radius);
//        ofLine(0.5 * radius, 0, 0.66 * radius, 0.33 * radius);
//        ofLine(0.66 * radius, 0, radius, -0.2 * radius);
//        ofLine(0.5 * radius, 0, 0.66 * radius, -0.33 * radius);
        
//        ofLine(-0.66 * radius, 0, -1 * radius, 0.2 * radius);
//        ofLine(-0.5 * radius, 0, -0.66 * radius, 0.33 * radius);
//        ofLine(-0.66 * radius, 0, -1 * radius, -0.2 * radius);
//        ofLine(-0.5 * radius, 0, -0.66 * radius, -0.33 * radius);
        
    }
    
    glPopMatrix();
	

}

