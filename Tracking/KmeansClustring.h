//
//  KmeansClustring.h
//  PlayerTracking
//
//  Created by yuukifujita on 12/31/14.
//  Copyright (c) 2014 yuukifujita. All rights reserved.
//

#ifndef __PlayerTracking__KmeansClustring__
#define __PlayerTracking__KmeansClustring__

#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

struct cluster {
    long cur_x, cur_y;
    int no;
    long next_x, next_y;
};

double dist(struct cluster cluster, int x, int y, int loopNum);
vector<Point2i> clustering(int k, vector<Point2i> points, int loopNum, vector<Point2f> initVec);

#endif /* defined(__PlayerTracking__KmeansClustring__) */
