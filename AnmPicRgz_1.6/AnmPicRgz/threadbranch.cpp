#include "stdafx.h"
#include <algorithm>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <string.h>
#include <fstream>
#include "Data.h"
#include "Histogram.h"		
#include "mostmaybe.h"
#include "AnmPicRgzDlg.h"
#include "InitialBlock.h"
#include "AnmPlace.h"

using namespace std;
using namespace cv;

string tempbgname="0";
string tempname="0";
extern vector<AnmPlace> place_vct; //��ſ��ܴ��ڶ������Ϣ������


bool find_last(Data& b)//find_qq(vector<string>::value_type )
{
	if (b.everyfilename.find(tempbgname) != string::npos )
		return true;
	else
		return false;
}


bool find_place(AnmPlace& d)
{
	if (d.fname.find(tempname)!=string::npos)
		return true;
	else
		return false;
}


vector<int> bubble(const vector<MostMaybe>& c)  //ð�����򷨣����һ��vector<int> ���������������
{
	vector<MostMaybe> a(c);
	int n = a.size();
	for(int i=0;i<n;++i)
	{
		for(int j=0;j<n-i-1;++j)
		{
			if(a[j].comp > a[j+1].comp)
			{
				MostMaybe temp;
				temp = a[j];
				a[j] = a[j+1];
				a[j+1] = temp;
			}
		}
	}

	vector<int> b;

	b.push_back(a[0].num);
	b.push_back(a[1].num);
	b.push_back(a[2].num);

	return b;
}



UINT CompareThread(LPVOID pParam)
{  	

	//����������ȫ�ֱ���
	Histogram comp;
	extern bool thread_flag;
	extern vector<Data> que_vct;		//��ʶ������
	//extern vector<Data>::iterator que;	//��ʶ�������еĵ�����
	extern vector<Data> bg_vct;			//�����������
	extern vector<Data>::iterator bg;	//���������еĵ�����

	//===========================ȡ��������еĵ�һ��ͼƬ��Ϊ��ǰ����ͼƬ==============
	while(1)
	{
		if(!thread_flag)//ѭ����ֹ��ʶ
			break;
		if(bg_vct.size()!=0)
		{
			
			string bgname=(*bg).everyfilename;//��ǰ����ͼƬ��·�����ļ���
			Mat bgpic;		//��ǰ�ı���ͼƬ
			bgpic=cv::imread(bgname);
			Mat bghist=bg->wholehist;//bg->wholehist�ǵ�ǰ����ͼƬ������ͼ��ֱ��ͼ
			vector<Mat>::iterator bgbegin;
			bgbegin=bg->blockhistogram.begin();
			vector<Mat>::iterator bgend;
			bgend=bg->blockhistogram.end();
			double wholeres=0;
			//==========================Ѱ�ҵ�ǰ����ͼƬ֮��ĵ�һ��ͼƬ====================
			tempbgname=bgname;
			vector<Data> ::iterator LAST;	
			LAST=find_if(que_vct.begin(), que_vct.end(),find_last);//�ҵ����ű�����Ƭ��que_vct�е�λ��
			++LAST;//����һλ�������ĵ�һ��

			for(vector<Data>::iterator i=LAST;i!=que_vct.end();i++)
			{
				if(i->mark==0)			//�˴�iָ���˸��ű���֮��ĵ�һ�Ų�ȷ��(��markΪ0)��ͼƬ
				{
					Mat unshist=i->wholehist;
					wholeres=comp.compareH(bghist,unshist)/12000000;//����ͼֱ��ͼ�Ƚ�
					
					if(wholeres>=0.85)
					{
						tempname=i->everyfilename;
						vector<AnmPlace>::iterator it;
						it=find_if(place_vct.begin(),place_vct.end(),find_place);
						if (wholeres>(it->cmprslt))
						{
							it->cmprslt=wholeres;//�޸�place_vct�и���Ƭ��Ӧ��AnmPlace�е�����ͼ���ƶ�
							double tempres;
							vector<Mat>::iterator unsbegin;
							unsbegin=i->blockhistogram.begin();
							vector<Mat>::iterator unsend;
							unsend=i->blockhistogram.end();
							vector<Mat>::iterator k;
							k=unsbegin;
							//int tempflag=1;
							vector<Mat>::iterator j;
							vector<MostMaybe> maybe;
							vector<int> number;//number�д�ŵ��ǿ��ܴ��ڶ���ķֿ�ı�ţ���0~47��
							int counter=0;
							for(j=bgbegin;j!=bgend;++j)
							{
								tempres=comp.compareH(*j,*k)/250000;
								if(tempres<0.80)		//2014.12.9
								{
									MostMaybe may;
									may.comp=tempres;
									may.num=counter;
									maybe.push_back(may);
								}
								++k;
								++counter;
							}
							if(maybe.size()==0)//�����зֿ����ƶȶ���0.85���ϣ���������Ƭ�޶���
							{
								i->mark=-1;		//������޶���mark-1
							}
							else if(maybe.size()<=3) //�����С�ڵ����������ƶ�С��0.85�ķֿ飬���ж��ٸ�ָʾ���ٸ�
							{
								vector<MostMaybe>::iterator i;
								for(i=maybe.begin();i!=maybe.end();++i)
									number.push_back(i->num);
							}
							else
							{
								number = bubble(maybe);//����õ������ƶ���͵������ֿ飬����number��
							}
							vector<int>::iterator num;
							it->block.swap(vector<int>());
							for(num=number.begin();num!=number.end();num++)
							{
								it->block.push_back(*num);//�����number�е������µ�place_vct��
							}

						}
						else//������ű������Ǻ�ͼƬ���ƶ���ߵģ���һ�����㵽�в�����Ҫ��ķֿ飬����ֹͣ
						{
							double tempres=1;
							vector<Mat>::iterator unsbegin;
							unsbegin=i->blockhistogram.begin();
							vector<Mat>::iterator unsend;
							unsend=i->blockhistogram.end();
							vector<Mat>::iterator k;
							k=unsbegin;
							vector<Mat>::iterator j;
							j=bgbegin;
							while((tempres>=0.8)&&(j!=bgend))
							{
								tempres=comp.compareH(*j,*k)/250000;
								++j;
								++k;
							}
							if(j==bgend)
							{
								i->mark=-1;
							}
							
						}
					}
					i->counter-=1;
				}	
			}
			bg_vct.erase(bg_vct.begin());//�ڱ���ͼƬ������ɾ�������Ѿ�������ı���ͼƬ
		}
	}
	return 0;  
}


UINT InitializeThread(LPVOID pParam)
{
	//=====================������ʶ���ļ��������ļ�����˳��洢ֱ��ͼ��ÿ��48�飩��ʶ��״̬��Ĭ�� 0��===============
	extern vector<Data> que_vct;
	extern vector<string> pathList;		
	InitialBlock blockinitial;
	que_vct=blockinitial.storeblocks(pathList);//2014.12.9
	place_vct=blockinitial.storeplaces(pathList);//2014.12.21
	extern bool init_flag;
	init_flag=false;
	//�ȵõ���ʶ�ļ����������ļ����ļ����浽pathList��
	//que_vct����洢��ÿһ�����˱�������ж�Ϊ�޶���֮���ͼƬ��
	//���������ļ�����everyfilename)��48��ֱ��ͼ(blockhistogram)
	//��ʶ����(mark=0,��1,��2,+3)�Լ�����ͼ��ֱ��ͼ(wholehist)
	return 0;
}