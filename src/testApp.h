#pragma once

#include "ofMain.h"


#include "ofxPostProcessing.h"
#include "ofxUI.h"
#include "ofxBoids.h"

#include "ofxOsc.h"
#define PORT 12345

#include "audioAnalytics.h"

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void updateOSC();
    
    void drawWaves();
    void drawSun();
    void drawBirds();
    void drawWaveform();
    
    void setupGUI();
    void guiEvent(ofxUIEventArgs &e);
    
    void blend(ofFbo &base, ofFbo &blend, ofFbo &result, float mix, int mode);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
	
	audioAnalytics aa;
    
    ofxPostProcessing post;
    vector<RenderPass::Ptr> renderPasses;
    
    ofCamera cam;
    
    //waves
    vector<vector<float> > waveHistory;
    int track, track2;
    float bassAccum, time;
    
    //sun
    ofFbo startFbo, finalFbo;
    
    //boids
    int boidNum;
    ofVec3f target;
    vector<SteeredVehicle> boids;
    
    
    //UI variables
    ofxUICanvas *gui;
    
    float noiseScale;
    float meshRotateX;
    float camX, camY, camZ;
    float lookatX, lookatY, lookatZ;
    float sunY;
    ofFloatColor gradientStartPre;
    ofFloatColor gradientStart;
    ofFloatColor gradientEnd;
    ofFloatColor sunColor;
    
    //postproc vars
    float waveStrength, noiseStrength;
    float fboBlend;
    
    ofxOscReceiver receiver;
    

    bool drawPost;
    
    //belnd
    ofShader shaderBlend;
    float shadeBlendMix;
    int shadeBlendMode;
    
};
