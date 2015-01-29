#if!defined HISTOGRAM
#define HISTOGRAM
#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;


class Histogram
{
private:
	int histSize[3];
	float hranges[2];
	const float* ranges[3];
	int channels[3];
public:
	//构造函数
	Histogram()
	{
		histSize[0]= histSize[1]= histSize[2]= 16;
		hranges[0] = 0.0;
		hranges[1] = 256.0;
		ranges[0] = hranges;
		ranges[1] = hranges;
		ranges[2] = hranges;
		channels[0] = 0;
		channels[1] = 1;
		channels[2] = 2;
	}
	vector<Mat>  division(const Mat &image,int width,int height);//将图像进行分块
	Mat getHistogram(const Mat& image);//计算彩色图像直方图
	double compareH(Mat a,Mat b);//比较两张图片的直方图
};
#endif