//
//  ofxBox2dLogo.cpp
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 7/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ofxBox2dLogo.h"

//------------------------------------------------
ofxBox2dLogo::ofxBox2dLogo(){
}
//------------------------------------------------
void ofxBox2dLogo::imageSetup() {
    
    logo.loadImage("logo1.png");
    
    
}

//------------------------------------------------
void ofxBox2dLogo::draw() {
	
	if(!isBody()) return;
	
    radius = getRadius();

    rot += 1%360;
    //float var = ofRandom(10);
    glPushMatrix();
    glTranslatef(getPosition().x, getPosition().y, 0);
    
    ofSetColor(255);
    ofFill();
    ofRotate(rot);
    ofCircle(0,0, radius*2);
    logo.draw(-1.2 * radius, -1.2 * radius, radius * 2.4, radius * 2.4);
    
    glPopMatrix();
	
}