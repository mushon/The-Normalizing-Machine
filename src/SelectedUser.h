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

		AppTimer selectTimer;
		bool waitForSteady;

		SelectedUser() : selectTimer(8000)
		{
			reset();
		}

		void reset()
		{
			leftArm.reset();
			rightArm.reset();
			
			id = NO_USER;
			hovered = NO_HOVER;
			selectTimer.reset();
			waitForSteady = true;
		}

		void update()
		{
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
			//return leftArm;
			return rightArm;
		}

};