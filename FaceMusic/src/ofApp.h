#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxXmlSettings.h"
#include "Graph.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void mouseMoved(int x, int y);
	
	vector<Graph> graphs, expressionGraphs;
	
	bool enabled;
	ofVideoGrabber cam;
	ofxFaceTrackerThreaded tracker;
	ExpressionClassifier classifier;
	
	ofVec2f position;
	float scale;
	ofVec3f orientation;
	
	cv::Mat translation, rotation;
	ofMatrix4x4 pose;
};
