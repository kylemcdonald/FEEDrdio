#include "Graph.h"

#include "ofxMidi.h"

GLdouble modelviewMatrix[16], projectionMatrix[16];
GLint viewport[4];
inline void updateProjectionState() {
	glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);
}

inline ofVec3f worldToScreen(ofVec3f world) {
	updateProjectionState();
	GLdouble x, y, z;
	gluProject(world.x, world.y, world.z, modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
	ofVec3f screen(x, y, z);
	screen.y = ofGetHeight() - screen.y;
	return screen;
}

inline void glMap(ofVec2f minInput, ofVec2f maxInput, ofVec2f minOutput, ofVec2f maxOutput) {
	ofVec2f inputRange = maxInput - minInput, outputRange = maxOutput - minOutput;
	ofTranslate(minOutput.x, minOutput.y);
	ofScale(outputRange.x, outputRange.y);
	ofScale(1. / inputRange.x, 1. / inputRange.y);
	ofTranslate(-minInput.x, -minInput.y);
}

bool ready = false;
ofxMidiOut midi;
ofTrueTypeFont font;
void setupResources() {
	if(!ready) {
		midi.openVirtualPort();
		font.loadFont("uni05_53.ttf", 6, false);
		ready = true;
	}
}

void drawString(string text, int x, int y) {
	ofPushStyle();
	ofSetColor(0);
	ofFill();
	ofRectangle box = font.getStringBoundingBox(text, x, y);
	box.x -= 1;
	box.y -= 1;
	box.width += 2;
	box.height += 2;
	ofRect(box);
	ofSetColor(255);
	font.drawString(text, x, y);
	ofPopStyle();
}

Graph::Graph()
:lastTrigger(0)
,triggered(false)
,threshold(0)
,percentile(.98)
,smoothing(.999)
,activity(0)
,activitySmoothing(.99)
,midiNote(0)
,hoverState(false)
,minRange(0)
,maxRange(0) {
	setupResources();
	setSize(128, 32);
}

void Graph::setup(string name, int midiNote, float threshold) {
	setName(name);
	if(midiNote > 0) {
		setMidiNote(midiNote);
	}
	if(threshold > 0) {
		setThreshold(threshold);
	}
}
	
void Graph::setSize(int width, int height) {
	this->width = width;
	this->height = height;
	buffer.setMaxSize(width);
	derivative.setMaxSize(width);
}

void Graph::setMinMaxRange(float minRange, float maxRange) {
	this->minRange = minRange;
	this->maxRange = maxRange;
}

void Graph::setThreshold(float threshold) {
	this->threshold = threshold;
}

void Graph::addSample(float sample) {
	if(!buffer.empty()) {
		float diff = sample - buffer.back();
		if(!derivative.empty() && derivative.back() < threshold && diff > threshold) {
			lastTrigger = ofGetElapsedTimef();
			triggered = true;
		} else {
			triggered = false;
		}
		derivative.push_back(abs(diff));
	}
	buffer.push_back(sample);
	// could use a better datastructure to avoid resorting every sample
	float curThreshold = getMedian(derivative, percentile);
	if(threshold == 0 || threshold != threshold) {
		threshold = curThreshold;
	} else {
		threshold = ofLerp(curThreshold, threshold, smoothing);
	}
	bufferPolyline = buildPolyline(buffer);
	derivativePolyline = buildPolyline(derivative);
	
	bufferBox = getBoundingBox(buffer);
	derivativeBox = getBoundingBox(derivative);
	/*
	if(minRange != 0 || maxRange != 0) {
		bufferBox.height = MAX(bufferBox.y + bufferBox.height, maxRange) - bufferBox.y;
		bufferBox.y = MIN(bufferBox.y, minRange);
	}
	*/
	if(bufferBox.height > 0) {
		normalized = ofMap(buffer.back(), bufferBox.y, bufferBox.y + bufferBox.height, 0, 1);
	}
	if(derivativeBox.height > 0) {
		normalizedDerivative = ofMap(derivative.back(), derivativeBox.y, derivativeBox.y + derivativeBox.height, 0, 1);
	}
	
	activity = ofLerp(normalizedDerivative, activity, activitySmoothing);
	sendMidi();
}

void Graph::sendMidi() const {
	if(getTriggered()) {
		midi.sendNoteOn(1, midiNote, 127);
	} else {
		midi.sendNoteOff(1, midiNote);
	}
	midi.sendControlChange(1, midiNote, 127 * getNormalized());
}

void Graph::setMidiNote(int midiNote) {
	this->midiNote = midiNote;
}

bool Graph::getTriggered() const {
	return triggered;
}

float Graph::getNormalized() const {
	return normalized;
}

float Graph::getNormalizedDerivative() const {
	return normalizedDerivative;
}

float Graph::getActivity() const {
	return activity;
}

void Graph::clear() {
	//derivative.clear();
	//buffer.clear();
	// don't reset the threshold, we want to keep it as a guess
}

void Graph::setName(string name) {
	this->name = name;
}

void Graph::draw(int x, int y) {
	ofPushMatrix();
	ofPushStyle();
	ofTranslate(x, y);
	
	drawPosition = worldToScreen(ofVec2f(0, 0));
	
	ofFill();
	ofSetColor(ofMap(ofGetElapsedTimef() - lastTrigger, 0, .5, 255, 0, true));
	ofRect(0, 0, width, height);
	
	ofNoFill();
	ofSetColor(255);
	ofPushStyle();
	if(hoverState) {
		ofSetHexColor(0xffee00);
		ofSetLineWidth(2);
	}
	ofRect(0, 0, width, height);
	ofPopStyle();
	
	ofSetHexColor(0xec008c);
	drawBuffer(derivativePolyline, derivativeBox, threshold, 0, 0, width, height);
	
	ofSetColor(255);
	drawBuffer(bufferPolyline, bufferBox, 0, 0, 0, width, height);
	
	ofSetColor(255);
	drawString(name, 5, 10);
	drawString(ofToString(bufferBox.y) + " / " + ofToString(bufferBox.height), 5, 18);
	if(threshold != 0) {
		drawString(ofToString(threshold, 4), 5, 26);
	}
	
	ofPopStyle();
	ofPopMatrix();
}

ofMesh Graph::buildPolyline(const deque<float>& buffer) {
	ofMesh line;
	line.setMode(OF_PRIMITIVE_LINE_STRIP);
	for(int i = 0; i < buffer.size(); i++) {
		line.addVertex(ofVec2f(i, buffer[i]));
	}
	return line;
}

ofRectangle Graph::getBoundingBox(const deque<float>& buffer) {
	ofRectangle box;
	box.x = 0;
	box.width = buffer.size();
	float curMin, curMax;
	for(int i = 0; i < buffer.size(); i++) {
		if(i == 0 || buffer[i] < curMin) {
			curMin = buffer[i];
		}
		if(i == 0 || buffer[i] > curMax) {
			curMax = buffer[i];
		}
	}
	box.y = curMin;
	box.height = curMax - curMin;
	return box;
}

float Graph::drawBuffer(ofMesh& line, ofRectangle& box, float threshold, int x, int y, int width, int height) {
	ofPushMatrix();
	ofVec2f min(box.x, box.y), max(box.x + box.width, box.y + box.height);
	//glMap(box, ofRectangle(0, 0, width, height));
	glMap(min, max, ofVec2f(0, height), ofVec2f(width, 0));
	line.draw();
	ofPopMatrix();
	if(threshold != 0 && box.height != 0) {
		float mappedThreshold = ofMap(threshold, box.y, box.y + box.height, height, 0, true);
		ofLine(0, mappedThreshold, width, mappedThreshold);
	}
}

float Graph::getMedian(const deque<float>& buffer, float percentile) {
	if(buffer.empty()) {
		return 0;
	}
	vector<float> all;
	all.assign(buffer.begin(), buffer.end());
	ofSort(all);
	return all[(int) (all.size() * percentile)];
}

void Graph::keyPressed(int key) {
	if(hoverState) {
		if(key == 'n') {
			midi.sendNoteOn(1, midiNote, 127);
			midi.sendNoteOff(1, midiNote);
		}
		if(key == 'c') {
			midi.sendControlChange(1, midiNote, ofRandom(127));
		}
	}
}

void Graph::mouseMoved(int x, int y) {
	mousePosition = ofVec2f(x, y) - drawPosition;
	hoverState = ofRectangle(0, 0, width, height).inside(mousePosition);
}