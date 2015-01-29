#include "stdafx.h"
#include "Histogram.h"

vector<Mat> Histogram::division(const Mat &image,int width,int height)//直方图分块
{
	int m,n;
	m = image.rows/height;
	n = image.cols/width;
	vector<cv::Mat> imgOut;
	for (int j = 0;j<m;j++)
	{
		for(int i = 0;i<n;i++)
		{
			cv::Mat temImage(height,width,CV_8UC3,cv::Scalar(0,0,0));//mat(rows,cols,) 
			cv::Mat imageROI = image(cv::Rect(i*
				width,j*height,temImage.cols,temImage.rows));//rect(x, y, width, height)选定感兴趣区域
			cv::addWeighted(temImage,1.0,imageROI,1.0,0.,temImage);//复制扫描出的边界内数据
			imgOut.push_back(temImage);//添加temImage到当前vector末尾
		}
	}
	return imgOut;
}


Mat Histogram::getHistogram(const Mat& image)//计算彩色图像直方图
{
	Mat hist;
	//BGR直方图
	hranges[0]= 0.0;    
	hranges[1]= 256.0;
	channels[0]= 0;	
	channels[1]= 1; 
	channels[2]= 2; 
	//计算
	calcHist(&image,1,channels,Mat(),hist,3,histSize,ranges);
	//normalize(hist,hist,1.0);//归一化
	return hist;
}


double Histogram::compareH(Mat a,Mat b)//比较两张图片的直方图
{
	return compareHist(a,b,CV_COMP_INTERSECT);
}