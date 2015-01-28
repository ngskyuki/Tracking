//
//  TemplateMatching.cpp
//  PlayerTracking
//
//  Created by yuukifujita on 1/4/15.
//  Copyright (c) 2015 yuukifujita. All rights reserved.
//

#include "TemplateMatching.h"

using namespace std;
using namespace cv;

void templateMatching(string fileName[], vector<string> templateNames, int num, int matchingFunction) {
    
    /*************
     ** For import
     *************/
    VideoCapture capture;
    capture = VideoCapture(fileName[0]);
    Mat readImg;
    
    /*********************
     ** Prepare for output
     *********************/
    string exFileName = fileName[3]; //output file path
    ofstream ofs(exFileName, ios::app); // open output file in append-mode
    ofs << "Time" << "," << "ID" << "," << "X" << "," << "Y" << "," << "Pricision" << "," << "isCorrect" << "," << "Occlusion" << endl; //and write columns
    string OUT_VIDEO = fileName[4];
    Size S = Size((int)capture.get(CAP_PROP_FRAME_WIDTH),
                  (int)capture.get(CAP_PROP_FRAME_HEIGHT));
    VideoWriter writer(OUT_VIDEO, CV_FOURCC('S', 'V', 'Q', '3'),
                       (double)capture.get(CAP_PROP_FPS), S);
    
    /***************************************
     ** Prepare windows for result and debug
     ***************************************/
    string coef_window = "CCOEFF Image";
    namedWindow(coef_window, WINDOW_AUTOSIZE);
    string fgMask_window = "fgMasked Image";
    namedWindow(fgMask_window, WINDOW_AUTOSIZE);
    string debug_window = "Debug Image";
    namedWindow(debug_window, WINDOW_AUTOSIZE);
    
    /************************
     ** Points for homography
     ************************/
    Size dstSize = Size(550, 375);
    const int dstHeight = 375;
    const int dstWidth = 550;
    Point2f srcPt[] = {
        Point2f(202.0, 137.0),
        Point2f(38.0, 279.0),
        Point2f(687.0, 279.0),
        Point2f(522.0, 137.0)
    };
    Point2f dstPt[] = {
        Point2f(0.0, 0.0),
        Point2f(0, dstHeight),
        Point2f(dstWidth, dstHeight),
        Point2f(dstWidth, 0)
    };
    
    /**********************************
     ** Prepare for creating time stamp
     **********************************/
    double timeStamp = 0.0;
    int frameCount = 0;
    int secCount = 0;
    int sec = 0;
    
    /********************************************************
     ** Initialize for setting ROI to apply template matching
     ********************************************************/
    int roiX = 0;
    int roiY = 0;
    int roiW = 0;
    int roiH = 0;
    
    int jRoiX = 0;
    int jRoiY = 0;
    
    /********************************************************
     ** Initialize for template matching
     ********************************************************/
    Mat coefImg;
    Mat grayImg;
    Mat coef_tmpl;
    Mat coef_result;
    Rect searchRoiRect;
    Rect coef_roiRect;
    Mat coef_display;
    Mat canny;
    bool isMatchToFG = true;
    double matchRatio;
    
    bool coef_limitCome;
    double coef_maxVal;
    Point coef_maxPt;
    vector<Mat> templates = prepareTemplate(templateNames);
    vector<struct traj> trj = prepareTraj();
    
    for(int i = 0; i < trj.size(); i++) {
        trj[i].tmpl = templates[i];
    }
    
    /*******************************************
     ** Initialize for tracking judge trajectory
     *******************************************/
    Mat judgeResult;
    double judgeVal;
    Point judgePt;
    Rect judgeRoiRect;
    Rect judgeSearchRect;
    struct traj judge;
    judge.playerID = 99;
    judge.derivX = 0.0;
    judge.derivY = 0.0;
    Mat judgeTmpl = templates[templates.size() - 1];
    judgeTmpl.convertTo(judgeTmpl, 16);
    judgeRoiRect = Rect(0, 0, judgeTmpl.cols, judgeTmpl.rows);
    judgeSearchRect = Rect(0, 0, judgeTmpl.cols + 10, judgeTmpl.rows + 10);
    
    /****************************************
     ** Initialize for Background Subtraction
     ****************************************/
    Ptr<BackgroundSubtractorMOG2> pMOG;
    pMOG = createBackgroundSubtractorMOG2();
    pMOG->setHistory(1);
    Mat fgMaskMog;
    
    Mat bg1;
    Mat bg2;
    Mat bg;
    Mat tmpBg1;
    Mat tmpBg2;
    bg.create(375, 550, CV_8UC3);
    Rect mask1 = Rect(275, 0, (dstSize.width / 2), dstSize.height);
    Rect mask2 = Rect(0, 0, (dstSize.width / 2), dstSize.height);
    VideoCapture bgcap = VideoCapture(fileName[1]);
    VideoCapture bgcap2 = VideoCapture(fileName[2]);
    bgcap >> tmpBg1;
    bgcap2 >> tmpBg2;
    
    bg1 = transformHomography(tmpBg1, srcPt, dstPt, dstSize);
    bg2 = transformHomography(tmpBg2, srcPt, dstPt, dstSize);
    
    bg1(mask1).copyTo(bg(mask1));
    bg2(mask2).copyTo(bg(mask2));
    
    pMOG->apply(bg, fgMaskMog);
    pMOG->setHistory(0);
    
    /************************************
     ** Declare other necessory variables
     ************************************/
    bool isFirst = true;
    char keyPressed;
    
    while(1) {
        
        /********************
         ** Create time stamp
         ********************/
        if(secCount == 33) {
            secCount = 0;
            sec++;
        }
        frameCount++;
        timeStamp = (double)sec + secCount * 0.03;
        
        /**********************************
         ** Import video frame and transfer
         **********************************/
        capture >> readImg;
        readImg.convertTo(readImg, 16);
        coefImg = transformHomography(readImg, srcPt, dstPt, dstSize);
        
        /************************************************
         ** Apply Background Subtraction to current image
         ************************************************/
        pMOG->apply(coefImg, fgMaskMog, 0);
        
        coefImg.copyTo(coef_display);
        
        Mat grayFrame;
        cvtColor(coefImg, grayFrame, COLOR_BGR2GRAY);
        
        coef_limitCome = false;
        int coef_templateIndex = 0;
        coef_result = Mat(coefImg.cols, coefImg.rows, CV_64FC1);
        vector<Point2i> detectPoint;
        vector<struct traj> missedTrj;
        /*************************
         ** For judge man tracking
         *************************/
        if(isFirst) {
            matchTemplate(coefImg, judgeTmpl, judgeResult, TM_CCOEFF_NORMED);
            normalize(judgeResult, judgeResult, 0, 1, NORM_MINMAX, -1, Mat());
        } else {
            jRoiX = judge.derivX >= 0 ? judge.currPt.x : judge.currPt.x - 2;
            jRoiY = judge.derivY >= 0 ? judge.currPt.y : judge.currPt.y - 2;
            judgeSearchRect.y = jRoiY;
            judgeSearchRect.x = jRoiX;
            matchTemplate(coefImg(judgeSearchRect), judgeTmpl, judgeResult, TM_CCOEFF_NORMED);
            normalize(judgeResult, judgeResult, 0, 1, NORM_MINMAX, -1, Mat());
        }
        minMaxLoc(judgeResult, NULL, &judgeVal, NULL, &judgePt);
        judgeRoiRect.y = judgePt.y + jRoiY;
        judgeRoiRect.x = judgePt.x + jRoiX;
        rectangle(coef_display, judgeRoiRect, Scalar(255, 0, 0));
        rectangle(coefImg, judgeRoiRect, Scalar(0, 0, 255), CV_FILLED);
        
        if(isFirst) {
            judge.currPt.y = judgeRoiRect.y;
            judge.currPt.x = judgeRoiRect.x;
        } else {
            swap(judge.prevPt, judge.currPt);
            
            judge.currPt.y = judgeRoiRect.y;
            judge.currPt.x = judgeRoiRect.x;
            
            judge.derivX = judge.currPt.x - judge.prevPt.x;
            judge.derivY = judge.currPt.y - judge.prevPt.y;
        }
        
        /**************************
         ** Apply template matching
         **************************/
        for(int i = 0;  i < trj.size(); i++) {
            
            if(coef_limitCome) {
                if(coef_templateIndex < templates.size() - 1) coef_templateIndex++;
                coef_limitCome = false;
            }
            coef_tmpl = trj[i].tmpl;
            coef_roiRect = Rect(0, 0, coef_tmpl.cols, coef_tmpl.rows);
            if(isFirst) {
                matchTemplate(coefImg, coef_tmpl, coef_result, TM_CCOEFF_NORMED);
                normalize(coef_result, coef_result, 0, 1, NORM_MINMAX, -1, Mat());
            } else {
                /******************************************
                 ** Difine range to apply template matching
                 ******************************************/
                roiX = trj[i].derivX > 0 ? trj[i].currPt.x : trj[i].currPt.x - 2;
                roiY = trj[i].derivY > 0 ? trj[i].currPt.y : trj[i].currPt.y - 2;
                
                roiW = trj[i].occlusion ? coef_tmpl.cols + 10 : coef_tmpl.cols + 7;
                roiH = trj[i].occlusion ? coef_tmpl.rows + 10 : coef_tmpl.rows + 7;
                
                if(roiY < 0) roiY = 0;
                else if(roiY > coefImg.rows) roiY = coefImg.cols;
                if(roiX < 0) roiX = 0;
                else if(roiX > coefImg.cols) roiX = coefImg.rows;
                
                searchRoiRect = Rect(roiX, roiY, roiW, roiH);
                matchTemplate(coefImg(searchRoiRect), coef_tmpl, coef_result, TM_CCOEFF_NORMED);
                normalize(coef_result, coef_result, 0, 1, NORM_MINMAX, -1, Mat());
            }
            minMaxLoc(coef_result, NULL, &coef_maxVal, NULL, &coef_maxPt);
            
            /*******************************************************
             ** Fill and mark image with current player information
             *******************************************************/
            bool isMissed = false;
            
            /*
             if(isMissed) {
             cout << "Missed No." << i << " value is: " << coef_maxVal << endl;
             string newTmplPath;
             cin >> newTmplPath;
             Mat newTmpl = imread(newTmplPath, 21);
             newTmpl.convertTo(newTmpl, 16);
             trj[i].tmpl = newTmpl;
             }
             */
            
            if(isFirst) {
                coef_roiRect.y = coef_maxPt.y;
                coef_roiRect.x = coef_maxPt.x;
                
            } else {
                coef_roiRect.y = roiY + coef_maxPt.y;
                coef_roiRect.x = roiX + coef_maxPt.x;
            }
            /* TODO: Make some validation based on ForeGround-Mask image
             matchRatio = (double)countMatchToFG(coef_roiRect, fgMaskMog) / (coef_roiRect.width * coef_roiRect.height);
             
             if(matchRatio < 0.2) {
             cout << "No:" << i << "'s FG matched ratio is: " << matchRatio << endl;
             isMatchToFG = false;
             }
             
             if(!isMatchToFG) {
             Rect tmpROI;
             tmpROI = Rect(coef_roiRect.x, coef_roiRect.y, coef_roiRect.width, coef_roiRect.height);
             double tmpMaxRatio = matchRatio;
             double tmpRatio;
             Point tmpMaxPt = Point(coef_roiRect.x, coef_roiRect.y);
             for(int y = coef_roiRect.y - 5; y < coef_roiRect.y + coef_roiRect.height + 5; y++) {
             for(int x = coef_roiRect.x - 5; x < coef_roiRect.x + coef_roiRect.width + 5; x++) {
             tmpROI.x = fgMaskMog.cols > x ? x : fgMaskMog.cols;
             tmpROI.y = fgMaskMog.rows > y ? y : fgMaskMog.rows;
             tmpRatio = (double)countMatchToFG(tmpROI, fgMaskMog) / coef_roiRect.area();
             tmpMaxRatio = tmpRatio > tmpMaxRatio ? tmpRatio : tmpMaxRatio;
             tmpMaxPt = tmpRatio > tmpMaxRatio ? Point(x, y) : tmpMaxPt;
             }
             }
             cout << "reseted max value is: " << tmpMaxRatio << endl;
             cout << "and max point is: " << tmpMaxPt.x << " , " << tmpMaxPt.y << endl;
             isMatchToFG = true;
             }
             */
            detectPoint.push_back(Point2i(coef_roiRect.x, coef_roiRect.y));
            
            rectangle(coefImg, coef_roiRect, Scalar(0, 0, 255), CV_FILLED);
            rectangle(fgMaskMog, coef_roiRect, Scalar(0, 0, 255));
            rectangle(coef_display, coef_roiRect, Scalar(255, 0, 0));
            Rect prevRect = Rect(trj[i].prevPt.x, trj[i].prevPt.y, coef_tmpl.cols, coef_tmpl.rows);
            rectangle(coef_display, prevRect, Scalar(255, 0, 255));
            putText(coef_display, to_string(timeStamp), Point(0, 50), FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0, 255, 0));
            putText(coef_display, to_string(trj[i].playerID), Point(coef_roiRect.x + 1, coef_roiRect.y + 8), FONT_HERSHEY_COMPLEX, 0.3, Scalar(0, 0, 255));
            
            /****************************
             ** Update player information
             ****************************/
            if(isFirst) {
                trj[i].currPt = Point2d(coef_roiRect.x, coef_roiRect.y);
            } else {
                
                swap(trj[i].prevPt, trj[i].currPt);
                
                trj[i].currPt = Point2d(coef_roiRect.x, coef_roiRect.y);
                
                trj[i].derivX = trj[i].currPt.x - trj[i].prevPt.x;
                trj[i].derivY = trj[i].currPt.y - trj[i].prevPt.y;
                
                trj[i].precision = coef_maxVal;
                trj[i].isCorrect = (coef_maxVal > 0);
            }
            //for debug
            debugPrintTrj(trj[i], coef_maxVal);
            coef_limitCome = true;
        }
        
        /******************
         ** Occlusion check
         ******************/
        vector<struct traj>::iterator begin, end;
        vector<struct traj>::iterator tbegin, tend;
        begin = trj.begin();
        end = trj.end();
        for(; begin != end; begin++) {
            bool occlusion = false;
            tbegin = trj.begin();
            tend = trj.end();
            for(; tbegin != tend; tbegin++) {
                if(abs(tbegin->currPt.x - begin->currPt.x) < 4 &&
                   abs(tbegin->currPt.y - begin->currPt.y) < 15) {
                    occlusion = true;
                }
            }
            begin->occlusion = occlusion;
        }
        
        /**************
         ** Export part
         **************/
        //imshow(debug_window, coefImg);
        imshow(coef_window, coef_display);
        imshow(fgMask_window, fgMaskMog);
        exportData(exFileName, trj, timeStamp);
        if(isFirst) isFirst = false;
        
        keyPressed = waitKey(10);
        if(keyPressed == 'r') {
            break;
        }
        secCount++;
    }
}

