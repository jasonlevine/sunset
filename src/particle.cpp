//
//  particle.cpp
//  particleSystem
//
//  Created by Jason Levine on 2013-10-16.
//
//

#include "particle.h"

particle::particle(ofVec2f _pos, ofVec2f _vel) {
    pos = _pos;
    vel = _vel;
    decay = 0.999;
}

void particle::update() {
    pos += vel;
    vel *= decay;
}

void particle::draw() {
//    ofSetColor(255);
    ofCircle(pos, 5);
}