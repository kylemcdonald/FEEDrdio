#include "ofApp.h"

void ofApp::setup() {	
	//ofSetVerticalSync(true);
	//ofSetFrameRate(60);
	
	ofSetWindowTitle("FaceMusic");
	
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

void ofApp::loadSettings() {
	ofxXmlSettings xml;
	xml.loadFile("settings.xml");
	fullscreen = xml.getValue("fullscreen", false);
	
	side = xml.getValue("side", 0);
	
	xml.pushTag("projector");
	projectorWidth = xml.getValue("width", 640);
	projectorHeight = xml.getValue("height", 480);
	xml.popTag();
	
	xml.pushTag("screen");
	screenWidth = xml.getValue("width", 640);
	screenHeight = xml.getValue("height", 480);
	xml.popTag();
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