#include "stdafx.h"
#include "InitialBlock.h"
#include "AnmPlace.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;
extern string apt;
vector<Mat> InitialBlock::Getblockhist(string name)//�õ��ֿ��ֱ��ͼvector
{
	everyfilename=name;
	image=imread(apt+"\\"+name);
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


Mat InitialBlock::GetWhole()//�õ�����ͼ��ֱ��ͼMat
{
	vector<Mat>::iterator b_i;
	b_i=blockhistogram.begin();
	wholehist=*b_i;
	for(b_i=++blockhistogram.begin();b_i!=blockhistogram.end();b_i++)
	{
		add(*b_i,wholehist,wholehist);
	}
	//wholehist=hist.getHistogram(image);
	return wholehist;
}


vector<Data> InitialBlock::storeblocks(vector<string> list)//�洢������һ��vector<Data>
{
	vector<string>::iterator the_iterator;
	vector<Data> bigdata;
	the_iterator=list.begin();
	int Num=1;
	while(the_iterator!=list.end())
		{
			Data data;
			blockhistogram.swap(vector<Mat>());
			Getblockhist(*the_iterator);	//������һ��ͼƬ��48��ֱ��ͼ
			data.blockhistogram=blockhistogram;
			data.everyfilename=everyfilename;	//������һ��ͼƬ���ļ���
			data.mark=0;//������Ƭ�ĳ�ʼʶ���־Ϊ0������ȷ��
			data.counter=0;//ʶ�������־Ĭ��Ϊ0
			data.number=Num;
			data.wholehist=GetWhole();
			data.manual=true;
			bigdata.push_back(data);
			the_iterator++;
			Num++;
		}
	return bigdata;
}

vector<AnmPlace> InitialBlock::storeplaces(vector<string> list)//��ʼ��ÿһ��ͼƬ���ܴ��ڶ����λ��Ϊ0
{
	vector<string>::iterator i;
	vector<AnmPlace> anmp;
	i=list.begin();
	int Num=1;
	while(i!=list.end())
	{
		AnmPlace temp;
		temp.fname=(*i);
		temp.cmprslt=0;
		temp.num=Num;
		temp.block.swap(vector<int>());
		++i;
		++Num;
		anmp.push_back(temp);
	}
	return anmp;
}