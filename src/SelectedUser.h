#pragma once

#include "ofMain.h"
#include "AppTimer.h"

#include "SelectedArm.h"

// add NO_USER as Selected user type
// min Length = FLT_MAX

class SelectedUser
{
public:
		static const int NO_USER = -1;
		static const int NO_HOVER = -1;

		int hovered;

		int id;

		ofPoint headPoint;

		float distance; // to spot

		SelectedArm leftArm;
		SelectedArm rightArm;

		ofVec2f screenPoint;

		AppTimer lastSeen;

		AppTimer selectTimer;
		bool waitForSteady;

		float totalHeight; // distance(head, feet)
		float headHeight;  // distance(head, neck)
		float torsoLength; // distance(neck, torso)
		float shouldersWidth; // distance(shoulders)
		float armLength;


		SelectedUser()
		{
			id = NO_USER;
			distance = FLT_MAX;
		}

		void reset(unsigned long long timeout)
		{
			leftArm.reset();
			rightArm.reset();
			
			hovered = NO_HOVER;

			selectTimer.setTimeout(timeout);
			selectTimer.reset();
			waitForSteady = true;

			totalHeight = 0;
			headHeight = 0;
			torsoLength = 0;
			shouldersWidth = 0;
			armLength = 0;
		}

		void update()
		{
			lastSeen.reset();
			
			if (isSteady())
			{
				waitForSteady = false;
			}
			if (waitForSteady)
			{
				selectTimer.reset();
			}
		}

		ofVec3f getPointingDir() {
			return getSelectedArm().getPointingDir();
		}

		bool isSteady() {
			return getSelectedArm().isSteady();
		}

		float getProgress()
		{
			return waitForSteady ? 1.0f : selectTimer.getProgress();
		}


		SelectedArm& getSelectedArm() {
			/*
			float z = abs(leftArm.hand.z - rightArm.hand.z);
			float xl = abs(leftArm.hand.x - leftArm.shoulder.x);
			float xr = abs(rightArm.hand.x - rightArm.shoulder.x);
			if (xl > z && xl > xr) {
				return leftArm;
			}
			else if (xr > z && xr > xl) {
				return rightArm;
			}
			else {
				*/
				// z is biggest 
				return (leftArm.hand.z < rightArm.hand.z) ? leftArm : rightArm;
			//}
		}

};