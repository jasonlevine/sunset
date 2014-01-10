#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    aa.playStems(0);
    colorScheme.setup();
    colorScheme.gui->setVisible(false);
    
    for (int i = 0; i < 13; i++) {
        colors.push_back(ofFloatColor(ofRandomuf()));
    }
    
    for (int i = 0; i < 13; i++) {
         colorScheme.addColorRef(&colors[i]);
    }

    colorScheme.assignRandom(true);
    
    ofSetDepthTest(true);

    
    setupGUI();
    
    for (int i = 0; i < 120; i++) {
        vector<float> wave;
        wave.assign(512, 0);
        waveHistory.push_back(wave);
        waveLoHistory.push_back(ofFloatColor(0.3, 0.5, 0.3));
        waveHiHistory.push_back(ofFloatColor(0.8, 0.5, 0.7));
    }
    
//    waveLoHistory[15].set(1.0,0.0,0.0);
//    waveHiHistory[15].set(1.0,0.0,0.0);
//    
    
    width = 1920;
    height = 1080;
    
    // Setup post-processing chain
    post.init(width, height);
//    post.createPass<GodRaysPass>()->setEnabled(true);
//    post.createPass<DofPass>()->setEnabled(true);

    
    
    renderPasses = post.getPasses();



    lookatX = 0;
    lookatY = 0;
    lookatZ = -600;
    
    drawPost = false;
    
    
    ///blend
    shaderBlend.load("shaders/blendShader");
    shadeBlendMix = 0.5;
    shadeBlendMode = 1;
    
    state = 0;
    
    camYDecayed.setDecay(0.995);
    camYSmoothed.setNumPValues(35);
    
    
    // turn on smooth lighting //
    ofSetSmoothLighting(true);

	
    
    // Point lights emit light in all directions //
    // set the diffuse color, color reflected from the light source //
    pointLight.setDiffuseColor( ofColor(0.f, 255.f, 0.f));
    
    // specular color, the highlight/shininess color //
	pointLight.setSpecularColor( ofColor(255.f, 255.f, 255.f));
    pointLight.setPosition(cam.screenToWorld(ofVec3f(ofGetWidth()/2, ofGetHeight()/2, 0)));
    
    // shininess is a value between 0 - 128, 128 being the most shiny //
	material.setShininess( 64 );
    material.setAmbientColor(colorScheme.colorScheme[0][0]);
}

//--------------------------------------------------------------
void testApp::update(){
    aa.updateAnalytics();
    

//    colorScheme.setHue(ofMap(aa.pitch[0], 0, aa.maxPitch[0], 0.0, 1.0));
    colorScheme.setSaturation(ofMap(aa.centroidSmoothed.getMean(), 30, 55, 0.0, 1.0));
    colorScheme.setBrightness(aa.ampSmoothed.getMean());
//    colorScheme.setDistance(ofMap(aa.audioFeatures[0]->spectralFeatures["skew"], 2000, 7000, 0.0, 1.0));

    vector<float> wave;
    aa.taps[0]->getSamples(wave, 0);
    wave.resize(512);
    
    waveHistory.push_back(wave);
    if (waveHistory.size() > 120) waveHistory.erase(waveHistory.begin());
    
    for (int i = 0; i < wave.size(); i++) {
        if (waveHistory[waveHistory.size()-1][i] < 0) {
            waveHistory[waveHistory.size()-1][i] = 0;//*= -1;
        }
    }

    
    for (int i = 0; i < wave.size(); i++) {
        smoother heightSmooth;
        heightSmooth.setNumPValues(2);
        for (int j = waveHistory.size()-10; j < waveHistory.size(); j++) {
            heightSmooth.addValue(waveHistory[j][i]);
            waveHistory[j][i] = heightSmooth.getMean();
        }
    }
    
    

    waveHiHistory.push_back(colorScheme.getRandomColor());
    if (waveHiHistory.size() > 120) waveHiHistory.erase(waveHiHistory.begin());
    
    waveLoHistory.push_back(colorScheme.getRandomColor());
    if (waveLoHistory.size() > 120) waveLoHistory.erase(waveLoHistory.begin());
    


    
    
    gui->update();
    
    camYDecayed.addValue(waveHistory[35][256] * hScale * 180 + 100);
    camYDecayed.update();
    camYSmoothed.addValue(camYDecayed.getValue());
    camY = camYSmoothed.getMean();
    
    
    cam.setPosition(camX, camY, camZ);
    cam.lookAt(ofVec3f(lookatX, lookatY, lookatZ));
    
}

//--------------------------------------------------------------
void testApp::draw(){
//    aa.drawAnalytics();
    
    ofEnableAlphaBlending();
    
//    mainFbo.begin();
//    cam.begin();
    if (state == 0) {
        post.begin(cam);
        // enable lighting //
        ofEnableLighting();
        // the position of the light must be updated every frame,
        // call enable() so that it can update itself //
        pointLight.enable();
        material.begin();
        ofBackground(15); //colorScheme.colorScheme[0][0]
        drawWaves();
        post.end();
        material.end();
        ofDisableLighting();
    }
    else if (state == 1) {
        colorScheme.draw();
    }
    else if (state == 2) {
        aa.drawAnalytics();
    }
//    cam.end();
//    mainFbo.end();
//    post.process(mainFbo);
//    }
    

    ofDisableAlphaBlending();

}

