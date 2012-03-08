#pragma once

#include "ofMain.h"
#include "fixed_deque.h"

class Graph {
public:
	Graph();
	
	void setup(string name, int midiNote = 0, float threshold = 0);
	
	void setName(string name);
	void setMidiNote(int midiNote);
	void setThreshold(float threshold);
	void setSize(int width, int height);
	void setMinMaxRange(float minRange, float maxRange);
	
	void addSample(float sample);
	
	bool getTriggered() const;
	float getNormalized() const;
	float getNormalizedDerivative() const;
	float getActivity() const;
	
	void draw(int x, int y);
	void clear();
	void keyPressed(int key);
	void mouseMoved(int x, int y);
	
private:
	int width, height;
	string name;
	
	float threshold, smoothing, percentile;
	
	float lastTrigger;
	bool triggered;
	float normalized, normalizedDerivative;
	float activitySmoothing, activity;
	
	fixed_deque<float> buffer, derivative;
	
	ofPolyline buildPolyline(const deque<float>& buffer);
	float drawBuffer(ofPolyline& line, float threshold, int x, int y, int width, int height);
	void sendMidi() const;
	
	int midiNote;
	static float getMedian(const deque<float>& buffer, float percentile = .5);
	
	ofVec2f drawPosition, mousePosition;
	bool hoverState;
	
	float minRange, maxRange;
	float curMin, curMax;
};
