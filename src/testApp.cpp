#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(30);
    ofSetDepthTest(true);
    state = 0;
    
    aa.playStems(0);
    cs.setup();
    cs.gui->setVisible(false);
    
    tm.setup(&aa, &cs);
    cm.setup(&tm);
    lm.setup(&cs);
    pm.setup();
    
  
//post
//    width = 1920;
//    height = 1080;
//    
//    // Setup post-processing chain
//    post.init(width, height);
//    post.createPass<GodRaysPass>()->setEnabled(true);
//    
//    renderPasses = post.getPasses();
    
    useLights = true;
    
    
    setupGUI();
}

//--------------------------------------------------------------
void testApp::update(){
    aa.updateAnalytics();
    
//    cs.setHue(ofMap(aa.kurtosisSmoothed.getMean(), 0, aa.maxKurtosis[0], 0.0, 1.0));
    cs.setSaturation(ofMap(aa.centroidSmoothed.getMean(), 30, 55, 0.0, 1.0));
    cs.setBrightness(aa.ampSmoothed.getMean());
    cs.setDistance(ofMap(aa.pitchSmoothed.getMedian(), 50, 90, 0.0, 0.5));

    tm.update();
    cm.update();
    lm.update();
    
    gui->update();
    
}

//--------------------------------------------------------------
void testApp::draw(){

    ofEnableAlphaBlending();
    
    if (state == 0) {
        pm.post.begin(cm.cam);
        if (useLights) lm.begin();
        
        ofFloatColor bgColor = cs.colorScheme[0][0];
        float complementHue = bgColor.getHue() + 0.5;
        if (complementHue > 1.0) complementHue-=1.0;
        bgColor.setHue(complementHue);
        bgColor.setSaturation(bgColor.getSaturation() - 0.2);

        ofBackground(bgColor);
        tm.draw();
        
        if (useLights) lm.end();
        pm.post.end();
    }
    else if (state == 1) {
        cs.draw();
    }
    else if (state == 2) {
        aa.drawAnalytics();
    }

    ofDisableAlphaBlending();
}



//--------------------------------------------------------------
void testApp::setupGUI(){
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 60);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("camX", -260, 260, &cm.camX, length-xInit, dim);
    gui->addSlider("camY", 0.0, 500, &cm.camY, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("lookatX", -260, 260, &cm.lookatX, length-xInit, dim);
    gui->addSlider("lookatY", -500, 500, &cm.lookatY, length-xInit, dim);
    
    gui->toggleVisible();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}


//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
//    if(name == "Bloom") //kind == OFX_UI_WIDGET_LABELBUTTON
//    {
//        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
//        post[0]->setEnabled(button->getValue());
//    }
//    
//    if(name == "GodRays") //kind == OFX_UI_WIDGET_LABELBUTTON
//    {
//        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
//        post[1]->setEnabled(button->getValue());
//    }

}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
    unsigned idx = key - '0';
    if (idx < pm.post.size()) pm.post[idx]->setEnabled(!pm.post[idx]->getEnabled());

    
    
    switch (key) {

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            aa.setMode(key - 48);
            break;

                            
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'g':
            gui->toggleVisible();
            cs.gui->toggleVisible();
            break;
            
            
        case 's':
            state = (state + 1) % 3;
            if (state == 0){
                cs.gui->setVisible(false);
            }
            else if (state == 1) {
                gui->setVisible(false);
                cs.gui->setVisible(true);
            }
            else if (state == 2) {
                gui->setVisible(false);
                cs.gui->setVisible(false);
            }
            break;
            
        case 'p':
            aa.playStems(0);
            break;
        
        case 'r':
            cs.assignRandom(true);
            break;
            
        case 'k':
            
            cout << "kurtosis " << aa.audioFeatures[0]->spectralFeatures["kurtosis"] << endl;
            break;
            
        case 'l':
            useLights = !useLights;
            break;
        
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
