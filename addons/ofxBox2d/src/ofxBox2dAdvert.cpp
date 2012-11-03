//
//  ofxBox2dAdvert.cpp
//  ofxBox2dExample
//
//  Created by Ezer Lichtenstein on 7/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ofxBox2dAdvert.h"

//------------------------------------------------
ofxBox2dAdvert::ofxBox2dAdvert(){
}
//------------------------------------------------
void ofxBox2dAdvert::imageSetup() {
    
    advert.loadImage("advert.png");
    
    
}

//------------------------------------------------
void ofxBox2dAdvert::draw() {
	
	if(!isBody()) return;
	
//    rot += 10%360;
    //float var = ofRandom(10);
    glPushMatrix();
    glTranslatef(getPosition().x, getPosition().y, 0);
    
    advert.draw(ofPoint(0,0), 150, 32);
    
    glPopMatrix();
	
}