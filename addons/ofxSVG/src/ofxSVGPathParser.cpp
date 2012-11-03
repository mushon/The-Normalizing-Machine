#include "ofxSVGPathParser.h"

static int currentToken;

void ofxSVGPathParser::parse(string& path)
{
	nbuf = 0;
	char cmd;
	float args[10];
	int nargs;
	int rargs;
	float cpx, cpy, cpx2, cpy2;
	const char* tmp[4];
	char closedFlag;
	int i;
	char item[64];
	
	string::iterator s = path.begin();
	
	pathInstance->newSubPath();
	closedFlag = 0;
	nargs = 0;
	
	while (s != path.end())
	{
		getNextPathItem(s, item);
		if (!*item) break;
		
		if (isnum(item[0]))
		{
			if (nargs < 10) {
				args[nargs++] = (float) atof(item);
			}
			
			if (nargs >= rargs)
			{
				cout << " command " << cmd << endl;
				switch (cmd)
				{
					case 'm':
					case 'M':
					case 'l':
					case 'L':
						pathLineTo(&cpx, &cpy, args, (cmd == 'm' || cmd == 'l') ? 1 : 0);
						break;
					case 'H':
					case 'h':
						pathHLineTo(&cpx, &cpy, args, cmd == 'h' ? 1 : 0);
						break;
					case 'V':
					case 'v':
						pathVLineTo(&cpx, &cpy, args, cmd == 'v' ? 1 : 0);
						break;
					case 'C':
					case 'c':
						pathCubicBezTo(&cpx, &cpy, &cpx2, &cpy2, args, cmd == 'c' ? 1 : 0);
						break;
					case 'S':
					case 's':
						pathCubicBezShortTo(&cpx, &cpy, &cpx2, &cpy2, args, cmd == 's' ? 1 : 0);
						break;
					case 'Q':
					case 'q':
						pathQuadBezTo(&cpx, &cpy, &cpx2, &cpy2, args, cmd == 'q' ? 1 : 0);
						break;
					case 'T':
					case 't':
						pathQuadBezShortTo(&cpx, &cpy, &cpx2, &cpy2, args, cmd == 's' ? 1 : 0);
						break;
					default:
						if (nargs >= 2)
						{
							cpx = args[nargs-2];
							cpy = args[nargs-1];
						}
						break;
				}
				nargs = 0;
			}
		}
		else
		{
			cmd = item[0];
			rargs = getArgsPerElement(cmd);
			if (cmd == 'M' || cmd == 'm')
			{
				// Commit path->	
				// TODO get all the transform/translate/etc data from the parent?
				// or just return out of this and handle in the parent?
				//path = new vector<ofVec2f*>();
				// Start new subpath->
				//svgResetPath();
				pathInstance->close();
				closedFlag = 0;
				nargs = 0;
				cpx = 0; cpy = 0;
			}
			else if (cmd == 'Z' || cmd == 'z')
			{
				closedFlag = 1;
				// Commit path->
				//path = new vector<ofVec2f*>();
				// Start new subpath->
				pathInstance->close();
				closedFlag = 0;
				nargs = 0;
			}
		}
	}
	pathInstance->close();
}


void ofxSVGPathParser::cubicBez(float x1, float y1, float cx1, float cy1, float cx2, float cy2, float x2, float y2)
{
	cubicBezRec(x1,y1, cx1,cy1, cx2,cy2, x2,y2, 0); 
	//svgPathPoint(x2, y2);
	//pathInstance->lineTo(x1, y1);
}

void ofxSVGPathParser::quadBezRec( float x1, float y1, float x2, float y2, float x3, float y3, int level)
{
	float x12,y12,x23,y23,x123,y123,d;
	
	if (level > 12) return; // don't go too deep
	
	x12 = (x1+x2)*0.5f;                
	y12 = (y1+y2)*0.5f;
	x23 = (x2+x3)*0.5f;
	y23 = (y2+y3)*0.5f;
	x123 = (x12+x23)*0.5f;
	y123 = (y12+y23)*0.5f;
	
	d = distPtSeg(x123, y123, x1,y1, x3,y3);
	if (level > 0 && d < 1000) // tol*tol)
	{
		pathInstance->quadBezierTo(ofPoint(x1, x2), ofPoint(y2, y3), ofPoint( x123, y123 ));
		return;
	}
	
	quadBezRec(x1,y1, x12,y12, x123,y123, level+1); 
	quadBezRec(x123,y123, x23,y23, x3,y3, level+1); 
}

