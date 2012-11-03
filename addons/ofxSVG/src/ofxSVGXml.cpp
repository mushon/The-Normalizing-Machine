#include "ofxSVGXml.h"


// ofxSVGXml additions
//-------------------------------------------------------------------------
int ofxSVGXml::getNumTags(){
	TiXmlHandle tagHandle = storedHandle;
	int count = 0;
	TiXmlElement* child = storedHandle.FirstChildElement().ToElement();
	for (count = 0; child; child = child->NextSiblingElement(), ++count){}
	return count;
}
string ofxSVGXml::getName(int which){
	TiXmlHandle tagHandle = storedHandle;

    int i = 0;
	TiXmlElement* child = storedHandle.FirstChildElement().ToElement();
	for (; i<which && child; i++, child = child->NextSiblingElement()){}

    return (i==which) ? child->ValueStr() : "";
}
string ofxSVGXml::getValue(int which){
	TiXmlHandle tagHandle = storedHandle;

    int i = 0;
	TiXmlElement* child = storedHandle.FirstChildElement().ToElement();
	for (; i<which && child; i++, child = child->NextSiblingElement()){}
    const char* value;
    if(i==which) value = child->GetText();

    return (value!=NULL) ? value : "";
}
string ofxSVGXml::getAttribute(const string& attribute, int which){
	TiXmlHandle tagHandle = storedHandle;
    int i = 0;
	TiXmlElement* child = storedHandle.FirstChildElement().ToElement();
	for (; i<which && child; i++, child = child->NextSiblingElement()){}
    const char* attributeValue;
    if(child && i==which) attributeValue = child->Attribute(attribute.c_str());
    return (attributeValue!=NULL) ? attributeValue : "";
}
bool ofxSVGXml::pushTag(int which){
	TiXmlHandle isRealHandle = storedHandle.Child(which);

	if( isRealHandle.ToNode() ){
		storedHandle = isRealHandle;
		level++;
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
