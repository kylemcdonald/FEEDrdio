/*
 *  ofxBox2dConvexPoly.cpp
 *  ofxBox2dExample
 *
 *  Created by Todd Vanderlin on 3/7/11.
 *  Copyright 2011 Interactive Design. All rights reserved.
 *
 */

#include "ofxBox2dConvexPoly.h"
#include "ofxBox2dPolygon.h"





ofPolyline convexHull(ofPolyline & line){
    
    vector < hPoint > ptsIn;
    for (int i = 0; i < line.getVertices().size(); i++){
        hPoint pt;
        pt.x = line.getVertices()[i].x;
        pt.y = line.getVertices()[i].y;
        
        ptsIn.push_back(pt);
    }
    vector < hPoint > ptsOut;
    
    ptsOut =  calcConvexHull(ptsIn);
    
    ofPolyline out;
    
    for (int i = 0; i < ptsOut.size(); i++){
        out.addVertex(ofPoint(ptsOut[i].x, ptsOut[i].y));
    }
    
    return out;
    
}


////----------------------------------------
//ofxBox2dConvexPoly::~ofxBox2dPolygon() { 
//}
//
////----------------------------------------
//void ofxBox2dConvexPoly::destroy() {
//	ofxBox2dBaseShape::destroy();
//	//clear();
//}



//------------------------------------------------
ofxBox2dConvexPoly::ofxBox2dConvexPoly() {
}

//------------------------------------------------
void ofxBox2dConvexPoly::setup(b2World * b2dworld, ofPolyline & _line){

    //b2PolygonShape
    
    /*std::reverse(_line.getVertices().begin(), _line.getVertices().end());
    cout << _line.size() << endl;
    cout << _line.getArea() << endl;
    */
    ofPolyline line = convexHull(_line);
    line.getVertices().erase(line.getVertices().end()-1);
    
    
    b2Vec2 * vertices;
    int32  vertexCount = line.getVertices().size();
    vertices = new b2Vec2[vertexCount];
    ofPoint pos;
    ghettoRadius = 0;
    for (int i = 0; i < vertexCount; i++){
        vertices[i].x = line.getVertices()[i].x;
        vertices[i].y = line.getVertices()[i].y;
        pos.x += line.getVertices()[i].x;
        pos.y += line.getVertices()[i].y;
        
    }
    pos /= (float)vertexCount;
    
    for (int i = 0; i < vertexCount; i++){
        float dist = (pos - line.getVertices()[i]).length();
        if (dist > ghettoRadius){
            ghettoRadius = dist;
        }
    }

    
    for (int i = 0; i < vertexCount; i++){
        vertices[i].x /= OFX_BOX2D_SCALE;
        vertices[i].y /= OFX_BOX2D_SCALE;
    }
    
    ofPoint posCent = ofPoint(200,200) - pos;
    
    pos /= OFX_BOX2D_SCALE;
    posCent /= OFX_BOX2D_SCALE;
    ghettoRadius    /= OFX_BOX2D_SCALE;

    
    for (int i = 0; i < vertexCount; i++){
        vertices[i].x -= pos.x;
        vertices[i].y -= pos.y;
        
        polyPts.addVertex(ofPoint(vertices[i].x, vertices[i].y));
    }
    
    
    
    float x = pos.x ;
    float y = pos.y ;
	
	if(b2dworld == NULL) {
		ofLog(OF_LOG_NOTICE, "ofxBox2dConvexPoly :: setup : - must have a valid world -");
		return;
	}
	
	// these are used to create the shape
	
    shape.Set(vertices, vertexCount);
    
    delete vertices;
    
	fixture.shape		= &shape;
	fixture.density		= density;
	fixture.friction	= friction;
	fixture.restitution	= bounce;
	
	if(density == 0.f)	bodyDef.type	= b2_staticBody;
	else				bodyDef.type	= b2_dynamicBody;
	
	bodyDef.position.Set(x,y);
	
	body  = b2dworld->CreateBody(&bodyDef);
	body->CreateFixture(&fixture);
    
    scale = 1;
}

