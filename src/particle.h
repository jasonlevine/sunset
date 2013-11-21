//
//  particle.h
//  particleSystem
//
//  Created by Jason Levine on 2013-10-16.
//
//

#include "ofMain.h"

class particle {
public:
    
    particle(ofVec2f _pos, ofVec2f _vel);
    void update();
    void draw();

    ofVec2f pos, vel;
    float decay;
    
    
};
