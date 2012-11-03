#include "ofxSVG.h"

//--------------------------------------------------------------
void ofxSVG::load(string svgPath){

    bVerbose = true;

    if(bVerbose){
        cout<<"ofxSVG: Loading \""<<svgPath<<"\""<<endl<<endl;
    }

    TiXmlBase::SetCondenseWhiteSpace(false);

    ifstream file(ofToDataPath(svgPath).c_str());

    // Check if File Exist
    //--------------------------------------------
    if (!file && bVerbose){
        printf("ofxSVG: File not found.");
        return;
    } else {
        // Extract string from file
        //----------------------------------------
        string line;
        string svgString;
        while (getline(file, line)) {
            svgString += "\n"+line;
        }

        // Check if SVG
        //----------------------------------------
        if(svgString.find("<svg") == -1 && bVerbose){
            cout<<"ofxSVG: Unknown Format."<<endl;
            return;
        }
        else {
            // Remove Header
            //------------------------------------
            int svgTagPos = svgString.find("<svg");
            svgString = svgString.substr(svgTagPos, svgString.length() - svgTagPos);

            // Load File to TinyXml
            //--------------------------------------------
            svgXml.loadFromBuffer(svgString);
        }

        file.close();
    }

    // Get Metadatas
    //--------------------------------------------
    svgVersion  = svgXml.getAttribute("svg", "version", "undefined");

    docWidth    = svgXml.getAttribute("svg", "width", 0);
    docHeight   = svgXml.getAttribute("svg", "height", 0);

    if(bVerbose){
        cout<<"ofxSVG: SVG Version: "<<svgVersion<<endl;
        cout<<"ofxSVG: Document size: "<<docWidth<<"x"<<docHeight<<endl;
    }

    // Enter SVG Main Layer
    //--------------------------------------------
    svgXml.pushTag("svg");

	int pops;
	if(svgXml.tagExists("switch")) {
		svgXml.pushTag("switch");
		pops++;
	}

	// Read Number of Layers
	int nLayers = svgXml.getNumTags("g");

	//if(nLayers == 0) { // Only one layer
	if(nLayers == 0 || nLayers == 1) {
		
		if(bVerbose) {
			cout<<"ofxSVG: Loading one layer."<<endl;
		}
		
		if(nLayers == 1) {
			svgXml.pushTag("g");
			pops++;
		}
		
		ofxSVGLayer layer;
		layer.name = svgXml.getAttribute("g", "id", "");
		
		cout << layer.name << endl;
		
		string transform = svgXml.getAttribute("g", "transform", "");
		cout << transform << endl;
		if(transform != "") {
			ofxSVGUtils utils;
			utils.parseTransform(layer.transformation, transform);
			layer.hasTransform = 1;
		}
		
		layers.push_back( layer );
		
		parseLayer();
		
	} else { // Multiple Layers
		
		if(bVerbose) {
			cout<<"ofxSVG: Loading "<<nLayers<<" layers."<<endl;
		}
		
		for(int i=0; i<nLayers; i++){
			
			ofxSVGLayer layer(drawingMode);
			layer.name = svgXml.getAttribute("g", "id", "", i);
			
			string transform = svgXml.getAttribute("g", "transform", "", i);
			if(transform != "") {
				ofxSVGUtils utils;
				utils.parseTransform(layer.transformation, transform);
				layer.hasTransform = 1;
			}
			
			layers.push_back( layer );

			if(bVerbose) {
				cout<<"ofxSVG: Layer \""<<layer.name<<"\" :"<<endl;
			}

			svgXml.pushTag("g", i);
				parseLayer();
			svgXml.popTag();
		}
	}

	for(int i=0;i<pops; i++) 
		svgXml.popTag();

    svgXml.popTag();

    if(bVerbose){
        cout<<endl<<"ofxSVG: Loading success!"<<endl;
    }

    TiXmlBase::SetCondenseWhiteSpace(true);
	
	ofSetCircleResolution(200);
}


ofxSVG::ofxSVG( ofFbo &fbo ) 
{ 
	fboForDrawing = fbo;
	drawingMode = DRAW_FBO;
}

ofxSVG::ofxSVG( ofFbo *fbo ) {
	fboForDrawing = *fbo;
	drawingMode = DRAW_FBO;
}

ofxSVG::ofxSVG( ofTexture &tex ) 
{ 
	texForDrawing = tex;
	drawingMode = DRAW_TEXTURE;
}

