//
//  TemplateMatching.h
//  PlayerTracking
//
//  Created by yuukifujita on 1/4/15.
//  Copyright (c) 2015 yuukifujita. All rights reserved.
//

#ifndef __PlayerTracking__TemplateMatching__
#define __PlayerTracking__TemplateMatching__

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "HomographyTransformation.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

struct traj {
    int playerID;
    Point2d currPt;
    Point2d prevPt;
    double derivX;
    double derivY;
    double precision;
    bool isCorrect;
    bool occlusion;
    Mat tmpl;
};

void templateMatching(string fileName[], vector<string> templateNames, int num, int matchingFunction = 0);

vector<Mat> prepareTemplate(vector<string> templateNames);

vector<struct traj> prepareTraj();

int matchCount(Rect rect, vector<Point2i> vec);
int countMatchToFG(Rect rect, Mat maskImg);
void exportData(string exFileName, vector<struct traj> trj, double timeStamp);


//for debug
void debugPrintTrj(struct traj trj, double maxVal);

#endif /* defined(__PlayerTracking__TemplateMatching__) */
