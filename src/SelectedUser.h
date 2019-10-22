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

		int id;

		ofPoint headPoint;

		float distance; // to spot

		SelectedArm leftArm;
		SelectedArm rightArm;

		AppTimer lastSeen;

		AppTimer selectionTimer;
		bool waitForSteady;

		float totalHeight; // distance(head, feet)
		float headHeight;  // distance(head, neck)
		float torsoLength; // distance(neck, torso)
		float shouldersWidth; // distance(shoulders)
		float armLength;

		bool handRaised;

		SelectedUser()
		{
			id = NO_USER;
			distance = FLT_MAX;
		}

		void reset()
		{
			leftArm.reset();
			rightArm.reset();
			
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
				selectionTimer.reset();
			}

			// TODO update: handRaised 
			// getSelectedArm().hand.z < getSelectedArm().shoulder.z - handShoulderDistance 
			//	|| getSelectedArm().hand.x - getSelectedArm().shoulder.x > abs(handShoulderDistance)

		}

		ofVec3f getPointingDir() const {
			return getSelectedArm().getPointingDir();
		}

		bool isSteady() const {
			return getSelectedArm().isSteady();
		}

		float getProgress() const 
		{
			return waitForSteady ? 1.0f : selectionTimer.getProgress();
		}


		const SelectedArm& getSelectedArm() const {
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
				// TODO: add hysteresis for switching hands. Track arm state.
			//}
		}


};