//------------------------------------------------
void ofxBox2dConvexPoly::setScale(float _scale){
    
    
    if(!isBody()) return;
    
    b2Fixture* fix = body->GetFixtureList();
    
    scale = _scale;
    
    b2PolygonShape* shape = (b2PolygonShape*) fix->GetShape();
    
    for (int i = 0; i < polyPts.size(); i++){
        shape->m_vertices[i].Set(polyPts[i].x*scale, polyPts[i].y*scale); 
    }
    
    
}



void ofxBox2dConvexPoly::addAttractionPoint (ofVec2f pt, float amt) {
    // we apply forces at each vertex. 
    if(body != NULL) {
        const b2Transform& xf = body->GetTransform();
		
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            b2PolygonShape* poly = (b2PolygonShape*)f->GetShape();
            
            if(poly) {
                b2Vec2 P(pt.x/OFX_BOX2D_SCALE, pt.y/OFX_BOX2D_SCALE);
                
                for(int i=0; i<poly->GetVertexCount(); i++) {
                    b2Vec2 qt = b2Mul(xf, poly->GetVertex(i));
                    b2Vec2 D = P - qt; 
                    b2Vec2 F = amt * D;
                    body->ApplyForce(F, P);
                }                    
            }
        }
    }
}


//----------------------------------------
void ofxBox2dConvexPoly::addAttractionPoint (float x, float y, float amt) {
    addAttractionPoint(ofVec2f(x, y), amt);
}

//----------------------------------------
void ofxBox2dConvexPoly::addRepulsionForce(float x, float y, float amt) {
	addRepulsionForce(ofVec2f(x, y), amt);
}
void ofxBox2dConvexPoly::addRepulsionForce(ofVec2f pt, float amt) {
	// we apply forces at each vertex. 
    if(body != NULL) {
        const b2Transform& xf = body->GetTransform();
		
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            b2PolygonShape* poly = (b2PolygonShape*)f->GetShape();
            
            if(poly) {
                b2Vec2 P(pt.x/OFX_BOX2D_SCALE, pt.y/OFX_BOX2D_SCALE);
                
                for(int i=0; i<poly->GetVertexCount(); i++) {
                    b2Vec2 qt = b2Mul(xf, poly->GetVertex(i));
                    b2Vec2 D = P - qt; 
                    b2Vec2 F = amt * D;
                    body->ApplyForce(-F, P);
                }                    
            }
        }
    }
}




//------------------------------------------------
void ofxBox2dConvexPoly::draw() {
	
	if(!isBody()) return;
	
	ofPushMatrix();
	ofTranslate(getPosition().x, getPosition().y, 0);
	ofRotate(getRotation(), 0, 0, 1);
	ofCircle(0, 0, radius);
	
    ofPushStyle();
    ofEnableAlphaBlending();
    ofSetColor(0);
	ofLine(0, 0, radius, 0);
    if(isSleeping()) {
        ofSetColor(255, 100);
        ofCircle(0, 0, radius);
    }
    ofPopStyle();
    
	ofPopMatrix();
    
    
    ofPushMatrix();
	ofTranslate(getPosition().x, getPosition().y, 0);
    
    //cout << "-- p2 " << getPosition() << endl;
    //cout << "-- r2  " << getRotation() << endl;
    
    
    ofRotate(getRotation(), 0, 0, 1);
    ofBeginShape();
    for (int i = 0; i < shape.GetVertexCount(); i++){
        ofVertex(shape.GetVertex(i).x*OFX_BOX2D_SCALE*scale, shape.GetVertex(i).y*OFX_BOX2D_SCALE*scale);
    }
    ofEndShape();
    ofPopMatrix();
    
//    const b2Vec2& GetVertex(int32 index) const;
    
    
	
}



