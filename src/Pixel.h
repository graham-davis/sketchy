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
    void setVelocity(float x, float y, float z);
    void setSize(float newRadius);
    void setColor(ofColor newColor);
    void setOpacity(float newOpacity);
    void draw(float ww, float wh);
    void setType(int newType) { pixelType = newType; };
    
    void dissolve() { dissolving = true; };
    ofVec3f getPosition() { return pos; };
    float getRadius() { return radius; };
    int getType() { return pixelType; };
    float getOpacity() { return opacity; };
    
private:
    float radius;
    float opacity;
    ofColor color;
    ofVec3f pos;
    ofVec3f vel;
    bool dissolving;
    int pixelType;
};

#endif /* defined(____Pixel__) */
