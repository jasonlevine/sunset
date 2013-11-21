#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    aa.setupVectors();
    aa.loadTracks();
    aa.setupAUGraph();
    aa.playStems();
    
    track = 2;
    track2 = 0;
    ofSetDepthTest(true);
    
    bassAccum = 0.0;
    
    setupGUI();
    
    for (int i = 0; i < 120; i++) {
        vector<float> wave;
        wave.assign(512, 0);
        waveHistory.push_back(wave);
    }
    
    
    // Setup post-processing chain
    post.init(1920, 1080); //ofGetWidth(), ofGetHeight()
    post.createPass<FxaaPass>()->setEnabled(false);
    post.createPass<BloomPass>()->setEnabled(false);
    post.createPass<DofPass>()->setEnabled(false);
    post.createPass<GodRaysPass>()->setEnabled(false);
    post.createPass<RimHighlightingPass>()->setEnabled(false);
    
    renderPasses = post.getPasses();
    //boids
    boidNum = 20;
	target = ofVec3f(0, 0, 0);
	
	for (int i = 0; i < boidNum; i++)
	{
		SteeredVehicle v(ofRandom(-50, 50), ofRandom(0, 100), ofRandom(-300, -400));
		v.maxForce = 0.5f;
		v.inSightDist = 120.0f;
        v.tooCloseDist = 80.0f;
		boids.push_back(v);
	}

    //sunset
    gradientStartPre = ofFloatColor::orange;
    gradientEnd = ofFloatColor::violet;
    sunColor = ofFloatColor::gold;

    //osc
    receiver.setup(PORT);

}

//--------------------------------------------------------------
void testApp::update(){
    aa.updateAnalytics();

    vector<float> wave;
    aa.taps[track]->getSamples(wave, 0);
    waveHistory.push_back(wave);
    if (waveHistory.size() > 120) waveHistory.erase(waveHistory.begin());
    
    
    vector<float> wave2;
    aa.taps[track2]->getSamples(wave2, 0);
    waveHistory2.push_back(wave2);
    if (waveHistory2.size() > 120) waveHistory2.erase(waveHistory2.begin());
    
    bassAccum += aa.amp[3] / 4;
    
    gui->update();
    
    cam.setPosition(0, camY, camZ);
    cam.lookAt(ofVec3f(0,0,-600));
    
    
    for (int i = 0; i < boidNum; i++)
	{

        if (aa.amp[6] > 0) boids[i].seek(ofVec3f(0,250,-500));
		boids[i].flock(boids);
		boids[i].update();
		boids[i].bounce(1000, 500, 500);
	}
    
    gradientStart = gradientStartPre + aa.amp[1] * 2;
    
    updateOSC();
}

//--------------------------------------------------------------
void testApp::draw(){
//    aa.drawAnalytics();
    
    ofEnableAlphaBlending();
    
    post.begin(cam);
//    cam.begin();
    drawWaves();
    drawSun();
    drawBirds();
//    cam.end();
    post.end();
    ofDisableAlphaBlending();

}

//--------------------------------------------------------------
void testApp::updateOSC() {

	while(receiver.hasWaitingMessages()){

		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
		if(m.getAddress() == "/sunY"){
			sunY = ofMap(m.getArgAsFloat(0), 0.0, 1.0, 0, 300);
		}
        
        if(m.getAddress() == "/camY"){
			camY = ofMap(m.getArgAsFloat(0), 0.0, 1.0, 0, 600);
		}

        if(m.getAddress() == "/meshRotateX"){
			meshRotateX = ofMap(m.getArgAsFloat(0), 0.0, 1.0, -90, 90);
		}

        if(m.getAddress() == "/sunColor"){
            sunColor.set((float)m.getArgAsInt32(0)/255, (float)m.getArgAsInt32(1)/255, (float)m.getArgAsInt32(2)/255);
		}

        if(m.getAddress() == "/gradientStart"){
            gradientStartPre.set((float)m.getArgAsInt32(0)/255, (float)m.getArgAsInt32(1)/255, (float)m.getArgAsInt32(2)/255);
		}

        if(m.getAddress() == "/gradientEnd"){
            gradientEnd.set((float)m.getArgAsInt32(0)/255, (float)m.getArgAsInt32(1)/255, (float)m.getArgAsInt32(2)/255);
		}

        if(m.getAddress() == "/noiseScale"){
			noiseScale = ofMap(m.getArgAsFloat(0), 0.0, 1.0, 0, 0.1);
		}
        
	}

}


