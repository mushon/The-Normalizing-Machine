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
		ofVec2f screenPoint01;

		AppTimer lastSeen;

		AppTimer selectTimer;
		bool waitForSteady;

		float totalHeight; // distance(head, feet)
		float headHeight;  // distance(head, neck)
		float torsoLength; // distance(neck, torso)
		float shouldersWidth; // distance(shoulders)


		SelectedUser() : selectTimer(3000)
		{
			id = NO_USER;
			distance = FLT_MAX;

			reset();
		}

		void reset()
		{
			leftArm.reset();
			rightArm.reset();
			
			hovered = NO_HOVER;

			selectTimer.reset();
			waitForSteady = true;

			totalHeight = 0;
			headHeight = 0;
			torsoLength = 0;
			shouldersWidth = 0;
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
			return (leftArm.hand.z < rightArm.hand.z) ? leftArm : rightArm;
		}

};