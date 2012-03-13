#include "animationManager.h"
#include "PebbleMaker.h"


using namespace ofxCv;
using namespace cv;


void animationManager::loadImageSet(vector < ofImage * > & imgs, string partName, ofPolyline & maskShape){
    
    
    
    
    ofImage mask;
    
    if (side == 0)
        mask.loadImage("masks/" + partName + ".png");
    else 
        mask.loadImage("masks_b/" + partName + ".png");
    
    ofImage gray;
    gray.setUseTexture(false);
    gray.allocate(400,400, OF_IMAGE_GRAYSCALE);
    ofDirectory dir;
    if (side == 0)
        dir.listDir("output/" + partName);
    else 
        dir.listDir("output_b/" + partName);
    
    for (int i = 0; i < dir.size(); i++){
        ofImage * img = new ofImage();
        img->loadImage(dir.getPath(i));
        img->setImageType(OF_IMAGE_COLOR_ALPHA);
        unsigned char * pixelsA = mask.getPixels();
        unsigned char * pixelsB = img->getPixels();
        unsigned char * pixelsC = gray.getPixels();
        int count= 0;
        for (int i = 3; i < 400*400*4; i+=4){
            pixelsB[i] = pixelsA[i];
            pixelsC[count++] = pixelsA[i];
        }
        img->update();
        gray.update();
        imgs.push_back(img);
    }
    ContourFinder CF;
    CF.setSimplify(true);
    CF.setMinArea(600);
    CF.setThreshold(127);
    CF.findContours(toCv(gray));
    if (CF.size() > 0){
        maskShape = CF.getPolyline(0);
    }
    
    
}



