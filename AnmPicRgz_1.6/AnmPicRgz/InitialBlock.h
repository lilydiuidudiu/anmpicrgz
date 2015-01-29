#include "Histogram.h"
#include "Data.h"
#include "AnmPlace.h"
using namespace std;
using namespace cv;

class InitialBlock
{
private:
	Mat RGBHist;
	Mat image;
	Histogram hist;
	string everyfilename;		//这里everyfilename是一张图片的文件名和路径
	vector<Mat> blockhistogram;		//这里blockhistogram是一张图片中48个分块的直方图
	Mat wholehist;
	int mark;
	
public:
	vector<Mat> Getblockhist(string name);//得到分块的直方图vector
	Mat GetWhole();//得到整幅图的直方图Mat
	vector<Data> storeblocks(vector<string> list);//存储数据至一个vector<Data>
	vector<AnmPlace> storeplaces(vector<string> list);//初始化每一张图片可能存在动物的位置为0
};
