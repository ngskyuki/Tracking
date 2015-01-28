//
//  MathUtil.h
//  Tracking
//
//  Created by yuukifujita on 1/20/15.
//  Copyright (c) 2015 yuukifujita. All rights reserved.
//

#ifndef __Tracking__MathUtil__
#define __Tracking__MathUtil__

#include <stdio.h>
#include <math.h>

class MathUtil {
public:
    static double gaussianPDF(double mean, double var, double randVal);
};
#endif /* defined(__Tracking__MathUtil__) */
