#pragma once

#include "ofMain.h"
#include "ofxStk.h"

//-----------------------------------------------------------------------------
// Preprocessor definitions
//-----------------------------------------------------------------------------
#define MY_SRATE         44100            // sample rate
#define MY_CHANNELS      2                // number of channels
#define MY_BUFFERHISTORY 50               // number of buffers to save
#define MY_BUFFERSIZE    512              // number of frames in a buffer
#define MY_NBUFFERS      2                // number of buffers latency

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void audioOut(float * input, int bufferSize, int nChannels);
    
private:
    // Private member functions
    void readFiles();
    
    // Private variables
    bool audioReady;
    ofSoundStream soundStream;
    
    int numTextures;
    
    vector<stk::StkFrames> textures;
    
    vector<stk::FileWvIn> files;
    
    vector<int> cursors;
    
    vector<bool> playTexture;
    
    vector<ofColor> colors;
    
    ofTrueTypeFont	verdana30;
};
