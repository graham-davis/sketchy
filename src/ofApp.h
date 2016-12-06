#pragma once

#include "ofMain.h"
#include "ofxStk.h"
#include "math.h"
#include "pixel.h"
#include "Sticker.h"
#include "FaustReverb.h"
#include "smooth.h"
#include "ofGLRenderer.h"

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
    void exit();

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
// PRIVATE MEMBER FUNCTIONS
    stk::StkFrames drawAudio(int bufferSize, int j, float gain);
    stk::StkFrames redrawAudio(int bufferSize);
    void readFiles();
    void drawTopNav();
    void drawToolbox();
    void drawFullGrid();
    void drawPixels();
    void redrawPixels();
    void drawStroke(int index);
    void resetStroke(int index);
    void drawSizeSlider();
    void drawStickerbox();
    void drawStickers();
    void checkNearbyStickers(Pixel *pixel);
    void clearStrokes();
    void clearSingleStroke(int index);
    void saveFile();
    void loadFile();
    
// AUDIO VARIABLES
    bool audioReady;
    ofSoundStream soundStream;
    
    // Granular variables
    vector<stk::Granulate> granulators;
    float grainSize;
    int grainStart;
    
    // Reverb variables
    stk::FreeVerb reverb;
    
    // Pitch shift variables
    vector<stk::PitShift> shifters;
    
    // Chorus
    stk::Chorus chorus;
    
    // Smoothers
    vector<Smooth> onOff;
    Smooth redrawSmooth;
    float redrawGain;
    vector<float> drawGain;
    
// WINDOW VARIABLES
    float ww;
    float wh;
    float gutterWidth;
    float gutterHeight;
    float mouseX;
    float mouseY;
    float mouseVel;
    
// COLORS
    vector<ofColor> colors;
    ofColor offWhite;
    ofColor lightGrey;
    ofColor mediumGrey;
    ofColor darkGrey;
    
// FONT
    
    ofTrueTypeFont titleFont;
    
// TOP NAVIGATION
    ofRectangle topNav;
    float topNavHeight;
    
    // Icons
    float topNavIconHeight;
    
    ofImage tileIcon;
    ofImage settingsIcon;
    ofImage stickersIcon;
    ofImage undoIcon;
    ofImage trashIcon;
    
    // Clickable icon ghosts
    ofRectangle tileGhost;
    ofRectangle settingsGhost;
    ofRectangle stickersGhost;
    ofRectangle undoGhost;
    ofRectangle trashGhost;
    
    // Interaction booleans
    bool drawGrid;
    bool drawSettings;
    bool drawStickerMenu;
    bool trashing;
    bool undoing;

// TOOLBOX
    
    ofRectangle toolbox;
    float toolboxHeight;
    float toolboxStart;
    float toolboxWidth;
    
    // Texture boxes
    int numTextures;
    int selectedTexture;
    vector<ofRectangle> textureBoxes;
    vector<float> playTexture;
    float textureBoxWidth;
    
    // Slider
    float sliderHeight;
    float sliderPosition;
    float sliderWidth;
    float sliderStart;
    ofRectangle sliderCircle;
    bool sliding;
    
// STICKERS
    vector<Sticker> stickerSelectors;
    vector<Sticker> canvasStickers;
    ofRectangle stickerBox;
    bool placingSticker;
    Sticker brushSticker;
    float stickerBoxHeight;
    float stickerIconSize;
    int stickersAdded;
    
// BRUSH
    ofPath brush;
    float brushRadius;
    float minBrushRadius;
    float maxBrushRadius;
    float opacity;
    
// STROKES AND DRAWING
    
    int redraw;
    bool clickCheck;
    bool drawing;
    int strokesToDissolve;
    
    // Stroke struct
    struct stroke {
        int length;
        vector<Pixel> pixels;
        int textureType;
        
        // Redraw and playback
        int redrawing;
        bool redrawFinished;
        int playbackPixel;

        // Destruction
        bool dissolving;
        
        // Sticker influence variables
        float macros[5];
        int repeats;
        float delay;
        int stutter;
        
        // Associated stickers
        vector<Sticker> stickers;
        int numStickers;
        
        Smooth env;
    };
    vector<bool> redrawingTexture;
    vector<int> strokesToRedraw;
    int maxStrokes;
    int maxPixelsPerStroke;
    int maxStickersPerStroke;
    int currentStroke;
    
    vector<stroke> strokes;
    
    
// KEYBOARD
    
    bool shiftKey;
 
// MUTEX
    
    mutex redrawLock;
};
