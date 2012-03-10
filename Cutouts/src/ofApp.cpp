#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
}

void ofApp::update() {
	if(ofGetFrameNum() % 20 == 0) {
		inner = PebbleMaker::generate();
	}
}

void ofApp::draw() {
	ofBackground(0);
	
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	ofScale(ofGetWidth() / 4, ofGetHeight() / 4);
	
	ofRotate(mouseX);
	
	inner.draw();
	
	if(ofGetMousePressed()) {
		ofNoFill();
		ofSetColor(ofColor::blue);
		inner.getTessellation().drawWireframe();
	}
}