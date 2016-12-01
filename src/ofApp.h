#pragma once

#include "ofMain.h"
#include "ofxStk.h"
#include "math.h"
#include "pixel.h"
#include "Sticker.h"
#include "FaustReverb.h"
#include "smooth.h"

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
    void drawAudio(float *input, int bufferSize, int j, float gain);
    void redrawAudio(float *input, int bufferSize);
    void readFiles();
    void drawTopNav();
    void drawToolbox();
    void drawFullGrid();
    void drawPixels();
    void redrawPixels();
    void drawSizeSlider();
    void drawStickerbox();
    void drawStickers();
    void checkNearbyStickers(Pixel *pixel);
    void clearStrokes();
    
    // Audio Variables
    bool audioReady;
    ofSoundStream soundStream;
    
    // Mouse Variables
    float mouseX;
    float mouseY;
    
    // Texture variables
    int numTextures;
    vector<float> playTexture;
    
    // GUI/Color Variables
    vector<ofColor> colors;
    ofColor offWhite;
    ofColor lightGrey;
    ofColor darkGrey;
    
    ofRectangle topNav;
    ofRectangle toolbox;
    ofRectangle stickerBox;
    vector<ofRectangle> textureBoxes;
    int selectedTexture;
    
    // Brush variables
    ofPath brush;
    float brushRadius;
    float minBrushRadius;
    float maxBrushRadius;
    float opacity;
    
    float ww;
    float wh;
    float bufferWidth;
    float bufferHeight;
    
    // Top nav variables
    float topNavHeight;
    ofImage tileIcon;
    ofImage settingsIcon;
    ofImage stickersIcon;
    float tileIconHeight;
    bool drawGrid;
    bool drawSettings;
    bool drawStickerMenu;
    
    ofRectangle tileGhost;
    ofRectangle settingsGhost;
    ofRectangle stickersGhost;
    
    // Toolbox variables
    float toolboxHeight;
    float toolboxStart;
    float toolboxWidth;
    float textureBoxWidth;
    
    // Slider Variables
    float sliderHeight;
    float sliderPosition;
    float sliderWidth;
    float sliderStart;
    ofRectangle sliderCircle;
    bool sliding;
    
    // Font variables
    ofTrueTypeFont verdana;
    
    // Drawing variables
    bool redraw;
    int redrawSampleCount;
    
    bool canDraw;
    bool drawing;
    bool mouseDown;
    int maxDrawnElements;
    int elementsDrawn;
    bool dissolvingPixels;
    vector<Pixel> pixels;
    
    // Sticker variables
    bool placingSticker;
    Sticker brushSticker;
    float stickerBoxHeight;
    float stickerIconSize;
    int stickersAdded;
    
    vector<Sticker> stickerSelectors;
    vector<Sticker> canvasStickers;
    
    // Stroke variables
    struct stroke {
        int length;
        vector<Pixel> pixels;
    };
    int maxStrokes;
    int maxPixelsPerStroke;
    int currentStroke;
    int currentStrokePixel;
    int maxStrokeLength;
    int redrawPixel;
    
    vector<stroke> strokes;
    
    // Granular variables
    vector<stk::Granulate> granulators;
    float grainSize;
    int grainStart;
    
    // Reverb variables
    stk::FreeVerb reverb;
    stk::StkFrames outputFrames;
    
    // Pitch shift variables
    stk::PitShift shift;
    
    // Smoothers
    vector<Smooth> onOff;
    float gainValue;
};
