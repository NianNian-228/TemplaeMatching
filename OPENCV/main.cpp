#include "main.h"

void main()
{
    cv::utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);
    Mat src = imread("C:\\Users\\54681\\source\\repos\\OPENCV\\pic\\template.bmp");
    vector<vector<Point2i>> points;

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    medianBlur(gray, gray, 3);
    threshold(gray, gray, 127, 255, THRESH_BINARY);

    Mat canny,rotate_canny;
    
    Canny(gray, canny, 60, 180);
    rotate_image(canny, rotate_canny, 30);

    imshow("123", rotate_canny);
    waitKey(0);
}