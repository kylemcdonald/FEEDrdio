#include "faceTrackerManager.h"

using namespace ofxCv;
using namespace cv;

void faceTrackerManager::setup() {
	ofxXmlSettings xml;
	xml.loadFile("settings.xml");
	xml.pushTag("camera");
	camWidth = xml.getValue("width", 640);
	camHeight = xml.getValue("height", 480);
	xml.popTag();
	cam.initGrabber(camWidth, camHeight);
	
	tracker.setRescale(.5);
	tracker.setup();
	enabled = true;
	
	graphs.resize(12);
	graphs[0].setup("mouth width", 1, 1.65);
	graphs[1].setup("mouth + jaw", 2, 1.08);
	graphs[2].setup("eyebrows", 3, 1.22);
	graphs[3].setup("eye openness", 4, 0.48);
	graphs[4].setup("x rotation", 5, 0.04);
	graphs[5].setup("y rotation", 6, 0.04).setBidirectional(true);
	graphs[6].setup("z rotation", 7, 0.03).setBidirectional(true);
	graphs[7].setup("x position", 8, 8.40).setBidirectional(true);
	graphs[8].setup("y position", 9, 8.40).setBidirectional(true);
	graphs[9].setup("scale", 10, 0.10);
	graphs[10].setup("activity", 11).setMinMaxRange(1, 2);
	graphs[11].setup("presence", 12, 0).setMinMaxRange(0, 1);
	
	graphs[10].setSmoothing(.95, .95); // smooth activity
	graphs[11].setSmoothing(.99, .5); // smooth presence
	
	keyPressed('l');
}

void faceTrackerManager::update() {
	cam.update();
	if(enabled && cam.isFrameNew()) {
		if(tracker.update(toCv(cam))) {	
			classifier.classify(tracker);
			
			position = tracker.getPosition();
			scale = tracker.getScale();
			orientation = tracker.getOrientation();
			
			graphs[0].addSample(tracker.getGesture(ofxFaceTracker::MOUTH_WIDTH));
			graphs[1].addSample(tracker.getGesture(ofxFaceTracker::MOUTH_HEIGHT) + tracker.getGesture(ofxFaceTracker::JAW_OPENNESS));
			graphs[2].addSample(tracker.getGesture(ofxFaceTracker::LEFT_EYEBROW_HEIGHT) + tracker.getGesture(ofxFaceTracker::RIGHT_EYEBROW_HEIGHT));
			graphs[3].addSample(-tracker.getGesture(ofxFaceTracker::LEFT_EYE_OPENNESS) + -tracker.getGesture(ofxFaceTracker::RIGHT_EYE_OPENNESS));
			graphs[4].addSample(orientation.x);
			graphs[5].addSample(orientation.y);
			graphs[6].addSample(orientation.z);
			graphs[7].addSample(position.x);
			graphs[8].addSample(position.x);
			graphs[9].addSample(scale);
			
			float activity = 0;
			for(int i = 0; i < 10; i++) {
				activity += graphs[i].getActivity();
			}
			graphs[10].addSample(activity);
			
			for(int i = 0; i < classifier.size(); i++) {
				expressionGraphs[i].addSample(classifier.getProbability(i));
			}
			
			graphs[11].addSample(1);
		} else {
			for(int i = 0; i < graphs.size(); i++) {
				graphs[i].clear();
			}
			for(int i = 0; i < expressionGraphs.size(); i++) {
				expressionGraphs[i].clear();
			}
			graphs[10].addSample(0);
			graphs[11].addSample(0);
		}
	}
	
	if (tracker.getFound()){
		FA.calculate(tracker);
	}
}

void faceTrackerManager::draw() {
	ofSetColor(enabled ? 255 : cyanPrint);
	cam.draw(0, 0);
	ofSetColor(255);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
	
	ofPushMatrix();
	ofTranslate(5, 5);
	for(int i = 0; i < graphs.size(); i++) {
		graphs[i].draw(0, i * 34);
	}
	ofPopMatrix();
	
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(128 + 10, 5);
	for(int i = 0; i < expressionGraphs.size(); i++) {
		expressionGraphs[i].draw(0, i * 34);
	}
	ofPopMatrix();
	ofPopStyle();
	
	/*
	drawHighlightString(string() +
											"tab - pause input\n" +
											"r - reset expressions\n" +
											"e - add expression\n" +
											"s - add sample\n" +
											"s - save expressions\n" +
											"l - load expressions\n" +
											"c - send selected control change\n" +
											"n - send selected note\n",
											14, ofGetHeight() - 12 * 12);
											*/
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), ofGetWidth() - 20, ofGetHeight() - 10);
	
	if(tracker.getFound()) {
		tracker.draw();
	} else {
		if(sin(ofGetElapsedTimef() * 12) > 0) {
			ofDrawBitmapStringHighlight("no face", cam.getWidth() / 2 - 50, cam.getHeight() / 2);
		}
	}
}

void faceTrackerManager::keyPressed(int key) {
	if(key == '\t') {
		enabled = !enabled;
	}
	if(key == 'r') {
		classifier.reset();
		expressionGraphs.clear();
	}
	if(key == 'e') {
		classifier.addExpression();
		Graph graph;
		graph.setName(classifier.getDescription(classifier.size() - 1));
		graph.setMidiNote(64 + expressionGraphs.size());
		expressionGraphs.push_back(graph);
	}
	if(key == 's') {
		classifier.addSample(tracker);
	}
	if(key == 's') {
		classifier.save("expressions");
	}
	if(key == 'l') {
		classifier.setSigma(100);
		classifier.load("expressions");
		for(int i = 0; i < classifier.size(); i++) {
			Graph graph;
			graph.setName(classifier.getDescription(i));
			graph.setMidiNote(64 + expressionGraphs.size());
			expressionGraphs.push_back(graph);
		}
	}
	for(int i = 0; i < graphs.size(); i++) {
		graphs[i].keyPressed(key);
	}
	for(int i = 0; i < expressionGraphs.size(); i++) {
		expressionGraphs[i].keyPressed(key);
	}
}
void faceTrackerManager::mouseMoved(int x, int y) {
	for(int i = 0; i < graphs.size(); i++) {
		graphs[i].mouseMoved(x, y);
	}
	for(int i = 0; i < expressionGraphs.size(); i++) {
		expressionGraphs[i].mouseMoved(x, y);
	}
}