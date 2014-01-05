#pragma once

#include "ofMain.h"

#include "ofxPostProcessing.h"
#include "ofxUI.h"

#include "audioAnalytics.h"
#include "colorSchemeDesigner.h"
#include "smoother.h"
#include "decayer.h"

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void updateOSC();
    
    void drawWaves();
    void drawWaveform();
    
    void blend(ofFbo &base, ofFbo &blend, ofFbo &result, float mix, int mode);
    
    void setupGUI();
    void guiEvent(ofxUIEventArgs &e);

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
    colorSchemeDesigner colorScheme;
    
    ofxPostProcessing post;
    vector<RenderPass::Ptr> renderPasses;
    
    ofCamera cam;
    
    int state;
    
    //waves
    vector<vector<float> > waveHistory;
    vector<ofFloatColor> waveHiHistory;
    vector<ofFloatColor> waveLoHistory;
    

    //UI variables
    ofxUICanvas *gui;
    
    float meshRotateX;
    float camX, camY, camZ;
    float lookatX, lookatY, lookatZ;

    //postproc vars
    bool drawPost;
    
    //blend
    ofShader shaderBlend;
    float shadeBlendMix;
    int shadeBlendMode;
    
    float hScale, colScale;
    
    float width, height;
    
    //ofFloatColor waveLo, waveHi, bg;
    vector<ofFloatColor> colors;
    
    smoother centroidSmoothed;
    decayer rmsDecayed;
    
};
