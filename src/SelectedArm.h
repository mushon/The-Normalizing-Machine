#pragma once


class SelectedArm {


public:

	float handSmoothingFactor;
	float shoulderSmoothingFactor;

	SelectedArm() : handSmoothingFactor(0.1f), shoulderSmoothingFactor(0.05f), steady(300)
	{
		reset();
	}


	void reset() {
		hand = ofVec3f();
		shoulder = ofVec3f();
		handVelocity = ofVec3f();
		steady.reset();
	}


	void update(ofPoint h, ofPoint s) {

		if (hand == ofVec3f() && shoulder == ofVec3f())
		{
			hand = h;
			shoulder = s;
		}
		else
		{
			hand.interpolate(h, handSmoothingFactor);
			shoulder.interpolate(s, shoulderSmoothingFactor);
			handVelocity = prevHand - h;
		}

		if (handVelocity.length() > 10)
		{
			steady.reset();
		}

		prevHand = h;

	}

	ofVec3f getPointingDir() const {
		return hand - shoulder;
	}

	bool isSteady() const {
		return steady.getCountDown() <= 0;
	}


	ofPoint hand;
	ofPoint shoulder;

	ofPoint prevHand;
	ofVec3f handVelocity;
	AppTimer steady;
};
