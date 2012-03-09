#include "ofApp.h"

void ofApp::setup() {	
	//ofSetVerticalSync(true);
	//ofSetFrameRate(60);
	
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    FTM.setup();
    ofSetWindowShape(FTM.camWidth * 2, FTM.camHeight);
    AM.FA = &FTM.FA;
    AM.FTM = &FTM;
    AM.setup();

}

void ofApp::update() {
	FTM.update();
	AM.update();
}

void ofApp::draw() {
	ofPushMatrix();
	ofTranslate(FTM.cam.getWidth(), 0);
	AM.draw();
	ofPopMatrix();
	FTM.draw();
}

void ofApp::keyPressed(int key) {
	FTM.keyPressed(key);
	
}
void ofApp::mouseMoved(int x, int y) {
	FTM.mouseMoved(x,y);
}