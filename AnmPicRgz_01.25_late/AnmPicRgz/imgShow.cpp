#include "stdafx.h"
#include "AnmPicRgz.h"
#include "AnmPicRgzDlg.h"
#include "afxdialogex.h"
#include "AnmPlace.h"

#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <string.h>

using namespace std;
using namespace cv;

int num_anmplc;
extern vector<Data> que_vct;
extern vector<Data>::iterator que;
extern vector<AnmPlace> place_vct;

void CAnmPicRgzDlg::FillBitmapInfo( BITMAPINFO *bmi, int width, int height, int bpp )  
{  
    ASSERT( bmi && width > 0 && height > 0 &&  
        (bpp == 8 || bpp == 24 || bpp == 32) );  
  
    BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);  
      
    memset( bmih, 0, sizeof(*bmih));  
      
    bmih->biSize   = sizeof(BITMAPINFOHEADER);  
    bmih->biWidth = width;  
    bmih->biHeight = -abs(height);  
    bmih->biPlanes = 1;  
    bmih->biBitCount = bpp;  
    bmih->biCompression = BI_RGB;  
      
    if( bpp == 8 )  
    {  
        RGBQUAD* palette = bmi->bmiColors;  
        int i;  
        for( i = 0; i < 256; i++ )  
        {  
            palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;  
            palette[i].rgbReserved = 0;  
        }  
    }  
}  

void CAnmPicRgzDlg::ShowImage(IplImage *pImg, CWnd *wnd, BITMAPINFO *bmi)  
{  
    CDC *pDC = wnd->GetDC();  
    HDC hDC = pDC->GetSafeHdc();  
    CRect rect;  
    wnd->GetClientRect(&rect);  
      
    if((bmi->bmiHeader).biBitCount== 8)  
    {  
        CPalette pal;  
        HPALETTE hpal = NULL;  
        HPALETTE hOldPal = NULL;  
        ::SetPaletteEntries(hpal, 0, 256,(LPPALETTEENTRY)bmi->bmiColors);  
        hOldPal = ::SelectPalette(pDC->GetSafeHdc(), hpal, TRUE);  
    }  
    ::SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);  
    ::StretchDIBits(pDC->GetSafeHdc(),  
        rect.left,  
        rect.top,  
        //pImg->width,pImg->height,  
        rect.right-rect.left, rect.bottom-rect.top,  
        0,0,  
        pImg->width,pImg->height,pImg->imageData,bmi,DIB_RGB_COLORS,SRCCOPY);  
  
    wnd->ReleaseDC( pDC );  
}  

void CAnmPicRgzDlg::ShowImageAuto( IplImage *pImg, CWnd *wnd )  
{  
    if ( 0 == pImg || 0 == wnd )  
    {  
        return;  
    }  
  
    BITMAPINFO* bmi;    
    unsigned int buffer[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256];   
    bmi = (BITMAPINFO*)buffer;  
  
    FillBitmapInfo(bmi, pImg->width, pImg->height, pImg->depth*pImg->nChannels );  
  
    ShowImage( pImg, wnd, bmi );  
}  

bool find_anmplc(AnmPlace& d)
{
	if (d.num==num_anmplc)
		return true;
	else
		return false;
}

void CAnmPicRgzDlg::DrawArrow()
{
	if(!que_vct.empty())
	{
		if(que!=que_vct.end())
		{
			num_anmplc=que->number;
			vector<AnmPlace>::iterator it;
			it=find_if(place_vct.begin(),place_vct.end(),find_anmplc);
			if(!(*it).block.empty())
			{
				CRect rect_pic;   //获取pic控件大小
				GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&rect_pic);
				int width=rect_pic.Width()/8;
				int height=rect_pic.Height()/6;

				vector<int>::iterator it_block;
				for(it_block=it->block.begin();it_block!=it->block.end();it_block++)
				{	
					div_t a=div(*it_block,8);
					int quot=a.quot;	//商，决定行数，y坐标
					int rem=a.rem;		//余数，决定列数，x坐标
					int P1x,P1y,P2x,P2y,P3x,P3y;
					if(rem<=3)
					{
						P1x=rem*width+width/4;
						P2x=rem*width+width*3/4;
					}
					else
					{
						P1x=rem*width+width*3/4;
						P2x=rem*width+width/4;
					}
						P1y=quot*height+height/4;
						P2y=quot*height+height*3/4;
						P3x=rem*width+width/2;
						P3y=quot*height+height/2;

					CClientDC dc(GetDlgItem(IDC_STATIC_PIC));//获取客户窗口DC
					CPen pen,pen1,*oldpen;
					pen.CreatePen(PS_SOLID,5,RGB(255,0,0));
					pen1.CreatePen(PS_SOLID,5,RGB(255,0,0));
					oldpen=dc.SelectObject(&pen);
					dc.MoveTo(P1x,P1y);
					dc.LineTo(P2x,P2y);
					dc.SelectObject(&pen1);
					dc.MoveTo(P2x,P2y);
					dc.LineTo(P3x,P2y);
					dc.MoveTo(P2x,P2y);
					dc.LineTo(P2x,P3y);
					dc.SelectObject(oldpen);
				}
			}
		}
	}
}
