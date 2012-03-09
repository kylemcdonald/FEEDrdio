
#include "faceAnalyzer.h"




static bool sortHoriz(ofPoint a, ofPoint b){
    return a.x > b.x;
}

static bool sortVert(ofPoint a, ofPoint b){
    return a.y > b.y;
}

ofPoint midPoint(ofPolyline input){
	ofPoint midPt;
	midPt.set(0,0,0);
	for (int i = 0; i < input.getVertices().size(); i++){
		midPt += (input.getVertices()[i]);
	}
	midPt /= MAX(1,input.getVertices().size());
	return midPt;
}

void addToPolyline(ofPolyline & a, ofPolyline & b){
    for (int i = 0; i < b.getVertices().size(); i++){
        a.getVertices().push_back(b.getVertices()[i]);
    }
}
  
void offsetPolyline(ofPolyline & a, ofPoint offset){
    for (int i = 0; i < a.getVertices().size(); i++){
        a.getVertices()[i] += offset;
    }

}



void calculateSizeAngle(ofPolyline & pos, faceFeatureAnalysis & ft, bool bUseHoriz, bool bDontSort = false){
    
    vector < ofPoint > pts = pos.getVertices();
    
    if (bDontSort == false){
    if (bUseHoriz == true){
        sort(pts.begin(), pts.end(), sortHoriz);
    } else {
        sort(pts.begin(), pts.end(), sortVert);
    }
    }
    
    float dist = (pts[0] - pts[pts.size()-1]).length();
    float angle = atan2(pts[0].y - pts[pts.size()-1].y, pts[0].x - pts[pts.size()-1].x);
    if (bUseHoriz == false){
        angle += PI/2;  // it's flipped around. 
    }
    ft.bHorizDistance = bUseHoriz;
    ft.angle = angle;
    ft.dist = dist;
    
}

void faceAnalyzer::calculate(ofxFaceTracker & tracker){
    

    
    ofPolyline leyePolyline = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
	ofPolyline reyePolyline = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
    ofPolyline nosePolyline = tracker.getImageFeature(ofxFaceTracker::NOSE_BRIDGE);
    
    ofPolyline noseBase = tracker.getImageFeature(ofxFaceTracker::NOSE_BASE);
    ofPoint midPtBase = midPoint(noseBase);
    ofPoint topOfLip = tracker.getImagePoint(51);
    nosePolyline.getVertices().push_back((midPtBase + topOfLip) / 2);
    
    ofPolyline mouthPolyline = tracker.getImageFeature(ofxFaceTracker::INNER_MOUTH);
    ofPolyline chinPolyline = tracker.getImageFeature(ofxFaceTracker::JAW);

    
    ofPolyline lbrow = tracker.getImageFeature(ofxFaceTracker::LEFT_EYEBROW);
    ofPolyline rbrow =  tracker.getImageFeature(ofxFaceTracker::RIGHT_EYEBROW);
    ofPolyline forheadPolyline;
    addToPolyline(forheadPolyline,lbrow );
    addToPolyline(forheadPolyline,rbrow );
    ofPoint topOfNose =  tracker.getImagePoint(27);
    offsetPolyline(lbrow,  (topOfNose-  (midPtBase + topOfLip) / 2));
    offsetPolyline(rbrow,  (topOfNose-  (midPtBase + topOfLip) / 2));
    addToPolyline(forheadPolyline,lbrow );
    addToPolyline(forheadPolyline,rbrow );
    
    
    ofPolyline leftEar;
    leftEar.addVertex(tracker.getImagePoint(0) - (tracker.getImagePoint(2)-tracker.getImagePoint(1)));
    leftEar.addVertex(tracker.getImagePoint(0));
    leftEar.addVertex(tracker.getImagePoint(1));
    leftEar.addVertex(tracker.getImagePoint(2));
    
    ofPolyline rightEar;
    rightEar.addVertex(tracker.getImagePoint(14));
    rightEar.addVertex(tracker.getImagePoint(15));
    rightEar.addVertex(tracker.getImagePoint(16));
    rightEar.addVertex(tracker.getImagePoint(16) + (tracker.getImagePoint(15) - tracker.getImagePoint(14)));
    
    
    lEye.pos = midPoint(leyePolyline);
    rEye.pos = midPoint(reyePolyline);
    nose.pos = midPoint(nosePolyline);
    mouth.pos  = midPoint(mouthPolyline);
    chin.pos = midPoint(chinPolyline);
    forehead.pos = midPoint(forheadPolyline);
    lEar.pos = midPoint(leftEar);
    rEar.pos = midPoint(rightEar);
    
    calculateSizeAngle(leyePolyline, lEye, true);
    calculateSizeAngle(reyePolyline, rEye, true);
    calculateSizeAngle(nosePolyline, nose, false);
    calculateSizeAngle(mouthPolyline, mouth, true);
    calculateSizeAngle(chinPolyline, chin, false, true);
    calculateSizeAngle(forheadPolyline, forehead, false, true);
    
    calculateSizeAngle(leftEar, lEar, false, true);
    calculateSizeAngle(rightEar, rEar, false, true);
    
    
}

void faceAnalyzer::drawFeature(faceFeatureAnalysis & ft){
    
    ofSetColor(255,0,0,100);
    ofCircle(ft.pos, ft.dist/2);
    ofSetColor(255,255,255,100);
    ofCircle(ft.pos, ft.dist/2);
    
    ofPushMatrix();
    ofTranslate(ft.pos.x, ft.pos.y);
    ofRotateZ(ft.angle*RAD_TO_DEG);
    ofLine(0,0,ft.dist/2, 0);
    ofPopMatrix();
}

void faceAnalyzer::draw(){
    
    drawFeature(lEye);
    drawFeature(rEye);
    drawFeature(nose);
    drawFeature(mouth);
    drawFeature(chin);
    drawFeature(forehead);
    drawFeature(lEar);
    drawFeature(rEar);
}




