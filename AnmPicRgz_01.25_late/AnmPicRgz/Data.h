#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;
class Data
{
public:
	string everyfilename;		//这里everyfilename是一张图片的文件名无路径
	vector<Mat> blockhistogram;		//这里blockhistogram是一张图片中48个分块的直方图
	//string sta;
	Mat wholehist;	//整幅图的直方图
	int mark;		//识别结果（0是不确定，1是有动物，-1是无动物）
	int counter;	//待比较次数
	int number;		//照片ID
	bool manual;		//照片是人工识别的标记，true是人工的，false是计算机的
};