ofxSVG::ofxSVG( ofTexture *tex ) {
	texForDrawing = *tex;
	drawingMode = DRAW_TEXTURE;
}

ofxSVG::ofxSVG( ofVbo &vbo ) 
{ 
	vboForDrawing = vbo;
	drawingMode = DRAW_VBO;
}

ofxSVG::ofxSVG( ofVbo *vbo ) {
	vboForDrawing = *vbo;
	drawingMode = DRAW_VBO;
}

ofxSVG::ofxSVG( ) {
	drawingMode = DRAW_VERTEX_ARRAY;
}

ofxSVG::~ofxSVG() {
	
	// lotta cleanup to do here
}

// Parsing
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void ofxSVG::parseLayer(){
    int layerId = (int) (layers.size()) - 1;

    // Get Number of Objects in this Layer
    //--------------------------------------------
    int numObjects = svgXml.getNumTags();
    if(bVerbose) cout<<"ofxSVG: numObjects: "<<numObjects<<endl;
	

    // Loop threw all objects, parse and render them
    //--------------------------------------------
    for(int i=0; i<numObjects; i++) {
        currentIteration = i;

        string name = svgXml.getName(currentIteration);
		
		cout << "name " << name << endl;

        if(name == "rect")  parseRect();
        else if(name == "circle") parseCircle();
        else if(name == "ellipse") parseEllipse();
        else if(name == "line") parseLine();
        else if(name == "polygon") parsePolygon();
        else if(name == "text") parseText();
        else if(name == "path")parsePath();
		else if(name == "image")parseImage();
        else if(name == "g"){
            svgXml.pushTag(i);
                parseLayer();
            svgXml.popTag();
        }
    }

}

GLint ofxSVG::getImageColorType(ofImage &image) {
	GLint imgType;
	int channum = image.getPixelsRef().getNumChannels();
	if(channum == 3) {
		return GL_RGB;
	} else if( channum == 4 ) {
		return GL_RGBA;
	}
	return GL_LUMINANCE;
}

void ofxSVG::parseImage() {
	
	string path = svgXml.getAttribute("xlink:href", currentIteration);
	int imgWidth, imgHeight, x, y;
	imgWidth = atoi(svgXml.getAttribute("width", currentIteration).c_str());
	imgHeight = atoi(svgXml.getAttribute("height", currentIteration).c_str());
	x = atoi(svgXml.getAttribute("x", currentIteration).c_str());
	y = atoi(svgXml.getAttribute("y", currentIteration).c_str());
	ofImage tmpimg;
	tmpimg.loadImage(path);

	ofxSVGImage *img = new ofxSVGImage();
	img->tex = new ofTexture;
	
	img->tex->allocate(tmpimg.width, tmpimg.height, getImageColorType(tmpimg));
	img->tex->loadData(tmpimg.getPixels(), tmpimg.width, tmpimg.height, getImageColorType(tmpimg));
	
	img->tex->draw(x, y, imgWidth, imgHeight);
	
	layers[layers.size()-1].objects.push_back(img);
}


void ofxSVG::parseRect(){
    string xStr     = svgXml.getAttribute("x", currentIteration);
    string yStr     = svgXml.getAttribute("y", currentIteration);
    float x         = (xStr!="") ? ofToFloat(xStr) : 0.0f;
    float y         = (yStr!="") ? ofToFloat(yStr) : 0.0f;
    float width     = ofToFloat(svgXml.getAttribute("width", currentIteration));
    float height    = ofToFloat(svgXml.getAttribute("height", currentIteration));
    string id       = svgXml.getAttribute("id", currentIteration);

    // Extract Rotation from
    //------------------------------------
    string transform = svgXml.getAttribute("transform", currentIteration);
	string opacity = svgXml.getAttribute("opacity", currentIteration);
	string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

    if(!(fill=="none" && stroke== "")){

        ofxSVGRectangle* obj = new ofxSVGRectangle;
		
		if(transform != "") {
			ofxSVGUtils utils;
			utils.parseTransform(obj->transformation, transform);
			obj->hasTransform = 1;
		}

        // Shape info
        //--------------------------------
        obj->name        = id;
        obj->x           = x;
        obj->y           = y;
        obj->width       = width;
        obj->height      = height;

		if(fill!="none"){
			parseFill(&svgXml, obj, fill, opacity);
		} else {
			obj->fill = 0;
		}
		
		if(stroke!="" && stroke!="none"){
			parseStroke(&svgXml, obj, stroke, opacity);
		} else {
			obj->strokeColor = ofColor(0, 0, 0, 255);
		}

        // Vertexs
        obj->vertexs.push_back(ofPoint(x, y));
        obj->vertexs.push_back(ofPoint(x+width, y));
        obj->vertexs.push_back(ofPoint(x+width, y+height));
        obj->vertexs.push_back(ofPoint(x, y+height));

        layers[layers.size()-1].objects.push_back(obj);
    }
}

