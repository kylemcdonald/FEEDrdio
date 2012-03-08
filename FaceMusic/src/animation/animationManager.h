//
//  faceTrackerManager.cpp
//  FaceMusic
//
//  Created by molmol on 3/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <iostream>
#include "ofMain.h"

#include "faceAnalyzer.h"
#include "ofxBox2d.h"
#include "ofxBox2dConvexPoly.h"

class animationManager {
    
    public:   
    
    void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void loadImageSet(vector < ofImage * > & imgs, string partName, ofPolyline & maskShape);
    
    
    faceAnalyzer * FA;
    
    vector < vector < ofImage * > > faceImages;
    vector < ofPolyline > maskPolys;
    vector < int > which;
    void drawImageWithInfo(ofImage * temp, faceFeatureAnalysis & ft, ofxBox2dConvexPoly & poly, ofPoint offset, float scaler, bool bFlip, float angleAdd);
    //ofxBlur blur;
    //vector <particle *> myParticles;
    ofxBox2d box2d;
    int chin, forehead, lear, rear, reye, leye, nose, mouth;
    vector < ofPoint >     offsets;
    vector <ofxBox2dConvexPoly >		polys;
    ofPolyline temp;
    
    
};