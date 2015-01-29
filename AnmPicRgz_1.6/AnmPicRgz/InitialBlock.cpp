#include "stdafx.h"
#include "InitialBlock.h"
#include "AnmPlace.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

vector<Mat> InitialBlock::Getblockhist(string name)//得到分块的直方图vector
{
	everyfilename=name;
	image=imread(name);
	vector<Mat> imgout;
	imgout=hist.division(image,500,500);
	vector<Mat> ::iterator i;		
	i=imgout.begin();		
	while(i!=imgout.end())
	{
		RGBHist=hist.getHistogram(*i);
		blockhistogram.push_back(RGBHist);			
		i++;
	}
	return blockhistogram;
}


Mat InitialBlock::GetWhole()//得到整幅图的直方图Mat
{
	wholehist=hist.getHistogram(image);
	return wholehist;
}


vector<Data> InitialBlock::storeblocks(vector<string> list)//存储数据至一个vector<Data>
{
	vector<string>::iterator the_iterator;
	vector<Data> bigdata;
	the_iterator=list.begin();
	while(the_iterator!=list.end())
		{
			Data data;
			blockhistogram.swap(vector<Mat>());
			Getblockhist(*the_iterator);	//这里是一张图片的48块直方图
			data.blockhistogram=blockhistogram;
			data.everyfilename=everyfilename;	//这里是一张图片的文件名
			data.mark=0;//所有照片的初始识别标志为0，即不确定
			data.counter=0;//识别次数标志默认为0
			data.wholehist=GetWhole();
			bigdata.push_back(data);
			the_iterator++;
		}
	return bigdata;
}

vector<AnmPlace> InitialBlock::storeplaces(vector<string> list)//初始化每一张图片可能存在动物的位置为0
{
	vector<string>::iterator i;
	vector<AnmPlace> anmp;
	i=list.begin();
	while(i!=list.end())
	{
		AnmPlace temp;
		temp.fname=(*i);
		temp.cmprslt=0;
		temp.block.swap(vector<int>());
		++i;
		anmp.push_back(temp);
	}
	return anmp;
}