//--------------------------------------------------------------
void testApp::drawWaves(){
    
    float noiseVel = bassAccum;
    
    if (waveHistory[0].size() > 0) {
        int width = 512;
        int height = waveHistory.size();
        
        ofMesh mesh;
        
        for (int y = 0; y < height; y++){
            for (int x = 0; x<width; x++){
                float noiseValue = ofNoise(x * noiseScale, y * noiseScale, noiseVel);
                float h = waveHistory[y][x] * noiseValue + noiseValue * 0.5;
                mesh.addVertex(ofPoint(x, h, y)); //waveHistory[y][x] +
                //float h = (waveHistory[y][x] + 1.0) / 2;
                float r, g, b;
                r = h > 0.4 ? (h - 0.4) * 2: 0.0; // + aa.amp[3]
                g = r; //waveHistory2[y][x];
                b = h;
                float dist = float(abs(width / 2 - x)) / width;
                float distSq = dist * dist;
                ofFloatColor gradient = gradientStart * (1.0 - dist) + gradientEnd * dist;
                float yDist = float(y) / height;
                float yDistSq = yDist * yDist;
                mesh.addColor(ofFloatColor(r,g,b) * (1.0 - yDistSq) + gradient * yDistSq);  // add a color at that vertex
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
        ofScale(5,170,10);
        ofTranslate(-width/2, 0, -height/2);
        mesh.draw();
        ofPopMatrix();
    }

}

//--------------------------------------------------------------
void testApp::drawSun(){
    
    
    ofPushMatrix();
    
    ofPushMatrix();
    ofTranslate(0, sunY, -652);
    ofScale(2,2);
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2, 0);
    ofBackgroundGradient(gradientStart, gradientEnd);
    ofPopMatrix();
    
    ofSetColor(sunColor, 127);
    ofBeginShape();
    for (int i = 0; i < 50; i++) {
        int scale = 170 + abs((i % 2) - 1) * 20 + aa.audioFeatures[0]->spectrum[i] * 20;
        float x = cos(float(i) / 50 * TWO_PI) * scale;
        float y = sunY + sin(float(i) / 50 * TWO_PI) * scale;
        ofVertex(x,y, -651);
    }
    ofEndShape();
//    post.end();
    
    
    
    ofSetColor(sunColor);
//    ofSetCircleResolution(50);
//    
//    ofCircle(0, 0, -600, 150);
    ofDrawSphere(0,sunY,-650,150);
    
//    ofSetColor(gold);
//    for (int i = 0; i < particles.size(); i++) {
//        float size = ofMap(particles[i].pos.length(), 0, 170, 10, 5);
//        ofCircle(particles[i].pos.x, particles[i].pos.y, -600, size);
//    }
    ofPopMatrix();
    
    
}
//--------------------------------------------------------------
void testApp::drawBirds(){
    float sineWave = (sin(ofGetElapsedTimef() * 8) + 1) /2;
    
    ofPolyline wing1, wing2;
    ofVec3f center(0,0,0);
    ofVec3f w1b1(10,10+sineWave * 5,0);
    ofVec3f w1b2(20,10+sineWave * 6,0);
    ofVec3f w1b3(30,0+sineWave * 14,0);
    ofVec3f w2b1(-10,10+sineWave * 5,0);
    ofVec3f w2b2(-20,10+sineWave * 6,0);
    ofVec3f w2b3(-30,0+sineWave * 14,0);

//    wing1.arc(center,10, 10, 0, -90, true);
//    wing2.arc(10,0,0, 10, 10, 0, -90, false);
    wing1.addVertex(0,0,0);
    wing1.bezierTo(w1b1, w1b2, w1b3);
    
    wing2.addVertex(0,0,0);
    wing2.bezierTo(w2b1, w2b2, w2b3);
    
//    cam.begin();
    
    for (int i = 0; i < boidNum; i++)
	{
		ofPushMatrix();
		ofTranslate(boids[i].position.x, boids[i].position.y, boids[i].position.z);
		
        ofSetColor(0);
        wing1.draw();
        wing2.draw();
    
        ofPopMatrix();
	}
	
//	cam.end();
    
}


//--------------------------------------------------------------
void testApp::setupGUI(){
    noiseScale = 0.01;
    meshRotateX = 0.0;
    camY = 150;
    camZ = 600;
    sunY = 0;
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 1000);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("noiseScale", 0.0, 0.1, &noiseScale, length-xInit, dim);
    gui->addSlider("meshRotateX", -90.0, 90, &meshRotateX, length-xInit, dim);
    gui->addSlider("camY", 0.0, 500, &camY, length-xInit, dim);
    gui->addSlider("camZ", -600, 600, &camZ, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("sunY", 0, 300, &sunY, length-xInit, dim);
    gui->addSlider("sunColor.r", 0.0, 1.0, &sunColor.r, length-xInit, dim);
    gui->addSlider("sunColor.g", 0.0, 1.0, &sunColor.g, length-xInit, dim);
    gui->addSlider("sunColor.b", 0.0, 1.0, &sunColor.b, length-xInit, dim);
    gui->addSlider("gradientStart.r", 0.0, 1.0, &gradientStartPre.r, length-xInit, dim);
    gui->addSlider("gradientStart.g", 0.0, 1.0, &gradientStartPre.g, length-xInit, dim);
    gui->addSlider("gradientStart.b", 0.0, 1.0, &gradientStartPre.b, length-xInit, dim);
    gui->addSlider("gradientEnd.r", 0.0, 1.0, &gradientEnd.r, length-xInit, dim);
    gui->addSlider("gradientEnd.g", 0.0, 1.0, &gradientEnd.g, length-xInit, dim);
    gui->addSlider("gradientEnd.b", 0.0, 1.0, &gradientEnd.b, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("DofFocalDepth", 1.0, 1.01, 1.0, length-xInit, dim);
    gui->addSlider("DofFocalLength", 10, 1000, 500, length-xInit, dim);
    gui->addSlider("DofFStop", 1.0, 15.0, 1.0, length-xInit, dim);
    gui->addLabelToggle("DofShowFocus", false);
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}


//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
    if(name == "DofFocalDepth")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        shared_ptr<DofAltPass> Dof = static_pointer_cast<DofAltPass>(renderPasses[2]);
        Dof->setFocalDepth(slider->getScaledValue());
	}
    else if(name == "DofFocalLength")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        shared_ptr<DofAltPass> Dof = static_pointer_cast<DofAltPass>(renderPasses[2]);
        Dof->setFocalLength(slider->getScaledValue());
	}
    else if(name == "DofFStop")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        shared_ptr<DofAltPass> Dof = static_pointer_cast<DofAltPass>(renderPasses[2]);
        Dof->setFStop(slider->getScaledValue());
	}
    else if(name == "DofShowFocus") //kind == OFX_UI_WIDGET_LABELBUTTON
    {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        shared_ptr<DofAltPass> Dof = static_pointer_cast<DofAltPass>(renderPasses[2]);
        cout << button->getValue();
        Dof->setShowFocus(button->getValue());
    }
    else if(name == "sunY")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        shared_ptr<GodRaysPass> godRays = static_pointer_cast<GodRaysPass>(renderPasses[3]);
        godRays->setLightPositionOnScreen(ofVec3f(0, sunY, -600));
	}

    

    

