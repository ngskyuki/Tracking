//
//  KmeansClustring.cpp
//  PlayerTracking
//
//  Created by yuukifujita on 12/31/14.
//  Copyright (c) 2014 yuukifujita. All rights reserved.
//

#include "KmeansClustring.h"

double dist(struct cluster cluster, int x, int y) {
    double dist;
    dist = sqrt(abs((cluster.cur_x - (double)x) * (cluster.cur_x - (double)x)) + abs((cluster.cur_y - (double)y) * (cluster.cur_y - (double)y)));
    return dist;
}

int indexOf(double inArray[], int length, double seek) {
    for(int i = 0; i < length; i++) {
        if(abs((int)inArray[i] - (int)seek) < 2) return i;
    }
    return -1;
}

vector<Point2i> clustering(int k, vector<Point2i> points, int loopNum, vector<Point2f> initVec) {
    int x, y, i;
    int loop;
    struct cluster cluster[k];
    
    //Initialize struct cluster's new point
    for(i = 0; i < k; i++) {
        cluster[i].next_x = cluster[i].next_y = cluster[i].no = 0;
    }
    
    //Initialize struct cluster's current point with semi-random point
    for(i = 0; i < k; i++) {
        cluster[i].cur_x = (int)initVec[i].x;
        cluster[i].cur_y = (int)initVec[i].y;
    }
    
    //Prepare for iteration
    vector<Point2i>::iterator begin, end;
    
    //Iterate for input loop cout
    for(loop = 0; loop < loopNum; loop++) {
        begin = points.begin();
        end = points.end();
        for(; begin != end; begin++) {
            //Save the cluster number of minimum distance
            int clusterNo;
            
            //Take the current input vector's point-x and point-y
            x = begin->x;
            y = begin->y;
            
            //Prepare the minimum-distance variable and array to save distance
            double minDist;
            double distRepos[k];
            
            //Calculate the distance of cluster-C and input point-x and point-y
            for(int c = 0; c < k; c++) {
                distRepos[c] = dist(cluster[c], x, y);
            }
            
            //Pick up the minimum distance
            minDist = *min_element(distRepos, distRepos + k);
            //And pick up the index(cluster number) of minumum distance
            clusterNo = indexOf(distRepos, k, minDist);
            cluster[clusterNo].no++;
            cluster[clusterNo].next_x += x;
            cluster[clusterNo].next_y += y;
        }
        for(i = 0; i < k; i++) {
            //calculate average of i's cluster point
            if(cluster[i].no == 0) {
            } else {
                cluster[i].cur_x = (long)(cluster[i].next_x / cluster[i].no);
                cluster[i].cur_y = (long)(cluster[i].next_y / cluster[i].no);
            }
            //Initialize struct cluster
            cluster[i].next_x = cluster[i].next_y = cluster[i].no = 0;
        }
    }
    vector<Point2i> result;
    for(int i = 0; i < k; i++) {
        cout << "cluter " << i << "'s current x is: " << (int)cluster[i].cur_x;
        cout << " and y is: " << (int)cluster[i].cur_y << endl;
        result.push_back(Point((int)cluster[i].cur_x, (int)cluster[i].cur_y));
    }
    return result;
}