void animationManager::setup() {
    
    pebbleShader.load("shaders/pebble.vert", "shaders/pebble.frag");
    
	ofxXmlSettings xml;
	xml.loadFile("settings.xml");
	side = xml.getValue("side", 0);
	xml.pushTag("camera");
	camWidth = xml.getValue("width", 640);
	camHeight = xml.getValue("height", 480);
	xml.popTag();
	xml.pushTag("projector");
	width = xml.getValue("width", 640);
	height = xml.getValue("height", 480);
	xml.popTag();
    
    
    //fbo.allocate(width, height); 
	
    ofEnableAlphaBlending();
    
    
    for (int i = 0; i < 16; i++){
       // ofImage pebbles[16];
        //cout << "pebbles/pebbles-" + ofToString(i) + ".jpg" << endl;
        pebbles[i].loadImage("pebbles/pebbles-" + ofToString(i) + ".jpg");
        //cout << pebbles[i].getWidth() << endl;
        pebbles[i].setImageType(OF_IMAGE_COLOR_ALPHA);
        unsigned char * pixels = pebbles[i].getPixels();
        for (int j = 0; j < pebbles[i].getWidth()*pebbles[i].getHeight(); j++){
            int val = 255 - pixels[j*4];
            pixels[j*4] = 255;
            pixels[j*4+1] = 255;
            pixels[j*4+2] = 255;
            pixels[j*4+3] = val;
            
        }
        pebbles[i].update();
         
    }
    
    
    box2d.init();
		box2d.setGravity(0, 4);
		box2d.createBounds(0,0,camWidth,camHeight);
    box2d.setFPS(30.0);
    box2d.registerGrabbing();
    
    
    
    
    for (int i = 0; i < FA->nFeatures; i++){
        vector < ofImage * > imgs;
        faceImages.push_back(imgs);
        ofPolyline maskShape;
        loadImageSet(faceImages[i], FA->getFacePartName(i), maskShape);
        ofPolyline temp = maskShape.getResampledByCount(30);
        maskPolys.push_back(temp);
        
        ofxBox2dConvexPoly poly;
        poly.setPhysics(5.0, 0.03, 0.1);
        poly.setup(box2d.getWorld(), maskPolys[i]);
        poly.setScale(0.1);
        polys.push_back(poly);
        
        
        ofPoint midPt;
        for (int j = 0; j < temp.getVertices().size(); j++){
            midPt+= temp.getVertices()[j];
        }
        midPt /= (float)temp.getVertices().size();
        offsets.push_back(ofPoint(200,200) - polys[polys.size()-1].getPosition());   
        
        
        
        transformAdd add;
        add.scaleAdder = 0;
        add.angleAdder = 0;
        add.offsetAdder.set(0,0);
        transformAdds.push_back(add);
        
        /*
         float scaleAdder;
         float angleAdder;
         ofPoint offsetAdder;
         
         */
        
        //        ofxBox2dConvexPoly * poly = new ofxBox2dConvexPoly();
        //        poly->setPhysics(1.0, 0.13, 0.1);
        //        poly->setup(box2d.getWorld(), maskShape);
        //        polys.push_back(poly);
        
        
    }
    
    for (int i = 0; i < FA->nFeatures; i++){
        which.push_back(ofRandom(0,10000000));
    }
    
    
    chin = FA->getId("chin");
    forehead = FA->getId("forehead");
    lear = FA->getId("lear");
    rear = FA->getId("rear");
    reye = FA->getId("reye");
    leye = FA->getId("leye");
    nose = FA->getId("nose");
    mouth = FA->getId("mouth");
    
    presence = 0;
    faceLock = 0;
    faceLockTarget = 0;
    
    bWasPresentLastFrame = 0;
    lastNonPresenceTime = 0;
    // = 0;
    
    
    for (int i = 0; i < 60; i++){
        /*float r = ofRandom(3.5,11);		// a random radius 4px - 20px
        ofxBox2dCircle circle;
        circle.setPhysics(1.0, 0.53, 0.1);
		circle.setup(box2d.getWorld(), 300,300, r);
		circles.push_back(circle);*/
        
        ofPolyline path = PebbleMaker::generate();
        //ofPolyline pl = path.getOutline()[0];
        for (int i = 0; i < path.getVertices().size(); i++){
            path.getVertices()[i] *= 100.0;
            path.getVertices()[i] += ofPoint(200,200);
        }
        //cout << ofPolyline.getVertices()[0] << endl;
        ofPolyline pl2 = path.getSmoothed(5);
        ofPolyline pl3 = pl2.getResampledByCount(13);
        //maskPolys.push_back(temp);
        ofxBox2dConvexPoly poly;
        poly.setPhysics(1.0, 0.53, 0.1);
        poly.setup(box2d.getWorld(), pl3);
        poly.setScale(ofRandom(0.025, 0.22));
        circles.push_back(poly);
        
    }
    
    
    pebbleBg.loadImage("pebble_bg/depositphotos_6841792-Old-newspaper-background.jpeg");
    
    centroidSmoothed.set(camWidth/2, camHeight/2);
    presenceSmoothed = 0;
    stdDevSmoothed = 0;
    
    
}


void transform ( ofxBox2dConvexPoly & circle, faceFeatureAnalysis & ffa, transformAdd add, ofPoint offset, float angleAdder = 0, float extraScale = 1.0){
    
    float pct = 0.5f;
    ofPoint circlePos = circle.getPosition()/OFX_BOX2D_SCALE;
    ofPoint targetPos = ofPoint((ffa.pos.x + offset.x)/OFX_BOX2D_SCALE, (ffa.pos.y + offset.y)/OFX_BOX2D_SCALE);
    ofPoint mixPos = pct * circlePos + (1-pct) * targetPos;
    float angle = circle.getRotation() * DEG_TO_RAD;
    float targetAngle = ffa.angle + angleAdder;
    float diffAngle = targetAngle - angle;
    while (diffAngle < -PI) diffAngle += TWO_PI;
    while (diffAngle > PI) diffAngle -= TWO_PI;
    diffAngle *= pct;
    angle += diffAngle;
    while (angle > PI) angle-= TWO_PI;
    while (angle < -PI) angle+= TWO_PI;
    circle.body->SetTransform(b2Vec2(mixPos.x, mixPos.y), angle + add.angleAdder );
    circle.setScale(1.55 * (ffa.dist/2 / 200.0) * extraScale + add.scaleAdder);
    circle.body->SetLinearVelocity(b2Vec2(0,0));
    circle.body->SetAngularVelocity(0);
    
}

