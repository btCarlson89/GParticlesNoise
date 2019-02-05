//
//  gParticles.h
//  gpuParticleSystem
//
//  Created by Brian on 8/4/18.
//
//

#pragma once

#include "ofMain.h"

class GParticles
{
public:
    
    GParticles();
    ~GParticles();
    
    //  Texture enum
    enum TextureIndex
    {
        POSITION,
        VELOCITY,
        COLOR,
        SPEED
    };
    
    //  Constants
    const string UPDATE_SHADER_NAME = "update";
    const string DRAW_SHADER_NAME = "draw";
    
    //  Core methods
    void setup(unsigned int x_count, unsigned int y_count, ofPrimitiveMode primitive_mode = OF_PRIMITIVE_POINTS, unsigned int num_textures = 4);
    void update();
    void draw();
    
    //  Initialization
    void loadTexture(unsigned int index, float* texture);
    void initPositionTexture();
    void initVelocityTexture();
    void initColorTexture();
    void initColorTexture(ofColor color);
    void initSpeedTexture();
    void clearTexture(unsigned int index);
    
    //  Set functions
    void setAlphaFactor(float factor);
    
    //
    float r;
    float sense = 1.0f;
    
    //  Interaction
    float targetMouseX, targetMouseY;
    float currentMouseX, currentMouseY;
    
private:
    
    //  Fbos
    ofFbo fbos[2];
    ofFbo draw_fbo;
    
    //  Meshes
    ofVboMesh particle_mesh, bg_mesh;
    
    //Shaders
    ofShader update_shader, draw_shader;
    
    //  Texture
    unsigned int current_fbo_index;
    unsigned int x_dim, y_dim;
    
    //  Noise
    float noise_scale, noise_frequency;
    
    //  Color
    float alpha_factor;
    
    //  Data
    float w, h, t;
};
