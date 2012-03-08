//
//  faceTrackerManager.cpp
//  FaceMusic
//
//  Created by molmol on 3/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once


#include <iostream>
#include "ofxCv.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxXmlSettings.h"
#include "Graph.h"
#include "faceAnalyzer.h"

class faceTrackerManager {
    
    public:   
    
    void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void mouseMoved(int x, int y);
    
    
        vector<Graph> graphs, expressionGraphs;
        
    int camWidth, camHeight;
        bool enabled;
        ofVideoGrabber cam;
        ofxFaceTrackerThreaded tracker;
        ExpressionClassifier classifier;
        
        ofVec2f position;
        float scale;
        ofVec3f orientation;
        
        cv::Mat translation, rotation;
        ofMatrix4x4 pose;
    
        faceAnalyzer FA;
    
};