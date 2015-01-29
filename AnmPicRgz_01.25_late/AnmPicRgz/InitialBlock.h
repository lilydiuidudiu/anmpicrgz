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
	string everyfilename;		//����everyfilename��һ��ͼƬ���ļ�����·��
	vector<Mat> blockhistogram;		//����blockhistogram��һ��ͼƬ��48���ֿ��ֱ��ͼ
	Mat wholehist;
	int mark;
	
public:
	vector<Mat> Getblockhist(string name);//�õ��ֿ��ֱ��ͼvector
	Mat GetWhole();//�õ�����ͼ��ֱ��ͼMat
	vector<Data> storeblocks(vector<string> list);//�洢������һ��vector<Data>
	vector<AnmPlace> storeplaces(vector<string> list);//��ʼ��ÿһ��ͼƬ���ܴ��ڶ����λ��Ϊ0
};
