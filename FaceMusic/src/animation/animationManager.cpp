#include "animationManager.h"

using namespace ofxCv;
using namespace cv;


void animationManager::loadImageSet(vector < ofImage * > & imgs, string partName, ofPolyline & maskShape){
    
    
    
    ofImage mask;
    mask.loadImage("masks/" + partName + ".png");
    
    ofImage gray;
    gray.setUseTexture(false);
    gray.allocate(400,400, OF_IMAGE_GRAYSCALE);
    //cout << "mask size " << mask.getWidth() << " " << mask.getHeight() << endl;
    
    ofDirectory dir;
    dir.listDir("output/" + partName);
    for (int i = 0; i < dir.size(); i++){
        ofImage * img = new ofImage();
        img->loadImage(dir.getPath(i));
        img->setImageType(OF_IMAGE_COLOR_ALPHA);
        // do some pixel masking work?
        //cout << "img  " << i << " : " << img->getWidth() << " " << img->getHeight() << endl;
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

void transform ( ofxBox2dConvexPoly & circle, faceFeatureAnalysis & ffa, ofPoint offset, float angleAdder = 0){
    
    float pct = ofMap(ofGetMouseX(), 640,640*2, 1,0);
    
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
    
    
    circle.body->SetTransform(b2Vec2(mixPos.x, mixPos.y), angle );
    circle.setScale(1.55 * (ffa.dist/2 / 200.0));
    circle.body->SetLinearVelocity(b2Vec2(0,0));
    circle.body->SetAngularVelocity(0);
}

void animationManager::setup() {
    
    ofEnableAlphaBlending();
    
    
    box2d.init();
    box2d.setGravity(0, 0);
    box2d.createBounds(0,0,640,480);
    box2d.setFPS(30.0);
    box2d.registerGrabbing();
    
    
    
    
    for (int i = 0; i < FA->nFeatures; i++){
        vector < ofImage * > imgs;
        faceImages.push_back(imgs);
        ofPolyline maskShape;
        loadImageSet(faceImages[i], FA->getFacePartName(i), maskShape);
        
        
        
        //maskShape.simplify(1.4);
        ofPolyline temp = maskShape.getResampledByCount(30);
        
        
        
        maskPolys.push_back(temp);
        
        ofxBox2dConvexPoly poly;
        poly.setPhysics(1.0, 0.03, 0.1);
        poly.setup(box2d.getWorld(), maskPolys[i]);
        poly.setScale(0.1);
        polys.push_back(poly);
        
        
        ofPoint midPt;
        for (int j = 0; j < temp.getVertices().size(); j++){
            midPt+= temp.getVertices()[j];
        }
        midPt /= (float)temp.getVertices().size();
        offsets.push_back(ofPoint(200,200) - polys[polys.size()-1].getPosition());   
        
        
        
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
    
    
    faceLock = 0;
    faceLockTarget = 0;
    
}

void animationManager::update() {
	

//    if (FTM->FT.getFound()){
//        faceLockTarget = 1;
//    } else {
//        faceLockTarget = 0;
//    }
//    
//    if (faceLock < faceLockTarget){
//        faceLock = 0.99f * faceLock + 0.01 * faceLockTarget;
//    } else {
//        faceLock = 0.999f * faceLock + 0.001 * faceLockTarget;
//    }
    
    
    if (ofGetMouseX() > 640){
        
        
        for (int i = 0; i < FA->nFeatures; i++){
            
            ofPoint offsetme;
            
            switch (i){
                case 2:
                case 6:
                case 7:
                    transform(polys[i], *FA->features[i], ofPoint(0,0), PI);
                    break;
                case 5:
                    offsetme.x = 0.4 * 100 * cos(FA->features[i]->angle + PI/4);
                    offsetme.y = 0.4 * 100 * sin(FA->features[i]->angle + PI/4);
                    transform(polys[i], *FA->features[i], offsetme, PI/4);
                    break;
                case 4:
                    offsetme.x = 0.4 * -100 * cos(FA->features[i]->angle);
                    offsetme.y = 0.4 * -100 * sin(FA->features[i]->angle);
                    transform(polys[i], *FA->features[i], offsetme);
                    break;
                default:
                    transform(polys[i], *FA->features[i], ofPoint(0,0));
                    break;
                    
            }
            
        }
        
        
        
        
        
        for (int i = 0; i < FA->nFeatures; i++){
            polys[i].body->SetAwake(false);
        }
        
    } else {
        
        for (int i = 0; i < FA->nFeatures; i++){
            polys[i].body->SetAwake(true);
            ofPoint pt = (ofPoint(ofGetMouseX(), ofGetMouseY()) - polys[i].getPosition());
            pt.normalize();
            pt *= 100;
            polys[i].addAttractionPoint(ofPoint(ofGetMouseX(), ofGetMouseY()), 3);
        }
    }
    
    
    box2d.update();	
    
	ofBackground(0,0,0); //Grey background, NY style
    
    if (ofGetMousePressed() ){
        for (int i = 0; i < FA->nFeatures; i++)
            which[i] = ofRandom(0,10000000);
        
        
    }
    
    

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
    
    //temp->setAnchorPoint(200 + offset.x, 200 + offset.y);
    
    ofPushMatrix();
    ofTranslate(circle.getPosition().x, circle.getPosition().y);
    ofRotateZ(circle.getRotation() + angleAdd);
    //ofScale(scale, scale);
    
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
    
        
    ofSetColor(255,255,255,100);

    //int chin, forehead, lear, rear, reye, leye, nose, mouth;

    drawImageWithInfo(faceImages[chin][which[chin] % faceImages[chin].size()], FA->chin, polys[chin], offsets[chin], 2.0, true, 180);
    drawImageWithInfo(faceImages[forehead][which[forehead] % faceImages[forehead].size()], FA->forehead, polys[forehead],  offsets[forehead],  2.0, true, 180);

    ofSetColor(255,255,255,130);


    drawImageWithInfo(faceImages[lear][which[lear] % faceImages[lear].size()], FA->lEar, polys[lear],  offsets[lear],  2.0, true, 180);
    drawImageWithInfo(faceImages[rear][which[rear] % faceImages[rear].size()], FA->rEar, polys[rear],  offsets[rear],  2.0, true, 180);


    ofSetColor(255,255,255,255);

    float eyeA = 1.0;
//    float eyeA = MAX(ofMap(ofGetElapsedTimef() - graphs[2].lastTrigger, 0, 0.2, 1,0),ofMap(ofGetElapsedTimef() - graphs[3].lastTrigger, 0, 0.2, 1,0));

    eyeA = MIN(MAX(eyeA, 0), 1);

    drawImageWithInfo(faceImages[reye][which[reye] % faceImages[reye].size()], FA->rEye,  polys[reye], offsets[reye], 2.0, true);
    drawImageWithInfo(faceImages[leye][which[leye] % faceImages[leye].size()], FA->lEye,  polys[leye], offsets[leye], 2.0, true);

    drawImageWithInfo(faceImages[mouth][which[mouth] % faceImages[mouth].size()], FA->mouth,  polys[mouth], offsets[mouth], 1.6, true);
    drawImageWithInfo(faceImages[nose][which[nose] % faceImages[nose].size()], FA->nose,  polys[nose], offsets[nose], 2.0, true, 180);


    /*
    if (polys.size() > 0){
    float s = ofMap(ofGetMouseX(), 0, ofGetHeight(), 0.5, 1.5);
    polys[0].setScale(s);
    polys[0].body->SetTransform(b2Vec2(ofGetMouseX() / OFX_BOX2D_SCALE, ofGetMouseY() / OFX_BOX2D_SCALE), ofGetElapsedTimef());
    polys[0].body->SetLinearVelocity(b2Vec2(0,0));
    }
    */

    for(int i=0; i<polys.size(); i++) {
    ofFill();
    ofSetColor(255,0,0,150);
    //polys[i].draw();
    }

    //    for (int i = 0; i < maskPolys.size(); i++){
    //        maskPolys[i].draw();
    //    }
        
    
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