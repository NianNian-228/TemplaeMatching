#ifndef _MAKE_TEMPLATE_
#define _MAKE_TEMPLATE_

#include <iostream>
#include<opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>

#include "RotateImage.h"

using namespace cv;
using namespace std;

void makeTemplate(Mat& img, Mat& rotate_img,vector<vector<Point2i>> &points);
void matching(Mat& img, Mat& matching, Mat& output, vector<vector<Point2i>>& points, double score, double greedy );

#endif // !_MAKE_TEMPLATE_