void ofxSVG::parseCircle(){
    string xStr     = svgXml.getAttribute("cx", currentIteration);
    string yStr     = svgXml.getAttribute("cy", currentIteration);
    float x         = (xStr!="") ? ofToFloat(xStr) : 0.0f;
    float y         = (yStr!="") ? ofToFloat(yStr) : 0.0f;
    float r     = ofToFloat(svgXml.getAttribute("r", currentIteration));

    string id       = svgXml.getAttribute("id", currentIteration);
    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;



    ofxSVGCircle* obj = new ofxSVGCircle;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Circle;
    obj->name        = id;
    obj->x           = x;
    obj->y           = y;
    obj->r           = r;

	if(fill!="none"){
		parseFill(&svgXml, obj, fill, opacity);
	} else {
		obj->fill = 0;
	}
	
	if(stroke!="" && stroke!="none"){
		parseStroke(&svgXml, obj, stroke, opacity);
	} else {
		obj->strokeColor = ofColor(0, 0, 0, 255);
	}

    int res = 30;
    float angle = 0.0f;
    float theta = M_TWO_PI / (float) res;
	for(int i = 0; i < res; i++){
		obj->vertexs.push_back(ofVec2f(x + cos(angle) * r, y + sin(angle) * r));
		angle+=theta;
	}

    layers[layers.size()-1].objects.push_back(obj);
}

//-------------------------------------------------------------------------------------

void ofxSVG::parseEllipse(){
    string xStr     = svgXml.getAttribute("cx", currentIteration);
    string yStr     = svgXml.getAttribute("cy", currentIteration);
    float x         = (xStr!="") ? ofToFloat(xStr) : 0.0f;
    float y         = (yStr!="") ? ofToFloat(yStr) : 0.0f;
    float rx     = ofToFloat(svgXml.getAttribute("rx", currentIteration))*2.0;
    float ry     = ofToFloat(svgXml.getAttribute("ry", currentIteration))*2.0;

    string id       = svgXml.getAttribute("id", currentIteration);
    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;


    ofxSVGEllipse* obj = new ofxSVGEllipse;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Ellipse;

    obj->name        = id;
    obj->x           = x;
    obj->y           = y;
    obj->rx          = rx;
    obj->ry          = ry;

	if(fill!="none"){
		parseFill(&svgXml, obj, fill, opacity);
	}
	
	if(stroke!="" && stroke!="none"){
		parseStroke(&svgXml, obj, stroke, opacity);
	}

    // Vertexs
    //--------------------------------

    int res = 300;
    float angle = 0.0f;
    float theta = M_TWO_PI / (float) res;
	for(int i = 0; i < res; i++){
		obj->vertexs.push_back(ofVec2f(x + cos(angle) * rx * 0.5f, y + sin(angle) * ry * 0.5f));
		angle+=theta;
	}

    layers[layers.size()-1].objects.push_back(obj);
}

//-------------------------------------------------------------------------------------

void ofxSVG::parseLine(){
    float x1     = ofToFloat(svgXml.getAttribute("x1", currentIteration));
    float y1     = ofToFloat(svgXml.getAttribute("y1", currentIteration));
    float x2     = ofToFloat(svgXml.getAttribute("x2", currentIteration));
    float y2     = ofToFloat(svgXml.getAttribute("y2", currentIteration));

    string id       = svgXml.getAttribute("id", currentIteration);

    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

    ofxSVGLine* obj = new ofxSVGLine;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Line;

    obj->name        = id;
    obj->x1          = x1;
    obj->y1          = y1;
    obj->x2          = x2;
    obj->y2          = y2;

	if(fill!="none"){
		parseFill(&svgXml, obj, fill, opacity);
	} else {
		obj->fill = 0;
	}
	
	if(stroke!="" && stroke!="none"){
		parseStroke(&svgXml, obj, stroke, opacity);
	} else {
		obj->strokeColor = ofColor(0, 0, 0, 255);
	}

    // Vertexs
    //--------------------------------
    obj->vertexs.push_back(ofPoint(x1, y1));
    obj->vertexs.push_back(ofPoint(x2, y2));

    layers[layers.size()-1].objects.push_back(obj);
}

