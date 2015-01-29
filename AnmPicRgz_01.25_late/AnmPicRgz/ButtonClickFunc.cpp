#include "stdafx.h"
#include "AnmPicRgzDlg.h"

#include <algorithm>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <string.h>
#include <fstream>
#include "Histogram.h"
#include "InitialBlock.h"
#include "InitialFile.h"		
#include "mostmaybe.h"
#include "AnmPlace.h"
#include "ConfirmDlg.h"
using namespace std;
using namespace cv;

vector<Data> que_vct;			//��ʶ������
vector<Data>::iterator que;		//��ʶ�������еĵ�����
vector<Data> bg_vct;			//�����������
vector<Data>::iterator bg;		//���������еĵ�����

vector<AnmPlace> place_vct;		//���ÿ����Ƭ���ܴ��ڶ����λ����Ϣ���Լ����ǵ�����ͼ���ƶ�2012.12.21

vector<string> pathList;		//������Ƭ·������InitializeThread��ʹ��
vector<string> allList;			//2015.1.13������Ƭ����Ϣ,ÿ��pahtList��ȡ1000����Ȼ����allList��ɾ����1000��
string apt,						//�û�·��
	   signal="\\";

bool thread_flag=false;			//�Ƚ��߳����б�־
bool init_flag=false;			//��ʼ���߳����б�־
bool flashmark;					//����������˸�ı��
int  filenum=0,					//ÿ�γ�ʼ�����ܹ���ʣ������
	 groupNum=0,				//�ڼ��ֳ�ʼ��
	 sumNum=0;					//���ļ�������Ƭ����
extern IplImage *pImg;


void CAnmPicRgzDlg::OnBnClickedButtonStart()		//��������������������ʼ!��������������������
{
	UpdateData();
	apt=(LPCSTR)CStringA(AppointFile);		//�û������ʶ��·��
	thread_flag=false;
	que_vct.swap(vector<Data>());
	bg_vct.swap(vector<Data>());
	pathList.swap(vector<string>());
	place_vct.swap(vector<AnmPlace>());
	cvReleaseImage(&pImg);
	Invalidate(true);					//���ͼ��
	text_mark="";
	text_count="";
	UpdateData(false);
	if(apt=="")
		AfxMessageBox(_T("��ѡ����ȷ���ļ���"));
	else
	{
		InitialFile fileinitial;		//2014.12.9
		filenum=fileinitial.searchFileInDirectroy(apt, allList);//2015.1.13
		sumNum=filenum;

		if(filenum==-1)
			AfxMessageBox(_T("���ļ���������Ƭ"));
		else
		{
			//GetDlgItem(IDC_MFCEDITBROWSE1)->EnableWindow(false);
			GetDlgItem(IDC_STATIC_COPYRIGHT)->ShowWindow(false);
			GetDlgItem(IDC_STATIC_PIC)->ShowWindow(true);
			InitialAll(&allList,&filenum);
		}
	}
}



void CAnmPicRgzDlg::InitialAll(vector<string>* AllList,int* num)//�ṩlist��num���ʼ������Ӧ�ó�ʼ���Ķ���
{
	groupNum++;
	InitialFile fileinitial;
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(false);
	if (NULL == p_InitDlg)   
	{   
		p_InitDlg = new CInitDlg();						// ������ģ̬�Ի���ʵ��   
		p_InitDlg->Create(IDD_INIT_DIALOG, this);   

		int int_time=(filenum+50)/100;					//�����ʼ������ʱ��
		if(int_time>0)
		{
			CString str_time;
			str_time.Format(_T("%d"),int_time);
			p_InitDlg->text_init=_T("  ���ڶ�����Ƭ����������\n\nԤ������ʱ�䣺")+str_time+_T(" ����");
		}
		else
			p_InitDlg->text_init=_T("\n  ���ڶ�����Ƭ����������");

		p_InitDlg->UpdateData(false);
	}   
	p_InitDlg->ShowWindow(SW_SHOW);						// ��ʾ��ģ̬�Ի���   
	p_InitDlg->UpdateWindow();
	pathList=fileinitial.ThousandOnly(AllList,num);//2015.1.13
	InitialBlock blockinitial;
	que_vct=blockinitial.storeblocks(pathList);//2014.12.9
	place_vct=blockinitial.storeplaces(pathList);//2014.12.21
	if (NULL != p_InitDlg)   
	{   
		delete p_InitDlg;			// ɾ����ģ̬�Ի������   
		p_InitDlg=NULL;
	}   
	que=que_vct.begin();
	//cvReleaseImage(&pImg);
	//pImg = cvLoadImage((*que).everyfilename.c_str(),1);
	//ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));	
	//text_mark="";
	//UpdateData(false);
	ShowPicAndInfo();
	thread_flag=true;
	CWinThread* pCompThread;
	pCompThread=AfxBeginThread(CompareThread,NULL);		//������̨�Ƚ��߳�
	
	GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(true);
}



