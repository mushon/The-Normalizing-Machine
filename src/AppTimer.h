#pragma once

#include "ofMain.h"

class AppTimer
{

	unsigned long long timeout;
	unsigned long long last;

public:

	AppTimer(unsigned long long _timeout = 0) : timeout(_timeout)
	{
		reset();
	}

	void setTimeout(unsigned long long _timeout)
	{
		timeout = _timeout;
	}

	int getCountDown() const
	{
		int countdown = last + timeout - ofGetSystemTime();
		return max(0, countdown);
	}

	// 0 <= x <= 1, 0 -> 0
	float getProgress()
	{
		return float(getCountDown()) / timeout;
	}

	void reset()
	{
		last = ofGetSystemTime();
	}		
};

