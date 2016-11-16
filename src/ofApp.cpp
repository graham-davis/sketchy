#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Initialize audio variables
    soundStream.setup(this, MY_CHANNELS, MY_CHANNELS, MY_SRATE, MY_BUFFERSIZE, MY_NBUFFERS);
    audioReady = false;
    numTextures = 3;

    colors.resize(numTextures);
    colors[0] = ofColor(242, 95, 92);
    colors[1] = ofColor(255, 224, 102);
    colors[2] = ofColor(36, 123, 160);
    
    
    // Read audio files into buffers
    readFiles();
    
    ofSetFrameRate(20);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    for (int i = 0; i < numTextures; i++) {
        if (playTexture[i]) {
            ofSetColor(colors[i]);
            ofDrawCircle(ofRandom(0,ofGetWindowWidth()), ofRandom(0,ofGetWindowHeight()), ofRandom(10, 50));
        }
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key-49 >= 0 && key-49 < numTextures) {
        playTexture[key-49] = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key-49 >= 0 && key-49 < numTextures) {
        playTexture[key-49] = false;
        cursors[key-49] = 0;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
        float left = 0;
        float right = 0;
        for (int i = 0; i < bufferSize; i++) {
            for (int j = 0; j < numTextures; j++) {
                if (playTexture[j] && cursors[j] < files[j].getSize()) {
                    left = textures[j](cursors[j], 0);
                    left = textures[j](cursors[j], 1);
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

    for (int i=0; i<numTextures; i++) {
        textures[i] = stk::StkFrames(files[i].getSize(), 2);
        files[i].tick(textures[i]);
        cursors[i] = 0;
        playTexture[i] = false;
    }
    
    audioReady = true;
}