void CAnmPicRgzDlg::OnBnClickedButtonYes()
{
	if(que_vct.empty())
		AfxMessageBox(_T("���ȵ������ʼ����ť"));
	else
	{
		if(que->mark==1)
		{
			text_mark="���ж�Ϊ �ж���";
			UpdateData(false);
			AfxMessageBox(_T("���ж�Ϊ���ж���������ظ��ж�"));
		}
		else if(que->mark==-1)
		{
			text_mark="���ж�Ϊ �޶���";
			UpdateData(false);
			CConfirmDlg ConfirmDlg;
			ConfirmDlg.text_orig="�޶���";
			ConfirmDlg.text_new="�ж��� ��";
			INT_PTR nRes = ConfirmDlg.DoModal();		//����ģ̬�Ի���
			if(IDOK == nRes)
			{
				que->mark=1;
				text_mark="���ж�Ϊ �ж���";
				UpdateData(false);
			}			
		}
		else		//mark=0ʱ������һ���ж�
		{
			/*if(que>(que_vct.end()-5))
				GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);*/
			
			que->mark=1;		//�˹��ж���mark+2

			if(que==que_vct.end()-1)
				TreatToLastOne();
			else 
			{
				AutoNextPic();
				if(que==que_vct.end())
					TreatToLastOne();
			}
		}
	}
}


void CAnmPicRgzDlg::OnBnClickedButtonNo()
{
	if(que_vct.empty())
		AfxMessageBox(_T("���ȵ������ʼ����ť"));
	else
	{
		if(que->mark==-1)
		{
			text_mark="���ж�Ϊ �޶���";
			UpdateData(false);
			AfxMessageBox(_T("���ж�Ϊ���޶���������ظ��ж�"));
		}
		else if(que->mark==1)
		{
			text_mark="���ж�Ϊ �ж���";
			UpdateData(false);
			CConfirmDlg ConfirmDlg;
			ConfirmDlg.text_orig="�ж���";
			ConfirmDlg.text_new="�޶��� ��";
			INT_PTR nRes = ConfirmDlg.DoModal();		//����ģ̬�Ի���
			if(IDOK == nRes)
			{
				que->mark=-1;
				text_mark="���ж�Ϊ �޶���";
				UpdateData(false);
			}
		}
		else		//mark=0ʱ������һ���ж�
		{
			/*if(que>(que_vct.end()-5))
				GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);*/
			
			que->mark=-1;		//�˹��޶���mark-2
			Data BackG;
			BackG=(*que);

			if(que==que_vct.end()-1)
				TreatToLastOne();
			else 
			{
				vector<Data>::iterator i;
				for(i=que+1;i!=que_vct.end();++i)
				{
					if(i->mark==0)
						i->counter+=1;
				}
				bg_vct.push_back(BackG);
				bg=bg_vct.begin();
				NextPicControl(que);

				if(que==que_vct.end())
					TreatToLastOne();
			}
		}
	}
}

void CAnmPicRgzDlg::TreatToLastOne()
{
	if(que==que_vct.end()) que--;
	if(allList.empty())
	{
		thread_flag=false;
		GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(true);
		AfxMessageBox(_T("���ļ����Ѵ�����ɣ���ִ�з���"),MB_OK|MB_ICONINFORMATION);
	}
	else 
	{
		GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);
		thread_flag=false;
		Copy();		//����ַ�
		pathList.swap(vector<string>());
		que_vct.swap(vector<Data>());
		place_vct.swap(vector<AnmPlace>());
		bg_vct.swap(vector<Data>());
		cvReleaseImage(&pImg);
		Invalidate(true);		
		text_mark="";
		text_count="";
		UpdateData(false);
		InitialAll(&allList,&filenum);
		GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(true);
	}
}


