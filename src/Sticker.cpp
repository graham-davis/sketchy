//
//  Sticker.cpp
//  
//
//  Created by Graham Davis on 11/28/16.
//
//

#include "Sticker.h"

Sticker::Sticker(){
    popped = false;
    popping = false;
}

Sticker::~Sticker(){}

void Sticker::setType(int type) {
    switch(type) {
        case 0:
            stickerType = 0;
            color = ofColor(242, 95, 92);
            icon.load(ofToDataPath("sunSketch.png", true));
            break;
        case 1:
            stickerType = 1;
            color = ofColor(255, 224, 102);
            icon.load(ofToDataPath("lightningSketch.png", true));
            break;
        case 2:
            stickerType = 2;
            color = ofColor(36, 123, 160);
            icon.load(ofToDataPath("rainSketch.png", true));
            break;
        case 3:
            stickerType = 3;
            color = ofColor(112, 193, 179);
            icon.load(ofToDataPath("windSketch.png", true));
            break;
        case 4:
            stickerType = 4;
            color = ofColor(80, 81, 79);
            icon.load(ofToDataPath("vortexSketch.png", true));
            break;
        default:
            break;
    }
}

void Sticker::setPosition(float x, float y) {
    pos = ofVec2f(x, y);
}

void Sticker::draw(float ww, float wh){
    if (size < 0.05) {
        popped = true;
    } else {
        if (popping) {
            size = size * 0.8;
        }
        ofSetColor(color);
        border.set(pos[0]*ww, pos[1]*wh, size, size);
        ofDrawRectRounded(border, size/2);
        
        float innerSize = size * 0.75;
        
        ofSetColor(ofColor(240, 240, 240));
        icon.draw((pos[0]*ww) + ((size - innerSize) / 2), (pos[1]*wh) + ((size - innerSize) / 2), innerSize, innerSize);
    }
}

bool Sticker::inside(float x, float y) {
    return border.inside(x, y);
}

void Sticker::pop() {
    popping = true;
}
