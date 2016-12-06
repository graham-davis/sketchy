 #include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
// AUDIO VARIABLES
    
    soundStream.setup(this, MY_CHANNELS, MY_CHANNELS, MY_SRATE, MY_BUFFERSIZE, MY_NBUFFERS);
    audioReady = false;
    numTextures = 5;
    
    // Smoothers and gain
    onOff.resize(numTextures);
    for (int i=0; i<numTextures; i++) {
        onOff[i].setSmooth(.8);
    }
    redrawSmooth.setSmooth(.8);
    redrawGain = 0;
    
    // Reverb
    reverb.setEffectMix(0.3);
    reverb.setRoomSize(0.6);
    
    // Pitch shift
    shift.clear();
    shift.setShift(1.0);
    
// WINDOW VARIABLES
    
    ww = 0.0;
    wh = 0.0;
    mouseX = 0.0;
    mouseY = 0.0;
    mouseVel = 0.0;
    gutterWidth = 16;
    gutterHeight = 8;
    
// COLORS
    
    colors.resize(numTextures);
    colors[0]   = ofColor(242, 95, 92);
    colors[1]   = ofColor(255, 224, 102);
    colors[2]   = ofColor(36, 123, 160);
    colors[3]   = ofColor(112, 193, 179);
    colors[4]   = ofColor(80, 81, 79);
    offWhite    = ofColor(240, 240, 240);
    lightGrey   = ofColor(220, 220, 220);
    mediumGrey  = ofColor(190, 190, 190);
    darkGrey    = ofColor(160, 160, 160);
    
// FONT
    
    titleFont.load("denne.ttf", 50);
    titleFont.setLineHeight(40.0f);
    titleFont.setLetterSpacing(1.035);
    
        
// TOP NAVIGATION
    
    // Load icons for top navigation
    tileIcon.load(ofToDataPath("tileSketch.png", true));
    settingsIcon.load(ofToDataPath("settingsSketch.png", true));
    stickersIcon.load(ofToDataPath("stickerSketch.png", true));
    undoIcon.load(ofToDataPath("undoSketch.png", true));
    trashIcon.load(ofToDataPath("trashSketch.png", true));
    
    // Size variables for top navigation
    topNavHeight    = 100;
    topNavIconHeight  = 50;
    
    // Top navigation interaction booleans
    drawGrid = false;
    trashing = false;
    undoing = false;
    drawSettings = true;
    drawStickerMenu = false;
    
// TOOLBOX
    
    // Size variables for toolbox
    toolboxHeight   = 220;
    toolboxWidth    = ww-(gutterWidth*8);
    toolboxStart    = 4 * gutterWidth;
    
    // Slider variables
    sliding = false;
    sliderHeight    = 4;
    sliderPosition  = 0.5;
    sliderStart     = gutterWidth*4;
    
    // Texture box variables
    textureBoxes.resize(numTextures);
    textureBoxWidth = (toolboxWidth/2)/numTextures - (gutterWidth);
    selectedTexture = -1;
    
// STICKERS
    
    canvasStickers.resize(30);
    stickerIconSize     = 75.0;
    stickerBoxHeight    = 100.0;
    stickersAdded       = 0;
    
    // Sticker menu
    stickerSelectors.resize(5);
    for (int i=0; i<5; i++) {
        Sticker sticker;
        sticker.setSize(stickerIconSize);
        sticker.setType(i);
        stickerSelectors[i] = sticker;
    }
    
// BRUSH
    
    minBrushRadius  = 5.0;
    maxBrushRadius  = 70.0;
    brushRadius     = 20.0;
    opacity         = 1.0;
    
    
// STROKES AND DRAWING

    maxStrokes           = 100;
    maxPixelsPerStroke   = 1000;
    maxStickersPerStroke = 20;
    currentStroke        = 0;
    strokes.resize(maxStrokes);
    for (int i=0; i<maxStrokes; i++) {
        resetStroke(i);
    }
    
    // Dissolve strokes
    strokesToDissolve = 0;
    
    // Drawing variables
    redraw      = false;
    clickCheck  = true;
    drawing     = false;
    
// KEYBOARD
    
    shiftKey = false;
    
    
// READ AUDIO FILES
    
    readFiles();
    
    
// SET FRAME RATE
    ofSetFrameRate(120);
}

