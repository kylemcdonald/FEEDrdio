#include "ofApp.h"
#include "ofAppGlutWindow.h"
#include "ofxXmlSettings.h"

int main() {
	ofxXmlSettings xml;
	xml.loadFile("settings.xml");
	bool fullscreen = xml.getValue("fullscreen", false);
	xml.pushTag("projector");
	int projectorWidth = xml.getValue("width", 640);
	int projectorHeight = xml.getValue("height", 480);
	xml.popTag();
	xml.pushTag("screen");
	int screenWidth = xml.getValue("width", 640);
	int screenHeight = xml.getValue("height", 480);
	xml.popTag();
	
	int windowWidth = screenWidth + projectorWidth;
	int windowHeight = MAX(screenHeight, projectorHeight);

	ofAppGlutWindow window;
	window.setGlutDisplayString("rgba double samples>=8 depth");
	ofSetupOpenGL(&window, windowWidth, windowHeight, fullscreen ? OF_FULLSCREEN : OF_WINDOW);
	ofRunApp(new ofApp());
}
