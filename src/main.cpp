#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLWindowSettings settings;
    settings.setGLVersion(4,1);
    settings.width = 900;
    settings.height = 900;
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
