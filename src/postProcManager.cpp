//
//  postProcManager.cpp
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#include "postProcManager.h"

void postProcManager::setup() {
    width = 1920;
    height = 1080;
    
    // Setup post-processing chain
    post.init(width, height);
    post.createPass<GodRaysPass>()->setEnabled(true);
    
    renderPasses = post.getPasses();
}


    