/****************************
 ** For preparation templates
 ****************************/
vector<Mat> prepareTemplate(vector<string> templateNames) {
    vector<Mat> templates;
    Mat tmpTmpl;
    for(int i = 0; i < templateNames.size(); i++) {
        tmpTmpl = imread(templateNames[i], 21);
        tmpTmpl.convertTo(tmpTmpl, 16);
        templates.push_back(tmpTmpl);
    }
    return templates;
}

/******************************
 ** For counting matched points
 ******************************/

int countMatchToFG(Rect rect, Mat maskImg) {
    int matchedCount = 0;
    for(int y = rect.y; y < rect.y + rect.height; y++) {
        for(int x = rect.x; x < rect.x + rect.width; x++) {
            if((int) maskImg.at<uchar>(y, x) == 255) matchedCount++;
        }
    }
    return matchedCount;
}


int matchCount(Rect rect, vector<Point2i> vec) {
    int matchCount = 0;
    vector<Point2i>::iterator begin, end;
    for(int y = rect.y; y < rect.y + rect.height; y++) {
        for(int x = rect.x; x < rect.x + rect.width; x++) {
            begin = vec.begin();
            end = vec.end();
            for(; begin != end; begin++){
                if(y == begin->y && x == begin->x) {
                    matchCount++;
                }
            }
        }
    }
    return matchCount;
}

