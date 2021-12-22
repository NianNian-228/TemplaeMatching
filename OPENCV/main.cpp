#include "main.h"


int main() {
    cv::Mat src = cv::imread("C:\\Users\\54681\\source\\repos\\OPENCV\\pic\\template.bmp");
    cv::Mat dst;
    Mat gray;
    cvtColor(src, src, COLOR_BGR2GRAY);
    medianBlur(src, gray, 3);
    threshold(gray, src, 127, 255, THRESH_BINARY);
  
    for (int i = 0; i < 361; i++) {
        rotate_image(src, dst, i);
        Canny(dst, dst, 60, 180);
        cv::imshow("src", src);
        cv::imshow("dst", dst);
        cv::waitKey(10);

    }

    return 0;
}