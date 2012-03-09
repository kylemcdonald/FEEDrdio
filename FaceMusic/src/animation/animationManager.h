#pragma once

#include <iostream>
#include "ofMain.h"

#include "faceAnalyzer.h"
#include "ofxBox2d.h"
#include "ofxBox2dConvexPoly.h"
#include "faceTrackerManager.h"

typedef struct {
  
    float scaleAdder;
    float angleAdder;
    ofPoint offsetAdder;
    
} transformAdd;


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
    
		ofVec2f getAttractor();
		
		int width, height;
		int side;
    
    faceTrackerManager  * FTM;
    faceAnalyzer * FA;
    float faceLockTarget;
    float faceLock;
    
    //---------------------------------------------- image and masks
    vector < vector < ofImage * > > faceImages;
    vector < ofPolyline > maskPolys;
    vector < int > which;
    void drawImageWithInfo(ofImage * temp, faceFeatureAnalysis & ft, ofxBox2dConvexPoly & poly, ofPoint offset, float scaler, bool bFlip, float angleAdd);
    
    //------------------------------------------------ box 2d
    ofxBox2d box2d;
    vector <ofxBox2dConvexPoly >		polys;
    vector <ofxBox2dCircle >            circles;

    
    
    //------------------------------------------------ box 2d
    int chin, forehead, lear, rear, reye, leye, nose, mouth;
    vector < ofPoint >  offsets;
    
    vector < transformAdd > transformAdds;
    
    ofPolyline temp;
    
    float presence;
    float lastNonPresenceTime;
    bool bWasPresentLastFrame; 
    float fistNonPresenceTime;
    
    ofImage pebbles[16];
    
    
    
    
    
};