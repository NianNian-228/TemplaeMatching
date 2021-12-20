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

void makeTemplate(Mat const img, vector<vector<Point2i>> points);

#endif // !_MAKE_TEMPLATE_

