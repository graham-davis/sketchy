//
//  Pixel.cpp
//  
//
//  Created by Graham Davis on 11/21/16.
//
//

#include "Pixel.h"

Pixel::Pixel(){
    radius = 0;
    opacity = 0;
    vel = ofVec3f(0, 0, 0);
    dissolving = false;
}

Pixel::~Pixel(){}

void Pixel::setPosition(ofVec3f newPos) {
    pos = newPos;
}

void Pixel::setVelocity(float x, float y, float z) {
    vel = ofVec3f(x, y, z);
}

void Pixel::setSize(float newRadius) {
    radius = newRadius;
}

void Pixel::setOpacity(float newOpacity) {
    opacity = newOpacity;
}

void Pixel::setColor(ofColor newColor) {
    color = newColor;
}

void Pixel::draw(float ww, float wh){
    if (dissolving) {
        radius = radius * 0.85;
    }
    ofSetColor(color, 255*opacity);
    ofDrawCircle(pos[0]*ww, pos[1]*wh, pos[2], radius);
}