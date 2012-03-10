#pragma once

#include "ofMain.h"

class PebbleMaker {
public:
	
	static ofPath generate() {
		// settings
		float minSize = .8, maxSize = 1.1;
		int minVertices = 3, maxVertices = 9;
		float maxEllipticality = 25;
		float maxRotation = 45;
		float baseDisplacement = .1;
		int minSubdivisionSteps = 2, maxSubdivisionSteps = 15;
		float minInnerWidth = .1, maxInnerWidth = .1;
		float randomWalkWeight = .4;
		float minBendWeight = .1, maxBendWeight = 1.5;
		
		ofPolyline base;
		ofPath inner;
		
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
		for(int i = 0; i < n; i++) {
			int subdivisionSteps = ofRandom(minSubdivisionSteps, maxSubdivisionSteps);
			ofVec2f start = base[i];
			ofVec2f end = base[(i + 1) % n];
			ofVec2f normal = (end - start).rotate(-90);
			float innerRandomWalk = 0;
			float bendWeight = ofRandom(minBendWeight, maxBendWeight);
			for(int j = 0; j < subdivisionSteps; j++) {
				ofVec2f interpolated = start.getInterpolated(end, (float) j / subdivisionSteps);
				innerRandomWalk += ofRandomf() * randomWalkWeight;
				float bend = sin(ofMap(j, 0, subdivisionSteps, 0, PI)) * bendWeight;
				ofVec2f innerInterpolated = interpolated + normal * bend * (minInnerWidth + abs(innerRandomWalk) * maxInnerWidth);
				inner.lineTo(innerInterpolated);
			}
		}
		inner.close();
		inner.setColor(ofColor::white);
		
		return inner;
	}
};