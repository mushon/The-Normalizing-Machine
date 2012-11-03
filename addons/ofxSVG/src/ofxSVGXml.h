
#pragma mark once
#include "ofxXmlSettings.h"


class ofxSVGXml : public ofxXmlSettings {
public:
		int     getNumTags();

        string  getName(int which = 0);
		string 	getValue(int which = 0);
		string	getAttribute(const string& attribute, int which = 0);

		bool    pushTag(int which = 0);


        using ofxXmlSettings::pushTag;
        using ofxXmlSettings::popTag;
        using ofxXmlSettings::getNumTags;
        using ofxXmlSettings::getValue;
        using ofxXmlSettings::getAttribute;
        using ofxXmlSettings::setVerbose;
        using ofxXmlSettings::loadFile;
        using ofxXmlSettings::tagExists;

};

