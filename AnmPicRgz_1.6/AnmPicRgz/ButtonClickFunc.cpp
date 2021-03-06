#include "stdafx.h"
//#include "AnmPicRgz.h"
#include "AnmPicRgzDlg.h"
//#include "afxdialogex.h"

#include <algorithm>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <string.h>
#include <fstream>
//#include "Data.h"
#include "Histogram.h"
#include "InitialBlock.h"
#include "InitialFile.h"		
#include "mostmaybe.h"
#include "AnmPlace.h"
#include "ConfirmDlg.h"
using namespace std;
using namespace cv;

vector<Data> que_vct;			//待识别序列
vector<Data>::iterator que;		//待识别序列中的迭代器
vector<Data> bg_vct;			//背景缓冲队列
vector<Data>::iterator bg;		//背景序列中的迭代器

vector<AnmPlace> place_vct;		//存放每张照片可能存在动物的位置信息，以及他们的整幅图相似度2012.12.21

vector<string> pathList;		//储存照片路径，在InitializeThread中使用
vector<string> allList;			//2015.1.13所有照片的信息,每次pahtList读取1000个，然后在allList中删除这1000个
string apt,						//用户路径
	   signal="\\";

int judge_flag=0;				//上下翻页是否调回当前处理照片的标志
bool thread_flag=false;			//比较线程运行标志
bool init_flag=false;			//初始化线程运行标志
int filenum;
extern IplImage *pImg;

//extern vector<string> foldlist;			//储存子文件夹路径
//extern vector<string>::iterator it_fold;
//extern bool multi_flag;					//多文件夹处理标志



void CAnmPicRgzDlg::OnBnClickedButtonStart()		//！！！！！！！！！开始!！！！！！！！！！！
{
	thread_flag=false;
	que_vct.swap(vector<Data>());
	bg_vct.swap(vector<Data>());
	pathList.swap(vector<string>());
	place_vct.swap(vector<AnmPlace>());
	Invalidate(true);					//清除图像
	
	UpdateData();
	apt=(LPCSTR)CStringA(AppointFile);		//用户输入的识别路径
	if(apt=="")
		AfxMessageBox(_T("请选择正确的文件夹"));
	else
	{
		InitialFile fileinitial;		//2014.12.9
		filenum=fileinitial.searchFileInDirectroy(apt, allList);//2015.1.13
		
		if(filenum==-1)
			AfxMessageBox(_T("该文件夹中无照片"));
		else
		{
			GetDlgItem(IDC_MFCEDITBROWSE1)->EnableWindow(false);
			InitialAll(&allList,&filenum);
		}
	}
}



void CAnmPicRgzDlg::InitialAll(vector<string>* AllList,int* num)//提供list和num则初始化所有应该初始化的东西
{
	
	InitialFile fileinitial;
	
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(false);
			
	if (NULL == p_InitDlg)   
	{   
		p_InitDlg = new CInitDlg();						// 创建非模态对话框实例   
		p_InitDlg->Create(IDD_INIT_DIALOG, this);   
		p_InitDlg->text_init="正在读入照片，请稍候 ...";
		p_InitDlg->UpdateData(false);
	}   
	p_InitDlg->ShowWindow(SW_SHOW);						// 显示非模态对话框   
	p_InitDlg->UpdateWindow();
	pathList=fileinitial.ThousandOnly(AllList,num);//2015.1.13
	InitialBlock blockinitial;
	que_vct=blockinitial.storeblocks(pathList);//2014.12.9
	place_vct=blockinitial.storeplaces(pathList);//2014.12.21

	que=que_vct.begin();
	cvReleaseImage(&pImg);
	pImg = cvLoadImage((*que).everyfilename.c_str(),1);
	ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
	//cvReleaseImage(&pImg);
			
	thread_flag=true;
	CWinThread* pCompThread;
	pCompThread=AfxBeginThread(CompareThread,NULL);		//开启后台比较线程

	if (NULL != p_InitDlg)   
	{   
		delete p_InitDlg;			// 删除非模态对话框对象   
		p_InitDlg=NULL;
	}   

			
	GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(true);
}