//
//void transform2 ( ofxBox2dConvexPoly & circle, faceFeatureAnalysis & ffa, ofPoint offset, float angleAdder = 0, float extraScale = 1.0){
//    
//    float angle = circle.getRotation() * DEG_TO_RAD;
//    float targetAngle = ffa.angle + angleAdder;
//    float diffAngle = targetAngle - angle;
//    while (diffAngle < -PI) diffAngle += TWO_PI;
//    while (diffAngle > PI) diffAngle -= TWO_PI;
//    diffAngle *= 0.36;
//    circle.addAttractionPoint(ffa.pos.x, ffa.pos.y, 6);
//    circle.body->ApplyAngularImpulse(diffAngle);
//    circle.setScale(1.55 * (ffa.dist/2 / 200.0) * extraScale);
//    
//}


void animationManager::update() {
	
    if (ofGetFrameNum() % 120 == 0){
        pebbleShader.load("shaders/pebble.vert", "shaders/pebble.frag");
    }
    
    if (FTM->tracker.getFound()){
        presence = 0.6f * presence + 0.4f * 1.0;
    } else {
        presence = 0.4f * presence + 0.6f * 0.0;
    }
    
    // where the pebbles want to go! 
    ofPoint facePoint = FTM->tracker.getPosition() - ofPoint(0,100);

    
  
    if (FTM->graphs[2].getTriggered() || FTM->graphs[3].getTriggered()){
        transformAdds[leye].scaleAdder +=  0.1;
        transformAdds[reye].scaleAdder += 0.1;
        transformAdds[leye].scaleAdder = MIN(0.8, transformAdds[leye].scaleAdder);
        transformAdds[reye].scaleAdder = MIN(0.8, transformAdds[reye].scaleAdder);
        
        transformAdds[leye].angleAdder += ofRandom(-transformAdds[leye].scaleAdder*5, transformAdds[leye].scaleAdder*5);
        transformAdds[reye].angleAdder += ofRandom(-transformAdds[leye].scaleAdder*5, transformAdds[leye].scaleAdder*5);
        
        while(transformAdds[leye].angleAdder > PI) transformAdds[leye].angleAdder -= TWO_PI;
        while(transformAdds[leye].angleAdder < -PI) transformAdds[leye].angleAdder += TWO_PI;
        
        
        facePoint = (int)(ofGetElapsedTimef()) % 2 == 0 ? FA->features[leye]->pos :  FA->features[reye]->pos;
        
        while(transformAdds[reye].angleAdder > PI) transformAdds[reye].angleAdder -= TWO_PI;
        while(transformAdds[reye].angleAdder < -PI) transformAdds[reye].angleAdder += TWO_PI;
        //transformAdds[reye].angleAdder += ofRandom(-transformAdds[leye].scaleAdder, transformAdds[leye].scaleAdder);
    } 
    
    /*
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
     */
    
     if (FTM->graphs[7].getTriggered() || FTM->graphs[8].getTriggered() ){
         
         transformAdds[nose].scaleAdder +=  0.01;
         transformAdds[nose].scaleAdder += 0.01;
         transformAdds[nose].scaleAdder = MIN(0.07, transformAdds[nose].scaleAdder);
         transformAdds[nose].scaleAdder = MIN(0.07, transformAdds[nose].scaleAdder);
         transformAdds[nose].angleAdder += ofRandom(-transformAdds[nose].scaleAdder*0.009, transformAdds[nose].scaleAdder*0.009);
         transformAdds[nose].angleAdder += ofRandom(-transformAdds[nose].scaleAdder*0.009, transformAdds[nose].scaleAdder*0.009);
         while(transformAdds[nose].angleAdder > PI) transformAdds[nose].angleAdder -= TWO_PI;
         while(transformAdds[nose].angleAdder < -PI) transformAdds[nose].angleAdder += TWO_PI;
         
     }
    
    
    if (FTM->graphs[4].getTriggered() || FTM->graphs[5].getTriggered() || FTM->expressionGraphs[6].getTriggered()){
        
        transformAdds[lear].scaleAdder +=  0.05;
        transformAdds[lear].scaleAdder += 0.05;
        transformAdds[lear].scaleAdder = MIN(0.2, transformAdds[lear].scaleAdder);
        transformAdds[lear].scaleAdder = MIN(0.2, transformAdds[lear].scaleAdder);
        transformAdds[lear].angleAdder += ofRandom(-transformAdds[lear].scaleAdder*0.04, transformAdds[lear].scaleAdder*0.04);
        transformAdds[lear].angleAdder += ofRandom(-transformAdds[lear].scaleAdder*0.04, transformAdds[lear].scaleAdder*0.04);
        while(transformAdds[lear].angleAdder > PI) transformAdds[lear].angleAdder -= TWO_PI;
        while(transformAdds[lear].angleAdder < -PI) transformAdds[lear].angleAdder += TWO_PI;
        
        transformAdds[rear].scaleAdder +=  0.05;
        transformAdds[rear].scaleAdder += 0.05;
        transformAdds[rear].scaleAdder = MIN(0.2, transformAdds[rear].scaleAdder);
        transformAdds[rear].scaleAdder = MIN(0.2, transformAdds[rear].scaleAdder);
        transformAdds[rear].angleAdder += ofRandom(-transformAdds[rear].scaleAdder*0.04, transformAdds[rear].scaleAdder*0.04);
        transformAdds[rear].angleAdder += ofRandom(-transformAdds[rear].scaleAdder*0.04, transformAdds[rear].scaleAdder*0.04);
        while(transformAdds[rear].angleAdder > PI) transformAdds[rear].angleAdder -= TWO_PI;
        while(transformAdds[rear].angleAdder < -PI) transformAdds[rear].angleAdder += TWO_PI;

        transformAdds[chin].scaleAdder +=  0.01;
        transformAdds[chin].scaleAdder += 0.01;
        transformAdds[chin].scaleAdder = MIN(0.15, transformAdds[chin].scaleAdder);
        transformAdds[chin].scaleAdder = MIN(0.15, transformAdds[chin].scaleAdder);
        transformAdds[chin].angleAdder += ofRandom(-transformAdds[chin].scaleAdder*0.04, transformAdds[chin].scaleAdder*0.04);
        transformAdds[chin].angleAdder += ofRandom(-transformAdds[chin].scaleAdder*0.04, transformAdds[chin].scaleAdder*0.04);
        while(transformAdds[chin].angleAdder > PI) transformAdds[chin].angleAdder -= TWO_PI;
        while(transformAdds[chin].angleAdder < -PI) transformAdds[chin].angleAdder += TWO_PI;
    }
    
    if (FTM->graphs[0].getTriggered() || FTM->graphs[1].getTriggered() || FTM->expressionGraphs[2].getTriggered()){
        
        transformAdds[mouth].scaleAdder +=  0.1;
        transformAdds[mouth].scaleAdder += 0.1;
        transformAdds[mouth].scaleAdder = MIN(0.5, transformAdds[mouth].scaleAdder);
        transformAdds[mouth].scaleAdder = MIN(0.5, transformAdds[mouth].scaleAdder);
        transformAdds[mouth].angleAdder += ofRandom(-transformAdds[mouth].scaleAdder*0.4, transformAdds[mouth].scaleAdder*0.4);
        transformAdds[mouth].angleAdder += ofRandom(-transformAdds[mouth].scaleAdder*0.4, transformAdds[mouth].scaleAdder*0.4);
        while(transformAdds[mouth].angleAdder > PI) transformAdds[mouth].angleAdder -= TWO_PI;
        while(transformAdds[mouth].angleAdder < -PI) transformAdds[mouth].angleAdder += TWO_PI;

        
        facePoint = FA->features[mouth]->pos;
    }
    
    
    for (int i = 0; i < transformAdds.size(); i++){
        transformAdds[i].angleAdder *= 0.97f;
        transformAdds[i].scaleAdder *= 0.97f;
        transformAdds[i].offsetAdder *= 0.97f;
    }
   
    
    
    
    
    if (presence > 0.5){
        
        bWasPresentLastFrame = true;
				bool didSwitch = false;
				float curTime = ofGetElapsedTimef();
				float clickWaitTime = .5; // wait for this much non-face until randomizing
				float clickingTime = 2; // how long the randomizing lasts
				float presenceLength = curTime - lastNonPresenceTime;
				float nonPresenceLength = lastNonPresenceTime - fistNonPresenceTime;
        if (presenceLength < clickingTime && nonPresenceLength > clickWaitTime) {
            float pct = pow(ofNormalize(presenceLength, 0, clickingTime), .1f);
            for (int i = 0; i < FA->nFeatures; i++){
                if (ofRandomf() > pct){
                    which[i] = ofRandom(0,10000000);
										didSwitch = true;
                }
            }
        }
				if(didSwitch) {
					Graph::sendManualNote(96);
				}
        
        for (int i = 0; i < FA->nFeatures; i++){
            ofPoint offsetme;
            switch (i){
                case 0:
                    transform(polys[i], *FA->features[i], transformAdds[i], ofPoint(0,0), 0);
                    break;
                case 2:
                case 6:
                case 7:
                    transform(polys[i], *FA->features[i], transformAdds[i], ofPoint(0,0), PI);
                    break;
                case 5:
                    offsetme.x = 0.4 * 100 * cos(FA->features[i]->angle + PI/4);
                    offsetme.y = 0.4 * 100 * sin(FA->features[i]->angle + PI/4);
                    transform(polys[i], *FA->features[i], transformAdds[i], offsetme, PI/4);
                    break;
                case 4:
                    offsetme.x = 0.4 * -100 * cos(FA->features[i]->angle);
                    offsetme.y = 0.4 * -100 * sin(FA->features[i]->angle);
                    transform(polys[i], *FA->features[i], transformAdds[i], offsetme);
                    break;
                default:
                    transform(polys[i], *FA->features[i], transformAdds[i], ofPoint(0,0));
                    break;
            }
        }

        
        for (int i = 0; i < FA->nFeatures; i++){
            polys[i].body->SetAwake(true);
            b2Filter filter;
            if (i == chin || i == forehead){
                filter.categoryBits = 0x0004;
                filter.maskBits = 0x0004;
                filter.groupIndex = 0;
            } else {
                filter.categoryBits = 0x0002;
                filter.maskBits = 0x0002;
                filter.groupIndex = 0;
            }
            polys[i].setFilterData(filter);
        }
        
    } else {
        
        if (bWasPresentLastFrame == true){
            fistNonPresenceTime = ofGetElapsedTimef();
        }
        bWasPresentLastFrame = false;
        
        lastNonPresenceTime = ofGetElapsedTimef();
        for (int i = 0; i < FA->nFeatures; i++){
            polys[i].body->SetAwake(true);
            polys[i].addAttractionPoint(getAttractor(), 3);
        }
        
        for (int i = 0; i < FA->nFeatures; i++){
            b2Filter filter;
            filter.categoryBits = 0x0001;
            filter.maskBits = 0xFFFF;
            filter.groupIndex = 0;
            polys[i].setFilterData(filter);
        }
        
        
    }
    
    
     if (presence > 0.5){
         // if we have a face, let's make the circle go towards the face. 
        // otherwise, let's do something else
        for (int i = 0; i < circles.size(); i++){
            
           
            circles[i].body->SetLinearDamping(0.04);
            circles[i].addAttractionPoint(facePoint.x, facePoint.y, 4);
            //(diff.x, diff.y);
            b2Filter filter;
            filter.categoryBits = 0x0002;
            filter.maskBits = 0xFFFF;
            filter.groupIndex = 0;
            circles[i].setFilterData(filter);
        }
     } else {
         for (int i = 0; i < circles.size(); i++){
				     circles[i].addAttractionPoint(getAttractor());
             b2Filter filter;
             filter.categoryBits = 0x0001;
             filter.maskBits = 0xFFFF;
             filter.groupIndex = 0;
             circles[i].setFilterData(filter);
             
         }         
     }
    
    
    
    box2d.update();	
    
	ofBackground(0,0,0); //Grey background, NY style
}