void ofxSVGPathParser::quadBez(float x1, float y1, float cx, float cy, float x2, float y2)
{
	quadBezRec(x1,y1, cx,cy, x2,y2, 0); 
}


void ofxSVGPathParser::pathCubicBezTo(float* cpx, float* cpy, float* cpx2, float* cpy2, float* args, int rel)
{
	
	float x1, y1, x2, y2, cx1, cy1, cx2, cy2;
	
	x1 = *cpx;
	y1 = *cpy;
	if (rel)
	{
		cx1 = *cpx + args[0];
		cy1 = *cpy + args[1];
		cx2 = *cpx + args[2];
		cy2 = *cpy + args[3];
		x2 = *cpx + args[4];
		y2 = *cpy + args[5];
	}
	else
	{
		cx1 = args[0];
		cy1 = args[1];
		cx2 = args[2];
		cy2 = args[3];
		x2 = args[4];
		y2 = args[5];
	}
	
	cubicBez(x1,y1, cx1,cy1, cx2,cy2, x2,y2);
	//pathInstance->bezierTo(*x1, *y1, *cx1, *cy1, *cx2, *cy2, *x2, *y2);
	
	*cpx2 = cx2;
	*cpy2 = cy2;
	*cpx = x2;
	*cpy = y2;
}

void ofxSVGPathParser::pathCubicBezShortTo(float* cpx, float* cpy,	float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx1, cy1, cx2, cy2;
	
	x1 = *cpx;
	y1 = *cpy;
	if (rel)
	{
		cx2 = *cpx + args[0];
		cy2 = *cpy + args[1];
		x2 = *cpx + args[2];
		y2 = *cpy + args[3];
	}
	else
	{
		cx2 = args[0];
		cy2 = args[1];
		x2 = args[2];
		y2 = args[3];
	}
	
	cx1 = 2*x1 - *cpx2;
	cy1 = 2*y1 - *cpy2;
	
	cubicBez(x1,y1, cx1,cy1, cx2,cy2, x2,y2);
	//pathInstance->quadBezierTo(x1, y1, cx1, cy1, cx2, cy2, x2, y2);
	
	*cpx2 = cx2;
	*cpy2 = cy2;
	*cpx = x2;
	*cpy = y2;
}

void ofxSVGPathParser::pathQuadBezTo(float* cpx, float* cpy, float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx, cy;
	
	x1 = *cpx;
	y1 = *cpy;
	if (rel)
	{
		cx = *cpx + args[0];
		cy = *cpy + args[1];
		x2 = *cpx + args[2];
		y2 = *cpy + args[3];
	}
	else
	{
		cx = args[0];
		cy = args[1];
		x2 = args[2];
		y2 = args[3];
	}
	
	quadBez(x1,y1, cx,cy, x2,y2);
	//pathInstance->quadBezierTo(x1, y1, cx, cy, x2, y2);
	
	*cpx2 = cx;
	*cpy2 = cy;
	*cpx = x2;
	*cpy = y2;
}

void ofxSVGPathParser:: ofxSVGPathParser::pathQuadBezShortTo(float* cpx, float* cpy, float* cpx2, float* cpy2, float* args, int rel)
{
	float x1, y1, x2, y2, cx, cy;
	
	x1 = *cpx;
	y1 = *cpy;
	if (rel)
	{
		x2 = *cpx + args[0];
		y2 = *cpy + args[1];
	}
	else
	{
		x2 = args[0];
		y2 = args[1];
	}
	
	cx = 2*x1 - *cpx2;
	cy = 2*y1 - *cpy2;
	
	quadBez(x1,y1, cx,cy, x2,y2);
	
	*cpx2 = cx;
	*cpy2 = cy;
	*cpx = x2;
	*cpy = y2;
}