//    if(name == "noiseScale")
//    {
//        ofxUISlider *slider = (ofxUISlider *) e.widget;
//        noiseScale = slider->getScaledValue();
////        cout << "value: " << slider->getScaledValue() << endl;
//	}
//	else if(name == "meshRotateX")
//	{
//		ofxUISlider *slider = (ofxUISlider *) e.widget;
//		meshRotateX = slider->getScaledValue();
//        cout << "value: " << slider->getScaledValue() << endl;
//	}
//	else if(name == "camY")
//	{
//		ofxUISlider *slider = (ofxUISlider *) e.widget;
//		camY = slider->getScaledValue();
//        cout << "value: " << slider->getScaledValue() << endl;
//    }
//    else if(name == "camZ")
//	{
//		ofxUISlider *slider = (ofxUISlider *) e.widget;
//		camZ = slider->getScaledValue();
//        cout << "value: " << slider->getScaledValue() << endl;
//    }
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
    unsigned idx = key - '0';
    if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());

    
    
    switch (key) {

//        case '0':
//        case '1':
//        case '2':
//        case '3':
//        case '4':
//        case '5':
//        case '6':
//            aa.setMode(key - 48);
////            track = key - 48;
//            break;

            
        case 'a':
            for (int i = 0; i < boidNum; i++)
            {
                boids[i].seek(ofVec3f(0,sunY,-500));
            }
            break;
                            
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'b':
            
            break;
            
        case 'g':
        {
            gui->toggleVisible();
        }
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