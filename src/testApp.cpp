#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(30);
    ofSetDepthTest(true);
    state = 3;
    
    aa.playStems(0);
    cs.setup();
    cs.gui->setVisible(false);
    
    tm.setup(&aa, &cs);
    cm.setup(&tm);
    lm.setup(&cs);
    pm.setup();
    
    useLights = true;
    
    setupGUI();
    
    isShaderDirty = true;
    
	mLigDirectional.setup();
	mLigDirectional.setDirectional();
    
	mCamMainCam.setupPerspective(false);
    cm.cam.setupPerspective(false);
	mLigDirectional.setAmbientColor(ofColor::fromHsb(100, 0, 100));
	mLigDirectional.setDiffuseColor(ofColor::fromHsb(20, 120, 128));
	mLigDirectional.setSpecularColor(ofColor(255,255,255));
	
	mMatMainMaterial.setDiffuseColor(ofColor(0,0,0));
	mMatMainMaterial.setSpecularColor(ofColor(200,200,200));
	mMatMainMaterial.setShininess(25.0f);
	
	mCamMainCam.setDistance(200);
	
	
	shouldDrawBuiltinBox = false;
	shouldRenderNormals = false;
	shouldUseFlatShading = true;

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
    
    if (isShaderDirty){
		
		GLuint err = glGetError();	// we need this to clear out the error buffer.
		
		if (mShdPhong != NULL ) delete mShdPhong;
		mShdPhong = new ofShader();
		mShdPhong->load("shaders/phong");
		err = glGetError();	// we need this to clear out the error buffer.
		ofLogNotice() << "Loaded Shader: " << err;
        
		
		isShaderDirty = false;
	}

    
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
    else if (state == 3) {
        glShadeModel(GL_SMOOTH);
        glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
        
        ofBackgroundGradient(ofColor::fromHsb(0, 0, 120), ofColor::fromHsb(0, 0, 0));
        
//        cm.cam.begin();
        mCamMainCam.begin();
        ofEnableLighting();
        
        mLigDirectional.setGlobalPosition(1000, 1000, 1000);
        mLigDirectional.lookAt(ofVec3f(0,0,0));
        
        ofEnableSeparateSpecularLight();
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        mLigDirectional.enable();
        ofSetColor(mLigDirectional.getDiffuseColor());
        mMatMainMaterial.begin();
        
        mShdPhong->begin();
        
        if (shouldRenderNormals){
            mShdPhong->setUniform1f("shouldRenderNormals", 1.0);
        } else {
            mShdPhong->setUniform1f("shouldRenderNormals", 0.0);
        }
        
        glPushAttrib(GL_SHADE_MODEL);
        if (shouldUseFlatShading){
            mShdPhong->setUniform1f("shouldUseFlatShading", 1.0);
            glShadeModel(GL_FLAT);
            glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);		// OpenGL default is GL_LAST_VERTEX_CONVENTION
        } else {
            mShdPhong->setUniform1f("shouldUseFlatShading", 0.0);
            glShadeModel(GL_SMOOTH);
            glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
        }
        
        tm.draw();
        // restores shade model
        glPopAttrib();
        // restores vertex convention defaults.
        glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
        
        mShdPhong->end();
        
        // we revert to default values, to not end up
        
        
        mMatMainMaterial.end();
        mLigDirectional.disable();
        
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        
        ofDisableLighting();
        
        
//        cm.cam.end();
        mCamMainCam.end();

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

}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
    pm.keyPressed(key);
    
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
            state = (state + 1) % 4;
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
        
        case 'n':
			shouldRenderNormals ^= true;
			break;
		case 'q':
			shouldUseFlatShading ^= true;
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