//-------------------------------------------------------------------------------------

void ofxSVG::parsePolygon(){
    string id       = svgXml.getAttribute("id", currentIteration);
    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

    vector<string> pointsStrings = ofSplitString(svgXml.getAttribute("points", currentIteration), " ");

    ofxSVGPolygon* obj = new ofxSVGPolygon;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Polygon;
    obj->name        = id;

    // Vertexs
    //--------------------------------
    for(int i=0; i<pointsStrings.size(); i++){
        vector<string> pointString = ofSplitString(pointsStrings[i], ",");
        obj->vertexs.push_back(ofVec2f(strtod(pointString[0].c_str(), NULL), strtod(pointString[1].c_str(), NULL)));
    }

	
    beginRenderer();

	if(fill!="none"){
		parseFill(&svgXml, obj, fill, opacity);
	} else {
		obj->fill = 0;
	}
	
	if(stroke!="" && stroke!="none"){
		parseStroke(&svgXml, obj, stroke, opacity);
	} else {
		obj->strokeColor = ofColor(0, 0, 0, 255);
	}

    endRenderer();

    layers[layers.size()-1].objects.push_back(obj);
}

void ofxSVG::parseText(){

    svgXml.pushTag(currentIteration);
    int numTSpans = svgXml.getNumTags("tspan");
    svgXml.popTag();

    // Multiline Text
    //------------------------------------
    if(numTSpans>0){

        // Extract XY From matrix
        //------------------------------------
        ofPoint pos = posFromMatrix(svgXml.getAttribute("transform", currentIteration));

        // Get Alpha
        //------------------------------------
        string opacity = svgXml.getAttribute("opacity", currentIteration);
        float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

        svgXml.pushTag(currentIteration);

        // Parse and Load Fonts
        //--------------------------------
        for(int j=0; j<numTSpans; j++){
            int fontSize    = svgXml.getAttribute("tspan", "font-size", 0, j);
            string fontName = svgXml.getAttribute("tspan", "font-family", "", j);
            fontName        = fontName.substr(1, fontName.length() - 2);
            string fontExt  = ".ttf";


            // Check if Font is already loaded
            //------------------------------------
            if(fonts.count(fontName+ofToString(fontSize)) == 0){
                    ofTrueTypeFont* font = new ofTrueTypeFont();

                // Find Font Extension
                // and check if file exist
                //--------------------------------
                ifstream ttfFile(ofToDataPath("fonts/"+fontName+fontExt).c_str());
                if(!ttfFile) fontExt = ".otf";

                ifstream otfFile(ofToDataPath("fonts/"+fontName+fontExt).c_str());
                if(!otfFile && bVerbose)
                    printf("ofxSVG: Font file '%s' not found or format not supported\n", fontName.c_str());
                else otfFile.close();

                // Load font and add to font map
                //--------------------------------
					fonts[fontName+ofToString(fontSize)].loadFont("fonts/"+fontName+fontExt, fontSize*0.75f, true, true, true);

                //fonts.insert(make_pair(fontName+ofToString(fontSize), font));
            }
        }

        ofxSVGText* obj = new ofxSVGText;

        // Shape info
        //--------------------------------
        obj->type        = ofxSVGObject_Text;
    
        obj->name        = svgXml.getAttribute("id", currentIteration);

        // Display List
        //--------------------------------
		
        for(int j=0; j<numTSpans; j++){

            // Parse Current Text Metadata
            //------------------------------------
            float x         = svgXml.getAttribute("tspan", "x", 0.0f, j) + pos.x;
            float y         = svgXml.getAttribute("tspan", "y", 0.0f, j) + pos.y;

            string text     = svgXml.getValue("tspan", "", j);

            int fontSize    = svgXml.getAttribute("tspan", "font-size", 0, j);
            string fontName = svgXml.getAttribute("tspan", "font-family", "", j);
            fontName        = fontName.substr(1, fontName.length() - 2);

            obj->position = ofVec2f(x, y);
            obj->text = text;
            obj->fontName = fontName;
			obj->fontSize = fontSize;

            // Draw font
            //------------------------------------
            if(svgXml.attributeExists("tspan", "fill", j)){
                string col = svgXml.getAttribute("tspan", "fill", "", j);
                int color = strtol(("0x"+col.substr(1, col.length()-1)).c_str(), NULL, 0);
                float r = (color >> 16) & 0xFF;
                float g = (color >> 8) & 0xFF;
                float b = (color) & 0xFF;
				if(opacity!="") {
					ofEnableAlphaBlending();
				}
                ofColor c(r,g,b,alpha);
                obj->color = c;
            }
            else {
                ofColor c(0, 0, 0,alpha);
                obj->color = c;
            }

            fonts[fontName+ofToString(fontSize)].drawString(text, x, y);
			ofDisableAlphaBlending();
        }
        endRenderer();

        layers[layers.size()-1].objects.push_back(obj);

        svgXml.popTag();
    }
    // Single Line Text
    //------------------------------------
    else {

        // Extract XY From matrix
        //------------------------------------
        ofPoint pos = posFromMatrix(svgXml.getAttribute("transform", currentIteration));

        // Get Alpha
        //------------------------------------
        string opacity = svgXml.getAttribute("opacity", currentIteration);
        float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

        // Parse Current Text Metadata
        //------------------------------------
        string text     = svgXml.getValue(currentIteration);

        int fontSize    = strtol(svgXml.getAttribute("font-size", currentIteration).c_str(), NULL, 0);
        string fontName = svgXml.getAttribute("font-family", currentIteration);
        fontName        = fontName.substr(1, fontName.length() - 2);
        string col = svgXml.getAttribute("fill", currentIteration);
        string fontExt  = ".ttf";

        // Check if Font is already loaded
        //------------------------------------
        //if(fonts.count(fontName+ofToString(fontSize)) == 0){

		if(fonts.find(fontName+ofToString(fontSize)) == fonts.end()){
            // Find Font Extension
            // and check if file exist
            //--------------------------------
            ifstream ttfFile(ofToDataPath("fonts/"+fontName+fontExt).c_str());
            if(!ttfFile) fontExt = ".otf";

            ifstream otfFile(ofToDataPath("fonts/"+fontName+fontExt).c_str());
            if(!otfFile && bVerbose)
                printf("ofxSVG: Font file '%s' not found or format not supported\n", fontName.c_str());
            else otfFile.close();

            // Load font and add to font map
            //--------------------------------
                fonts[fontName+ofToString(fontSize)].loadFont("fonts/"+fontName+fontExt, fontSize*0.75f, true, true, true);
        }

        ofxSVGText* obj = new ofxSVGText;

        // Shape info
        //--------------------------------

        obj->type        = ofxSVGObject_Text;
    
        obj->name        = svgXml.getAttribute("id", currentIteration);

        // Draw font
        //------------------------------------
        if(col!="" && col!="none"){
            int color = strtol(("0x"+col.substr(1, col.length()-1)).c_str(), NULL, 0);
            float r = (color >> 16) & 0xFF;
            float g = (color >> 8) & 0xFF;
            float b = (color) & 0xFF;
            ofSetColor(r,g,b,alpha);
        }
        else ofSetColor(0,0,0,alpha);

        fonts[fontName+ofToString(fontSize)].drawString(text, pos.x, pos.y);

        layers[layers.size()-1].objects.push_back(obj);
    }

}

