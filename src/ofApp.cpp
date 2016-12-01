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
    
    // Initialize sticker variables
    stickerIconSize = 75;
    stickerBoxHeight = 100;
    stickerSelectors.resize(5);
    canvasStickers.resize(30);
    stickersAdded = 0;
    for (int i=0; i<5; i++) {
        Sticker sticker;
        sticker.setSize(stickerIconSize);
        sticker.setType(i);
        stickerSelectors[i] = sticker;
    }
    
    // Initialize stroke variables
    maxStrokes = 100;
    maxPixelsPerStroke = 1000;
    currentStroke = 0;
    currentStrokePixel = 0;
    maxStrokeLength = 0;
    redrawPixel = 0;
    strokes.resize(maxStrokes);
    for (int i=0; i<maxStrokes; i++) {
        strokes[i].pixels.resize(maxPixelsPerStroke);
    }
    
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
    mouseVel = 0;
    
    // Initialize GUI variables
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
    sliderPosition = 0.5;
    sliding = false;
    sliderStart = bufferWidth*4;
    selectedTexture = -1;
    
    drawGrid = false;
    drawSettings = true;
    drawStickerMenu = false;
    
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
    redraw = false;
    canDraw = false;
    drawing = false;
    mouseDown = false;
    maxDrawnElements = 10000;
    elementsDrawn = 0;
    pixels.resize(10000);
    
    // Initialize smoothing parameters
    onOff.resize(numTextures);
    for (int i=0; i<numTextures; i++) {
        onOff[i].setSmooth(.8);
    }
    redrawSmooth.setSmooth(.8);
    redrawGain = 0;
    
    // Initialize reverb
    reverb.setEffectMix(0.3);
    reverb.setRoomSize(0.6);
    
    // Initialize pitch shift
    shift.clear();
    shift.setShift(1.0);
    
    // Read audio files into buffers
    readFiles();
    
    ofSetFrameRate(120);
}

//--------------------------------------------------------------
void ofApp::update(){
    ww = ofGetWindowWidth();
    wh = ofGetWindowHeight();
    
    mouseVel = sqrt(pow((ofGetMouseX() - mouseX), 2) + pow((ofGetMouseY() - mouseY), 2));
    if (mouseVel < 5) mouseVel = 5;
    if (mouseVel > 50) mouseVel = 50;
    mouseX = ofGetMouseX();
    mouseY = ofGetMouseY();
    
    toolboxHeight = wh * .2;
    stickerIconSize = stickerBoxHeight * 0.75;
    sliderWidth = (toolbox.getWidth()/2) - (sliderStart * 2);
    
   // brushRadius = mouseVel;
    brushRadius = minBrushRadius + (sliderPosition * maxBrushRadius);
    
    // Handle whether we can draw based on mouse location
    if (redraw || placingSticker || (drawSettings && toolbox.inside(mouseX, mouseY)) || mouseY <= topNavHeight || (drawStickerMenu && stickerBox.inside(mouseX, mouseY))) {
        canDraw = false;
    } else {
        canDraw = true;
    }
    
    // Check if we should still be redrawing
    redrawLock.lock();
    if (redrawPixel >= maxStrokeLength) {
        redraw = 0;
        
        // Clear reverb delay lines
        if (maxStrokeLength != 0) {
            reverb.clear();
        }
    }
    redrawLock.unlock();
    
    // If placing sticker, update brush sticker position
    if (placingSticker && !redraw) {
        brushSticker.setPosition((mouseX - (stickerIconSize/2))/ww, (mouseY - (stickerIconSize/2))/wh);
    }
    
    // If drawing is allowed and mouse is down, add pixel
    if (selectedTexture != -1 && canDraw && mouseDown && elementsDrawn < maxDrawnElements && !dissolvingPixels) {
        drawing = true;
        
        // Build new Pixel
        Pixel newPixel;
        newPixel.setPosition(ofVec3f(mouseX/ww, mouseY/wh, 0));
        newPixel.setVelocity(0, 0, 0);
        newPixel.setSize(brushRadius);
        newPixel.setColor(colors[selectedTexture]);
        newPixel.setOpacity(opacity);
        newPixel.setType(selectedTexture);
        
        // Add pixel to list of drawn elements
        pixels[elementsDrawn] = newPixel;
        elementsDrawn++;
        
        // Update current Stroke
        strokes[currentStroke%maxStrokes].pixels[currentStrokePixel%maxPixelsPerStroke] = newPixel;
        strokes[currentStroke%maxStrokes].length++;
        currentStrokePixel++;
    }
}

