/*
 *  ofxSVGUtils.h
 *  
 *
 */

#pragma mark once

#include "ofMain.h"
#include "ofxSVGConstants.h"
#include "ofxSVGTypes.h"
#include <Poco/Ascii.h>

enum errs {
	//tbd
	NO_ERR, ERR
};

class ofxSVGUtils {
public:
	//err_t parseTransform(TransformF& dst, const StringW& str)
	int parseTransform(transformInfo &transformation, const string& _str);
	
	void skew( ofMatrix4x4& mat, float x, float y) {
		
		float p0 = tan(x);
		float p1 = tan(y);
		
		if (abs(p0) < 0.00001) { p0 = 0.0; }
		if (abs(p1) < 0.00001) { p1 = 0.0; }
		
		ofMatrix4x4 skew(1.0, p1, 0.0, 0.0,
						 p0, 1.0, 0.0, 0.0,
						 0.0, 0.0, 1.0, 0.0,
						 0.0, 0.0, 0.0, 0.0);
		
		mat.postMult(skew);
	}
	
	void matrixFromString(string &smat, ofMatrix3x3 &mat) {
		
		size_t i, j;
		string spaceconst = " ";
		
		i = smat.find(spaceconst, 0);
		while(i != string::npos) {
			
			mat[j] = atof(smat.substr(i, smat.find(spaceconst, i) - i).c_str());
			i = smat.find(spaceconst, i);
			j++;
		}
	}

	static void stringFromMatrix(string& smat, ofMatrix3x3 mat) {
   		int i = 0;
   		stringstream s;
	    s << "matrix(";
	    while(i < 9) {   
	    	s << mat[i] << " ";
	    	i++;
	    }
	    s << ")";
	    //  int length = s.str().length();
	    smat.insert(0, s.str());
	}
	
	static void matrixFromNum(float a, float b, float c, float d, float e, float f, ofMatrix3x3 &mat) {
		mat[0] = a;
		mat[3] = b;
		mat[1] = c;
		mat[4] = d;
		mat[2] = e;
		mat[5] = f;
		mat[8] = 1.f;
	}
	
	void multSVGMatTo4x4(ofMatrix3x3 &three, ofMatrix4x4 &four) {
		four(0, 0) *= three[0];
		four(0, 1) *= three[1];
		four(0, 2) *= three[2];
		four(1, 0) *= three[3];
		four(1, 1) *= three[4];
		four(1, 2) *= three[5];
	}
	
	void setSVGMatTo4x4(ofMatrix3x3 &three, ofMatrix4x4 &four) {
		four(0, 0) = three[0];
		four(0, 1) = three[1];
		four(0, 2) = three[2];
		four(1, 0) = three[3];
		four(1, 1) = three[4];
		four(1, 2) = three[5];
	}
	
private:
	
	errs err;
	
	 string::iterator strCur;
	// char* strEnd;
	 string::iterator functionName;
	 size_t functionLen;
	
	 float d[6];
	 size_t d_count;
	 string str;

	 void getFunction(transformInfo &transformation);
	 void parseFunction(transformInfo &transformation, errs err);
	 void findArguments(transformInfo &transformation);
	 int endTransformParse(errs err) {
		return err;
	};

	 void stringToDouble( float* dst, string::iterator &strit, size_t length, char decimalPoint, size_t* pEnd);
	 bool stringEquals(string::iterator a, const string& b, size_t length)
	{
		
		string::iterator t(a);
		string::const_iterator t2 = b.begin();
		
		for (; t2 != b.end() && *t; ++t, ++t2)
		{
			if (*t != *t2)
				return false;
		}
		return true;
	};
	
	 void stringToDouble( const string& str, double& d  ) {
		
		d = ::atof(str.c_str());
	};

};