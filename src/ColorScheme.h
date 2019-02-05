//
//  ColorScheme.h
//  noise_particles_gpu
//
//  Created by Brian on 9/13/18.
//
//

#pragma once

#include "ofMain.h"

class ColorScheme {
public:
    
    ColorScheme() {}
    ~ColorScheme(){}
    
    void setBgColor(ofColor bg) {
        bg_color = bg;
    }
    
    void setColor1(ofColor c1) {
        color1 = c1;
    }
    
    void setColor2(ofColor c2) {
        color2 = c2;
    }
    
    void setColor3(ofColor c3) {
        color3 = c3;
    }
    
    ofColor bg_color;
    
    ofColor color1;
    ofColor color2;
    ofColor color3;
    
};
