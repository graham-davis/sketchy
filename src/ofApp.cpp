#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Initialize audio variables
    soundStream.setup(this, MY_CHANNELS, MY_CHANNELS, MY_SRATE, MY_BUFFERSIZE, MY_NBUFFERS);
    audioReady = false;
    numTextures = 5;
    
    // Initialize top nav variables
    tileIcon.load(ofToDataPath("tile.png", true));
    settingsIcon.load(ofToDataPath("settings.png", true));
    stickersIcon.load(ofToDataPath("sticker.png", true));
    tileIconHeight = 50;
    
    // Initialize program colors
    colors.resize(numTextures);
    colors[0] = ofColor(242, 95, 92);
    colors[1] = ofColor(255, 224, 102);
    colors[2] = ofColor(36, 123, 160);
    colors[3] = ofColor(112, 193, 179);
    colors[4] = ofColor(80, 81, 79);
    offWhite = ofColor(240, 240, 240);
    lightGrey = ofColor(220, 220, 220);
    darkGrey = ofColor(160, 160, 160);
    
    mouseX = 0;
    mouseY = 0;
    
    // Initialize GUI varialbes
    ww = 0;
    wh = 0;
    bufferWidth = 16;
    bufferHeight = 8;
    
    topNavHeight = 100;
    
    toolboxHeight = 220;
    toolboxStart = 4 * bufferWidth;
    toolboxWidth = ww-(bufferWidth*8);
    textureBoxWidth = (toolboxWidth/2)/numTextures - (bufferWidth);
    sliderHeight = 4;
    sliderPosition = 0;
    sliding = false;
    sliderStart = bufferWidth*4;
    selectedTexture = -1;
    
    drawGrid = false;
    drawSettings = true;
    drawStickers = false;
    
    textureBoxes.resize(numTextures);
    
    // Initialize font
    verdana.load("verdana.ttf", 40);
    verdana.setLineHeight(40.0f);
    verdana.setLetterSpacing(1.035);
    
    // Initialize brush variables
    minBrushRadius = 8;
    maxBrushRadius = 36;
    brushRadius = 20;
    opacity = 1;
    
    // Initialize drawing variables
    canDraw = false;
    mouseDown = false;
    maxDrawnElements = 10000;
    elementsDrawn = 0;
    pixels.resize(10000);
    
    // Read audio files into buffers
    readFiles();
    
    ofSetFrameRate(120);
}

//--------------------------------------------------------------
void ofApp::update(){
    ww = ofGetWindowWidth();
    wh = ofGetWindowHeight();
    
    mouseX = ofGetMouseX();
    mouseY = ofGetMouseY();
    
    toolboxHeight = wh * .15;
    sliderWidth = (toolbox.getWidth()/2) - (sliderStart * 2);
    
    brushRadius = minBrushRadius + (sliderPosition * maxBrushRadius);
    
    // Handle whether we can draw based on mouse location
    if ((drawSettings && toolbox.inside(mouseX, mouseY)) || mouseY <= topNavHeight) {
        canDraw = false;
    } else {
        canDraw = true;
    }
    
    // If drawing is allowed and mouse is down, add pixel
    if (selectedTexture != -1 && canDraw && mouseDown && elementsDrawn < maxDrawnElements && !dissolvingPixels) {
        Pixel newPixel;
        newPixel.setPosition(ofVec3f(mouseX/ww, mouseY/wh, 0));
        newPixel.setVelocity(ofVec3f(0, 0, 0));
        newPixel.setSize(brushRadius);
        newPixel.setColor(colors[selectedTexture]);
        newPixel.setOpacity(opacity);
        pixels[elementsDrawn] = newPixel;
        elementsDrawn++;
    }
}

//----------------Graphics Methods------------------------------
void ofApp::draw(){
    ofBackgroundGradient(offWhite, lightGrey);
    if (drawGrid) drawFullGrid();
    drawPixels();
    drawTopNav();
    if (drawSettings) drawToolbox();
    if (drawStickers) drawStickerbox();
    
    // Only draw brush if texture is selected
    if (selectedTexture != -1 && canDraw && !dissolvingPixels) {
        ofSetColor(colors[selectedTexture], 256*opacity);
        ofDrawCircle(mouseX, mouseY, brushRadius);
    }
}