//--------------------------------------------------------------
void ofApp::update(){
    // Get window variables
    ww = ofGetWindowWidth();
    wh = ofGetWindowHeight();
    
    // Compute mouse velocity
    mouseVel = sqrt(pow((ofGetMouseX() - mouseX), 2) + pow((ofGetMouseY() - mouseY), 2));
    if (mouseVel < minBrushRadius) mouseVel = minBrushRadius;
    if (mouseVel > maxBrushRadius) mouseVel = maxBrushRadius;
    
    // Update mouse position variables
    mouseX = ofGetMouseX();
    mouseY = ofGetMouseY();

    // Update brush size
    brushRadius = minBrushRadius + (sliderPosition * maxBrushRadius);
    
    // Update variables size elements
    toolboxHeight = wh * .2;
    stickerIconSize = stickerBoxHeight * 0.75;
    sliderWidth = (toolbox.getWidth()/2) - (sliderStart * 2);
    
    // If placing sticker, update brush sticker position
    if (placingSticker && !redraw) {
        brushSticker.setPosition((mouseX - (stickerIconSize/2))/ww, (mouseY - (stickerIconSize/2))/wh);
    }
    
    // Add new pixel if currently drawing and not dissolving or redrawing previous strokes
    if (drawing && !strokesToDissolve && !redraw && mouseX/ww >= 0 && mouseX/ww < 1) {
        
        // Create new Pixel
        Pixel newPixel;
        newPixel.setPosition(ofVec3f(mouseX/ww, mouseY/wh, 0));
        newPixel.setVelocity(0, 0, 0);
        newPixel.setSize(brushRadius);
        newPixel.setColor(colors[selectedTexture]);
        newPixel.setOpacity(opacity);
        newPixel.setType(selectedTexture);
        
        // Add new pixel to current stroke
        strokes[currentStroke%maxStrokes].pixels[strokes[currentStroke%maxStrokes].length%maxPixelsPerStroke] = newPixel;
        strokes[currentStroke%maxStrokes].length++;
    }
}

//----------------------Drawing Methods-------------------------
void ofApp::draw(){
    // Set background gradient
    ofBackgroundGradient(offWhite, lightGrey);
    
    // If grid mode is selected, draw grid
    if (drawGrid) drawFullGrid();
    
    // Redraw pixels if currently redrawing, otherwise draw all pixels
    if (redraw) {
        redrawPixels();
    } else {
        drawPixels();
    }
    
    // Only draw brush if texture is selected, pixels are not dissolving, and sticker isn't being placed
    if (placingSticker) {
        brushSticker.draw(ww, wh);
    } else if (selectedTexture != -1  && !redraw && !strokesToDissolve) {
        ofSetColor(colors[selectedTexture], 256*opacity);
        ofDrawCircle(mouseX, mouseY, brushRadius);
    }
    
    drawStickers();
    
    // Draw top navigation
    drawTopNav();
    
    if (drawSettings) drawToolbox();
    if (drawStickerMenu) drawStickerbox();
}

// This function draws all currently placed stickers onto the canvas
void ofApp::drawStickers() {
    for (int index = 0; index < currentStroke+1; index++) {
        int j = 0;
        while (j < strokes[index].numStickers) {
            if (strokes[index].stickers[j].isPopped()) {
                strokes[index].stickers[j] = strokes[index].stickers[strokes[index].numStickers];
                strokes[index].numStickers--;
            } else {
                strokes[index].stickers[j].draw(ww, wh);
                j++;
            }
        }

    }
}

// This function handles the redrawing of strokes while in redraw mode
void ofApp::redrawPixels() {
    bool strokesToRedraw = false;
    
    for (int i=0; i<currentStroke; i++) {
        stroke currStroke = strokes[i];

        if (!currStroke.redrawFinished) {
            strokesToRedraw = true;
            
            if (!currStroke.redrawing && !redrawingTexture[currStroke.textureType]) {
                strokes[i].redrawing = 1;
                redrawingTexture[currStroke.textureType] = true;
            }
        } else {
            drawStroke(i);
        }
        if (currStroke.redrawing) {
            if (currStroke.playbackPixel == currStroke.length) {
                if (currStroke.macros[4]) {
                    strokes[i].playbackPixel = 0;
                    strokes[i].macros[4]--;
                    strokes[i].repeats++;
                } else {
                    strokes[i].macros[4] = strokes[i].repeats;
                    strokes[i].repeats = 0;
                    strokes[i].redrawFinished = true;
                    strokes[i].redrawing = 0;
                    strokes[i].playbackPixel--;
                    redrawingTexture[currStroke.textureType] = false;
                }
                drawStroke(i);
            } else {
                for (int j=0; j<currStroke.playbackPixel; j++) {
                    currStroke.pixels[j].draw(ww, wh);
                }
                redrawLock.lock();
                strokes[i].playbackPixel++;
                redrawLock.unlock();
            }
        }
    }
    if (!strokesToRedraw) {
        redraw = 0;
        
        reverb.clear();
    }
}

