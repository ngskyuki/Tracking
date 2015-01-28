//
//  HomographyTransformation.h
//  PlayerTracking
//
//  Created by yuukifujita on 12/31/14.
//  Copyright (c) 2014 yuukifujita. All rights reserved.
//

#ifndef __PlayerTracking__HomographyTransformation__
#define __PlayerTracking__HomographyTransformation__

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

Mat_<Vec3b> transformHomography(Mat_<Vec3b> imgSrc, Point2f srcPt[], Point2f dstPt[], Size dstSize);

#endif /* defined(__PlayerTracking__HomographyTransformation__) */