//----------------Graphics Methods------------------------------
void ofApp::draw(){
    ofBackgroundGradient(offWhite, lightGrey);
    if (drawGrid) drawFullGrid();
    if (redraw) {
        redrawPixels();
    } else {
        drawPixels();
    }
    drawStickers();
    drawTopNav();
    if (drawSettings) drawToolbox();
    if (drawStickerMenu) drawStickerbox();
    
    // Only draw brush if texture is selected, pixels are not dissolving, and sticker isn't being placed
    if (placingSticker) {
        brushSticker.draw(ww, wh);
    } else if (selectedTexture != -1 && canDraw && !dissolvingPixels) {
        ofSetColor(colors[selectedTexture], 256*opacity);
        ofDrawCircle(mouseX, mouseY, brushRadius);
    }
}

void ofApp::drawStickers() {
    for (int i=0; i<stickersAdded; i++) {
        canvasStickers[i].draw(ww, wh);
    }
}

void ofApp::redrawPixels() {
    for (int i=0; i<currentStroke; i++) {
        for (int j=0; j<redrawPixel; j++)
        strokes[i].pixels[j].draw(ww, wh);
    }
    redrawLock.lock();
    redrawPixel++;
    redrawLock.unlock();
}

void ofApp::drawPixels() {
    int i = 0;
    while (i < elementsDrawn) {
        ofVec3f pixelPos = pixels[i].getPosition();
        float pixelRad = pixels[i].getRadius();
        if (pixelPos[0] < 0 || pixelPos[0] >  1 || pixelPos[1] < 0 || pixelPos[1] > 1 || pixelRad < 0.5) {
            pixels[i] = pixels[elementsDrawn];
            elementsDrawn--;
            if (elementsDrawn == 0 && dissolvingPixels) {
                dissolvingPixels = false;
            }
        } else {
            pixels[i].draw(ww, wh);
            checkNearbyStickers(&pixels[i]);
            i++;
        }
    }
}

// This function checks a pixel's nearby stickers, and influences the pixel accordingly

