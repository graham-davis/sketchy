//
//  Pixel.cpp
//  
//
//  Created by Graham Davis on 11/21/16.
//
//

#include "Pixel.h"

Pixel::Pixel(){}

Pixel::~Pixel(){}

void Pixel::setPosition(ofVec3f newPos) {
    pos = newPos;
}

void Pixel::setVelocity(ofVec3f newVel) {
    vel = newVel;
}

void Pixel::setSize(float newRadius) {
    radius = newRadius;
}

void Pixel::setColor(ofColor newColor) {
    color = newColor;
}

void Pixel::draw(float ww, float wh){
    if (dissolving) {
        radius = radius * 0.9;
    }
    pos[0] = pos[0]+vel[0];
    pos[1] = pos[1]+vel[1];
    pos[2] = pos[2]+vel[2];
    ofSetColor(color);
    ofDrawCircle(pos[0]*ww, pos[1]*wh, pos[2], radius);
}