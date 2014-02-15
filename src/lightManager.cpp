//
//  lightManager.cpp
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#include "lightManager.h"

void lightManager::setup(colorSchemeDesigner * _cs) {
    cs = _cs;
    
    ofSetSmoothLighting(true);
    pointLight.setAmbientColor( cs->colorScheme[0][2]);
    pointLight.setDiffuseColor( cs->colorScheme[0][1]);
    
	pointLight.setSpecularColor( ofColor(255.f, 255.f, 255.f));

    pointLight.setPosition(0.5, 0.5, 0.5);
    
	material.setShininess( 128 );
    material.setAmbientColor(cs->colorScheme[0][0]);
}

void lightManager::update() {
    //    pointLight.setPosition(camX, camY, -camZ);
    pointLight.setAmbientColor( cs->colorScheme[0][2]);
    pointLight.setDiffuseColor( cs->colorScheme[0][1]);
}

void lightManager::begin() {
    ofEnableLighting();
    pointLight.enable();
    material.begin();
}

void lightManager::end() {
    material.end();
    ofDisableLighting();
}

