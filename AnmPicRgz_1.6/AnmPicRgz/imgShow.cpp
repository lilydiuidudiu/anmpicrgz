#include "stdafx.h"
#include "AnmPicRgz.h"
#include "AnmPicRgzDlg.h"
#include "afxdialogex.h"

#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <string.h>

using namespace std;
using namespace cv;

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
void CAnmPicRgzDlg::DrawArrow(CPoint p1,CPoint p2,double theta,int length)
{
	theta=3.1415926*theta/180;//转换为弧度
	double Px,Py,P1x,P1y,P2x,P2y;
	//以P2为原点得到向量P2P1（P）
	Px=p1.x-p2.x;
	Py=p1.y-p2.y;
	//向量P旋转theta角得到向量P1
	P1x=Px*cos(theta)-Py*sin(theta);
	P1y=Px*sin(theta)+Py*cos(theta);
	//向量P旋转-theta角得到向量P2
	P2x=Px*cos(-theta)-Py*sin(-theta);
	P2y=Px*sin(-theta)+Py*cos(-theta);
	//伸缩向量至制定长度
	double x1,x2;
	x1=sqrt(P1x*P1x+P1y*P1y);
	P1x=P1x*length/x1;
	P1y=P1y*length/x1;
	x2=sqrt(P2x*P2x+P2y*P2y);
	P2x=P2x*length/x2;
	P2y=P2y*length/x2;
	//平移变量到直线的末端
	P1x=P1x+p2.x;
	P1y=P1y+p2.y;
	P2x=P2x+p2.x;
	P2y=P2y+p2.y;
	CClientDC dc(GetDlgItem(IDC_STATIC_PIC));//获取客户窗口DC
	CPen pen,pen1,*oldpen;
	pen.CreatePen(PS_SOLID,5,RGB(255,0,0));
	pen1.CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
	oldpen=dc.SelectObject(&pen);
	dc.MoveTo(p1.x,p1.y);
	dc.LineTo(p2.x,p2.y);
	dc.SelectObject(&pen1);
	dc.MoveTo(p2.x,p2.y);
	dc.LineTo(P1x,P1y);
	dc.MoveTo(p2.x,p2.y);
	dc.LineTo(P2x,P2y);
	dc.SelectObject(oldpen);
}
