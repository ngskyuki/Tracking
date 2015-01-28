//
//  main.cpp
//  Tracking
//
//  Created by yuukifujita on 1/20/15.
//  Copyright (c) 2015 yuukifujita. All rights reserved.
//

#include <iostream>
#include <opencv2/core.hpp>
#include "TemplateMatching.h"

using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    
    if(argc != 7) {
        cerr << "Incorrect input list" << endl;
        cerr << "exiting..." << endl;
        return EXIT_FAILURE;
    }
    
    string fileName[6];
    string tmplRepos;
    for(int i = 1; i < argc; i++) {
        fileName[i - 1] = argv[i];
        if(i == 4) tmplRepos = argv[4];
    }
    
    vector<string> templateNames;
    for(int i = 1; i < 21; i++) {
        stringstream ss;
        ss << i;
        templateNames.push_back(tmplRepos + "tmpl" + ss.str() + ".png");
    }
    templateNames.push_back(tmplRepos + "judge.png");
    templateMatching(fileName, templateNames, 20);
    return EXIT_SUCCESS;
}
