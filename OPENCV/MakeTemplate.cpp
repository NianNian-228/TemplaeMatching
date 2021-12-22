#include "MakeTemplate.h"
#include <math.h>


void makeTemplate(Mat&  img, Mat& rotate_im,vector<vector<Point2i>>& points) {

    Mat gray, rotate_canny, rotate_m;
	
    for (double i = 0; i < 360; i++) {
        rotate_image(img, rotate_m, i);
        Canny(rotate_m, rotate_canny, 60, 180);
        vector<Point2i> contours;
        int cols = rotate_canny.cols;
        int rows = rotate_canny.rows;
        for (int x = 0; x < cols; x++) {
            for (int y = 0; y <rows; y++) {
                if (rotate_canny.at<uchar>(x,y) != 0&&rand()%4 == 0 ) contours.push_back(Point2i(x, y));
            }
        }
        points.push_back(contours);
        contours.clear();
    }
    cout << points.size() << endl;
    rotate_im = rotate_canny;
}


void matching(Mat& img, Mat& matching,Mat& output, vector<vector<Point2i>>& points, double score , double greedy) {
    Mat ro_image,roi_canny;
    makeTemplate(img, ro_image,points);
    cout << "template is finished" << endl;
    int x = ro_image.cols;
    int y = ro_image.rows;

    for (int i = 0; i < matching.cols; i++) {
        for (int j = 0; j < matching.rows; j++) {
            Rect rect(i, j, i+x, j+y);

            if (i + x > matching.cols || j + y > matching.rows) continue;
            Mat img_roi = matching(rect);
            Canny(img_roi, roi_canny, 60, 180);
            // 遍历每一个角度
            for (double pos = 0; pos < points.size(); pos++) {
                double round_score = 0;
                vector<Point2i> this_points = points[pos];
                // 计算一个角度对应的匹配分数
                for (int count = 0; count < this_points.size(); count++) {
                    
                    Point2i p = this_points[count];
                    if (roi_canny.at<uchar>(p.x,p.y)!=0) {
                        round_score += 1 / this_points.size();
                    }
                    //if (round_score < score)
                    //   continue;
                    if (round_score > score) {

                        cout << pos  << endl;
                        cout << "(" << i << "," << j << ")" << endl;
                        continue;
                    }
                }
                

            }
        }
    }


}