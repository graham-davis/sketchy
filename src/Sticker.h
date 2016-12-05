//
//  Sticker.h
//  
//
//  Created by Graham Davis on 11/28/16.
//
//

#ifndef ____Sticker__
#define ____Sticker__

#include <stdio.h>
#include "ofMain.h"

class Sticker {
public:
    Sticker();
    ~Sticker();
    
    void setType(int type);
    void draw(float ww, float wh);
    void setPosition(float x, float y);
    void pop();
    
    // Inline definition of sticker functions
    void setSize(float newSize) { size = newSize; };
    float getSize() { return size; };
    int getType() { return stickerType; };
    ofVec2f getPosition() { return pos; };
    bool inside(float x, float y);
    bool isPopped() { return popped; };
    
private:
    int stickerType;
    float size;
    ofImage icon;
    bool popped;
    bool popping;
    ofRectangle border;
    ofColor color;
    ofVec2f pos;
};

#endif /* defined(____Sticker__) */
