
//#include "stdafx.h"
#include <iostream>
#include <time.h>

#include "GeoMatch.h"
#include "CommandParser.h"

using namespace std;
using namespace cv;
int main(int argc, char** argv)
{


	GeoMatch GM;				// 进行几何匹配的对象
	int lowThreshold = 10;		
	int highThreashold = 100;	//默认值

	double minScore = 0.7;		
	double greediness = 0.8;		

	double total_time = 0;
	double score = 0;
	Point result;

	Mat templateImage = imread("D:\\QT workspace\\pic\\new_image\\1.bmp");
	resize(templateImage, templateImage, Size(), 0.4, 0.4);
	medianBlur(templateImage, templateImage, 5);
	//加载预匹配图片
	Mat searchImage = imread("D:\\QT workspace\\pic\\new_image\\14.bmp");
	resize(searchImage, searchImage, Size(), 0.4, 0.4);
	medianBlur(searchImage, searchImage, 5);

	Size templateSize = Size(templateImage.cols, templateImage.rows);
	Mat grayTemplateImg;
	cvtColor(templateImage, grayTemplateImg, COLOR_RGB2GRAY);
	// 转灰度图

	if (!GM.CreateGeoMatchModel(grayTemplateImg, lowThreshold, highThreashold))
	{
		cout << "ERROR: could not create model...";
		return 0;
	}
	GM.DrawContours(templateImage, Scalar(255, 0, 0), 1);
	namedWindow("Template", WINDOW_AUTOSIZE);
	imshow("Template", templateImage);
	cout << " Shape model created.." << "with  Low Threshold = " << lowThreshold << " High Threshold = " << highThreashold << endl;
	Size searchSize = Size(searchImage.cols, searchImage.rows);
	Mat graySearchImg ;
	cvtColor(searchImage, graySearchImg, COLOR_RGB2GRAY);
	cout << " Finding Shape Model.." << " Minumum Score = " << minScore << " Greediness = " << greediness << "\n\n";
	cout << " ------------------------------------\n";
	clock_t start_time1 = clock();
	score = GM.FindGeoMatchModel(graySearchImg, minScore, greediness, result);
	clock_t finish_time1 = clock();
	total_time = (double)(finish_time1 - start_time1) / CLOCKS_PER_SEC;
	cout << score << endl;
	if (score > minScore) 
	{
		cout << " Found at [" << result.x << ", " << result.y << "]\n Score = " << score << "\n Searching Time = " << total_time * 1000 << "ms";
		GM.DrawContours(searchImage, result, CV_RGB(0, 255, 0), 1);
	}
	else
		cout << " Object Not found";

	cout << "\n ------------------------------------\n\n";
	cout << "\n Press any key to exit!";

	////Display result
	
	namedWindow("Search Image", WINDOW_AUTOSIZE);
	imshow("Search Image", searchImage);
	// wait for both windows to be closed before releasing images
	waitKey(0);
	destroyWindow("Search Image");
	destroyWindow("Template");
	
	
	

	return 1;
}


void WrongUsage()
{
	cout << "\n Edge Based Template Matching Program\n";
	cout << " ------------------------------------\n";
	cout << "\nProgram arguments:\n\n";
	cout << "     -t Template image name (image to be searched)\n\n";
	cout << "     -h High Threshold (High threshold for creating template model)\n\n";
	cout << "     -l Low Threshold (Low threshold for creating template model)\n\n";
	cout << "     -s Search image name (image we are trying to find)\n\n";
	cout << "     -m Minumum score (Minimum score required to proceed with search [0.0 to 1.0])\n\n";
	cout << "     -g greediness (heuistic parameter to terminate search [0.0 to 1.0] )\n\n";

	cout << "Example: GeoMatch -t Template.jpg -h 100 -l 10 -s Search1.jpg -m 0.7 -g 0.5 \n\n";
}

