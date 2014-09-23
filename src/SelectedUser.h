#pragma once

#include "ofMain.h"
#include "AppTimer.h"

class SelectedUser
{
public:
		static const int NO_USER = -1;

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

		AppTimer steady;
		AppTimer steadySelect;
		bool steadyResetWhenMove;

		SelectedUser() : 
			id(NO_USER), handSmoothingFactor(0.1f), shoulderSmoothingFactor(0.05f), steady(1000), steadySelect(5000)
		{
			steadyResetWhenMove = true;
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
				if (steadyResetWhenMove) steadySelect.reset();
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

};