void ofxSVG::beginRenderer() 
{
	if( drawingMode == DRAW_FBO ) {
		fboForDrawing.begin();
	}
	
	if( drawingMode == DRAW_TEXTURE ) {
		texForDrawing.bind();
	}
}

void ofxSVG::endRenderer()
{
	if( drawingMode == DRAW_FBO ) {
		fboForDrawing.end();
	}
	
	if( drawingMode == DRAW_TEXTURE ) {
		texForDrawing.unbind();
	}
}

bool ofxSVG::isInsidePolygon(ofxSVGPath *path, ofPoint p)
{
	/* Based on code from:
	   http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
	   adapted to work with openframeworks/ofxSVG by Noto Yota multimedialab, 2010 */
	
	int counter = 0;
	int i,N;
	double xinters;
	
	vector<ofVec2f> points;
	
	vector<ofSubPath>::iterator it = path->path.getSubPaths().begin();
	vector<ofSubPath::Command>::iterator c;
	
	while (it != path->path.getSubPaths().end()) {
		
		c = it->getCommands().begin();
		
		while( c != it->getCommands().end() ) {
			points.push_back(c->to);
			++c;
		}
		
		++it;
	}


	ofVec2f p1,p2;

	p1 = points[0];
	N = points.size();
	
	for (i=1;i<=N;i++) {
		p2 = points[i % N];
		if (p.y > MIN(p1.y,p2.y)) {
			if (p.y <= MAX(p1.y,p2.y)) {
				if (p.x <= MAX(p1.x,p2.x)) {
					if (p1.y != p2.y) {
						xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || p.x <= xinters)
							counter++;
						}
					}
				}
			}
		p1 = p2;
		}
	
	if (counter % 2 == 0)
		return(false);
	else
	return(true);
}

