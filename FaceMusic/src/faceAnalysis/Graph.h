#pragma once

#include "ofMain.h"
#include "fixed_deque.h"

class Graph {
public:
	Graph();
	
	Graph& setup(string name, int midiNote = 0, float threshold = 0);
	
	void setName(string name);
	void setMidiNote(int midiNote);
	void setThreshold(float threshold);
	void setBidirectional(bool bidirectional);
	void setSize(int width, int height);
	void setMinMaxRange(float minRange, float maxRange);
	void setSmoothing(float downSmoothing, float upSmoothing);
	
	void addSample(float sample);
	
	bool getTriggered() const;
	float getNormalized() const;
	float getNormalizedDerivative() const;
	float getActivity() const;
	
	void draw(int x, int y);
	void clear();
	void keyPressed(int key);
	void mouseMoved(int x, int y);
	
	static void sendManualNote(int midiNote);
	
//private:
	int width, height;
	string name;
	
	float threshold, thresholdSmoothing, percentile;
	bool noData;
	float lastTrigger;
	bool triggered;
	float normalized, normalizedDerivative;
	float activitySmoothing, activity;
	
	float downSmoothing, upSmoothing;
	fixed_deque<float> buffer, derivative;
	ofMesh bufferPolyline, derivativePolyline;
	ofRectangle bufferBox, derivativeBox;
	
	ofRectangle getBoundingBox(const deque<float>& buffer);
	ofMesh buildPolyline(const deque<float>& buffer);
	float drawBuffer(ofMesh& line, float threshold, ofRectangle& from, ofRectangle& to);
	void sendMidi() const;
	void drawString(string text, int x, int y) const;
	
	int midiNote;
	static float getMedian(const deque<float>& buffer, float percentile = .5);
	
	ofVec2f drawPosition, mousePosition;
	bool hoverState;
	
	bool bidirectional;
	float minRange, maxRange;
};
