#pragma once

#include "ofMain.h"
#include "ColorScheme.h"
#include "GParticles.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 10002

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void reset();

    void setupColors();
    
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
		
    int nums;
    float noise_scale;
    float noise_frequency;

    
    ofFbo fbo;
    
    //  Colors
    ColorScheme scheme1;
    ColorScheme scheme2;
    ColorScheme scheme3;
    
    ColorScheme activeScheme;
    
    float target_update;
    float period;
    
    //*********************************
    //  Particles
    GParticles particles1;
    GParticles particles2;
    GParticles particles3;
    
    //  Camera
    ofEasyCam cam;
    
    //  Constants
    const int x_dim = 200;
    const int y_dim = 200;
    
    //  osc
    ofxOscSender sender;
    
    //  Data
    float w, h, t;
};
