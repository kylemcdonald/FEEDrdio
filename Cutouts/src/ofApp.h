#pragma once

#include "ofMain.h"
//#include "ofxGui.h"
#include "PebbleMaker.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	//ofxPanel gui;
	ofPath inner;
};