void CAnmPicRgzDlg::OnBnClickedButtonLast()
{
	if(que_vct.empty())
		AfxMessageBox(_T("���ȵ������ʼ����ť"));
	else if(que==que_vct.begin())
		AfxMessageBox(_T("���Ǳ��δ����һ��"));
	else
	{
		//GetDlgItem(IDC_STATIC_WARN)->ShowWindow(false);

		if(que==que_vct.end()) que-=2; else que--;	//que����que_vct.end()�����⴦��
		/*cvReleaseImage(&pImg);
		pImg = cvLoadImage((*que).everyfilename.c_str(),1);
		ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
		DrawArrow();
		if(que->mark>0)			text_mark="���ж�Ϊ �ж���";
		else if(que->mark<0)	text_mark="���ж�Ϊ �޶���";
		else					text_mark="";
		UpdateData(false);*/
		ShowPicAndInfo();
		
		WarnNForbidJudge();
	}
}

void CAnmPicRgzDlg::OnBnClickedButtonNext()
{
	//GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(true);	//��Ҫ�ԣ�
	if(que_vct.empty())
		AfxMessageBox(_T("���ȵ������ʼ����ť"));
	else if(que==que_vct.end()-1||que==que_vct.end())	//����que����que_vct.end()���������
		AfxMessageBox(_T("���Ǳ��δ������һ��"));
	else
	{
		//GetDlgItem(IDC_STATIC_WARN)->ShowWindow(false);
		que++;
		/*cvReleaseImage(&pImg);
		pImg = cvLoadImage((*que).everyfilename.c_str(),1);
		ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
		DrawArrow();
		if(que->mark>0)			text_mark="���ж�Ϊ �ж���";
		else if(que->mark<0)	text_mark="���ж�Ϊ �޶���";
		else					text_mark="";
		UpdateData(false);*/
		ShowPicAndInfo();

		WarnNForbidJudge();
	}
}

void CAnmPicRgzDlg::WarnNForbidJudge()
{
	if(que==que_vct.begin())
	{
		GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);			//���ư�ť����
		GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
		KillTimer(5);
		GetDlgItem(IDC_STATIC_WARN)->ShowWindow(false);
	}
	else
	{
		vector<Data>::iterator it;
		for(it=que-1;it!=que_vct.begin()&&it->mark!=0;--it)
		{

		}
		if(it==que_vct.begin()&&it->mark!=0)
		{
			GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);			//���ư�ť����
			GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
			KillTimer(5);
			GetDlgItem(IDC_STATIC_WARN)->ShowWindow(false);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_YES)->EnableWindow(false);		//���ư�ť������
			GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);
			//AfxMessageBox(_T("ǰ�滹����Ƭδ����"));
			GetDlgItem(IDC_STATIC_WARN)->ShowWindow(true);
			flashmark=true;
			SetTimer(5,500,NULL);
		}
	}
}

void CAnmPicRgzDlg::OnBnClickedButtonCopy()
{
	if(que_vct.empty())
		AfxMessageBox(_T("û����Ƭ�ɷַ�"));
	else if(que_vct.begin()->mark==0)
		AfxMessageBox(_T("û����Ƭ�ɷַ�"));
	else
	{
		KillTimer(5);
		GetDlgItem(IDC_STATIC_WARN)->ShowWindow(false);
		GetDlgItem(IDC_BUTTON_YES)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_NO)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(false);
		cvReleaseImage(&pImg);
		Invalidate(true);
		text_mark="";
		text_count="";
		UpdateData(false);
		/*string nowfilename;
		nowfilename=que->everyfilename;*/
		Copy(/*nowfilename*/);
		if(que_vct.empty()&&allList.empty())
		{
			AfxMessageBox(_T("�ַ�����ɣ��봦����һ�ļ���"),MB_OK|MB_ICONINFORMATION);
			GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(false);
			GetDlgItem(IDC_MFCEDITBROWSE1)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
		}
		else if(que_vct.empty()&&!allList.empty())
		{
			AfxMessageBox(_T("�ַ�����ɣ���������ʼ���Լ���ʶ��ǰ�ļ��У�������ѡ���ļ��д���"),MB_OK|MB_ICONINFORMATION);
			GetDlgItem(IDC_MFCEDITBROWSE1)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
		}
		else
		{
			AfxMessageBox(_T("�ַ�����ɣ������ʶ��ǰ�ļ��У�������ѡ���ļ��д���"),MB_OK|MB_ICONINFORMATION);que=que_vct.begin();
			que=que_vct.begin();//2015.1.21
			/*cvReleaseImage(&pImg);
			pImg = cvLoadImage((*que).everyfilename.c_str(),1);
			ShowImageAuto(pImg,GetDlgItem(IDC_STATIC_PIC));
			DrawArrow();
			text_mark="";
			UpdateData(false);*/
			ShowPicAndInfo();
			GetDlgItem(IDC_MFCEDITBROWSE1)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_LAST)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_YES)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_NO)->EnableWindow(true);
		}
	}
}



