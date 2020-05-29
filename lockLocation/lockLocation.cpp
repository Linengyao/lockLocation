
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

float* Cal_cellHist(Mat src)
{
	//计算角度及梯度
	Mat gx, gy;
	Mat mag, angle;
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);
	cartToPolar(gx, gy, mag, angle, true);

	float hist[8] = { 0 };
	for (int j = 0; j < 16; j++)
	{
		for (int i = 0; i < 16; i++)
		{
			int angle_average = angle.at<float>(j, i);
			hist[angle_average/45]+= mag.at<float>(j, i);
		}
	}
	return hist;
}


int main()
{
	Mat img1 = imread("E:\\课程\\大二下\\数字图像处理\\template.png", 0);
	Mat srcimg2 = imread("E:\\课程\\大二下\\数字图像处理\\img.png", 0);
	int cellSize = 16;
	int nX = img1.cols / cellSize;
	int nY = img1.rows / cellSize;
	Mat img2 = Mat::zeros(img1.rows, img1.cols, CV_8UC1);
	int num = nX * nY * 8;

	float* srcHist = new float[num];
	memset(srcHist, 0, sizeof(float) * num);

	float* img1Hist = new float[num];
	memset(img1Hist, 0, sizeof(float) * num);

	float* img2Hist = new float[num];
	memset(img2Hist, 0, sizeof(float) * num);

	int cnt1 = 0;
	int cnt2 = 0;
	float distance = 0;
	float distance_min = 10e100;
	int x = 0;
	int y = 0;
	for (int q = 85; q < 105; q++)                //为了提高运算速度，限制了搜索范围
	{
		for (int p = 85; p < 105; p++)          //为了提高运算速度，限制了搜索范围
		{
			for (int m = 0; m < img1.rows; m++) 
			{
				for (int n = 0; n < img1.cols; n++) {
					img2.at<uchar>(m, n) = srcimg2.at<uchar>(q + m, p + n);
				}
			}
			//img1
			cnt1 = 0;
			for (int j = 0; j < nY; j++)
			{
				for (int i = 0; i < nX; i++)
				{
					Rect rectL(i * cellSize, j * cellSize, cellSize, cellSize);
					Mat nowMat = img1(rectL);
					for (int k = 0; k < 8; k++)
					{
						img1Hist[k + cnt1 * 8] = Cal_cellHist(nowMat)[k];
					}
					cnt1++;
				}
			}
			//img2
			cnt2 = 0;
			for (int j = 0; j < nY; j++)
			{
				for (int i = 0; i < nX; i++)
				{
					Rect rectL(i * cellSize, j * cellSize, cellSize, cellSize);
					Mat nowMat = img2(rectL);
					for (int k = 0; k < 8; k++)
					{
						img2Hist[k + cnt2 * 8] = Cal_cellHist(nowMat)[k];
					}
					cnt2++;
				}
			}
			distance = 0;
			for (int i = 0; i < num; i++)
			{
				distance += (img1Hist[i] - img2Hist[i]) * (img1Hist[i] - img2Hist[i]);
				if (distance < distance_min) 
				{
					distance_min = distance;
					x = q;
					y = p;
				}
			}
		}
	}
	cout << "最小值" << distance_min << endl;
	cout << "x" << x << endl;
	cout << "y" << y << endl;
	Rect rect(y, x, img1.cols, img1.rows);//左上坐标（x,y）和矩形的长(x)宽(y)
	cv::rectangle(srcimg2, rect, Scalar(255, 0, 0), 1, LINE_8, 0);
	imshow("dstMat", srcimg2);
	waitKey(0);
	delete[] img1Hist;
	delete[] img2Hist;
	return 0;

}