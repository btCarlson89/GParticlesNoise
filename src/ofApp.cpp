#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //  OF Settings
    //ofBackground(12, 27, 51);
    ofSetFrameRate(60);
    ofSetBackgroundAuto(false);
    ofHideCursor();
    
    //  Colors
    setupColors();
    
    //  Particles
    particles1.setup(x_dim, y_dim);
    particles2.setup(x_dim, y_dim);
    particles3.setup(x_dim/20.0, y_dim/20.0);
    
    particles1.setAlphaFactor(2.0f);
    
    particles1.initColorTexture(activeScheme.color1);
    particles2.initColorTexture(activeScheme.color2);
    particles3.initColorTexture(activeScheme.color3);
    
    particles1.r = 4.0f;
    particles2.r = 5.0f;
    particles3.r = 24.0f;
    
    period = 3.5;
    target_update = ofGetElapsedTimef() + period;
    
    //  Fbo

    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F_ARB);
    fbo.begin();
    ofClear(0, 0, 0, 0);
    fbo.end();
    
    //  Osc
    sender.setup( HOST, PORT );
}

//--------------------------------------------------------------
void ofApp::setupColors(){
    //scheme1.setBgColor(ofColor(12,  27,  51));
    //scheme1.setColor1(ofColor(  3, 181, 170));
    //scheme1.setColor2(ofColor(219, 254, 135));
    //scheme1.setColor3(ofColor(255, 255, 255));
    
    //scheme1.setBgColor(ofColor( 238,  98, 135));
    //scheme1.setBgColor(ofColor( 0,  0, 0));
    //scheme1.setColor2(ofColor(238,  98, 135));
    //scheme1.setColor1(ofColor( 135,  98, 238));
    //scheme1.setColor3(ofColor(  88, 169, 214));
    
    scheme1.setBgColor(ofColor(237, 235, 215));
    scheme1.setColor2(ofColor(66, 62, 55));
    scheme1.setColor1(ofColor(204, 226, 59));
    scheme1.setColor3(ofColor(163, 149, 148));
    
    scheme2.setBgColor(ofColor(18, 16, 16));
    scheme2.setColor2(ofColor(74, 74, 74));
    scheme2.setColor1(ofColor(148,  148, 148));
    scheme2.setColor3(ofColor(255, 250, 255));

    
    scheme3.setBgColor(ofColor(255));
    scheme3.setColor2(ofColor(238, 148, 128));
    scheme3.setColor1(ofColor(214,  87, 128));
    scheme3.setColor3(ofColor(239, 210, 203));
    
    activeScheme = scheme1;
}

//--------------------------------------------------------------
void ofApp::reset(){
    //  Particles
    particles1.setup(x_dim, y_dim);
    particles2.setup(x_dim, y_dim);
    particles3.setup(x_dim/10.0, y_dim/10.0);
    
    particles1.setAlphaFactor(2.0f);
    
    particles1.initColorTexture(activeScheme.color1);
    particles2.initColorTexture(activeScheme.color2);
    particles3.initColorTexture(activeScheme.color3);
    
    particles1.r = 4.0f;
    particles2.r = 5.0f;
    particles3.r = 24.0f;
}


//--------------------------------------------------------------
void ofApp::update(){

    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    
    w = ofGetWidth();
    h = ofGetHeight();
    t = ofGetElapsedTimef();
    
    float rand = ofRandom(1.0);
    float sense;
    if(rand > 0.95) sense = -10.0f;
    else if(rand > 0.15) sense = 0.0f;
    else sense = 1.0f;
    
    if(sense < 0.0 && ofRandom(1.0) > 0.95)
    {
        reset();
    }
    
    particles1.sense = sense;
    particles2.sense = sense;
    particles3.sense = sense;
    
    sense = ofClamp(10.0 + sense, 0, 1);
    
    if(t >= target_update)
    {
        target_update = t + period;
        
        float randX = ofRandom(w);
        float randY = ofRandom(h);
        particles1.targetMouseX = randX;
        particles1.targetMouseY = randY;
    }
    
    particles1.update();
    particles2.update();
    particles3.update();
    
    float x_offset = mouseX / w;
    float y_offset = mouseY / h;
    
    float dx = x_offset;
    float dy = y_offset;
    
    //  osc
    float ns = ofMap(dx, 0, 1.0, 0.1, 0.3, true);
    float nf = ofMap(dy, 0, 1.0, 0.1, 0.3, true);
    
    ofxOscMessage m;
    m.setAddress( "/scale" );
    m.addFloatArg( ns );
    sender.sendMessage( m );
    
    m.setAddress( "/frequency" );
    m.addFloatArg( nf );
    sender.sendMessage( m );
    
    m.setAddress( "/sense" );
    m.addFloatArg( nf );
    sender.sendMessage( m );
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(activeScheme.bg_color);
    
    fbo.begin();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofSetColor(activeScheme.bg_color, 8);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    particles1.draw();
    particles2.draw();
    particles3.draw();
    ofDisableBlendMode();
    
    fbo.end();
    
    fbo.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch(key)
    {
        case '1' :
            activeScheme = scheme1;
            reset();
            break;
        case '2' :
            activeScheme = scheme2;
            reset();
            break;
        case '3' :
            activeScheme = scheme3;
            reset();
            break;
        case 'c' :
            ofBackground(activeScheme.bg_color);
            break;
        case 'p' :
            break;
        default :
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    switch(key)
    {
        case 'p' :
            ofBackground(activeScheme.bg_color);
            reset();
            break;
        default:
            break;
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
    setup();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
