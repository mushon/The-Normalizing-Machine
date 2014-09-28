#pragma once

#include "ofMain.h"
#include "AppTimer.h"

class SelectedUser
{
public:
		static const int NO_USER = -1;
		static const int NO_HOVER = -1;

		float handSmoothingFactor;
		float shoulderSmoothingFactor;
		int hovered;

		int id;

		ofPoint rightHand;
		ofPoint rightShoulder;
		ofPoint headPoint;
		ofVec2f dist;

		ofVec3f rightHandVelocity;

		ofPoint hand; //filtered
		ofPoint shoulder; //filtered

		ofVec2f screenPoint;
		ofVec2f screenPoint01;

		AppTimer steady;
		AppTimer selectTimer;
		bool waitForSteady;

		SelectedUser() : 
			handSmoothingFactor(0.1f), shoulderSmoothingFactor(0.05f), steady(300), selectTimer(5000)
		{
			reset();
		}

		void reset()
		{
			hand = ofVec3f();
			shoulder = ofVec3f();
			rightHandVelocity = ofVec3f();
			
			id = NO_USER;
			hovered = NO_HOVER;
			steady.reset();
			selectTimer.reset();
			waitForSteady = true;
		}

		void updatePoints(ofPoint h, ofPoint s)
		{
			if (hand == ofVec3f() && shoulder == ofVec3f())
			{
				hand = h;
				shoulder = s;
			}
			else
			{
				hand.interpolate(h, handSmoothingFactor);
				shoulder.interpolate(s, shoulderSmoothingFactor);
				rightHandVelocity = rightHand - h;
			}

			if (!isHandSteady())
			{
				steady.reset();
			}
			if (isSteady())
			{
				waitForSteady = false;
			}
			if (waitForSteady)
			{
				selectTimer.reset();
			}

			rightHand = h;
		}

		ofVec3f getPointingDir()
		{
			return hand - shoulder;
		}

		bool isHandSteady()
		{
			return rightHandVelocity.length() < 10;
		}

		bool isSteady() {return steady.getCountDown() <= 0;}

		float getProgress()
		{
			return waitForSteady ? 1.0f : selectTimer.getProgress();
		}



};