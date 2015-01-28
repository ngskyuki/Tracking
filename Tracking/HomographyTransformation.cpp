//
//  HomographyTransformation.cpp
//  PlayerTracking
//
//  Created by yuukifujita on 12/31/14.
//  Copyright (c) 2014 yuukifujita. All rights reserved.
//

#include "HomographyTransformation.h"

Mat_<Vec3b> transformHomography(Mat_<Vec3b> imgSrc, Point2f srcPt[], Point2f dstPt[], Size dstSize) {
    Mat_<Vec3b> src = imgSrc;
    Mat_<Vec3b> imgDst(imgSrc.rows, imgSrc.cols, Vec3b(0, 0, 0));
    
    Mat homographyMtx = getPerspectiveTransform(srcPt, dstPt);
    
    warpPerspective(imgSrc, imgDst, homographyMtx, dstSize, INTER_LANCZOS4 + WARP_FILL_OUTLIERS);
    
    return imgDst;
}