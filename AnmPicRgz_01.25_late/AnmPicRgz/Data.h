#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;
class Data
{
public:
	string everyfilename;		//����everyfilename��һ��ͼƬ���ļ�����·��
	vector<Mat> blockhistogram;		//����blockhistogram��һ��ͼƬ��48���ֿ��ֱ��ͼ
	//string sta;
	Mat wholehist;	//����ͼ��ֱ��ͼ
	int mark;		//ʶ������0�ǲ�ȷ����1���ж��-1���޶��
	int counter;	//���Ƚϴ���
	int number;		//��ƬID
	bool manual;		//��Ƭ���˹�ʶ��ı�ǣ�true���˹��ģ�false�Ǽ������
};