void CAnmPicRgzDlg::OnBnClickedButtonYes()
{
	if(que_vct.empty())
		AfxMessageBox(_T("请先点击“开始”按钮"));
	else
	{
		if(que->mark==1)
			AfxMessageBox(_T("已判断为“有动物”，无需重复判断"));
		else if(que->mark==-1)
		{
			CConfirmDlg ConfirmGlg;
			ConfirmGlg.text_orig="无动物";
			ConfirmGlg.text_new="有动物 ？";
			INT_PTR nRes = ConfirmGlg.DoModal();		//弹出模态对话框

			if(IDOK == nRes)
			{
				que->mark=1;
				text_mark="已判断为 有动物";
				UpdateData(false);
			}			
		}
		else		//mark=0时，即第一次判断
		{
			que->mark=1;		//人工有动物mark+2
			if(que==(que_vct.end()-1)&&allList.empty())
			{
				//extern bool thread_flag;			//线程运行标志
				thread_flag=false;
				GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(true);
				AfxMessageBox(_T("该文件夹已处理完成，请执行分类"),MB_OK|MB_ICONINFORMATION);
			}
			else if(que==(que_vct.end()-1)&&!allList.empty())
			{
				thread_flag=false;
				//加入分发
				Copy();
				pathList.swap(vector<string>());
				que_vct.swap(vector<Data>());
				place_vct.swap(vector<AnmPlace>());
				bg_vct.swap(vector<Data>());
				Invalidate(true);		
				InitialAll(&allList,&filenum);
			}
			else 
			{
				AutoNextPic();
			}
			if(que==que_vct.end()&&allList.empty())
			{
				thread_flag=false;
				GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(true);
				AfxMessageBox(_T("该文件夹已处理完成，请执行分类"),MB_OK|MB_ICONINFORMATION);
			}
			else if(que==que_vct.end()&&!allList.empty())
			{
				thread_flag=false;
				//加入分发
				Copy();
				pathList.swap(vector<string>());
				que_vct.swap(vector<Data>());
				place_vct.swap(vector<AnmPlace>());
				bg_vct.swap(vector<Data>());
				Invalidate(true);		
				InitialAll(&allList,&filenum);
			}
		}
	}
}


void CAnmPicRgzDlg::OnBnClickedButtonNo()
{
	if(que_vct.empty())
		AfxMessageBox(_T("请先点击“开始”按钮"));
	else
	{
		if(que->mark==-1)
			AfxMessageBox(_T("已判断为“无动物”，无需重复判断"));
		else if(que->mark==1)
		{
			CConfirmDlg ConfirmGlg;
			ConfirmGlg.text_orig="有动物";
			ConfirmGlg.text_new="无动物 ？";
			INT_PTR nRes = ConfirmGlg.DoModal();		//弹出模态对话框

			if(IDOK == nRes)
			{
				que->mark=-1;
				text_mark="已判断为 无动物";
				UpdateData(false);
			}
		}
		else		//mark=0时，即第一次判断
		{
			que->mark=-1;		//人工无动物mark-2
			Data BackG;
			BackG=(*que);
			bg_vct.push_back(BackG);			//加入背景集
			bg=bg_vct.begin();
			if((que==que_vct.end()-1)&&allList.empty())
			{
				thread_flag=false;		//给线程一个结束标志
				GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(true);
				AfxMessageBox(_T("该文件夹已处理完成，请执行分类"),MB_OK|MB_ICONINFORMATION);
			}
			else if(que==que_vct.end()-1&&!allList.empty())
			{
			
				thread_flag=false;
				//加入分发
				Copy();
				pathList.swap(vector<string>());
				que_vct.swap(vector<Data>());
				place_vct.swap(vector<AnmPlace>());
				bg_vct.swap(vector<Data>());
				Invalidate(true);	
				InitialAll(&allList,&filenum);
			}

			else 
			{
				vector<Data>::iterator i;
				for(i=que+1;i!=que_vct.end();++i)
				{
					if(i->mark==0)
						i->counter+=1;
				}
				NextPicControl(que);
			}

			if(que==que_vct.end()&&allList.empty())
			{
				thread_flag=false;
				GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(true);
				AfxMessageBox(_T("该文件夹已处理完成，请执行分类"),MB_OK|MB_ICONINFORMATION);
			}
			else if(que==que_vct.end()&&!allList.empty())
			{
				thread_flag=false;
				//加入分发
				Copy();
				pathList.swap(vector<string>());
				que_vct.swap(vector<Data>());
				place_vct.swap(vector<AnmPlace>());
				bg_vct.swap(vector<Data>());
				Invalidate(true);		
				InitialAll(&allList,&filenum);
			}
		}
	}
}


//void CAnmPicRgzDlg::OnTimer(UINT_PTR nIDEvent) 
//{ 
//	switch(nIDEvent) 
//	{ 
//		case 10:
//		{
//			GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);	
//			AutoNextPic();
//			GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);	
//			KillTimer(10);
//			if(que==que_vct.end())
//			{
//				thread_flag=false;		//给线程一个结束标志
//				GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(true);
//				AfxMessageBox(_T("该文件夹已处理完成，请执行分类"),MB_OK|MB_ICONINFORMATION);
//			}
//			break;
//		}
//	} 
//	CDialog::OnTimer(nIDEvent);
//} 


void CAnmPicRgzDlg::OnBnClickedButtonLast()
{
	
	if(que_vct.empty())
		AfxMessageBox(_T("请先点击“开始”按钮"));
	else if(que==que_vct.begin())
		AfxMessageBox(_T("已是该文件夹第一张"));
	else
	{
		if(que==que_vct.end()) 
			que-=2; 
		else 
			que--;		//que到达que_vct.end()的特殊处理
		cvReleaseImage(&pImg);
		pImg = cvLoadImage((*que).everyfilename.c_str(),1);
		ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
		
		if(que->mark>0)
			text_mark="已判断为 有动物";
		else if(que->mark<0)
			text_mark="已判断为 无动物";
		else
			text_mark="";
		UpdateData(false);
		
		if(que==que_vct.begin())
		{
			GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);			//控制按钮可用
			GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
		}
		else
		{
			vector<Data>::iterator it;
			for(it=que-1;it!=que_vct.begin()&&it->mark!=0;--it)
			{

			}
			if(it==que_vct.begin()&&it->mark!=0)
			{
				GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);			//控制按钮可用
				GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
			}
			else
			{
				GetDlgItem(IDC_BUTTON_YES)->EnableWindow(false);		//控制按钮不可用
				GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);
				AfxMessageBox(_T("前面还有照片未处理"));
			}
		}
	}
}



