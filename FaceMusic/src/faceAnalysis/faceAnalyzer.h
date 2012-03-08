#pragma once

#include "ofxFaceTracker.h"




typedef struct faceFeatureAnalysis{
    
    float angle;
    ofPoint pos;
    float dist;
    bool bHorizDistance;
};


class faceAnalyzer {
    
public: 
    
    int getId(string facePart){
        return faceLocation[facePart];
    }
    string getFacePartName(int id){
        return featureNames[id];
    }
    
    faceAnalyzer(){
        
        nFeatures = 8;
        
        features[0] = &lEye;
        features[1] = &rEye;
        features[2] = &nose;
        features[3] = &mouth;
        features[4] = &chin;
        features[5] = &forehead;
        features[6] = &lEar;
        features[7] = &rEar;
        
        featureNames[0] = "leye";
        featureNames[1] = "reye";
        featureNames[2] = "nose";
        featureNames[3] = "mouth";
        featureNames[4] = "chin";
        featureNames[5] = "forehead";
        featureNames[6] = "lear";
        featureNames[7] = "rear";
        
        for (int i = 0; i < 8; i++){
            faceLocation[featureNames[i]] = i;
        }
    
    }
    
    int nFeatures;
    
    string featureNames[8];
    
    faceFeatureAnalysis *  features[8];
    
    faceFeatureAnalysis     lEye;
    faceFeatureAnalysis     rEye;
    faceFeatureAnalysis     nose;
    faceFeatureAnalysis     mouth;
    faceFeatureAnalysis     chin;
    faceFeatureAnalysis     forehead;
    faceFeatureAnalysis     lEar;
    faceFeatureAnalysis     rEar;
    
    map < string, int > faceLocation;
    
    
    void calculate(ofxFaceTracker & ft);
    
    void drawFeature(faceFeatureAnalysis & ft);
    void draw();
    

};
