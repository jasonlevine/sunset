//
//  lightManager.h
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#pragma once

#include "ofMain.h"
#include "colorSchemeDesigner.h"

class lightManager {
public:
    void setup(colorSchemeDesigner * _cs);
    void update();
    void begin();
    void end();
    
    colorSchemeDesigner * cs;
    
    ofLight pointLight;
    ofMaterial material;
    
    ofShader*   mShdPhong;
	ofLight		mLigDirectional;
	ofMaterial	mMatMainMaterial;
    
	ofMesh		mMshCube;
	
	bool		isShaderDirty;
	bool		shouldDrawBuiltinBox;
	bool		shouldRenderNormals;
	bool		shouldUseFlatShading;

};
