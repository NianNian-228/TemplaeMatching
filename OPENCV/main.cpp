#include <iostream>
#include <opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "RotateImage.h"

using namespace std;
using namespace cv;

int templateHeight;
int templatWidth;
int noOfCordinates;

vector<vector<Point>> cordinates360(360);
vector<vector<double>> edgeMagnitude360(360);
vector<vector<double>> edgeDerivativeX360(360);
vector<vector<double>> edgeDerivativeY360(360);
vector<Point> centroid360(360);

// new一个doubel类型矩阵  
void CreateDoubleMatrix(double**& matrix, Size size)
{
	matrix = new double* [size.height];
	for (int iInd = 0; iInd < size.height; iInd++)
		matrix[iInd] = new double[size.width];
}
// release一个doubel类型矩阵
void ReleaseDoubleMatrix(double**& matrix, int size)
{
	for (int iInd = 0; iInd < size; iInd++)
		delete[] matrix[iInd];
}


int CreateTemplate(Mat& templateImg, double maxContrast, double minContrast) {
	for (int angle = 0; angle < 360; angle++) {

		Mat gx;
		Mat gy;

		Size Ssize;
		cv::Mat src = templateImg.clone();
		rotate_image(src, src, angle);
		Ssize.height = src.rows;
		Ssize.width = src.cols;
		templateHeight = src.rows;		//保存模板高度
		templatWidth = src.cols;			//保存模板宽度
		Mat nmsEdges(templateHeight, templatWidth, CV_64FC1);

		double** magMat;
		CreateDoubleMatrix(magMat, Ssize);
		noOfCordinates = 0;
		vector<Point>cordinates(templateHeight * templatWidth);                //选中的边缘
		vector<double>edgeMagnitude(templateHeight * templatWidth);    //模板图像中选定点的梯度幅值
		vector<double>edgeDerivativeX(templateHeight * templatWidth); 	//模板图像中选定点的x方向导数
		vector<double>edgeDerivativeY(templateHeight * templatWidth); 	//模板图像中选定点的y方向导数
		vector<int>orients(templateHeight * templatWidth); 	//模板图像中梯度方向

		Sobel(src, gx, CV_32F, 1, 0);
		Sobel(src, gy, CV_32F, 0, 1);
		double direction;
		double MaxGradient = -99999.99;
		double fdx, fdy;
		double MagG, DirG;

		int i, j;
		int count = 0;
		for (i = 1; i < Ssize.height - 1; i++)
		{
			for (j = 1; j < Ssize.width - 1; j++)
			{

				fdx = gx.ptr<float>(i)[j], fdy = gy.ptr<float>(i)[j];
				// x，y方向梯度
				//梯度幅值
				MagG = sqrt((float)(fdx * fdx) + (float)(fdy * fdy));
				//梯度方向
				direction = atan2((float)fdy, (float)fdx) * 180 / 3.1415;
				magMat[i][j] = MagG;

				if (MagG > MaxGradient)
					MaxGradient = MagG;
				// 得到最大梯度值为了后续归一化操作


			// 将方向分成4类
				if ((direction > 0 && direction < 22.5) ||
					(direction > 157.5 && direction < 202.5) ||
					(direction > 337.5 && direction < 360))
					direction = 0;
				else if ((direction > 22.5 && direction < 67.5) ||
					(direction > 202.5 && direction < 247.5))
					direction = 45;
				else if ((direction > 67.5 && direction < 112.5) ||
					(direction > 247.5 && direction < 292.5))
					direction = 90;
				else if ((direction > 112.5 && direction < 157.5) ||
					(direction > 292.5 && direction < 337.5))
					direction = 135;
				else
					direction = 0;

				orients[count] = (int)direction;
				count++;
			}
		}
		count = 0;
		double leftPixel, rightPixel;

		//非极大值抑制
		for (i = 1; i < Ssize.height - 1; i++)
		{
			for (j = 1; j < Ssize.width - 1; j++)
			{
				switch (orients[count])
				{
				case 0:
					leftPixel = magMat[i][j - 1];
					rightPixel = magMat[i][j + 1];
					break;
				case 45:
					leftPixel = magMat[i - 1][j + 1];
					rightPixel = magMat[i + 1][j - 1];
					break;
				case 90:
					leftPixel = magMat[i - 1][j];
					rightPixel = magMat[i + 1][j];
					break;
				case 135:
					leftPixel = magMat[i - 1][j - 1];
					rightPixel = magMat[i + 1][j + 1];
					break;
				}
				// 比较当前像素值与相邻像素值
				if ((magMat[i][j] < leftPixel) || (magMat[i][j] < rightPixel))
					(nmsEdges.ptr<float>(i))[j] = 0;
				else
					(nmsEdges.ptr<float>(i))[j] = (float)(magMat[i][j] / MaxGradient * 255);

				count++;
			}
		}
		int RSum = 0, CSum = 0;
		int curX, curY;
		int flag = 1;

		//滞后阈值
		for (i = 1; i < Ssize.height - 1; i++)
		{
			for (j = 1; j < Ssize.width; j++)
			{
				/*_sdx = (short*)(gx.ptr<float>(i));
				_sdy = (short*)(gy.ptr<float>(i));*/
				fdx = gx.ptr<float>(i)[j]; fdy = gy.ptr<float>(i)[j];

				MagG = sqrt(fdx * fdx + fdy * fdy); //梯度幅值
				DirG = atan2((float)fdy, (float)fdx) * 180 / 3.1415;	 //梯度方向

				//((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]= MagG;
				flag = 1;
				if (((double)((nmsEdges.ptr<float>(i)))[j]) < maxContrast)
				{
					if (((double)((nmsEdges.ptr<float>(i)))[j]) < minContrast)
					{

						(nmsEdges.ptr<float>(i))[j] = 0;
						flag = 0;
						// 移除
						////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]=0;
					}
					else
					{   // 如果 8 个相邻像素中的任何一个不大于预设最大值从边缘移除
						if ((double)nmsEdges.ptr<float>(i - 1)[j - 1] < maxContrast &&
							(double)nmsEdges.ptr<float>(i - 1)[j] < maxContrast &&
							(double)nmsEdges.ptr<float>(i - 1)[j + 1] < maxContrast &&
							(double)nmsEdges.ptr<float>(i)[j - 1] < maxContrast &&
							(double)nmsEdges.ptr<float>(i)[j + 1] < maxContrast &&
							(double)nmsEdges.ptr<float>(i + 1)[j - 1] < maxContrast &&
							(double)nmsEdges.ptr<float>(i + 1)[j] < maxContrast &&
							(double)nmsEdges.ptr<float>(i + 1)[j + 1] < maxContrast)
						{
							(nmsEdges.ptr<float>(i))[j] = 0;
							flag = 0;
							////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]=0;
						}
					}

				}
				// 保存选出的特征边缘点
				curX = i;	curY = j;
				if (flag != 0)
				{
					if (fdx != 0 || fdy != 0)
					{
						RSum = RSum + curX;	CSum = CSum + curY; // 质心的行总和和列总和

						cordinates[noOfCordinates].x = curX;
						cordinates[noOfCordinates].y = curY;
						edgeDerivativeX[noOfCordinates] = fdx;
						edgeDerivativeY[noOfCordinates] = fdy;

						//除数不为0
						if (MagG != 0)
							edgeMagnitude[noOfCordinates] = 1 / MagG;  // 梯度幅值
						else
							edgeMagnitude[noOfCordinates] = 0;

						noOfCordinates++;
					}
				}
			}
		}
		centroid360[angle].x = RSum / noOfCordinates;
		centroid360[angle].y = CSum / noOfCordinates;	// 质心坐标
		//以质心为原点改变坐标
		for (int m = 0; m < noOfCordinates; m++)
		{
			int temp;

			temp = cordinates[m].x;
			cordinates[m].x = temp - centroid360[angle].x;
			temp = cordinates[m].y;
			cordinates[m].y = temp - centroid360[angle].y;
		}
		////cvSaveImage("Edges.bmp",imgGDir);

		// free alocated memories
		//delete[] orients;
		////cvReleaseImage(&imgGDir);

		cordinates360[angle] = cordinates;
		edgeMagnitude360[angle] = edgeMagnitude;
		edgeDerivativeX360[angle] = edgeDerivativeX;
		edgeDerivativeY360[angle] = edgeDerivativeY;

		ReleaseDoubleMatrix(magMat, Ssize.height);



	}
	return 1;
}