void ofxSVG::parsePath() {
	string pathStr = svgXml.getAttribute("d", currentIteration);
	
	ofxSVGPath* obj = new ofxSVGPath();
	obj->type        = ofxSVGObject_Path;
	obj->name        = svgXml.getAttribute("id", currentIteration);
	
	ofxSVGPathParser parser(&obj->path);

	parser.parse(pathStr);
	
	string fill = svgXml.getAttribute("fill", currentIteration);
	string stroke = svgXml.getAttribute("stroke", currentIteration);
	string opacity = svgXml.getAttribute("opacity", currentIteration);
	float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;
	
	if(fill!="none"){
		parseFill(&svgXml, obj, fill, opacity);
	} else {
		obj->fill = 0;
	}
	
	if(stroke!="" && stroke!="none"){
		parseStroke(&svgXml, obj, stroke, opacity);
	} else {
		obj->strokeColor = ofColor(0, 0, 0, 255);
	}
	
	obj->render();
	
	layers[layers.size()-1].objects.push_back(obj);
	
}

ofPoint ofxSVG::posFromMatrix(string matrix){
    matrix = matrix.substr(7, matrix.length()-8);
    vector<string> matrixStrings = ofSplitString(matrix, " ");
    return ofVec2f(ofToFloat(matrixStrings[4]), ofToFloat(matrixStrings[5]));
}


float ofxSVG::rotFromMatrix(string matrix){
    int i = matrix.find("(");
    if(i!=-1){
        matrix = matrix.substr(i+1, matrix.length()-(i+2));
        vector<string> matrixStrings = ofSplitString(matrix, " ");
        return ofRadToDeg(atan2(ofToFloat(matrixStrings[3]),ofToFloat(matrixStrings[0])));
    }
    return 0.0f;
}

ofVec2f ofxSVG::scaleFromMatrix(string matrix) {
	matrix = matrix.substr(7, matrix.length()-8);
    vector<string> matrixStrings = ofSplitString(matrix, " ");
    return ofVec2f(ofToFloat(matrixStrings[1]), ofToFloat(matrixStrings[3]));	
}

float ofxSVG::scale(string scaleVal) {
    string floatVal = scaleVal.substr(scaleVal.find("("));
	floatVal = floatVal.substr(0, scaleVal.find(")"));
    return ofToFloat(floatVal);
}

//--------------------------------------------------------------
void ofxSVG::draw(){
    for(int i=0; i<layers.size(); i++){
        layers[i].draw();

    }
}
void ofxSVG::drawLayer(string layerName){
    for(int i=0; i<layers.size(); i++){
        if(layers[i].name == layerName){
            layers[i].draw();
            return;
        }
    }
}

void ofxSVG::drawLayer(int i){
    layers[i].draw();
}

//-------------------------------------------------------------------------
// begin save
//-------------------------------------------------------------------------


void ofxSVG::createRootSvg() {
	saveXml.addTag("svg");
	saveXml.addAttribute("svg", "xmlns", "http://www.w3.org/2000/svg", 0);
	saveXml.addAttribute("svg", "xmlns:xlink", "http://www.w3.org/1999/xlink", 0);
	saveXml.addAttribute("svg", "version", "1.1", 0);

}


void ofxSVG::addLayer(string layerName){

	if(saveXml.getValue("svg", "", 0) == "") {
		createRootSvg();
	}
	saveXml.pushTag("svg",0);
	saveXml.pushTag("g", 0);

}

void ofxSVG::saveToFile(string filename) {
	saveXml.saveFile(filename);
}

