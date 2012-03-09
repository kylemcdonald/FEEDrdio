#include "ofApp.h"

void ofApp::setup() {	
	//ofSetVerticalSync(true);
	//ofSetFrameRate(60);
	
	ofSetWindowTitle("FaceMusic");
	
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    FTM.setup();
    AM.FA = &FTM.FA;
    AM.FTM = &FTM;
    AM.setup();

}

void ofApp::update() {
	FTM.update();
	AM.update();
}

void ofApp::draw() {	

	FTM.draw();	
	// draw projection
	ofTranslate(FTM.width, 0);
	AM.draw();
}

void ofApp::keyPressed(int key) {
	FTM.keyPressed(key);
	if(key == 'f') {
		ofToggleFullscreen();
	}
}
void ofApp::mouseMoved(int x, int y) {
	FTM.mouseMoved(x,y);
}