void ofxSVGPathParser::cubicBezRec(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int level)
{
	float x12,y12,x23,y23,x34,y34,x123,y123,x234,y234,x1234,y1234;
	float d;
	
	if (level > 12) return;
	
	x12 = (x1+x2)*0.5f;
	y12 = (y1+y2)*0.5f;
	x23 = (x2+x3)*0.5f;
	y23 = (y2+y3)*0.5f;
	x34 = (x3+x4)*0.5f;
	y34 = (y3+y4)*0.5f;
	x123 = (x12+x23)*0.5f;
	y123 = (y12+y23)*0.5f;
	x234 = (x23+x34)*0.5f;
	y234 = (y23+y34)*0.5f;
	x1234 = (x123+x234)*0.5f;
	y1234 = (y123+y234)*0.5f;
	
	d = distPtSeg(x1234, y1234, x1,y1, x4,y4);
	if (level > 0 && d < 1000)//tol*tol)
	{
		//pathInstance->quadBezierTo( ofPoint(x1, y1), ofPoint(x2, y2), ofPoint(x3, y3) );//svgPathPoint(x1234, y1234);
		pathInstance->bezierTo( ofPoint(x2, y2), ofPoint(x3, y3), ofPoint(x4, y4) );
		return;
	}
	
	cubicBezRec(x1,y1, x12,y12, x123,y123, x1234,y1234, level+1); 
	cubicBezRec(x1234,y1234, x234,y234, x34,y34, x4,y4, level+1); 
}

void ofxSVGPathParser::pathLineTo(float* cpx, float* cpy, float* args, int rel)
{
	if (rel)
	{
		*cpx += args[0];
		*cpy += args[1];
	}
	else
	{
		*cpx = args[0];
		*cpy = args[1];
	}
	
	pathInstance->lineTo(*cpx, *cpy);
}

void ofxSVGPathParser::pathHLineTo(float* cpx, float* cpy, float* args, int rel)
{
	if (rel)
		*cpx += args[0];
	else
		*cpx = args[0];
	
	pathInstance->lineTo(*cpx, *cpy);
}

void ofxSVGPathParser::pathVLineTo(float* cpx, float* cpy, float* args, int rel)
{
	if (rel)
		*cpy += args[0];
	else
		*cpy = args[0];
	
	pathInstance->lineTo(*cpx, *cpy);
}

float ofxSVGPathParser::distPtSeg(float x, float y, float px, float py, float qx, float qy)
{
	float pqx, pqy, dx, dy, d, t;
	pqx = qx-px;
	pqy = qy-py;
	dx = x-px;
	dy = y-py;
	d = pqx*pqx + pqy*pqy;
	t = pqx*dx + pqy*dy;
	if (d > 0) t /= d;
	if (t < 0) t = 0;
	else if (t > 1) t = 1;
	dx = px + t*pqx - x;
	dy = py + t*pqy - y;
	return dx*dx + dy*dy;
}

int ofxSVGPathParser::getArgsPerElement(char cmd)
{
	
	cout << "ofxSVGPathParser::getArgsPerElement " << cmd << endl;
	
	switch (tolower(cmd))
	{
		case 'v':
		case 'h':
			return 1;
		case 'm':
		case 'l':
		case 't':
			return 2;
		case 'q':
		case 's':
			return 4;
		case 'c':
			return 6;
		case 'a':
			return 7;
	}
	return 0;
}

void ofxSVGPathParser::getNextPathItem(string::iterator &s, char* it)
{
	int i = 0;
	it[0] = '\0';
	// Skip white spaces and commas
	while (*s && (isspace(*s) || *s == ',')) ++s;
	if (!*s) return;
	if (*s == '-' || *s == '+' || isnum(*s))
	{
		while (*s == '-' || *s == '+')
		{
			if (i < 63) it[i++] = *s;
			++s;
		}
		while (*s && *s != '-' && *s != '+' && isnum(*s))
		{
			if (i < 63) it[i++] = *s;
			++s;
		}
		it[i] = '\0';
	}
	else
	{
		it[0] = *s;
		it[1] = '\0';
		++s;
		return;
	}
	return;
}

int ofxSVGPathParser::isnum(char c)
{
	return strchr("0123456789+-.eE", c) != 0;
}