void ofxSVG::rect(float x, float y, float w, float h) {

	saveXml.pushTag("svg", 0);
	//saveXml.pushTag("rect", 0);// <rect
	saveXml.addTag("rect");
	saveXml.setAttribute("rect", "height", h, 0);
	saveXml.setAttribute("rect", "width", w, 0);
	saveXml.setAttribute("rect", "x", x, 0);
	saveXml.setAttribute("rect", "y", y, 0);
	saveXml.setAttribute("rect", "fill", currentAttributes["color"], 0);
	saveXml.setAttribute("rect", "stroke", currentAttributes["stroke"], 0);
	saveXml.setAttribute("rect", "stroke-width", currentAttributes["strokewidth"], 0);
	saveXml.popTag();

}
void ofxSVG::ellipse(float x, float y, float rx, float ry) {

	saveXml.addTag("ellipse");
	saveXml.setAttribute("ellipse", "ry", ry/2.0, 0);
	saveXml.setAttribute("ellipse", "rx", rx/2.0, 0);
	saveXml.setAttribute("ellipse", "cx", x, 0);
	saveXml.setAttribute("ellipse", "cy", y, 0);
	saveXml.setAttribute("ellipse", "fill", currentAttributes["color"], 0);
	saveXml.setAttribute("ellipse", "stroke", currentAttributes["stroke"], 0);
	saveXml.setAttribute("ellipse", "stroke-width", currentAttributes["strokewidth"], 0);
	saveXml.popTag();

}
void ofxSVG::circle(float x, float y, float r) {


	// need a way to get the current color, stroke settings...hmm.
	// probably something like: current_vals;
	saveXml.pushTag("svg", 0);
	saveXml.addTag("circle");
	saveXml.setAttribute("circle", "r", r/2.0, 0);
	saveXml.setAttribute("circle", "cx", x, 0);
	saveXml.setAttribute("circle", "cy", y, 0);
	saveXml.setAttribute("circle", "fill", currentAttributes["color"], 0);
	saveXml.setAttribute("circle", "stroke", currentAttributes["stroke"], 0);
	saveXml.setAttribute("circle", "stroke-width", currentAttributes["strokewidth"], 0);
	saveXml.popTag();
}



void ofxSVG::beginPolygon(){
	currentAttributes["drawingpolygon"] = "true";
	
	saveXml.pushTag("svg", 0);
	saveXml.addTag("polygon");
	saveXml.setAttribute("polygon", "fill", currentAttributes["color"], 0);
	saveXml.setAttribute("polygon", "stroke", currentAttributes["stroke"], 0);
	saveXml.setAttribute("polygon", "stroke-width", currentAttributes["strokewidth"], 0);

}
void ofxSVG::endPolygon(){
	currentAttributes["drawingpolygon"] = "false";
}

void ofxSVG::beginPath() {

	currentAttributes["drawingpath"] = "true";
	saveXml.pushTag("svg", 0);
	saveXml.addTag("path");
	saveXml.setAttribute("path", "fill", currentAttributes["color"], 0);
	saveXml.setAttribute("path", "stroke", currentAttributes["stroke"], 0);
	saveXml.setAttribute("path", "stroke-width", currentAttributes["strokewidth"], 0);
	
}
void ofxSVG::endPath() {
	currentAttributes["drawingpath"] = "false";
	
	string currentString;
	
	saveXml.pushTag("svg");
	stringstream s;
	currentString = saveXml.getAttribute("path", "d", "");
	if(currentString.length() > 1) { // i.e. has a path already been started?
		
		currentString += "Z";		
		saveXml.setAttribute("path", "d", s.str(), currentSaveNode);
	
	}
}

void ofxSVG::vertex(float x, float y){

	string currentPath;
	if(currentAttributes["drawingpath"] != "" && currentAttributes["drawingpath"] != "false") {
		saveXml.pushTag("svg", 0);
		currentPath = saveXml.getAttribute("path", "points", "", 0);
		stringstream s;
		s << x << "," << y << " ";
		currentPath+=s.str();
		saveXml.setAttribute("path", "points", currentPath, 0);
	}

	if(currentAttributes["drawingpolygon"] != "" && currentAttributes["drawingpolygon"] != "false") {
		saveXml.pushTag("svg", 0);
		currentPath = saveXml.getAttribute("polygon", "points", "", 0);
		stringstream s;
		s << x << "," << y << " ";
		currentPath+=s.str();
		saveXml.setAttribute("polygon", "points", currentPath, 0);
	}
}

// this is going to be a tough one
// because we'll have to calculate the next
// point
void ofxSVG::bezierVertex(float x0, float y0, float x1, float y1, float x2, float y2) {

	//
	string currentString;
	if(currentAttributes["drawingpath"] != "" && currentAttributes["drawingpath"] != "false") {
		saveXml.pushTag("svg");
		stringstream s;
		currentString = saveXml.getAttribute("path", "d", "");
		if(currentString.length() > 1) { // i.e. has a path already been started?

			s << " Q "<< x0 << " "<< x0 << " "<< y0 << " "<< x1 << " "<< y1 << " "<< x2 << " "<< y2 << " ";

		} else {
			s << " M "<< x0 << " "<< x0 << " "<< y0 << " "<< x1 << " "<< y1 << " "<< x2 << " "<< y2 << " ";

		}

		saveXml.setAttribute("path", "d", s.str(), currentSaveNode);
		currentAttributes["drawingpath"] = s.str();
	}
}
void ofxSVG::stroke(string colorHex, int weight) {
	currentAttributes["stroke"] = colorHex;

	stringstream s;
	s << weight;
	currentAttributes["strokewidth"] = s.str();


}