ofVec2f animationManager::getAttractor() {
	switch(side) {
		case 0:	return ofVec2f(camWidth, camHeight);
		case 1: return ofVec2f(0, camHeight);
	}
	return ofVec2f(camWidth / 2, camHeight / 2);
}

void animationManager::drawImageWithInfo(ofImage * temp, faceFeatureAnalysis & ft, ofxBox2dConvexPoly & circle, ofPoint offset, float scaler = 1, bool bFlipHoriz = true, float angleAdd = 0){
    float imgSize;
    
    if (ft.bHorizDistance == true){
        imgSize = temp->width;
    } else {
        imgSize = temp->height;
    }
    
    
    float scale = circle.scale/2; //(circle.getRadius() * 2) / imgSize;
    scale *= scaler;
    
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofPushMatrix();
    ofTranslate(circle.getPosition().x, circle.getPosition().y);
    ofRotateZ(circle.getRotation() + angleAdd);
    
    if (ft.bHorizDistance == false){
        ofScale(-scale, -scale);
    } else {
        
        ofScale(scale, scale);
    }
    
    if (!bFlipHoriz){
        temp->draw(offset.x, offset.y);
    } else {
        temp->draw(offset.x, offset.y, temp->getWidth(), temp->getHeight());
        
    }
    ofPopMatrix();
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    
}



