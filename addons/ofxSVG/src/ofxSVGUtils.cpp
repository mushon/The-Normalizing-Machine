/*
 *  ofxSVGUtils.cpp
 *  emptyExample
 *
 *  Created by base on 02/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSVGUtils.h"

int ofxSVGUtils::parseTransform(transformInfo &transformation, const string& _str)
{
	str = _str;
	strCur = str.begin();
	
	memset(&d[0], 0, 6);
	
	d_count = 0;
	
	while( strCur < str.end() )
	{
		
		getFunction(transformation);
	}
	
	
	return endTransformParse(err);
}

void ofxSVGUtils::getFunction(transformInfo &transformation)
{
	
	using namespace Poco;
	
	// Skip spaces.
	for (;;)
	{
		if (strCur >= str.end()) return;
		
		else if (Poco::Ascii::isSpace(*strCur)) ++strCur;
		else break;
	}
	
	functionName = strCur;
	
	for (;;)
	{
		if (strCur == str.end()) return;
		else if (Poco::Ascii::isUpper(*strCur) || Poco::Ascii::isLower(*strCur)) ++strCur;
		else break;
	}
	functionLen = (size_t)(strCur - functionName);
	
	// Parse '('.
	if (*strCur != '(') return;
	++strCur;
	
	// Parse arguments.
	d_count = 0;
	for (;;)
	{
		if (strCur == str.end()) 
			return;
		
		// Parse number.
		size_t end;
		//float* dst,  const char* str, size_t length, char decimalPoint = '.'
		stringToDouble(&d[d_count++], strCur, (size_t)(str.end() - strCur), '.', &end);
		
		strCur += end;
		
		findArguments(transformation);
		
		if (d_count == 6) return;
	}
}

void ofxSVGUtils::findArguments(transformInfo &transformation)
{
	// Skip ',' and move to position of the next digit.
	bool commaParsed = false;
	
	for (;;)
	{
		if (strCur >= str.end() || !(*strCur)) return;// endTransformParse(err);
		
		if (Poco::Ascii::isSpace(*strCur))
		{
			++strCur;
		}
		else if (*strCur == ',')
		{
			++strCur;
			if (commaParsed) 
				return;
			
			commaParsed = true;
		}
		else if (*strCur == ')')
		{
			++strCur;
			if (commaParsed) 
				return;
			
			parseFunction(transformation, NO_ERR);
			return;
		}
		else if (Poco::Ascii::isDigit(*strCur) || *strCur == '-' || *strCur == '+')
		{
			return;
		}
		else
		{
			return;
		}
	}
}

void ofxSVGUtils::parseFunction(transformInfo &transformation, errs err)
{
	
	// matrix() function.
	if (functionLen == 6 && stringEquals(functionName, "matrix", 6))
	{
		
		cout << " matrix " << endl;
		
		if (d_count != 6) return;

		// this might be wrong
		transformation.rotate = atan2(d[3],d[0]);
		transformation.trans = ofVec2f(d[4], d[5]);
		transformation.scale = ofVec2f(d[1], d[3]);
		
	}
	// translate() function.
	else if (functionLen == 9 && stringEquals(functionName, "translate", 9))
	{
		cout << " translate " << endl;
		
		if (d_count != 1 && d_count != 2) return;
		// If ty is not provided, it's assumed to be zero.
		if (d_count == 1) d[1] = 0.0f;
		transformation.trans += ofVec2f(d[0], d[1]);
	}
	// scale() function.
	else if (functionLen == 5 && stringEquals(functionName, "scale", 5))
	{
		cout << " scale " << endl;
		
		if (d_count != 1 && d_count != 2) return;
		// If sy is not provided, it's assumed to be equal to sx.
		if (d_count == 1) d[1] = d[0];
		transformation.scale += ofVec2f(d[0], d[1]);
	}
	// rotate() function.
	else if (functionLen == 6 && stringEquals(functionName, "rotate", 6))
	{
		cout << " rotate " << d[0] << endl;
		
		if ( !(d_count == 1 || d_count == 3)) return;
		 
		if (d_count == 3) {
			transformation.rotate.y += d[1];
			transformation.rotate.z += d[2];
		}
		
		transformation.rotate.x += d[0];
		
	}
	// skewX() function.
	else if (functionLen == 5 && stringEquals(functionName, "skewX", 5))
	{
		cout << " skewX " << endl;
		if (d_count != 1) return;
		transformation.skew[0] = d[0];
	}
	// skewY() function.
	else if (functionLen == 5 && stringEquals(functionName, "skewY", 5))
	{
		cout << " skewY " << endl;
		if (d_count != 1) return;
		transformation.skew[1] = d[0];
	}
	else
	{
		return;
	}
	
	// Skip spaces.
	for (;;)
	{
		if (strCur == str.end()) break;

		if(Poco::Ascii::isSpace( *strCur ))
			++strCur;
		else
			return;
	}
}


void ofxSVGUtils::stringToDouble( float* dst, string::iterator &strit, size_t length, char decimalPoint = '.', size_t* pEnd = NULL  ) {
	
	size_t count = 0;
	string::iterator tmp(strit);
	string val = "";
	while( Poco::Ascii::isDigit(*tmp) || *tmp == decimalPoint || *tmp == '-') {
		
		cout << *tmp << endl;
		
		val += *tmp;
		++tmp;
		count++;
	}
	
	*pEnd = count;
	*dst = float (::atof(val.c_str()));
};