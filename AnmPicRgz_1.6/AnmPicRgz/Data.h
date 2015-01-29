#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;
class Data
{
public:
	string everyfilename;		//这里everyfilename是一张图片的文件名和路径
	vector<Mat> blockhistogram;		//这里blockhistogram是一张图片中48个分块的直方图
	//string sta;
	Mat wholehist;
	int mark;
	int counter;
};