#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
}

void ofApp::update() {
	if(ofGetFrameNum() % 20 == 0) {
		generate();
	}
}

void ofApp::draw() {
	ofBackground(0);
	
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	ofScale(ofGetWidth() / 4, ofGetHeight() / 4);
	
	ofRotate(mouseX);
	
	outer.draw();
	inner.draw();
	
	if(ofGetMousePressed()) {
		ofNoFill();
		ofSetColor(ofColor::red);
		base.draw();
	}
}

void ofApp::generate() {
	// settings
	float minSize = .8, maxSize = 1.1;
	int minVertices = 6, maxVertices = 14;
	float maxEllipticality = 30;
	float maxRotation = 45;
	float baseDisplacement = .1;
	int minSubdivisionSteps = 2, maxSubdivisionSteps = 10;
	float minOuterWidth = .01, minInnerWidth = .01;
	float maxOuterWidth = .05, maxInnerWidth = .1;
	
	int n = ofRandom(minVertices, maxVertices);
	float size = ofRandom(minSize, maxSize);
	float ellipticality = ofRandom(-maxEllipticality, maxEllipticality);
	ofVec2f ellipseScaling(1, 1);
	ellipseScaling.rotate(ellipticality);
	float globalRotation = ofRandom(-maxRotation, maxRotation);
	base.clear();
	for(int i = 0; i < n; i++) {
		ofVec2f cur(size, 0);
		cur.rotate(ofMap(i, 0, n, 0, 360));
		cur *= ellipseScaling;
		cur.rotate(globalRotation);
		cur += ofVec2f(ofRandomf(), ofRandomf()) * baseDisplacement;
		base.addVertex(cur);
	}
	base.close();
	
	inner.clear();
	outer.clear();
	for(int i = 0; i < n; i++) {
		int subdivisionSteps = ofRandom(minSubdivisionSteps, maxSubdivisionSteps);
		ofVec2f start = base[i];
		ofVec2f end = base[(i + 1) % n];
		ofVec2f normal = (end - start).rotate(-90);
		float outerRandomWalk = 0, innerRandomWalk = 0;
		for(int j = 0; j < subdivisionSteps; j++) {
			ofVec2f interpolated = start.getInterpolated(end, (float) j / subdivisionSteps);
			outerRandomWalk += ofRandomf();
			innerRandomWalk += ofRandomf();
			ofVec2f innerInterpolated = interpolated + normal * (minInnerWidth + -abs(innerRandomWalk) * maxInnerWidth);
			ofVec2f outerInterpolated = innerInterpolated + normal * (minOuterWidth + +abs(outerRandomWalk) * maxOuterWidth);
			outer.lineTo(outerInterpolated);
			inner.lineTo(innerInterpolated);
		}
	}
	inner.close();
	inner.setColor(ofColor::white);
	outer.close();
	outer.setColor(ofColor::gray);
}