void animationManager::draw() {
    float scaleAmount = (float) height / camHeight;
		ofVec2f sceneCenter(width / 2, height / 2), sceneScale(scaleAmount, scaleAmount), sceneOffset(-camWidth / 2, -camHeight / 2);
		
    
    
    //------------------------------------------------------------------------------ 
    // smooth out presence
    presenceSmoothed = 0.99f * presenceSmoothed + 0.01 * presence;
    
    // calc centroid of objects.  every one and a while a face part is outside of bounds (because it changes bounce group when face is present, it can fall offscreen until the next face). 
    
    ofPoint centroidOfObject;
    float stdDeviation;
    int countObj = 0;
    ofRectangle bounds;
    bounds.set(0,0,camWidth, camHeight);
    for (int i = 0; i < polys.size(); i++){
        if (bounds.inside(polys[i].getPosition())){
            countObj++;
            centroidOfObject += polys[i].getPosition();
        }
    }
    
    centroidOfObject /= (float)( MAX(1,countObj));
    
    // calc std deviation. 
    
    for (int i = 0; i < polys.size(); i++){
        if (bounds.inside(polys[i].getPosition())){
            stdDeviation +=  pow((centroidOfObject - polys[i].getPosition()).length(),2);
        }
    }

    stdDeviation = sqrt(stdDeviation / (float)( MAX(1,countObj)));
    
    // smooth them both out
    
    centroidSmoothed = 0.9f * centroidSmoothed + 0.1f * centroidOfObject;
    stdDevSmoothed = 0.9f * stdDevSmoothed + 0.1f * stdDeviation;
    
    // use stdDev to figure out how much to scale. 
    
    float centerAmount = (camWidth / (stdDevSmoothed*2)) * 0.1 * presenceSmoothed;
    
    //------------------------------------------------------------------------------ 
   
    // this is the normal translate and scale
    
    float scalex = sceneScale.x;
    float scaley = sceneScale.y;
    ofTranslate(sceneCenter);
    ofScale(scalex, scaley);
    ofTranslate(sceneOffset);
   
    //------------------------------------------------------------------------------ 
    
    // figure out where the center of the screen is in this new coordinate space
    // also the centroid, etc. 
    // lots of trial and error math here, not sure if it's right. there's a draw centroid at the bottom of draw for debugging. 
    
    ofVec2f centerScreen = (ofPoint(camWidth/2, camHeight/2) + sceneOffset) * sceneScale + sceneCenter;
    ofVec2f centroidOfObject2 = (centroidSmoothed + sceneOffset) * sceneScale + sceneCenter;
    ofVec2f diff = centerScreen - centroidOfObject2;
    diff *= centerAmount;
    
    ofPoint centerOnCentroid = -centroidSmoothed * centerAmount;
    float scaleUp = 1+centerAmount;
    ofPoint backToCenter = diff /= (1+centerAmount);
    
    ofTranslate(centerOnCentroid);
    ofScale(scaleUp,scaleUp);
    ofTranslate(backToCenter);
    
    //------------------------------------------------------------------------------ 
    
    
    
    
    ofSetColor(255,255,255,100);

    //int chin, forehead, lear, rear, reye, leye, nose, mouth;

    drawImageWithInfo(faceImages[chin][which[chin] % faceImages[chin].size()], FA->chin, polys[chin], offsets[chin], 2.0, true, 180);
    drawImageWithInfo(faceImages[forehead][which[forehead] % faceImages[forehead].size()], FA->forehead, polys[forehead],  offsets[forehead],  2.0, true, 180);

    ofSetColor(255,255,255,130);


    drawImageWithInfo(faceImages[lear][which[lear] % faceImages[lear].size()], FA->lEar, polys[lear],  offsets[lear],  2.0, true, 180);
    drawImageWithInfo(faceImages[rear][which[rear] % faceImages[rear].size()], FA->rEar, polys[rear],  offsets[rear],  2.0, true, 180);

    
    pebbleShader.begin();
    pebbleBg.getTextureReference().bind();
    
    ofSeedRandom(0);
    ofSetColor(120, 150);
    ofSetRectMode(OF_RECTMODE_CENTER);
	for (int i = 0; i < circles.size(); i++){
		float padding = .2;
		ofVec2f texCenter;
		texCenter.x = ofRandom(pebbleBg.getWidth() * padding, pebbleBg.getWidth() * (1 - padding));
		texCenter.y = ofRandom(pebbleBg.getHeight() * padding, pebbleBg.getHeight() * (1 - padding));
		pebbleShader.setUniform1f("rotation", circles[i].getRotation() * DEG_TO_RAD);
		pebbleShader.setUniform2f("texCenter", texCenter.x, texCenter.y);
		circles[i].draw();
	}
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofSeedRandom();
    
    pebbleBg.getTextureReference().unbind();
    pebbleShader.end();
    
    

    ofSetColor(255);

    float eyeA = 1.0;

    eyeA = MIN(MAX(eyeA, 0), 1);

    drawImageWithInfo(faceImages[reye][which[reye] % faceImages[reye].size()], FA->rEye,  polys[reye], offsets[reye], 2.0, true);
    drawImageWithInfo(faceImages[leye][which[leye] % faceImages[leye].size()], FA->lEye,  polys[leye], offsets[leye], 2.0, true);

    drawImageWithInfo(faceImages[mouth][which[mouth] % faceImages[mouth].size()], FA->mouth,  polys[mouth], offsets[mouth], 1.6, true);
    drawImageWithInfo(faceImages[nose][which[nose] % faceImages[nose].size()], FA->nose,  polys[nose], offsets[nose], 2.0, true, 180);


    ofSetColor(255,0,0);
    //ofCircle(centroidOfObject, 10);
    
    
}


//--------------------------------------------------------------
void animationManager::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void animationManager::mousePressed(int x, int y, int button) {
    
	
}

//--------------------------------------------------------------
void animationManager::mouseReleased(int x, int y, int button) {
    
} 




void animationManager::keyPressed(int key) {
}