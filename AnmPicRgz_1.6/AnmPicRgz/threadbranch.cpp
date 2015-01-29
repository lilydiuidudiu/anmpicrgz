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
extern vector<AnmPlace> place_vct; //存放可能存在动物的信息的序列


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


vector<int> bubble(const vector<MostMaybe>& c)  //冒泡排序法，输出一个vector<int> 里面存放了三个编号
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

	//变量声明，全局变量
	Histogram comp;
	extern bool thread_flag;
	extern vector<Data> que_vct;		//待识别序列
	//extern vector<Data>::iterator que;	//待识别序列中的迭代器
	extern vector<Data> bg_vct;			//背景缓冲队列
	extern vector<Data>::iterator bg;	//背景序列中的迭代器

	//===========================取缓冲队列中的第一张图片作为当前背景图片==============
	while(1)
	{
		if(!thread_flag)//循环终止标识
			break;
		if(bg_vct.size()!=0)
		{
			
			string bgname=(*bg).everyfilename;//当前背景图片的路径和文件名
			Mat bgpic;		//当前的背景图片
			bgpic=cv::imread(bgname);
			Mat bghist=bg->wholehist;//bg->wholehist是当前背景图片的整幅图的直方图
			vector<Mat>::iterator bgbegin;
			bgbegin=bg->blockhistogram.begin();
			vector<Mat>::iterator bgend;
			bgend=bg->blockhistogram.end();
			double wholeres=0;
			//==========================寻找当前背景图片之后的第一张图片====================
			tempbgname=bgname;
			vector<Data> ::iterator LAST;	
			LAST=find_if(que_vct.begin(), que_vct.end(),find_last);//找到该张背景照片在que_vct中的位置
			++LAST;//后移一位，至其后的第一张

			for(vector<Data>::iterator i=LAST;i!=que_vct.end();i++)
			{
				if(i->mark==0)			//此处i指向了该张背景之后的第一张不确定(即mark为0)的图片
				{
					Mat unshist=i->wholehist;
					wholeres=comp.compareH(bghist,unshist)/12000000;//整幅图直方图比较
					
					if(wholeres>=0.85)
					{
						tempname=i->everyfilename;
						vector<AnmPlace>::iterator it;
						it=find_if(place_vct.begin(),place_vct.end(),find_place);
						if (wholeres>(it->cmprslt))
						{
							it->cmprslt=wholeres;//修改place_vct中该照片对应的AnmPlace中的整幅图相似度
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
							vector<int> number;//number中存放的是可能存在动物的分块的编号（从0~47）
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
							if(maybe.size()==0)//即所有分块相似度都在0.85以上，即该张照片无动物
							{
								i->mark=-1;		//计算机无动物mark-1
							}
							else if(maybe.size()<=3) //如果有小于等于三个相似度小于0.85的分块，则有多少个指示多少个
							{
								vector<MostMaybe>::iterator i;
								for(i=maybe.begin();i!=maybe.end();++i)
									number.push_back(i->num);
							}
							else
							{
								number = bubble(maybe);//这里得到了相似度最低的三个分块，放在number中
							}
							vector<int>::iterator num;
							it->block.swap(vector<int>());
							for(num=number.begin();num!=number.end();num++)
							{
								it->block.push_back(*num);//这里把number中的数更新到place_vct中
							}

						}
						else//如果本张背景不是和图片相似度最高的，则一旦计算到有不满足要求的分块，立刻停止
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
			bg_vct.erase(bg_vct.begin());//在背景图片队列中删除该张已经处理过的背景图片
		}
	}
	return 0;  
}


UINT InitializeThread(LPVOID pParam)
{
	//=====================遍历待识别文件夹所有文件，按顺序存储直方图（每张48块）和识别状态（默认 0）===============
	extern vector<Data> que_vct;
	extern vector<string> pathList;		
	InitialBlock blockinitial;
	que_vct=blockinitial.storeblocks(pathList);//2014.12.9
	place_vct=blockinitial.storeplaces(pathList);//2014.12.21
	extern bool init_flag;
	init_flag=false;
	//先得到待识文件夹中所有文件的文件名存到pathList中
	//que_vct里面存储了每一个除了被计算机判断为无动物之外的图片，
	//包括它的文件名（everyfilename)，48块直方图(blockhistogram)
	//和识别结果(mark=0,±1,±2,+3)以及整幅图的直方图(wholehist)
	return 0;
}