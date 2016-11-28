//
//  Pixel.h
//  
//
//  Created by Graham Davis on 11/21/16.
//
//

#ifndef ____Pixel__
#define ____Pixel__

#include <stdio.h>
#include "ofGraphics.h"

class Pixel {
public:
    Pixel();
    ~Pixel();
    void setPosition(ofVec3f newPos);
    void setVelocity(ofVec3f newVel);
    void setSize(float newRadius);
    void setColor(ofColor newColor);
    void setOpacity(float newOpacity);
    void draw(float ww, float wh);
    
    void dissolve() { dissolving = true; };
    ofVec3f getPosition() { return pos; };
    float getRadius() { return radius; };
    
private:
    float radius;
    float opacity;
    ofColor color;
    ofVec3f pos;
    ofVec3f vel;
    bool dissolving;
};

#endif /* defined(____Pixel__) */
