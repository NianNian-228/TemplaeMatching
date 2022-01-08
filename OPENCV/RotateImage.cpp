#include <iostream>
#include<opencv2/opencv.hpp>

using namespace cv;

void rotate_image(Mat &src, Mat& dst, float angle)
{
    float radian = (float)(angle / 180.0 * CV_PI);

    //填充图像
    int maxBorder = (int)(max(src.cols, src.rows) * 1.414); //即为sqrt(2)*max
    int dx = (maxBorder - src.cols) / 2;
    int dy = (maxBorder - src.rows) / 2;
    copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_REPLICATE);

    //旋转
    Point2f center((float)(dst.cols / 2), (float)(dst.rows / 2));
    Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);//求得旋转矩阵
    warpAffine(dst, dst, affine_matrix, dst.size(), INTER_AREA, BORDER_REPLICATE);

    ////计算图像旋转之后包含图像的最大的矩形
    //float sinVal = abs(sin(radian));
    //float cosVal = abs(cos(radian));
    //Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
    //    (int)(src.cols * sinVal + src.rows * cosVal));

    ////剪掉多余边框
    //int x = (dst.cols - targetSize.width) / 2;
    //int y = (dst.rows - targetSize.height) / 2;
    //Rect rect(x, y, targetSize.width, targetSize.height);
    //dst = Mat(dst, rect);
}