// Draw a full stroke
void ofApp::drawStroke(int index) {
    for (int j = 0; j < strokes[index].length; j++) {
        strokes[index].pixels[j].draw(ww, wh);
    }
}

// Draw all strokes if we are not in redraw mode
void ofApp::drawPixels() {
    int i = 0;
    while (i < currentStroke + 1) {
        if (strokes[i].dissolving && strokes[i].pixels[0].getRadius() < 0.5) {
            strokesToDissolve--;
            strokes[i] = strokes[currentStroke];
            resetStroke(currentStroke);
            currentStroke--;
        } else {
            drawStroke(i);
            i++;
        }
    }
}

// Reset all variables associated with a single stroke
void ofApp::resetStroke(int index) {
    // Reset pixels
    strokes[index].pixels.clear();
    strokes[index].pixels.resize(maxPixelsPerStroke);
    strokes[index].length = 0;
    
    // Reset Stickers
    strokes[index].stickers.clear();
    strokes[index].stickers.resize(maxStickersPerStroke);
    strokes[index].numStickers = 0;
    
    strokes[index].env.setSmooth(0.75);
    strokes[index].repeats = 0;
    
    strokes[index].dissolving = false;
    for (int i = 0; i < 5; i++) {
        strokes[index].macros[i] = 0;
    }
}

// Begin process of removing a single stroke from the canvas
void ofApp::clearSingleStroke(int index) {
    strokes[index].dissolving = true;
    strokesToDissolve++;
    for (int i = 0; i < strokes[index].length; i++) {
        strokes[index].pixels[i].dissolve();
    }
    for (int j = 0; j < strokes[index].numStickers; j++) {
        strokes[index].stickers[j].pop();
    }
}

// Begin process of removing all strokes from the canvas
void ofApp::clearStrokes(){
    for (int i=0; i<currentStroke; i++) {
        clearSingleStroke(i);
    }
}

// Draws canvas grid
void ofApp::drawFullGrid() {
    ofSetColor(100, 100, 100, 80);
    for(int i=0; i<20; i++) {
        // Vertical lines
        ofDrawLine((ww/20)*i, topNavHeight, (ww/20)*i, wh);
        
        // Horizontal lines
        ofDrawLine(0, (topNavHeight+(wh*(i+1))/20), ww, (topNavHeight+(wh*(i+1))/20));
    }
}

