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
	//���캯��
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
	vector<Mat>  division(const Mat &image,int width,int height);//��ͼ����зֿ�
	Mat getHistogram(const Mat& image);//�����ɫͼ��ֱ��ͼ
	double compareH(Mat a,Mat b);//�Ƚ�����ͼƬ��ֱ��ͼ
};
#endif