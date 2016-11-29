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
    void draw();
    void setSize(float newSize) { size = newSize; };
    void setPosition(float x, float y);
    
    float getSize() { return size; };
    int getType() { return stickerType; };
    bool inside(float x, float y);
    
private:
    int stickerType;
    float size;
    ofImage icon;
    ofRectangle border;
    ofColor color;
    ofVec2f pos;
};

#endif /* defined(____Sticker__) */