void CAnmPicRgzDlg::Copy()
{
	if (NULL == p_InitDlg)   
	{   
		p_InitDlg = new CInitDlg();						// ������ģ̬�Ի���ʵ��   
		p_InitDlg->Create(IDD_INIT_DIALOG, this);   

		int int_time=(que->number+250)/500;					//����ַ�����ʱ��
		if(int_time>0)
		{
			CString str_time;
			str_time.Format(_T("%d"),int_time);
			p_InitDlg->text_init=_T("  �����ƶ���Ƭ����������\n\nԤ������ʱ�䣺")+str_time+_T(" ����");
		}
		else
			p_InitDlg->text_init=_T("\n  �����ƶ���Ƭ����������");

		p_InitDlg->UpdateData(false);
	}   
	p_InitDlg->ShowWindow(SW_SHOW);						// ��ʾ��ģ̬�Ի���   
	p_InitDlg->UpdateWindow();
	
	int pos = apt.find_last_of('\\');	
	string foldname(apt.substr(pos));				//ȥ������·������foldname����ļ���������\��
	string fold_yes,fold_no;
	fold_yes=apt+foldname+"-�ж���";
	fold_no =apt+foldname+"-�޶���";
	string Cmd_yes="cmd /c md "+fold_yes;
	string Cmd_no="cmd /c md "+fold_no;	
	WinExec((LPCSTR)Cmd_yes.c_str(),SW_HIDE);		//�����ж���޶����ļ���
	WinExec((LPCSTR)Cmd_no.c_str(),SW_HIDE);
	vector<Data> ::iterator cpy;
	vector<AnmPlace>::iterator p_cpy;
	cpy=que_vct.begin();
	p_cpy=place_vct.begin();
	char Command[260];
	for(;cpy!=que_vct.end();)
	{
		string filename=cpy->everyfilename;
		string path_orig,path_new;
		path_orig=apt+signal+filename;
		/*int pos2 = path_orig.find_last_of('\\');	
		string filename(path_orig.substr(pos2));	*/	//ȥ��·������filename����ļ�������\������һ���ƶ���
		if(cpy->mark>0)
		{
			path_new=fold_yes+signal+filename;	
			cpy=que_vct.erase(cpy);					//Ŀ���ļ��С��ж��
			p_cpy=place_vct.erase(p_cpy);
			sprintf_s(Command,"cmd /c move \"%s\" \"%s\"",path_orig.c_str(),path_new.c_str());
			WinExec((LPCSTR)Command,SW_HIDE);				//�ַ�
		}
		else if(cpy->mark<0)
		{
			path_new=fold_no +signal+filename;				//Ŀ���ļ��С��޶��
			cpy=que_vct.erase(cpy);
			p_cpy=place_vct.erase(p_cpy);
			sprintf_s(Command,"cmd /c move \"%s\" \"%s\"",path_orig.c_str(),path_new.c_str());
			WinExec((LPCSTR)Command,SW_HIDE);				//�ַ�
		}
		else
		{
			++cpy;
			++p_cpy;
		}
	}

	if (NULL != p_InitDlg)   
	{   
		delete p_InitDlg;			// ɾ����ģ̬�Ի������   
		p_InitDlg=NULL;
	}   
}