void CAnmPicRgzDlg::OnBnClickedButtonNext()
{
	GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(true);
	if(que_vct.empty())
		AfxMessageBox(_T("请先点击“开始”按钮"));
	else if(que==que_vct.end()-1||que==que_vct.end())	//考虑que到达que_vct.end()的特殊情况
		AfxMessageBox(_T("已是该文件夹最后一张"));
	else
	{
		que++;
		cvReleaseImage(&pImg);
		pImg = cvLoadImage((*que).everyfilename.c_str(),1);
		ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
		
		if(que->mark>0)
			text_mark="已判断为 有动物";
		else if(que->mark<0)
			text_mark="已判断为 无动物";
		else
			text_mark="";
		UpdateData(false);
		vector<Data>::iterator it;
			for(it=que-1;it!=que_vct.begin()&&it->mark!=0;--it)
			{

			}
			if(it==que_vct.begin()&&it->mark!=0)
			{
				GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);			//控制按钮可用
				GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
			}
			else
			{
				GetDlgItem(IDC_BUTTON_YES)->EnableWindow(false);		//控制按钮不可用
				GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);
				AfxMessageBox(_T("前面还有照片未处理"));
			}
	}
}



void CAnmPicRgzDlg::OnBnClickedButtonCopy()
{
	GetDlgItem(IDC_BUTTON_YES)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(false);
	cvReleaseImage(&pImg);
	/*string nowfilename;
	nowfilename=que->everyfilename;*/
	Copy(/*nowfilename*/);
	if(que_vct.empty()&&allList.empty())
	{
		AfxMessageBox(_T("分类已完成，请处理下一文件夹"),MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);
		GetDlgItem(IDC_MFCEDITBROWSE1)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
	}
	else if(que_vct.empty()&&!allList.empty())
	{
		AfxMessageBox(_T("分类已完成，请继续识别当前文件夹或重新选择文件夹处理"),MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_MFCEDITBROWSE1)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);

	}
	else
	{
		AfxMessageBox(_T("分类已完成，请继续识别当前文件夹或重新选择文件夹处理"),MB_OK|MB_ICONINFORMATION);que=que_vct.begin();
		cvReleaseImage(&pImg);
		que=que_vct.begin();//2015.1.21
		pImg = cvLoadImage((*que).everyfilename.c_str(),1);
		ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
		GetDlgItem(IDC_MFCEDITBROWSE1)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
	}
}



void CAnmPicRgzDlg::Copy()
{
	int pos = apt.find_last_of('\\');	
	string foldname(apt.substr(pos));				//去掉多余路径，用foldname存放文件夹名，有\符
	string fold_yes,fold_no;
	fold_yes=apt+foldname+"-有动物";
	fold_no =apt+foldname+"-无动物";
	string Cmd_yes="cmd /c md "+fold_yes;
	string Cmd_no="cmd /c md "+fold_no;	
	WinExec((LPCSTR)Cmd_yes.c_str(),SW_HIDE);		//创建有动物、无动物文件夹
	WinExec((LPCSTR)Cmd_no.c_str(),SW_HIDE);
	vector<Data> ::iterator cpy;
	vector<AnmPlace>::iterator p_cpy;
	p_cpy=place_vct.begin();
	char Command[260];
	for(cpy=que_vct.begin();cpy!=que_vct.end();)
	{
		string path_orig=cpy->everyfilename,path_new;
		int pos2 = path_orig.find_last_of('\\');	
		string filename(path_orig.substr(pos2));		//去掉路径，用filename存放文件名，有\符，下一步移动用
		if(cpy->mark>0)
		{
			path_new=fold_yes+filename;	
			cpy=que_vct.erase(cpy);					//目标文件夹“有动物”
			p_cpy=place_vct.erase(p_cpy);
			sprintf_s(Command,"cmd /c move %s %s",path_orig.c_str(),path_new.c_str());
			WinExec((LPCSTR)Command,SW_HIDE);				//分发
		}
		else if(cpy->mark<0)
		{
			path_new=fold_no +filename;				//目标文件夹“无动物”
			cpy=que_vct.erase(cpy);
			p_cpy=place_vct.erase(p_cpy);
			sprintf_s(Command,"cmd /c move %s %s",path_orig.c_str(),path_new.c_str());
			WinExec((LPCSTR)Command,SW_HIDE);				//分发
		}
		else
		{
			++cpy;
			++p_cpy;
		}
	}
}