void ofApp::checkNearbyStickers(Pixel *pixel) {
    ofVec3f pixelPos = pixel->getPosition();
    for (int i=0; i<stickersAdded; i++) {
        int stickerType = canvasStickers[i].getType();
        ofVec2f stickerPos = canvasStickers[i].getPosition();
        float xDist = pixelPos[0]-stickerPos[0];
        float yDist = pixelPos[1]-stickerPos[1];
        float distance = sqrt((xDist*xDist)+(yDist*yDist));
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
    
    // Draw clickable "ghost" icons
    ofDrawRectangle(tileGhost);
    ofDrawRectangle(settingsGhost);
    ofDrawRectangle(stickersGhost);
    
    // Draw app name
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
    if (!drawStickerMenu) {
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
    sliderCircle.set(toolboxStart + sliderStart + (sliderWidth*sliderPosition), wh - (toolboxHeight/2) - brushRadius, brushRadius*2, brushRadius*2);
    ofDrawRectRounded(sliderCircle, (brushRadius));
}


void ofApp::drawStickerbox() {
    ofSetColor(offWhite, 190);
    stickerBox.set(toolboxStart, topNavHeight, toolboxWidth, stickerBoxHeight);
    ofDrawRectangle(stickerBox);
    
    for (int i = 0; i < 5; i++) {
        stickerSelectors[i].setSize(stickerIconSize);
        float stickerX = (toolboxStart + (bufferWidth*2*(i+1)) + (stickerIconSize*i))/ww;
        float stickerY = (topNavHeight + (stickerBoxHeight/2) - (stickerIconSize/2))/wh;
        stickerSelectors[i].setPosition(stickerX, stickerY);
        stickerSelectors[i].draw(ww, wh);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==32){
        redrawLock.lock();
        if (!redraw) {
            redraw = 1;
            redrawGain = redrawSmooth.tick(redraw);
        } else {
            redraw = 0;
            redrawGain = 0;
        }
        redrawPixel = 0;
        redrawLock.unlock();
    }
    if (key==127 && !redraw) {
        for(int i = 0; i < elementsDrawn; i++) {
            pixels[i].dissolve();
            pixels[i].setVelocity(ofRandom(-.05, .05), ofRandom(-.05, .05), ofRandom(-5, 5));
            dissolvingPixels = true;
        }
        clearStrokes();
    }
}

void ofApp::clearStrokes(){
    for (int i=0; i<currentStroke; i++) {
        strokes[i].pixels.clear();
        strokes[i].pixels.resize(500);
        strokes[i].length = 0;
    }
    maxStrokeLength = 0;
    currentStroke = 0;
    currentStrokePixel = 0;
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
    
    if (drawStickerMenu) {
        for (int i=0; i<5; i++) {
            if (stickerSelectors[i].inside(x,y)) {
                placingSticker = true;
                brushSticker = stickerSelectors[i];
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
        drawStickerMenu = !drawStickerMenu;
    }
    
    if (canDraw && selectedTexture != -1) {
        playTexture[selectedTexture] = 1;
    }
    
    if (sliderCircle.inside(x, y)) {
        sliding = true;
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mouseDown = false;
    if (drawing) {
        if (selectedTexture != -1) playTexture[selectedTexture] = 0;
        currentStroke++;
        if (currentStrokePixel > maxStrokeLength) maxStrokeLength = currentStrokePixel;
        currentStrokePixel = 0;
        drawing = false;
    }
    
    if (sliding) {
        sliding = false;
    }
    
    if (placingSticker) {
        if (!stickerBox.inside(x, y) && !topNav.inside(x, y) && (!toolbox.inside(x, y) || !drawSettings)) {
            Sticker newSticker;
            newSticker = brushSticker;
            canvasStickers[stickersAdded] = newSticker;
            stickersAdded++;
        }
        placingSticker = false;
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
    if (audioReady) {
        if (redrawGain > 0.005) {
            redrawAudio(output, bufferSize);
            redrawGain = redrawSmooth.tick(redraw);
        } else if (redrawGain > 0) {
            redrawGain = 0;
            redrawLock.lock();
            redrawPixel = 0;
            redrawLock.unlock();
            reverb.clear();
            shift.clear();
        } else {
            float gain = 0;
            for (int j = 0; j < numTextures; j++) {
                gain = onOff[j].tick(playTexture[j]);
                if (gain > 0.01) {
                    drawAudio(output, bufferSize, j, gain);
                } else if (gain > 0.005) {
                    reverb.clear();
                    shift.clear();
                }
            }
        }
    }
}

void ofApp::drawAudio(float * output, int bufferSize, int j, float gain) {
    float sample = 0;
    outputFrames.resize(MY_BUFFERSIZE, 2, 0);
    
    // Get granular settings
    grainSize = brushRadius*500;
    float fileSize = granulators[j].fileSize();
    float xRatio = mouseX / ww;
    if (xRatio < 0) xRatio = 0;
    if (xRatio > 0.99) xRatio = 0.99;
    float yRatio = 1 - ((mouseY-topNavHeight) / wh);
    if (yRatio > 1) yRatio = 1;
    if (yRatio < 0) yRatio = 0;
    grainStart = xRatio * fileSize;
    
    // Compute audio
    granulators[j].setGrainParameters(grainSize, 75, 0, 0, grainStart);
    granulators[j].tick(outputFrames);
    shift.setShift(2 * yRatio);
    shift.tick(outputFrames);
    reverb.tick(outputFrames);
    
    for (int i = 0; i < bufferSize; i++) {
        sample = outputFrames(i, 0)*opacity*gain;
        output[i*2] = sample;
        output[(i*2)+1] = sample;
    }
}


void ofApp::redrawAudio(float * output, int bufferSize) {
    float samples[bufferSize];
    for (int l = 0; l < bufferSize; l++) {
        samples[l] = 0.0;
    }
    outputFrames.resize(bufferSize, 2, 0);
    
    for (int i = 0; i < currentStroke; i++) {
        redrawLock.lock();
        if (redrawPixel <= strokes[i].length) {
            Pixel currPixel = strokes[i].pixels[redrawPixel-1];
            ofVec3f position = currPixel.getPosition();
            int pixelType = currPixel.getType();
            int pixelRadius = currPixel.getRadius();
            int grainStart = granulators[pixelType].fileSize()*position[0];

            granulators[pixelType].setGrainParameters(pixelRadius*500, 0, 0, 0, grainStart);
            granulators[pixelType].tick(outputFrames);
            float yRatio = 1 - (position[1] - (topNavHeight/wh));
            if (yRatio > 1) yRatio = 1;
            if (yRatio < 0) yRatio = 0;
            
            shift.setShift(2 * yRatio);
            shift.tick(outputFrames);
            reverb.tick(outputFrames);
            for (int j = 0; j < bufferSize; j++) {
                samples[j] += outputFrames(j, 0)*currPixel.getOpacity()*redrawGain;
            }
        }
        redrawLock.unlock();
    }
    
    for (int k = 0; k < bufferSize; k++) {
        output[k*2] = (samples[k]) / currentStroke;
        output[(k*2)+1] = (samples[k]) / currentStroke;
    }
}

void ofApp::readFiles(){
    stk::Stk::setSampleRate(MY_SRATE);
    
    grainSize = 5000;
    grainStart = 0;
    granulators.resize(numTextures);
    granulators[0].openFile(ofToDataPath("fire.wav", true));
    granulators[1].openFile(ofToDataPath("chimes.wav", true));
    granulators[2].openFile(ofToDataPath("future.wav", true));
    granulators[3].openFile(ofToDataPath("plectrum.wav", true));
    granulators[4].openFile(ofToDataPath("fiddle.wav", true));
    
    // Resize file/signal vectors
    playTexture.resize(numTextures);
    
    for (int i=0; i<numTextures; i++) {
        playTexture[i] = 0;
        granulators[i].setVoices(5);
        granulators[i].setGrainParameters(grainSize, 50, 0, 0, grainStart);
    }
    
    audioReady = true;
}