void ofxSVG::fill(string colorHex) {

	// figure out if it has an alpha
	int rgb = strtol(("0x"+colorHex.substr(1, colorHex.length()-1)).c_str(), NULL, 0);
	float r = (rgb >> 16) & 0xFF;
	float g = (rgb >> 8) & 0xFF;
	float b = (rgb) & 0xFF;
	
	currentAttributes["color"] = colorHex;
}

void ofxSVG::noFill() {

	currentAttributes["color"] = "";
}
void ofxSVG::noStroke(){
	currentAttributes["stroke"] = "";
}

void ofxSVG::setOpacity(float percent) {

	currentAttributes["opacity"] = "percent";
}

void ofxSVG::translate(float tx, float ty) {
	
	if(currentAttributes["matrix"] != "") {
		ofMatrix3x3 m = matrices[matrices.size() - 1];
		m.c += tx;
		m.f += ty;
		string s;
		ofxSVGUtils::stringFromMatrix(s, matrices[matrices.size() - 1]);
		currentAttributes["matrix"] = s;
	}
}
void ofxSVG::rotate(float r) {

	if(currentAttributes["matrix"] != "") {
		ofMatrix3x3 m = matrices[matrices.size() - 1];
		m.a += cos(r);
		m.b += -sin(r);
		m.d += sin(r);
		m.e += cos(r);
		matrices.push_back(m);
	}
}

void ofxSVG::pushMatrix() {
	// how to get this to work?
	// one option is a vector of transform matrices that are just summed up
	// another I suppose would be to see if SVG supports using multiple
	// matrices, but that 
	if(currentAttributes["matrix"] != "") {
		
		ofMatrix3x3 m;		
		m = matrices[matrices.size() - 1]; // build on the old one
		matrices.push_back(m);// this just copies the matrix over but it should be fine
	}
	
}
void ofxSVG::popMatrix() {
	if(currentAttributes["matrix"] != "") {
		
		matrices.pop_back();
		if(matrices.size() == 0) {
			currentAttributes["matrix"] = "";
		} else {
			string s;
			ofxSVGUtils::stringFromMatrix(s, matrices[matrices.size() - 1]);
			currentAttributes["matrix"] = s;

		}
	}
	
}

string ofxSVG::createAttribute(string element, ...) { // va_args ftw!

	int i, numberArgs;
	va_list vl;
	va_start(vl,numberArgs);
	va_arg(vl,int);
	for(i = 0; i<numberArgs; i++) {
		
		
	}
}

void ofxSVG::parseStroke(ofxSVGXml *svgXml, ofxSVGObject *obj, string stroke, string opacity) {
	string strokeWeight = svgXml->getAttribute("stroke-width", currentIteration);
	
	if(strokeWeight!="") 
		obj->strokeWeight = (ofToInt(strokeWeight));
	
	ofNoFill();
	if(opacity!="") {
		obj->opacity = atof(opacity.c_str());
	}
	
	int rgb = strtol(("0x"+stroke.substr(1, stroke.length()-1)).c_str(), NULL, 0);
	float r = (rgb >> 16) & 0xFF;
	float g = (rgb >> 8) & 0xFF;
	float b = (rgb) & 0xFF;
	ofColor strokec(r,g,b,obj->opacity);
	obj->strokeColor = strokec;
}

void ofxSVG::parseFill(ofxSVGXml *svgXml, ofxSVGObject *obj, string fill, string opacity) {
	if(opacity!="")
		obj->opacity = atof(opacity.c_str());
	
	if(fill!=""){
		int rgb = strtol(("0x"+fill.substr(1, fill.length()-1)).c_str(), NULL, 0);
		float r = (rgb >> 16) & 0xFF;
		float g = (rgb >> 8) & 0xFF;
		float b = (rgb) & 0xFF;
		ofColor fillc(r,g,b,obj->opacity);
		obj->fill = 1;
		obj->fillColor = fillc;
	}
}

