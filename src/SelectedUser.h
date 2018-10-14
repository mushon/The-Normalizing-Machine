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

		SelectedUser() : selectTimer(8000)
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