// Draw top navigation menu
void ofApp::drawTopNav() {
    ofSetColor(offWhite);
    topNav.set(0, 0, ww, topNavHeight);
    trashGhost.set(ww-topNavIconHeight-gutterWidth*2, (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
    undoGhost.set(ww-(topNavIconHeight*2)-(gutterWidth*4), (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
    tileGhost.set(ww-(topNavIconHeight*3)-(gutterWidth*8), (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
    settingsGhost.set(ww-(topNavIconHeight*4)-(gutterWidth*10), (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
    stickersGhost.set(ww-(topNavIconHeight*5)-(gutterWidth*12), (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);

    ofDrawRectangle(topNav);
    
    // Draw clickable "ghost" icons
    ofDrawRectangle(trashGhost);
    ofDrawRectangle(undoGhost);
    ofDrawRectangle(tileGhost);
    ofDrawRectangle(settingsGhost);
    ofDrawRectangle(stickersGhost);
    
    // Draw app name
    ofSetColor(colors[0]);
    titleFont.drawString("Sketchy", gutterWidth*2, (topNavHeight/2)+20);
    
    // Trash icon
    if (!trashing) ofSetColor(mediumGrey);
    trashIcon.draw(ww-topNavIconHeight-gutterWidth*2, (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
    
    // Undo icon
    if (!undoing) {
        ofSetColor(mediumGrey);
    } else {
        ofSetColor(colors[0]);
    }
    undoIcon.draw(ww-(topNavIconHeight*2)-(gutterWidth*4), (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
    
    // Dividing Line
    ofSetColor(mediumGrey);
    ofSetLineWidth(3);
    ofDrawLine(ww-(topNavIconHeight*2)-(gutterWidth*6), gutterHeight*2, ww-(topNavIconHeight*2)-(gutterWidth*6), topNavHeight-(gutterHeight*2));
    ofSetLineWidth(1);
    
    // Draw grid icon
    if (!drawGrid) {
        ofSetColor(mediumGrey);
    } else {
        ofSetColor(colors[0]);
    }
    
    tileIcon.draw(ww-(topNavIconHeight*3)-(gutterWidth*8), (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
    
    // Draw settings icon
    if (!drawSettings) {
        ofSetColor(mediumGrey);
    } else {
        ofSetColor(colors[0]);
    }
    settingsIcon.draw(ww-(topNavIconHeight*4)-(gutterWidth*10), (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
    
    //Draw stickers icon
    if (!drawStickerMenu) {
        ofSetColor(mediumGrey);
    } else {
        ofSetColor(colors[0]);
    }
    stickersIcon.draw(ww-(topNavIconHeight*5)-(gutterWidth*12), (topNavHeight/2)-(topNavIconHeight/2), topNavIconHeight, topNavIconHeight);
}

// Draw stroke settings toolbox
void ofApp::drawToolbox() {
    ofSetColor(offWhite, 190);
    toolboxWidth = ww-(gutterWidth*8);
    toolbox.set(toolboxStart, wh-toolboxHeight, toolboxWidth, toolboxHeight);
    ofDrawRectangle(toolbox);
    
    ofPoint toolboxCenter = toolbox.getCenter();
    textureBoxWidth = (toolboxWidth/(2*numTextures)) - (2*gutterWidth);

    for(int i=0; i<numTextures; i++) {
        if (selectedTexture == i) {
            ofSetColor(colors[i]);
        } else {
            ofSetColor(colors[i], 140);
        }
        textureBoxes[i].set(toolboxCenter[0]+(textureBoxWidth*i)+(gutterWidth*(i+4)), wh-(toolboxHeight-2*gutterHeight), textureBoxWidth, toolboxHeight-(4*gutterHeight));
        ofDrawRectangle(textureBoxes[i]);
    }
    drawSizeSlider();
}

// Draw slider within toolbox
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

// Draw sticker select menu
void ofApp::drawStickerbox() {
    ofSetColor(offWhite, 0);
    stickerBox.set(toolboxStart, topNavHeight, (gutterWidth * 2) + stickerIconSize, wh*0.6);
    ofDrawRectangle(stickerBox);
    
    for (int i = 0; i < 5; i++) {
        stickerSelectors[i].setSize(stickerIconSize);
        float stickerX = (toolboxStart + gutterWidth)/ww;
        float stickerY = (topNavHeight + stickerIconSize*i + gutterHeight*2*(i+1))/wh;
        stickerSelectors[i].setPosition(stickerX, stickerY);
        stickerSelectors[i].draw(ww, wh);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==OF_KEY_SHIFT) {
        shiftKey = true;
    } else if (key==32){
        for (int i = 0; i < currentStroke; i++) {
            strokes[i].redrawFinished = false;
            strokes[i].playbackPixel = 0;
            strokes[i].redrawing = 0;
        }
        redrawLock.lock();
        if (!redraw) {
            redraw = 1;
            redrawGain = redrawSmooth.tick(redraw);
        } else {
            redraw = 0;
            redrawGain = 0;
            reverb.clear();
        }
        redrawLock.unlock();
    } else if (key==127 && !redraw && !strokesToDissolve) {
        if (currentStroke != 0) {
            if (shiftKey) {
                clearStrokes();
            } else {
                clearSingleStroke(currentStroke-1);
            }
        }
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key==OF_KEY_SHIFT) {
        shiftKey = false;
    }
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
                clickCheck = false;
                brushSticker = stickerSelectors[i];
            }
        }
    }
    
    if (topNav.inside(x, y) || toolbox.inside(x, y)) {
        clickCheck = false;
    }
    
    if (tileGhost.inside(x, y)) {
        drawGrid = !drawGrid;
    } else if (settingsGhost.inside(x, y)) {
        drawSettings = !drawSettings;
    } else if (stickersGhost.inside(x, y)) {
        drawStickerMenu = !drawStickerMenu;
    } else if (sliderCircle.inside(x, y)) {
        clickCheck = false;
        sliding = true;
    } else if (undoGhost.inside(x, y) && !redraw && currentStroke != 0 && !strokesToDissolve) {
        undoing = true;
        clearSingleStroke(currentStroke-1);
    } else if (trashGhost.inside(x, y) && !redraw && currentStroke != 0 && !strokesToDissolve) {
        trashing = true;
        clearStrokes();
    }
    
    if (selectedTexture != -1 && clickCheck) {
        drawing = true;
        strokes[currentStroke].textureType = selectedTexture;
        playTexture[selectedTexture] = 1;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    clickCheck = true;
    if (drawing) {
        if (selectedTexture != -1) playTexture[selectedTexture] = 0;
        currentStroke++;
        drawing = false;
    } else if (sliding) {
        sliding = false;
    } else if (placingSticker) {
        if (!stickerBox.inside(x, y) && !topNav.inside(x, y) && (!toolbox.inside(x, y) || !drawSettings)) {
            Sticker newSticker;
            newSticker = brushSticker;
            
            // Add sticker to current stroke
            int addStrokeTo = (currentStroke > 0) ? currentStroke - 1 : currentStroke;
            strokes[addStrokeTo].stickers[strokes[addStrokeTo].numStickers] = newSticker;
            strokes[addStrokeTo].numStickers++;
            strokes[addStrokeTo].macros[newSticker.getType()]++;
        }
        placingSticker = false;
    } else if (trashing || undoing) {
        trashing = false;
        undoing = false;
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
        sample = outputFrames(i, 0)*gain*opacity;
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
        float currentGain = strokes[i].env.tick(strokes[i].redrawing);
        if (strokes[i].redrawing) {
            Pixel currPixel = strokes[i].pixels[strokes[i].playbackPixel];
            int pixelType = currPixel.getType();
            if (pixelType >= 0 && pixelType < 5) {
                ofVec3f position = currPixel.getPosition();
                int pixelRadius = currPixel.getRadius();
                int grainStart = granulators[pixelType].fileSize()*position[0];
                if (pixelRadius) {
                    granulators[pixelType].setGrainParameters(pixelRadius*500, 0, 0, 0, grainStart);
                    granulators[pixelType].tick(outputFrames);
                    float yRatio = 1 - (position[1] - (topNavHeight/wh));
                    if (yRatio > 1) yRatio = 1;
                    if (yRatio < 0) yRatio = 0;
                    
                    shift.setShift(2 * yRatio);
                    shift.tick(outputFrames);
                    reverb.tick(outputFrames);

                    for (int j = 0; j < bufferSize; j++) {
                        samples[j] += outputFrames(j, 0)*currPixel.getOpacity()*currentGain;
                    }
                }
                
                for (int k = 0; k < bufferSize; k++) {
                    output[k*2] = samples[k] / numTextures;
                    output[(k*2)+1] = samples[k] / numTextures;
                }
            }
        }
        redrawLock.unlock();
    }
}

void ofApp::readFiles(){
    stk::Stk::setSampleRate(MY_SRATE);
    
    grainSize = 5000;
    grainStart = 0;
    granulators.resize(numTextures);
    granulators[0].openFile(ofToDataPath("fire.wav", true));
    granulators[1].openFile(ofToDataPath("chimes.wav", true));
    granulators[2].openFile(ofToDataPath("birds.wav", true));
    granulators[3].openFile(ofToDataPath("plectrum.wav", true));
    granulators[4].openFile(ofToDataPath("fiddle.wav", true));
    
    // Resize file/signal vectors
    playTexture.resize(numTextures);
    redrawingTexture.resize(numTextures);
    
    for (int i=0; i<numTextures; i++) {
        playTexture[i] = 0;
        redrawingTexture[i] = false;
        granulators[i].setVoices(10);
        granulators[i].setGrainParameters(grainSize, 50, 0, 0, grainStart);
    }
    
    audioReady = true;
}

