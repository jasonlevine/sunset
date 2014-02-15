//
//  cameraManager.cpp
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#include "cameraManager.h"

void cameraManager::setup(terrainManager * _tm) {
    tm = _tm;
        
    lookatX = 0;
    lookatY = 0;
    lookatZ = -600;
    
    camYDecayed.setDecay(0.99);
    camYSmoothed.setNumPValues(25);
}

void cameraManager::update() {
    //    if (waveHistory[10][camX + 255] < waveHistory[5][camX + 256]) {
    //        camX--;
    //    }
    //    else if (waveHistory[10][camX + 257] < waveHistory[5][camX + 256]) {
    //        camX++;
    //    }
    
    
    camYDecayed.addValue(tm->waveHistory[45][camX + 256] * 20 * 180 + 100);
    camYDecayed.update();
    camYSmoothed.addValue(camYDecayed.getValue());
    camY = camYSmoothed.getMean();
    
    cam.setPosition(camX, camY, camZ);
    cam.lookAt(ofVec3f(lookatX, lookatY, lookatZ));
}