//
//
//
//
///// A convex polygon. It is assumed that the interior of the polygon is to
///// the left of each edge.
//class b2PolygonShape : public b2Shape
//{
//public:
//	b2PolygonShape();
//    
//	/// Implement b2Shape.
//	b2Shape* Clone(b2BlockAllocator* allocator) const;
//    
//	/// Copy vertices. This assumes the vertices define a convex polygon.
//	/// It is assumed that the exterior is the the right of each edge.
//	void Set(const b2Vec2* vertices, int32 vertexCount);
//    
//	/// Build vertices to represent an axis-aligned box.
//	/// @param hx the half-width.
//	/// @param hy the half-height.
//	void SetAsBox(float32 hx, float32 hy);
//    
//	/// Build vertices to represent an oriented box.
//	/// @param hx the half-width.
//	/// @param hy the half-height.
//	/// @param center the center of the box in local coordinates.
//	/// @param angle the rotation of the box in local coordinates.
//	void SetAsBox(float32 hx, float32 hy, const b2Vec2& center, float32 angle);
//    
//	/// Set this as a single edge.
//	void SetAsEdge(const b2Vec2& v1, const b2Vec2& v2);
//    
//	/// @see b2Shape::TestPoint
//	bool TestPoint(const b2Transform& transform, const b2Vec2& p) const;
//    
//	/// Implement b2Shape.
//	bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input, const b2Transform& transform) const;
//    
//	/// @see b2Shape::ComputeAABB
//	void ComputeAABB(b2AABB* aabb, const b2Transform& transform) const;
//    
//	/// @see b2Shape::ComputeMass
//	void ComputeMass(b2MassData* massData, float32 density) const;
//    
//	/// Get the supporting vertex index in the given direction.
//	int32 GetSupport(const b2Vec2& d) const;
//    
//	/// Get the supporting vertex in the given direction.
//	const b2Vec2& GetSupportVertex(const b2Vec2& d) const;
//    
//	/// Get the vertex count.
//	int32 GetVertexCount() const { return m_vertexCount; }
//    
//	/// Get a vertex by index.
//	const b2Vec2& GetVertex(int32 index) const;
//    
//	b2Vec2 m_centroid;
//	b2Vec2 m_vertices[b2_maxPolygonVertices];
//	b2Vec2 m_normals[b2_maxPolygonVertices];
//	int32 m_vertexCount;
//};
//
//inline b2PolygonShape::b2PolygonShape()
//{
//	m_type = e_polygon;
//	m_radius = b2_polygonRadius;
//	m_vertexCount = 0;
//	m_centroid.SetZero();
//}
//
//inline int32 b2PolygonShape::GetSupport(const b2Vec2& d) const
//{
//	int32 bestIndex = 0;
//	float32 bestValue = b2Dot(m_vertices[0], d);
//	for (int32 i = 1; i < m_vertexCount; ++i)
//	{
//		float32 value = b2Dot(m_vertices[i], d);
//		if (value > bestValue)
//		{
//			bestIndex = i;
//			bestValue = value;
//		}
//	}
//    
//	return bestIndex;
//}
//
//inline const b2Vec2& b2PolygonShape::GetSupportVertex(const b2Vec2& d) const
//{
//	int32 bestIndex = 0;
//	float32 bestValue = b2Dot(m_vertices[0], d);
//	for (int32 i = 1; i < m_vertexCount; ++i)
//	{
//		float32 value = b2Dot(m_vertices[i], d);
//		if (value > bestValue)
//		{
//			bestIndex = i;
//			bestValue = value;
//		}
//	}
//    
//	return m_vertices[bestIndex];
//}
//
//inline const b2Vec2& b2PolygonShape::GetVertex(int32 index) const
//{
//	b2Assert(0 <= index && index < m_vertexCount);
//	return m_vertices[index];
//}
//
//
//