void ofApp::drawPixels() {
    int i = 0;
    while (i < elementsDrawn) {
        ofVec3f pixelPos = pixels[i].getPosition();
        float pixelRad = pixels[i].getRadius();
        if (pixelPos[0] < 0 || pixelPos[0] >  1 || pixelPos[1] < 0 || pixelPos[1] > 1 || pixelRad < 0.5) {
            pixels[i] = pixels[elementsDrawn];
            elementsDrawn--;
            if (elementsDrawn == 0 && dissolvingPixels) dissolvingPixels = false;
        } else {
            pixels[i].draw(ww, wh);
            i++;
        }
    }
}

void ofApp::drawFullGrid() {
    ofSetColor(100, 100, 100, 80);
    for(int i=0; i<20; i++) {
        // Vertical lines
        ofDrawLine((ww/20)*i, topNavHeight, (ww/20)*i, wh);
        
        // Horizontal lines
        ofDrawLine(0, (topNavHeight+(wh*(i+1))/20), ww, (topNavHeight+(wh*(i+1))/20));
    }
}

void ofApp::drawTopNav() {
    ofSetColor(offWhite);
    topNav.set(0, 0, ww, topNavHeight);
    tileGhost.set(ww-tileIconHeight-bufferWidth*2, (topNavHeight/2)-(tileIconHeight/2), tileIconHeight, tileIconHeight);
    settingsGhost.set(ww-(tileIconHeight*2)-(bufferWidth*4), (topNavHeight/2)-(tileIconHeight/2), tileIconHeight, tileIconHeight);
    stickersGhost.set(ww-(tileIconHeight*3)-(bufferWidth*6), (topNavHeight/2)-(tileIconHeight/2), tileIconHeight, tileIconHeight);

    ofDrawRectangle(topNav);
    ofDrawRectangle(tileGhost);
    ofDrawRectangle(settingsGhost);
    ofDrawRectangle(stickersGhost);
    
    ofSetColor(colors[0]);
    verdana.drawString("Sketchy", bufferWidth*2, (topNavHeight/2)+20);
    
    // Draw grid icon
    if (!drawGrid) ofSetColor(lightGrey);
    tileIcon.draw(ww-tileIconHeight-bufferWidth*2, (topNavHeight/2)-(tileIconHeight/2), tileIconHeight, tileIconHeight);
    
    // Draw settings icon
    if (!drawSettings) {
        ofSetColor(lightGrey);
    } else {
        ofSetColor(colors[0]);
    }
    settingsIcon.draw(ww-(tileIconHeight*2)-(bufferWidth*4), (topNavHeight/2)-(tileIconHeight/2), tileIconHeight, tileIconHeight);
    
    //Draw stickers icon
    if (!drawStickers) {
        ofSetColor(lightGrey);
    } else {
        ofSetColor(colors[0]);
    }
    stickersIcon.draw(ww-(tileIconHeight*3)-(bufferWidth*6), (topNavHeight/2)-(tileIconHeight/2), tileIconHeight, tileIconHeight);
}

void ofApp::drawToolbox() {
    ofSetColor(offWhite, 190);
    toolboxWidth = ww-(bufferWidth*8);
    toolbox.set(toolboxStart, wh-toolboxHeight, toolboxWidth, toolboxHeight);
    ofDrawRectangle(toolbox);
    
    ofPoint toolboxCenter = toolbox.getCenter();
    textureBoxWidth = (toolboxWidth/(2*numTextures)) - (2*bufferWidth);

    for(int i=0; i<numTextures; i++) {
        if (selectedTexture == i) {
            ofSetColor(colors[i]);
        } else {
            ofSetColor(colors[i], 140);
        }
        textureBoxes[i].set(toolboxCenter[0]+(textureBoxWidth*(i))+(bufferWidth*(i+1)), wh-(toolboxHeight-2*bufferHeight), textureBoxWidth, toolboxHeight-(4*bufferHeight));
        ofDrawRectangle(textureBoxes[i]);
    }
    drawSizeSlider();
}

