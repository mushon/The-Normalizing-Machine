
/**
 *
 * WARNING, FYI, NOTA BENE, CUIDADO : This is experimental and not really ready yet.
 *
 * Much of this builds on nanosvg and so is LGPL'd hence the license below:
 *
 */


#pragma once

#include <cmath>
#include "ofMain.h"
#include "ofxSVGTypes.h"


class ofxSVGPathParser {
public:
	
	int nbuf, level, npos;
	
	ofPath* pathInstance;
	
	ofxSVGPathParser(ofPath* p) {
		pathInstance = p;
	}
	
	void  parse(string &path);
	
	float distPtSeg(float x, float y, float px, float py, float qx, float qy);
	void pathCubicBezTo(float* cpx, float* cpy, float* cpx2, float* cpy2, float* args, int rel);
	
	void pathCubicBezShortTo(float* cpx, float* cpy,float* cpx2, float* cpy2, float* args, int rel);
	
	void pathQuadBezTo(float* cpx, float* cpy, float* cpx2, float* cpy2, float* args, int rel);
	
	void  pathQuadBezShortTo(float* cpx, float* cpy, float* cpx2, float* cpy2, float* args, int rel);
	
	
	void cubicBezRec(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int level);
	void cubicBez( float x1, float y1, float cx1, float cy1, float cx2, float cy2, float x2, float y2);
	
	void quadBezRec(float x1, float y1, float x2, float y2, float x3, float y3, int level);
	
	void pathLineTo(float* cpx, float* cpy, float* args, int rel);
	
	void pathHLineTo(float* cpx, float* cpy, float* args, int rel);
	void pathVLineTo(float* cpx, float* cpy, float* args, int rel);
	
	void quadBez(float x1, float y1, float cx, float cy, float x2, float y2);
	//void svgPathPoint(float x, float y);
	int getArgsPerElement(char cmd);
	void getNextPathItem(string::iterator &s, char* it);
	int isnum(char c);
	
private:
	
	bool _absolute;
	bool _in_path;
	
	ofVec2f startPoint, endPoint;
    double m_rx, m_ry, m_rot_angle;
    bool m_large_arc, m_sweep;
    bool m_svg_compliant;
    double m_start_angle, m_end_angle;
    ofVec2f m_center;
	
    bool isDegenerate(ofVec2f p) {
		
        return ( are_near(p.x, 0.0f) ||  are_near(p.y, 0.0f) );
    }
	
	//bool are_near(double a, double b) { return fabs(a-b) <= 1e-5; }
	bool are_near(float a, float b) { return fabs(a-b) <= 1e-5; }

};

