//
//  ofxBox2dFITLogo.cpp
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 9/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ofxBox2dFITLogo.h"

//------------------------------------------------
ofxBox2dFITLogo::ofxBox2dFITLogo(){
}
//------------------------------------------------
void ofxBox2dFITLogo::imageSetup() {
    
    logo.loadImage("logo.jpeg");
//    fitFont.loadFont("ThonburiBold.ttf", 32);
    if(fitFont.isLoaded()){
        ofLog(OF_LOG_NOTICE, "Seems to be working");
    }
}

//------------------------------------------------
void ofxBox2dFITLogo::draw() {
	
	if(!isBody()) return;
	
    radius = getRadius();
    
    rot += 1%360;
    //float var = ofRandom(10);
    glPushMatrix();
    glTranslatef(getPosition().x, getPosition().y, 0);
    
    ofSetColor(36, 54, 108);
    ofFill();
    ofRotate(rot);
    ofCircle(0,0, radius*4.2);
    ofSetColor(255,255,255);
    ofSetLineWidth(radius/2);
    
//
//    // Letter F
//    ofLine(-(radius * 2), (radius * 3/2), -(radius * 2), -(radius * 3/2));
//    ofLine(-(radius * 2), -(radius * 3/2), -(radius), -(radius * 3/2));
//    ofLine(-(radius * 2), -(radius * 1/2), -(radius * 4/3), -(radius * 1/2));
//    
//    // Letter I
//    ofLine(0, (radius * 3/2), 0, -(radius * 3/2));
//    
//    
//    fitFont.loadFont("futura_md_bt_medium.ttf", radius * 3);
//    fitFont.setLetterSpacing(0.9);
//    fitFont.drawString("FIT", -(radius * 11/4),(radius * 3/2));
    logo.draw(-3 * radius, -3 * radius, radius * 6, radius * 6);
    
    glPopMatrix();
	
}