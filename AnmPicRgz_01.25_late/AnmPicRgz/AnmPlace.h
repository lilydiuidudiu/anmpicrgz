#pragma once
#include <string>
#include <vector>
using namespace std;
class AnmPlace
{
public:
	string fname;		//照片文件名
	double cmprslt;		//和该照片相似度最高的背景照片的整幅图相似度
	vector<int> block;	//可能出现动物的分块
	int num;			//照片ID
};