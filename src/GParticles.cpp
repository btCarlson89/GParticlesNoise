//
//  gParticles.cpp
//  gpuParticleSystem
//
//  Created by Brian on 8/4/18.
//
//

#include "gParticles.h"

//--------------------------------------------------------------
GParticles::GParticles() :
current_fbo_index(0),
alpha_factor(1.0)
{
    
}

//--------------------------------------------------------------
GParticles::~GParticles()
{
    
}

//--------------------------------------------------------------
void GParticles::setup(unsigned int x_count, unsigned int y_count, ofPrimitiveMode primitive_mode, unsigned int num_textures)
{
    x_dim = x_count;
    y_dim = y_count;
    
    //  Fbo setup
    ofFbo::Settings s;
    s.internalformat = GL_RGBA32F_ARB;
    s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    s.minFilter = GL_NEAREST;
    s.maxFilter = GL_NEAREST;
    s.wrapModeHorizontal = GL_CLAMP;
    s.wrapModeVertical = GL_CLAMP;
    s.width = x_dim;
    s.height = y_dim;
    s.numColorbuffers = num_textures;
    
    for(unsigned i = 0; i < 2; ++i)
    {
        fbos[i].allocate(s);
    }
    
    draw_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F_ARB);
    draw_fbo.begin();
    ofClear(255);
    draw_fbo.end();
    
    //  Particle mesh setup
    particle_mesh.clear();
    
    for(int y = 0; y < y_dim; ++y)
    {
        for(int x = 0; x < x_dim; ++x)
        {
            particle_mesh.addVertex(ofVec3f(200.f * x / (float)x_dim - 100.f, 200.f * y / (float)y_dim - 100.f, -500.f));
            particle_mesh.addTexCoord(ofVec2f(x, y));
        }
    }
    particle_mesh.setMode(primitive_mode);
    
    //  BG mesh setup
    bg_mesh.clear();
    bg_mesh.addVertex(ofVec3f(-1.f, -1.f, 0.f));
    bg_mesh.addVertex(ofVec3f( 1.f, -1.f, 0.f));
    bg_mesh.addVertex(ofVec3f( 1.f,  1.f, 0.f));
    bg_mesh.addVertex(ofVec3f(-1.f,  1.f, 0.f));
    
    bg_mesh.addTexCoord(ofVec2f(0.f, 0.f));
    bg_mesh.addTexCoord(ofVec2f(x_dim, 0.f));
    bg_mesh.addTexCoord(ofVec2f(x_dim, y_dim));
    bg_mesh.addTexCoord(ofVec2f(0.f, y_dim));
    
    bg_mesh.addIndex(0);
    bg_mesh.addIndex(1);
    bg_mesh.addIndex(2);
    bg_mesh.addIndex(0);
    bg_mesh.addIndex(2);
    bg_mesh.addIndex(3);
    
    bg_mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    //  Shaders
    update_shader.load("shaders/" + UPDATE_SHADER_NAME);
    draw_shader.load("shaders/" + DRAW_SHADER_NAME);
    
    //  Texture setup
    initPositionTexture();
    initVelocityTexture();
    initColorTexture();
    initSpeedTexture();
    
    //  Interaction
    targetMouseX = currentMouseX = ofGetMouseX();
    targetMouseY = currentMouseY = ofGetMouseY();
}

//--------------------------------------------------------------
void GParticles::update()
{
    
    //  Interaction
    //targetMouseX = ofGetMouseX();
    //targetMouseY = ofGetMouseY();
        
    currentMouseX += 0.002 * (targetMouseX - currentMouseX);
    currentMouseY += 0.002 * (targetMouseY - currentMouseY);
    
    noise_scale = ofMap(currentMouseX, 0, ofGetWidth(), 200, 800, true);
    noise_frequency = ofMap(currentMouseY, 0, ofGetHeight(), 1.0, 32.0, true);
    
    w = ofGetWidth();
    h = ofGetHeight();
    t = ofGetElapsedTimef();
    
    fbos[1 - current_fbo_index].begin(false);
    
    
    ofVec3f pos1, pos2, pos3;
    
    pos1 = ofVec3f(w  * (0.5 + 0.5 * ofSignedNoise(-1.15 * t)), h * (0.5 + 0.1 * ofSignedNoise(2.1 * t)), 0);
    pos2 = ofVec3f(w * (0.3 + 0.1 * ofSignedNoise(0.1 * t)), h * (0.25 + 0.1 * ofSignedNoise(0.2 * t)), 0);
    pos3 = ofVec3f(w * (0.7 + 0.1 * ofSignedNoise(0.2 * t)), h * (0.25 + 0.1 * ofSignedNoise(-0.15 * t)), 0);
    
    glPushAttrib(GL_ENABLE_BIT);
    glViewport(0, 0, x_dim, y_dim);
    glDisable(GL_BLEND);
    ofSetColor(255, 255, 255);
    fbos[1 - current_fbo_index].activateAllDrawBuffers();
    
    update_shader.begin();
    ofVec3f mouse(ofGetMouseX(), ofGetMouseY(), 0.0f);
    update_shader.setUniform3fv("mouse", mouse.getPtr());
    update_shader.setUniform1f("elapsed", t);
    update_shader.setUniform1f("radiusSquared", 250000.0f);
    update_shader.setUniform2f("resolution", ofVec2f(w, h));
    update_shader.setUniform3f("rep1", pos1);
    update_shader.setUniform3f("rep2", pos2);
    update_shader.setUniform3f("rep3", pos3);
    
    update_shader.setUniform1f("sense", sense);
    
    update_shader.setUniform1f("noise_scale", noise_scale);
    update_shader.setUniform1f("noise_frequency", noise_frequency);
    
    for(unsigned i = 0; i < fbos[current_fbo_index].getNumTextures(); ++i)
    {
        ostringstream stream;
        stream << "particles" << ofToString(i);
        update_shader.setUniformTexture(stream.str().c_str(), fbos[current_fbo_index].getTexture(i), i);
    }
    
    bg_mesh.draw();
    update_shader.end();
    glPopAttrib();
    
    fbos[1 - current_fbo_index].end();
    
    current_fbo_index = 1 - current_fbo_index;
}

