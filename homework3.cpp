#include <cstdio>
#include <cstring>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include <cmath>
#include <fstream>
#include <time.h>
using namespace std;
using namespace cv;


int main(int argc, char* argv[])
{
	//*****************read images****************************
	clock_t start, end;
	string path = "C:/Users/nicky/source/repos/StereoCalibration/StereoCalibration/resources/";
	Mat leftImage = imread(path + "left.jpg",0);
	Mat rightImage = imread(path + "right.jpg",0);

	imshow("left", leftImage);
	imshow("right", rightImage);
	const int imageHeight = leftImage.rows;  //288
	const int imageWidth = leftImage.cols;  //384

	Mat disparityMap;
	Mat leftDepth = Mat::zeros(imageHeight, imageWidth, CV_8U);
	Mat leftValid = Mat::zeros(imageHeight, imageWidth, CV_8U);
	Mat leftFilter = Mat::zeros(imageHeight, imageWidth, CV_8U);

	Mat rightDepth = Mat::zeros(imageHeight, imageWidth, CV_8U);
	Mat rightValid = Mat::zeros(imageHeight, imageWidth, CV_8U);
	Mat rightFilter = Mat::zeros(imageHeight, imageWidth, CV_8U);
	
	//****************create kernel***************************
	Size SADWindow = Size(5, 5); //choose kernel with size 5*5

	int sadH = SADWindow.height / 2, sadW = SADWindow.width / 2; //center of the kernel

	int minD = 0, maxD =15;
	
	int maxDiff = 200;
	int minDiff = 0;
	int pixelDiff = 0;
	unsigned char* pLeftPixel = NULL;
	unsigned char* pRightPixel = NULL;
	unsigned char leftPixel = 0;
	unsigned char rightPixel = 0;
	unsigned char* pPixel = NULL;
	int m, n;
	unsigned char pixel;
	start = clock();
	vector<vector<int>> dp(512, vector<int>(512));
	for (int i = 0; i < imageWidth; i++)
	{
		dp[0][i] = 0; 
		dp[i][0] = 0;
	}
	//******************start scanning*************************
	for (int i = sadH; i < imageHeight - sadH; i++)
	{
		//int rowLeft = max(0, i - sadH), rowRight = min(imageHeight - 1, i + sadH);
		for (int j = sadW; j < imageWidth - sadW; j++)
		{
			//int colUp = max(0, j - sadW), colDown = min(imageWidth - 1, j + sadW);
			//int minD = max(-disparityRange, -colUp), maxD = min(disparityRange, imageWidth - 1 - colDown);
			for (int k = j + minD; k <= j + maxD; k++) //calculate SAD
			{
				pixelDiff = 0;
				for (m = -sadH; m <= sadH; m++)
				{
					for (n = -sadW; n <= sadW; n++)
					{
						pRightPixel = (unsigned char*)rightImage.data + (i + m) * imageWidth + (j + n);
						rightPixel = *pRightPixel;
						if (k < sadW || k >= imageWidth - sadW)
						{
							pixelDiff += rightPixel;
						}
						else
						{
							pLeftPixel = (unsigned char*)leftImage.data +  (i + m) * imageWidth + (n + k);
							leftPixel = *pLeftPixel;

							pixelDiff += abs(leftPixel - rightPixel);
						}
						
						//cout << pixelDiff << endl;
					}
				}
				//if (k >= imageWidth - 1) continue;
				if (pixelDiff <= maxDiff)
				{
					dp[j + 1][k + 1] = dp[j][k] + 1;   //if the difference is small, update it
				}
				else if (dp[j][k + 1] > dp[j + 1][k]) dp[j + 1][ k + 1] = dp[j][k + 1];   //else update it with the previous larger one 
				else dp[j + 1][k + 1] = dp[j + 1][k];
			}
		}

		//***********************Sweep back, find the path**********************************
		//find the col coordinate in left and right image, larger dp val, smaller diff in pixels
		m = imageWidth-sadW;
		n = imageWidth - sadW;
		int m2 = m, n2 = n;

		while (m > 0 && n > 0)   //to find the max from dp[m-1][n-1], dp[m-1][n], dp[m][n-1]
		{
			if((m == m2-1 && n <= n2-1) || (m <= m2-1 && n == n2-1))
			{

				pPixel = (unsigned char*)rightDepth.data +  i * imageWidth + m;
				*pPixel = (n - m) * 10;
				pPixel = (unsigned char*)rightValid.data +  i * imageWidth + m;
				*pPixel = 255;
				
				m2 = m;
				n2 = n;
			}

			if (dp[m - 1][n - 1] >= dp[m][n - 1] && dp[m - 1][n - 1] >= dp[m - 1][n])
			{
				m--;
				n--;
			}
			else if (dp[m - 1][n] >= dp[m][n - 1] && dp[m - 1][n] >= dp[m - 1][n - 1])
			{
				m--;
			}
			else n--;
		}


	}
	end = clock();
	cout << "Running Time" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	imshow("Right Disparity", rightDepth);
	imwrite("C:/Users/nicky/source/repos/StereoCalibration/StereoCalibration/resources/disparity1.jpg", rightDepth);


	
	for (int j = 0; j <= imageWidth; j++)
	{
		for (int k = 0; k <= imageWidth; k++)
		{
			dp[j][k] = 0;
		}
	}
	minD = -maxD;
	maxD = 0;

	for (int i = sadH; i < imageHeight - sadH; i++)
	{

		//int rowLeft = max(0, i - sadH), rowRight = min(imageHeight - 1, i + sadH);
		for (int j = imageWidth - sadW -1 ; j >= sadW; j--)
		{
			//int colUp = max(0, j - sadW), colDown = min(imageWidth - 1, j + sadW);
			//int minD = max(-disparityRange, -colUp), maxD = min(disparityRange, imageWidth - 1 - colDown);
			for (int k = j + maxD; k >= j + minD; k--) //calculate SAD
			{
				pixelDiff = 0;
				for (m = -sadH; m <= sadH; m++)
				{
					for (n = -sadW; n <= sadW; n++)
					{
						pLeftPixel = (unsigned char*)leftImage.data + (i + m) * imageWidth + (j + n);
						leftPixel = *pLeftPixel;
						if (k < sadW || k >= imageWidth - sadW)
						{
							pixelDiff += leftPixel;
						}
						else
						{
							pRightPixel = (unsigned char*)rightImage.data + (i + m) * imageWidth + (n + k);
							rightPixel = *pRightPixel;

							pixelDiff += abs(leftPixel - rightPixel);
						}

						//cout << pixelDiff << endl;
					}
				}
				if (k < 0 || k > imageWidth-1) continue;
				if (pixelDiff <= maxDiff)
				{
					dp[j][k] = dp[j+1][k+1] + 1;   //if the difference is small, update it
				}
				else if (dp[j+1][k] > dp[j][k+1]) dp[j][k] = dp[j+1][k];   //else update it with the previous larger one 
				else dp[j][k] = dp[j][k+1];
			}
		}

		//***********************Sweep back, find the path**********************************
		//find the col coordinate in left and right image, larger dp val, smaller diff in pixels
		m = 0;
		n = 0;
		int m2 = m, n2 = n;

		while (m < imageWidth && n < imageWidth)
		{
			if ((m == m2 + 1 && n >= n2 + 1) || (m > m2 + 1 && n == n2 + 1))
			{
				pPixel = (unsigned char*)leftDepth.data + i * imageWidth + m;
				*pPixel = (m - n) * 10;
				pPixel = (unsigned char*)leftValid.data + i * imageWidth + m;
				*pPixel = 255;

				m2 = m;
				n2 = n;
			}
			if (dp[m + 1][n + 1] >= dp[m][n + 1] && dp[m + 1][n + 1] >= dp[m + 1][n])
			{

				m++;
				n++;
			}
			else if (dp[m + 1][n] >= dp[m][n + 1] && dp[m + 1][n] >= dp[m + 1][n + 1])
			{
				m++;
			}
			else
			{
				n++;
			}

		}
	}




	imshow("Left Disparity", leftDepth);
	imwrite("C:/Users/nicky/source/repos/StereoCalibration/StereoCalibration/resources/disparity2.jpg", leftDepth);
	
	
	waitKey(100000);
	return 0;
}