void ofApp::drawSizeSlider() {
    ofSetColor(lightGrey);
    ofDrawRectRounded(sliderStart + toolboxStart, wh - (toolboxHeight / 2) - (sliderHeight/2), sliderWidth, sliderHeight, 10);
    
    if (selectedTexture == -1) {
        ofSetColor(darkGrey);
    } else {
        ofSetColor(colors[selectedTexture], 256*opacity);
    }
    sliderCircle.set(toolboxStart + sliderStart + (sliderWidth * sliderPosition), wh - (toolboxHeight / 2) - brushRadius, brushRadius * 2, brushRadius * 2);
    ofDrawRectRounded(sliderCircle, (brushRadius));
}


void ofApp::drawStickerbox() {
    ofSetColor(offWhite, 190);
    stickerBox.set(toolboxStart, topNavHeight, toolboxWidth, toolboxHeight);
    ofDrawRectangle(stickerBox);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==127) {
        for(int i = 0; i < elementsDrawn; i++) {
            pixels[i].dissolve();
            pixels[i].setVelocity(ofVec3f(ofRandom(-.05, .05), ofRandom(-.05, .05), ofRandom(-5, 5)));
            dissolvingPixels = true;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (sliding) {
        sliderPosition = (x - toolboxStart - sliderStart - brushRadius) / sliderWidth;
        if (sliderPosition > 0.9) sliderPosition = 0.9;
        if (sliderPosition < 0) sliderPosition = 0;
        
        opacity += (-1*(y - wh + (toolboxHeight / 2))) / (toolboxHeight*4);
        if (opacity > 1) opacity = 1;
        if (opacity < 0.15) opacity = 0.15;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mouseDown = true;
    if (drawSettings) {
        for (int i=0; i<numTextures; i++) {
            if (textureBoxes[i].inside(x, y)) {
                selectedTexture = (selectedTexture == i) ? -1 : i;
            }
        }
    }
    
    if (tileGhost.inside(x, y)) {
        drawGrid = !drawGrid;
    }
    
    if (settingsGhost.inside(x, y)) {
        drawSettings = !drawSettings;
    }
    
    if (stickersGhost.inside(x, y)) {
        drawStickers = !drawStickers;
    }
    
    if (canDraw && selectedTexture != -1) {
        playTexture[selectedTexture] = true;
    }
    
    if (sliderCircle.inside(x, y)) {
        sliding = true;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mouseDown = false;
    if (canDraw && selectedTexture != -1) {
        playTexture[selectedTexture] = false;
        cursors[selectedTexture] = 0;
    }
    
    if (sliding) {
        sliding = false;
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


//----------------------Audio Methods---------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    if (audioReady && files.size() == numTextures && cursors.size() == numTextures) {
        float left = 0;
        float right = 0;
        float w = 0;
        for (int i = 0; i < bufferSize; i++) {
            for (int j = 0; j < numTextures; j++) {
                if (playTexture[j]) {
                    int start = (mouseX / ww) * files[j].getSize();
                    int end = start + brushRadius*100;
                    if(cursors[j]+start > end || cursors[j]+start > files[j].getSize()) {
                        cursors[j] = 0;
                    }
                    w = 0.5 * (1 - cos((2 * pi * cursors[j])) / (end - 1));
                    left = textures[j](cursors[j]+start, 0);
                    right = textures[j](cursors[j]+start, 1);
                    cursors[j]++;
                }
            }
            output[2*i] = left;
            output[2*i+1] = right;
        }
    }
}

void ofApp::readFiles(){
    stk::Stk::setSampleRate(MY_SRATE);
    
    // Resize file/signal vectors
    files.resize(numTextures);
    textures.resize(numTextures);
    cursors.resize(numTextures);
    playTexture.resize(numTextures);
    
    for (int i=0; i<numTextures; i++) {
        files[i].setRate(MY_SRATE);
    }
    
    files[0].openFile(ofToDataPath("cymbal.wav", true));
    files[1].openFile(ofToDataPath("chimes.wav", true));
    files[2].openFile(ofToDataPath("choir.wav", true));
    files[3].openFile(ofToDataPath("whistle.wav", true));
    files[4].openFile(ofToDataPath("clap.wav", true));

    for (int i=0; i<numTextures; i++) {
        textures[i] = stk::StkFrames(files[i].getSize(), 2);
        files[i].tick(textures[i]);
        cursors[i] = 0;
        playTexture[i] = false;
    }
    
    audioReady = true;
}