void MatchTemplate(cv::Mat& srcarr, double minScore, double greediness, vector<Point>& resultPoint)
{
	Mat Sdx, Sdy;

	vector<Point>cordinates(templateHeight * templatWidth);                //当前角度的边缘
	vector<double>edgeMagnitude(templateHeight * templatWidth);    //当前角度的梯度幅值
	vector<double>edgeDerivativeX(templateHeight * templatWidth); 	//当前角度的x方向导数
	vector<double>edgeDerivativeY(templateHeight * templatWidth); 	//当前角度的y方向导数

	//vector<Point>resultPoint(templateHeight * templatWidth);
	//vector<double>resultScore(templateHeight * templatWidth);

	double partialSum = 0;
	double sumOfCoords = 0;
	double partialScore;
	const short* _Sdx;
	const short* _Sdy;
	int i, j, m;
	int noOfMatchTemplate;
	double iTx, iTy, iSx, iSy;
	double gradMag;
	int curX, curY;

	double** matGradMag;  //梯度幅值矩阵

	cv::Mat src = srcarr.clone();


	// 图片尺寸
	Size Ssize;
	Ssize.width = src.cols;
	Ssize.height = src.rows;

	CreateDoubleMatrix(matGradMag, Ssize); // 保存梯度幅值



	Sobel(src, Sdx, CV_32F, 1, 0);  // x方向梯度
	Sobel(src, Sdy, CV_32F, 0, 1); // y方向梯度

	// 搜索的停止条件
	double normMinScore = minScore / noOfCordinates; // 预先计算的最低分数
	double normGreediness = ((1 - greediness * minScore) / (1 - greediness)) / noOfCordinates; // 预先计算的贪心值



		for (i = 0; i < Ssize.height; i++)
		{
			//_Sdx = (short*)(Sdx.ptr<float>(i));
			//_Sdy = (short*)(Sdy.ptr<float>(i));

			for (j = 0; j < Ssize.width; j++)
			{


				iSx = Sdx.ptr<float>(i)[j];
				iSy = Sdy.ptr<float>(i)[j];

				gradMag = sqrt((iSx * iSx) + (iSy * iSy));

				if (gradMag != 0)
					matGradMag[i][j] = 1 / gradMag;   //同上
				else
					matGradMag[i][j] = 0;

			}
		}

		for (int angle = 0; angle < 360; angle += 4) {    //匹配全角度的模板

			cordinates = cordinates360[angle];
			edgeMagnitude = edgeMagnitude360[angle];
			edgeDerivativeX = edgeDerivativeX360[angle];
			edgeDerivativeY = edgeDerivativeY360[angle];

		for (i = 0; i < Ssize.height; i++)
		{
			for (j = 0; j < Ssize.width; j++)
			{

				

				partialSum = 0;
				for (m = 0; m < noOfCordinates; m++)
				{
					curX = i + cordinates[m].x;	// 模板坐标
					curY = j + cordinates[m].y;
					iTx = edgeDerivativeX[m];
					iTy = edgeDerivativeY[m];    // 模板梯度

					if (curX<0 || curY<0 || curX>Ssize.height - 1 || curY>Ssize.width - 1)
						continue;



					iSx = Sdx.ptr<float>(curX)[curY]; // 从源图像获取相应的 X 导数
					iSy = Sdy.ptr<float>(curX)[curY];// 从源图像获取相应的 Y 导数

					if ((iSx != 0 || iSy != 0) && (iTx != 0 || iTy != 0))
					{
						//partial Sum  = Sum of(((Source X derivative* Template X drivative) + Source Y derivative * Template Y derivative)) / Edge magnitude of(Template)* edge magnitude of(Source))
						partialSum = partialSum + ((iSx * iTx) + (iSy * iTy)) * (edgeMagnitude[m] * matGradMag[curX][curY]);

					}

					sumOfCoords = m + 1;
					partialScore = partialSum / sumOfCoords;

					// 如果部分分数低于在该位置取得所需分数所需的分数便停下

					//if (partialScore < (MIN((minScore - 1) + normGreediness * sumOfCoords, normMinScore * sumOfCoords)))
					//	break;

				}
				if (partialScore > minScore)
				{
					
					std::cout << "(" << i*16<< "," <<j*16 <<"," << angle<<"," << partialScore << ")" << endl;
					//Point p;
					//p.x = i;			// 结果坐标	
					//p.y = j;
					//resultPoint[noOfMatchTemplate] = p;
					//++noOfMatchTemplate;
				}
			}
		} 

		

	} //angle 循环
	cout << " finish" << endl;
	// free used resources and return score
	//ReleaseDoubleMatrix(matGradMag, Ssize.width);



}

int main() {
	cv::utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);

	cv::Mat src = cv::imread("C:\\Users\\54681\\source\\repos\\OPENCV\\pic\\template.bmp");
	cv::Mat image = cv::imread("C:\\Users\\54681\\source\\repos\\OPENCV\\pic\\matching.bmp");
	cv::Mat dst;
	Mat gray;
	vector<Point>results(templateHeight * templatWidth);
	cvtColor(src, src, COLOR_BGR2GRAY);
	cvtColor(image, image, COLOR_BGR2GRAY);
	
	resize(src, src, Size(src.cols * 0.0625, src.rows * 0.0625));
	resize(image, image, Size(image.cols * 0.0625, image.rows * 0.0625));

	CreateTemplate(src, 30, 120);

	std::cout << "CreateFinished" << endl;
	MatchTemplate(image, 0.94,0.7,results);

	return 0;
}