/**********************************************
 ** For writing csv file of player trajectories
 **********************************************/
void exportData(string exFileName , vector<struct traj> trj, double timeStamp) {
    ofstream ofs(exFileName, ios::app);
    vector<struct traj>::iterator begin, end;
    begin = trj.begin();
    end = trj.end();
    for(; begin != end; begin++) {
        ofs << timeStamp << ",";
        ofs << begin->playerID << ",";
        ofs << begin->currPt.x << ",";
        ofs << begin->currPt.y << ",";
        ofs << begin->precision << ",";
        ofs << begin->isCorrect << ",";
        ofs << begin->occlusion << endl;
    }
    ofs.close();
    return;
}
/*******************************************************
 ** For preparation of player information data structure
 *******************************************************/
vector<struct traj> prepareTraj() {
    vector<struct traj> trj;
    
    struct traj trj1;
    struct traj trj2;
    struct traj trj3;
    struct traj trj4;
    struct traj trj5;
    struct traj trj6;
    struct traj trj7;
    struct traj trj8;
    struct traj trj9;
    struct traj trj10;
    
    struct traj trj11;
    struct traj trj12;
    struct traj trj13;
    struct traj trj14;
    struct traj trj15;
    struct traj trj16;
    struct traj trj17;
    struct traj trj18;
    struct traj trj19;
    struct traj trj20;
    
    trj.push_back(trj1);
    trj.push_back(trj2);
    trj.push_back(trj3);
    trj.push_back(trj4);
    trj.push_back(trj5);
    trj.push_back(trj6);
    trj.push_back(trj7);
    trj.push_back(trj8);
    trj.push_back(trj9);
    trj.push_back(trj10);
    trj.push_back(trj11);
    trj.push_back(trj12);
    trj.push_back(trj13);
    trj.push_back(trj14);
    trj.push_back(trj15);
    trj.push_back(trj16);
    trj.push_back(trj17);
    trj.push_back(trj18);
    trj.push_back(trj19);
    trj.push_back(trj20);
    
    vector<struct traj>::iterator begin, end;
    begin = trj.begin();
    end = trj.end();
    int pId = 1;
    for(; begin != end; begin++) {
        begin->playerID = pId;
        pId++;
    }
    return trj;
}

/**********************
 ** For output of debug
 **********************/
void debugPrintTrj(struct traj trj, double maxVal) {
    cout << "traj " << trj.playerID << "'s current x is: " << trj.currPt.x;
    cout << " y is: " << trj.currPt.y;
    cout << " and value is: " << to_string(maxVal) << endl;
}

