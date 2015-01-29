#include "stdafx.h"
#include "Histogram.h"

vector<Mat> Histogram::division(const Mat &image,int width,int height)//ֱ��ͼ�ֿ�
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
				width,j*height,temImage.cols,temImage.rows));//rect(x, y, width, height)ѡ������Ȥ����
			cv::addWeighted(temImage,1.0,imageROI,1.0,0.,temImage);//����ɨ����ı߽�������
			imgOut.push_back(temImage);//���temImage����ǰvectorĩβ
		}
	}
	return imgOut;
}


Mat Histogram::getHistogram(const Mat& image)//�����ɫͼ��ֱ��ͼ
{
	Mat hist;
	//BGRֱ��ͼ
	hranges[0]= 0.0;    
	hranges[1]= 256.0;
	channels[0]= 0;	
	channels[1]= 1; 
	channels[2]= 2; 
	//����
	calcHist(&image,1,channels,Mat(),hist,3,histSize,ranges);
	//normalize(hist,hist,1.0);//��һ��
	return hist;
}


double Histogram::compareH(Mat a,Mat b)//�Ƚ�����ͼƬ��ֱ��ͼ
{
	return compareHist(a,b,CV_COMP_INTERSECT);
}