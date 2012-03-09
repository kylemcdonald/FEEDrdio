#include "ofApp.h"
#include "ofAppGlutWindow.h"
#include "ofxXmlSettings.h"

int main() {
	// ofRunApp() deletes the app
	ofApp& app = *(new ofApp());
	
	app.loadSettings();
	int windowWidth = app.screenWidth + app.projectorWidth;
	int windowHeight = MAX(app.screenHeight, app.projectorHeight);
	
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, windowWidth, windowHeight, app.fullscreen ? OF_FULLSCREEN : OF_WINDOW);
	ofRunApp(&app);
}
