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

inline void glMap(const ofRectangle& from, const ofRectangle& to) {
	ofTranslate(to.x, to.y);
	ofScale(to.width, to.height);
	ofScale(1. / from.width, 1. / from.height);
	ofTranslate(-from.x, -from.y);
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

void Graph::drawString(string text, int x, int y) const {
	ofPushStyle();
	ofFill();
	ofRectangle box = font.getStringBoundingBox(text, x, y);
	box.x -= 1;
	box.y -= 1;
	box.width += 2;
	box.height += 2;
	ofSetColor(noData ? 64 : 0);
	ofRect(box);
	ofSetColor(noData ? 192 : 255);
	font.drawString(text, x, y);
	ofPopStyle();
}

Graph::Graph()
:width(0)
,height(0)

,threshold(0)
,thresholdSmoothing(.999)
,percentile(.98)

,noData(true) 
,lastTrigger(0)
,triggered(false)

,normalized(0)
,normalizedDerivative(0)

,activitySmoothing(.99)
,activity(0)

,downSmoothing(0)
,upSmoothing(0)

,midiNote(0)
,hoverState(false)
,bidirectional(false)
,minRange(0)
,maxRange(0) {
	setupResources();
	setSize(128, 32);
}

Graph& Graph::setup(string name, int midiNote, float threshold) {
	setName(name);
	if(midiNote > 0) {
		setMidiNote(midiNote);
	}
	if(threshold > 0) {
		setThreshold(threshold);
	}
	return *this;
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

void Graph::setBidirectional(bool bidirectional) {
	this->bidirectional = bidirectional;
}

void Graph::setSmoothing(float downSmoothing, float upSmoothing) {
	this->downSmoothing = downSmoothing;
	this->upSmoothing = upSmoothing;
}

void Graph::addSample(float sample) {
	noData = false;
	
	if(!buffer.empty() && (upSmoothing != 0 || downSmoothing != 0)) {
		if(sample > buffer.back()) {
			sample = ofLerp(sample, buffer.back(), upSmoothing);
		} else {
			sample = ofLerp(sample, buffer.back(), downSmoothing);
		}
	}
	
	if(minRange != 0 || maxRange != 0) {
		sample = ofClamp(sample, minRange, maxRange);
	}
	
	if(!buffer.empty()) {
		float diff = sample - buffer.back();
		float cmp = bidirectional ? abs(diff) : diff;
		if(!derivative.empty() && derivative.back() < threshold && cmp > threshold) {
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
		threshold = ofLerp(curThreshold, threshold, thresholdSmoothing);
	}
	
	bufferPolyline = buildPolyline(buffer);
	derivativePolyline = buildPolyline(derivative);
	
	bufferBox = getBoundingBox(buffer);
	derivativeBox = getBoundingBox(derivative);
	
	if(minRange != 0 || maxRange != 0) {
		bufferBox.y = minRange;
		bufferBox.height = maxRange - minRange;
		buffer.back() = ofClamp(buffer.back(), minRange, maxRange);
	}
	
	if(bufferBox.height > FLT_EPSILON) {
		normalized = ofMap(buffer.back(), bufferBox.y, bufferBox.y + bufferBox.height, 0, 1);
	}
	if(derivative.size() > 0 && derivativeBox.height > FLT_EPSILON) {
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
	noData = true;
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
	if(noData) {
		ofSetColor(128);
	}
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
	
	ofRectangle region(1, height - 1, width - 2, -(height - 2));
	ofSetHexColor(0xec008c);
	if(noData) {
		ofSetColor(200);
	}
	drawBuffer(derivativePolyline, threshold, derivativeBox, region);
	ofSetColor(255);
	drawBuffer(bufferPolyline, 0, bufferBox, region);
	
	ofSetColor(255);
	drawString(name, 5, 10);
	
	if(!buffer.empty() && !derivative.empty()) {
		ofPushMatrix();
		ofTranslate(width, 0);
		drawString(ofToString(bufferBox.y, 2) + "<" + (buffer.empty() ? "empty" :  ofToString(buffer.back(), 2)) + "<" + ofToString(bufferBox.y + bufferBox.height, 2), 5, 10);
		drawString(ofToString(derivativeBox.y, 2) + "<" + (derivative.empty() ? "empty" :  ofToString(derivative.back(), 2)) + "<" + ofToString(derivativeBox.y + derivativeBox.height, 2), 5, 18);
		drawString(ofToString(threshold, 2) + ", " + ofToString(buffer.back(), 2) + " (" + ofToString(getNormalized(), 2) + ") " + ofToString(derivative.back(), 2), 5, 26);
		ofPopMatrix();
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

float Graph::drawBuffer(ofMesh& line, float threshold, ofRectangle& from, ofRectangle& to) {
	ofPushMatrix();
	glMap(from, to);
	line.draw();
	if(threshold != 0) {
		float clampedThreshold = ofClamp(threshold, from.y, from.y + from.height);
		ofLine(from.x, clampedThreshold, from.width, clampedThreshold);
	}
	ofPopMatrix();
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