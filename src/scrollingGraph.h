//
//  scrollingGraph.h
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-01-06.
//
//


class scrollingGraph {
public:
    void setup(int size, float init, float min, float max);
    void addValue(float val);
    void draw();
    
    vector<vector<float> > valHistory;
    float minVal, maxVal;
};