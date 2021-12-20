#include "MakeTemplate.h"


void makeTemplate(Mat const img, vector<vector<Point2i>> points) {

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    medianBlur(gray, gray, 3);
    threshold(gray, gray, 127, 255, THRESH_BINARY);

    Mat canny, rotate_canny;

    Canny(gray, canny, 60, 180);
    rotate_image(canny, rotate_canny, 30);




}