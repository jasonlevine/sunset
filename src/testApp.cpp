#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(30);
    ofSetDepthTest(true);
    state = 0;
    
    aa.playStems(0);
    cs.setup();
    cs.gui->setVisible(false);
    
    for (int i = 0; i < 13; i++) {
        colors.push_back(ofFloatColor(ofRandomuf()));
    }
    
    for (int i = 0; i < 13; i++) {
         cs.addColorRef(&colors[i]);
    }

    cs.assignRandom(true);
    
    tm.setup(&aa, &cs);
    
    
//waves
//    for (int i = 0; i < 120; i++) {
//        vector<float> wave;
//        wave.assign(512, 0);
//        waveHistory.push_back(wave);
//        waveLoHistory.push_back(ofFloatColor(0.3, 0.5, 0.3));
//        waveHiHistory.push_back(ofFloatColor(0.8, 0.5, 0.7));
//    }
    
  
//post
    width = 1920;
    height = 1080;
    
    // Setup post-processing chain
    post.init(width, height);
    post.createPass<GodRaysPass>()->setEnabled(true);
    
    renderPasses = post.getPasses();
//    drawPost = false;


    //cam
    lookatX = 0;
    lookatY = 0;
    lookatZ = -600;
    
    
    camYDecayed.setDecay(0.99);
    camYSmoothed.setNumPValues(25);
    
    
    //lights
    
    // turn on smooth lighting //
    ofSetSmoothLighting(true);

	
    
    // Point lights emit light in all directions //
    // set the diffuse color, color reflected from the light source //
    pointLight.setAmbientColor( cs.colorScheme[0][2]);
    pointLight.setDiffuseColor( cs.colorScheme[0][1]);
    
    // specular color, the highlight/shininess color //
	pointLight.setSpecularColor( ofColor(255.f, 255.f, 255.f));
//    pointLight.setPosition(0, 300, 0);
    pointLight.setPosition(0.5, 0.5, 0.5);
    
    // shininess is a value between 0 - 128, 128 being the most shiny //
	material.setShininess( 128 );
    material.setAmbientColor(cs.colorScheme[0][0]);
    
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
    

    
//    if (waveHistory[10][camX + 255] < waveHistory[5][camX + 256]) {
//        camX--;
//    }
//    else if (waveHistory[10][camX + 257] < waveHistory[5][camX + 256]) {
//        camX++;
//    }
    
    

    
    camYDecayed.addValue(tm.waveHistory[45][camX + 256] * 20 * 180 + 100);
    camYDecayed.update();
    camYSmoothed.addValue(camYDecayed.getValue());
    camY = camYSmoothed.getMean();
    
    cam.setPosition(camX, camY, camZ);
    cam.lookAt(ofVec3f(lookatX, lookatY, lookatZ));
    
//    pointLight.setPosition(camX, camY, -camZ);
    pointLight.setAmbientColor( cs.colorScheme[0][2]);
    pointLight.setDiffuseColor( cs.colorScheme[0][1]);
    
    gui->update();
    
}

//--------------------------------------------------------------
void testApp::draw(){
//    aa.drawAnalytics();
    
    ofEnableAlphaBlending();
    
    if (state == 0) {
        post.begin(cam);
        if (useLights) {
            ofEnableLighting();
            pointLight.enable();
            material.begin();
        }
        ofFloatColor bgColor = cs.colorScheme[0][0];
        float complementHue = bgColor.getHue() + 0.5;
        if (complementHue > 1.0) complementHue-=1.0;
        bgColor.setHue(complementHue);
        bgColor.setSaturation(bgColor.getSaturation() - 0.2);

        ofBackground(bgColor);

        tm.draw();
        
        if (useLights) {
            material.end();
            ofDisableLighting();
        }
        post.end();
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
/*
void testApp::drawWaveform() {
    int width = 512;
    int height = waveHistory.size();
    
    startFbo.begin();
    ofClear(0, 0, 0, 0);
//    ofSetColor(0);
//    ofRect(0, 0, startFbo.getWidth(), startFbo.getHeight());
    cam.begin();
    ofBackground(0);
    ofSetColor(255);
    
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(3);
    ofPushMatrix();
    ofRotateX(meshRotateX);
    ofRotateY(180);
    ofScale(7,180,10);
    ofTranslate(-width/2, 0, -height/2);
    ofBeginShape();
    for (int x = 0; x<width; x++){
        ofVertex(x, waveHistory[waveHistory.size()-1][x], waveHistory.size()-1);
    }
    ofEndShape();
    ofPopMatrix();
    ofPopStyle();
    ofFill();
    cam.end();
    startFbo.end();

    
}*/



//--------------------------------------------------------------
void testApp::setupGUI(){

    camX = 0;
    camY = 150;
    camZ = 1200;
    
//    hScale = 20;
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
    gui->addSpacer(length-xInit, 1);
//    gui->addSlider("hScale", 1.0, 20.0, &hScale, length-xInit, dim);
//    gui->addSlider("colScale", 1.0, 10.0, &colScale, length-xInit, dim);
//    gui->addLabelToggle("Bloom", true);
//    gui->addLabelToggle("GodRays", true);
    gui->addSpacer(length-xInit, 1);
//    gui->addSlider("meshRotateX", -90.0, 90, &meshRotateX, length-xInit, dim);
    gui->addSlider("camX", -260, 260, &camX, length-xInit, dim);
    gui->addSlider("camY", 0.0, 500, &camY, length-xInit, dim);
    gui->addSlider("camZ", -600, 600, &camZ, length-xInit, dim);
    gui->addSlider("lookatX", -260, 260, &lookatX, length-xInit, dim);
    gui->addSlider("lookatY", -500, 500, &lookatY, length-xInit, dim);
    gui->addSlider("lookatZ", -600, 600, &lookatZ, length-xInit, dim);;

    
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
    if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());

    
    
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