//--------------------------------------------------------------
void testApp::drawWaves(){
    
    if (waveHistory[0].size() > 0) {
        int width = 512;
        int height = waveHistory.size();
        
        ofMesh mesh;
        
        for (int y = 0; y < height; y++){
            for (int x = 0; x<width; x++){
                float h = waveHistory[y][x];
                mesh.addVertex(ofPoint(x, h * hScale, y));
                
                float col = h * colScale;
                mesh.addColor(waveHiHistory[y] * col + waveLoHistory[y] * (1.0 - col));
                
                //float h = (waveHistory[y][x] + 1.0) / 2;
//                float r, g, b;
//                r = h > 0.4 ? (h - 0.4) * 2: 0.0; // + aa.amp[3]
//                g = r; //waveHistory2[y][x];
//                b = h;
//                float dist = float(abs(width / 2 - x)) / width;
//                float distSq = dist * dist;
//                ofFloatColor gradient = gradientStart * (1.0 - dist) + gradientEnd * dist;
//                float yDist = float(y) / height;
//                float yDistSq = yDist * yDist;
//                mesh.addColor(ofFloatColor(r,g,b) * (1.0 - yDistSq) + gradient * yDistSq);  // add a color at that vertex
            }
        }
        
        // now it's important to make sure that each vertex is correctly connected with the
        // other vertices around it. This is done using indices, which you can set up like so:
        for (int y = 0; y < height-1; y++){
            for (int x = 0; x < width-1; x++){
                mesh.addIndex(x+y*width);       // 0
                mesh.addIndex((x+1)+y*width);     // 1
                mesh.addIndex(x+(y+1)*width);     // 10
                
                mesh.addIndex((x+1)+y*width);     // 1
                mesh.addIndex((x+1)+(y+1)*width);   // 11
                mesh.addIndex(x+(y+1)*width);     // 10
            }
        }
        

        //        ofDrawAxis(100);
        ofPushMatrix();
        ofRotateX(meshRotateX);
        ofRotateY(180);
        ofScale(7,180,10);
        ofTranslate(-width/2, 0, -height/2);
        mesh.draw();
        ofPopMatrix();
        
    }

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
void testApp::blend(ofFbo &base, ofFbo &blend, ofFbo &result, float mix, int mode)
{
    ofTexture texBase = base.getTextureReference();
    ofTexture texBlend = blend.getTextureReference();
    
    result.begin();
    ofClear(0, 0, 0, 1);
    
    shaderBlend.begin();
    shaderBlend.setUniform1f( "blendmix",	mix );
    shaderBlend.setUniform1i( "blendmode",	mode);
    shaderBlend.setUniformTexture("texBase",   texBase, 0 );
    shaderBlend.setUniformTexture("texBlend",  texBlend, 1 );
    
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(ofGetWidth(), 0); glVertex3f(ofGetWidth(), 0, 0);
    glTexCoord2f(ofGetWidth(), ofGetHeight()); glVertex3f(ofGetWidth(), ofGetHeight(), 0);
    glTexCoord2f(0,ofGetHeight());  glVertex3f(0,ofGetHeight(), 0);
    glEnd();
    
    shaderBlend.end();
    result.end();
}


//--------------------------------------------------------------
void testApp::setupGUI(){

    meshRotateX = 0.0;
    camX = 0;
    camY = 150;
    camZ = 600;
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("hScale", 1.0, 20.0, &hScale, length-xInit, dim);
    gui->addSlider("colScale", 1.0, 10.0, &colScale, length-xInit, dim);
    gui->addLabelToggle("GodRays", false);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("meshRotateX", -90.0, 90, &meshRotateX, length-xInit, dim);
    gui->addSlider("camX", -260, 260, &camX, length-xInit, dim);
    gui->addSlider("camY", 0.0, 500, &camY, length-xInit, dim);
    gui->addSlider("camZ", -600, 600, &camZ, length-xInit, dim);
    gui->addSlider("lookatX", -260, 260, &lookatX, length-xInit, dim);
    gui->addSlider("lookatY", -500, 500, &lookatY, length-xInit, dim);
    gui->addSlider("lookatZ", -600, 600, &lookatZ, length-xInit, dim);;

    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}


//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
    if(name == "GodRays") //kind == OFX_UI_WIDGET_LABELBUTTON
    {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        post[0]->setEnabled(button->getValue());
    }
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
            break;
            
        case 'G':
            colorScheme.gui->toggleVisible();
            break;
            
        case 's':
            state = (state + 1) % 3;
            if (state == 0){
                colorScheme.gui->setVisible(false);
            }
            else if (state == 1) {
                gui->setVisible(false);
                colorScheme.gui->setVisible(true);
            }
            else if (state == 2) {
                gui->setVisible(false);
                colorScheme.gui->setVisible(false);
            }
            break;
            
        case 'p':
            aa.playStems(0);
            break;
        
        case 'r':
            colorScheme.assignRandom(true);
            break;
            
        case 'k':
            
            cout << "kurtosis " << aa.audioFeatures[0]->spectralFeatures["kurtosis"] << endl;
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
