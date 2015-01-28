//
//  MathUtil.cpp
//  Tracking
//
//  Created by yuukifujita on 1/20/15.
//  Copyright (c) 2015 yuukifujita. All rights reserved.
//

#include "MathUtil.h"

double MathUtil::gaussianPDF(double mean, double var, double randVal) {        double result;
    result = (1.0 / var * sqrt(2.0 * 3.14))
            * exp(-pow(randVal - mean, 2.0) / (2.0 * pow(var, 2.0)));
    return result;
}
