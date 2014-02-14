#pragma once

#include "ofMain.h"

#include "ofxPostProcessing.h"
#include "ofxUI.h"
#include "ofxMeshUtils.h"


#include "audioAnalytics.h"
#include "colorSchemeDesigner.h"
#include "terrainManager.h"
//#include "smoother.h"
#include "decayer.h"

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    
//    void drawWaves();
//    void drawWaveform();
//    
//    void blend(ofFbo &base, ofFbo &blend, ofFbo &result, float mix, int mode);
    
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
    colorSchemeDesigner cs;
        int state;
    
    terrainManager tm;
    
    //postproc vars
//    bool drawPost;
    float width, height;
    ofxPostProcessing post;
    vector<RenderPass::Ptr> renderPasses;
    
    
    //cam
    decayer camYDecayed;
    smoother camYSmoothed;
    ofCamera cam;
    

    
    //waves
//    vector<vector<float> > waveHistory;
//    vector<ofFloatColor> waveHiHistory;
//    vector<ofFloatColor> waveLoHistory;
//    
//    ofxMeshUtils meshUtils;
//    float hScale, colScale;

    

    //UI variables
    ofxUICanvas *gui;
    
    float camX, camY, camZ;
    float lookatX, lookatY, lookatZ;


    
    //ofFloatColor waveLo, waveHi, bg;
    vector<ofFloatColor> colors;
   
    ///lights
    ofLight pointLight;
    ofMaterial material;
    
    bool useLights;
    
};