//--------------------------------------------------------------
void GParticles::draw()
{
    //draw_fbo.begin();
    
    //ofSetColor(12, 27, 51, 4);
    //ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    glPointSize(3.0);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    
    draw_shader.begin();
    
    draw_shader.setUniform1f("radius", r);
    
    for(unsigned i = 0; i < fbos[current_fbo_index].getNumTextures(); ++i)
    {
        ostringstream stream;
        stream << "particles" << ofToString(i);
        draw_shader.setUniformTexture(stream.str().c_str(), fbos[current_fbo_index].getTexture(i), i);
    }
    
    particle_mesh.draw();
    draw_shader.end();
        
    //draw_fbo.end();
    
    //ofSetColor(255);
    //draw_fbo.draw(0,0);
}

//--------------------------------------------------------------
void GParticles::loadTexture(unsigned int index, float* texture)
{
    if(index < fbos[current_fbo_index].getNumTextures())
    {
        fbos[current_fbo_index].getTexture(index).bind();
        glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, x_dim, y_dim, GL_RGBA, GL_FLOAT, texture);
        fbos[current_fbo_index].getTexture(index).unbind();
    }
    else
    {
        ofLogError() << "Trying to load texture into non-existent buffer.";
    }
}

//--------------------------------------------------------------
void GParticles::initPositionTexture()
{
    float* pos_tex = new float[x_dim * y_dim * 4]; //4 for RGBA
    
    float screen_w = ofGetWidth();
    float screen_h = ofGetHeight();
    
    for(unsigned int y = 0; y < y_dim; ++y)
    {
        for(unsigned int x = 0; x < x_dim; ++x)
        {
            unsigned int index = y * x_dim + x;
            pos_tex[index * 4 + 0] = ofRandom(screen_w);
            pos_tex[index * 4 + 1] = ofRandom(screen_h);
            pos_tex[index * 4 + 2] = 0.0f;
            pos_tex[index * 4 + 3] = 0.0f;
        }
    }
    
    loadTexture(POSITION, pos_tex);
    delete[] pos_tex;
    
    return;
}

//--------------------------------------------------------------
void GParticles::initVelocityTexture()
{
    clearTexture(VELOCITY);
}

//--------------------------------------------------------------
void GParticles::initColorTexture()
{
    float* col_tex = new float[x_dim * y_dim * 4]; //4 for RGBA
    
    float screen_w = ofGetWidth();
    float screen_h = ofGetHeight();
    
    for(unsigned y = 0; y < y_dim; ++y)
    {
        for(unsigned x = 0; x < x_dim; ++x)
        {
            unsigned index = y * x_dim + x;
            
            float alpha  = ofMap(index, 0.0, x_dim * y_dim, 0.01, 0.5);
            
            col_tex[index * 4 + 0] = 0.33 + 0.67 * float(y) / float(y_dim);
            col_tex[index * 4 + 1] = 0.33 * float(y) / float(y_dim);
            col_tex[index * 4 + 2] = 1.0f - 0.67 * float(x) / float(x_dim);
            col_tex[index * 4 + 3] = 0.1;
        }
    }
    
    loadTexture(COLOR, col_tex);
    delete[] col_tex;
    
    return;
}

//--------------------------------------------------------------
void GParticles::initColorTexture(ofColor color)
{
    float* col_tex = new float[x_dim * y_dim * 4]; //4 for RGBA
    
    float screen_w = ofGetWidth();
    float screen_h = ofGetHeight();
    
    for(unsigned y = 0; y < y_dim; ++y)
    {
        for(unsigned x = 0; x < x_dim; ++x)
        {
            unsigned index = y * x_dim + x;
            
            float alpha  = ofMap(index, 0.0, x_dim * y_dim, 0.0, 1.0);
            
            col_tex[index * 4 + 0] = color.r / 255.0;
            col_tex[index * 4 + 1] = color.g / 255.0;
            col_tex[index * 4 + 2] = color.b / 255.0;
            col_tex[index * 4 + 3] = alpha * alpha_factor;
        }
    }
    
    loadTexture(COLOR, col_tex);
    delete[] col_tex;
    
    return;
}

//--------------------------------------------------------------
void GParticles::initSpeedTexture()
{
    float* spd_tex = new float[x_dim * y_dim * 4]; //4 for RGBA
    
    for(unsigned y = 0; y < y_dim; ++y)
    {
        for(unsigned x = 0; x < x_dim; ++x)
        {
            unsigned index = y * x_dim + x;
            spd_tex[index * 4 + 0] = ofRandom(0.1, 2.0);
            spd_tex[index * 4 + 1] = 0.0f;
            spd_tex[index * 4 + 2] = 0.0f;
            spd_tex[index * 4 + 3] = 0.0f;
        }
    }
    
    loadTexture(SPEED, spd_tex);
    delete[] spd_tex;
    
    return;
}

//--------------------------------------------------------------
void GParticles::clearTexture(unsigned int index)
{
    float* tex = new float[x_dim * y_dim * 4];
    memset(tex, 0, sizeof(float) * x_dim * y_dim * 4);
    
    loadTexture(index, tex);
    delete[] tex;
    
    return;
}



//--------------------------------------------------------------
void GParticles::setAlphaFactor(float factor)
{
    alpha_factor = factor;
}





