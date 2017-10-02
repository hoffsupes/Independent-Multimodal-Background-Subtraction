#include <opencv2/highgui/highgui.hpp>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include <cstdarg>
#include "opencv2/opencv.hpp"
#include "fstream"
#include "Ctracker.h"
#include <dirent.h>
#include <math.h>
#include <time.h>



using namespace std;
using namespace cv;

const int ROW_NO = 360; /// For testing purposes only remove later on!! 
const int COL_NO = 640;	/// Make sure that size forced to 360x640!

struct rgbtuple
{
int r,g,b,d;
};

struct BGtuple
{
vector<rgbtuple> RGBD;			// Each element is an rgbtuple structure that can be pushed back One vector per BGtuple to contain multiple values
};

struct graytuple
{
int age;
	
	graytuple()
	{
	age = 0;
	}
};

struct framelist
{
vector<Mat> frame;
int n;			// Zero based frame index
};

int max_three(int a,int b,int c)
{
int ret;
if(a > b){ ret = a; }
else { ret = b;	}
if(c > ret){ret = c;}

return ret;
}

int min_three(int a,int b,int c)
{
int ret;
if(a > b){ ret = b; }
else { ret = a;	}
if(c < ret){ret = c;}

return ret;
}

float max_three(float a,float b,float c)
{
float ret;
if(a > b){ ret = a; }
else { ret = b;	}
if(c > ret){ret = c;}

return ret;
}

void RegisterBackground(int n,Mat frame,BGtuple BIJ[ROW_NO][COL_NO],int N,int D,int A)
{	
 
	if (n == 0)	
	{
		for(int i = 0;i<frame.rows;i++)
		{
			for(int j = 0;j<frame.cols;j++)
			{
			rgbtuple rgtmp;
			rgtmp.r = (int)frame.at<Vec3b>(i,j)[2];
			rgtmp.g = (int)frame.at<Vec3b>(i,j)[1];
			rgtmp.b = (int)frame.at<Vec3b>(i,j)[0];
			rgtmp.d = 1;
			BIJ[i][j].RGBD.push_back(rgtmp);
			}
		}

	}
	else if(n == N-1)
	{
	
		for(int i = 0;i<frame.rows;i++)
		{
			for(int j = 0;j<frame.cols;j++)
			{
				for(int k = 0;k< BIJ[i][j].RGBD.size();k++)
				{
					if(BIJ[i][j].RGBD[k].d <= D)
					{
					BIJ[i][j].RGBD.erase(BIJ[i][j].RGBD.begin() + k);
					}
					
				}
				
			}
		}
		
			
		
	}
	else	
	{
			for(int i = 0;i<frame.rows;i++)
		{
			for(int j = 0;j<frame.cols;j++)
			{
				for(int k = 0;k<BIJ[i][j].RGBD.size();k++)
				{

if (max_three( (int)(frame.at<Vec3b>(i,j)[2]) - BIJ[i][j].RGBD[k].r , (int)(frame.at<Vec3b>(i,j)[0]) -BIJ[i][j].RGBD[k].b , (int)(frame.at<Vec3b>(i,j)[1]) - BIJ[i][j].RGBD[k].g ) <= A)
{
	
BIJ[i][j].RGBD[k].r = ((BIJ[i][j].RGBD[k].r * BIJ[i][j].RGBD[k].d) + (int)frame.at<Vec3b>(i,j)[2]) / ( BIJ[i][j].RGBD[k].d + 1);
BIJ[i][j].RGBD[k].b = ((BIJ[i][j].RGBD[k].b * BIJ[i][j].RGBD[k].d) + (int)frame.at<Vec3b>(i,j)[0]) / ( BIJ[i][j].RGBD[k].d + 1);
BIJ[i][j].RGBD[k].g = ((BIJ[i][j].RGBD[k].g * BIJ[i][j].RGBD[k].d) + (int)frame.at<Vec3b>(i,j)[1]) / ( BIJ[i][j].RGBD[k].d + 1);
BIJ[i][j].RGBD[k].d += 1;
break;
}

else
{
			rgbtuple rgtmp;
			rgtmp.r = (int)frame.at<Vec3b>(i,j)[2];
			rgtmp.g = (int)frame.at<Vec3b>(i,j)[1];
			rgtmp.b = (int)frame.at<Vec3b>(i,j)[0];
			rgtmp.d = 1;
			BIJ[i][j].RGBD.push_back(rgtmp);
	
}
					
				}
				
			}
		}
	
	}
}

Mat RegisterForeground(Mat frame,BGtuple BIJ[ROW_NO][COL_NO],int N,int D,int A)
{

	Mat fore = Mat::ones(frame.size(),CV_8UC1);
	
	for(int i = 0;i<frame.rows;i++)
	{
		for(int j = 0;j<frame.cols;j++)
		{
		
			if(!BIJ[i][j].RGBD.empty())
			{
			
				for(int k = 0;k< BIJ[i][j].RGBD.size();k++)
				{
if ( max_three((int)(frame.at<Vec3b>(i,j)[2]) - BIJ[i][j].RGBD[k].r , (int )(frame.at<Vec3b>(i,j)[0]) - BIJ[i][j].RGBD[k].b ,(int)(frame.at<Vec3b>(i,j)[1]) - BIJ[i][j].RGBD[k].g ) < A)
{
fore.at<uchar>(i,j) = 0;
break;
}
					
				}
				
			}
			
			
		}
	}
	
	return fore;
}

void rgb2hsv(float r,float g,float b,float& h,float& s,float& v)
{
r = r/255;
g = g/255;
b = b/255;

	float Cmax = max_three(r,g,b);
	float Cmin = min_three(r,g,b);
	float del  = Cmax - Cmin;

	if(Cmax == r)
	{
	h = 60 * (  (int)((g-b)/del) % 6); // Add mod6 operation here
	}
	else if(Cmax == g)
	{
	h = 60 * ((b-r)/del + 2);
	}
	else if(Cmax == b)
	{
	h = 60 * ((r-g)/del + 4);
	}
	
	if(Cmax == 0)
	{
	s = 0;
	}
	else
	{
	s = del/Cmax;
	}

	v = Cmax;
}

void hsv2rgb(int h, int s, int v, int &r,int& g,int &b)
{
int c = v*s;
int x = c*( 1 - ((h/60)%2 - 1));
int m = v-c;

if(h >= 0 && h < 60){r = c; g = x; b = 0;}
else if(h >= 60&&h<120){r = x; g = c; b = 0;}
else if(h>=120 && h<180){r = 0; g = c; b = x;}
else if(h>=180&&h<240){r = 0; g = x; b = c;}
else if(h>=240&&h<300){r = x; g = 0; b = c;}
else if(h>=300&&h<360){r = c; g = 0; b = x;}

r = r+m; g = g+m; b = b+m;
r*=255;
g*=255;
b*=255;
}

void RGB2HSV(float r,float g,float b,float& h,float& s,float& v)
{
	float imax, imin;
	float l;
	imax = max_three(r,g,b);
	imin = min_three(r,g,b);
	
	if(imax == r){	h = (g-b)/(imax-imin); h = h*60; h = ((int) h % 360);	}
	else if (imax == g){h = (b-r)/(imax-imin); h = h*60; h = ((int) h % 360);}
	else if(imax == b){h = (r-b)/(imax-imin); h = h*60; h = ((int) h % 360);}
	l = (imax+imin)/2;
	
	if(l>=0.5){	s = (imax - imin)/(imax+imin);	}
	else if(l<0.5){s = (imax - imin)/(2 - imax - imin);}
	
	v = (imax - imin)/2;
}

Mat shadow(Mat frame,BGtuple BIJ[ROW_NO][COL_NO])
{
	float alpha = 0.5,beta = 1.15;
	int taus = 30,tauh = 40;

	Mat M =  Mat(frame.size(),CV_8UC1);

		for(int i = 0;i<frame.rows;i++)
	{
		for(int j = 0;j<frame.cols;j++)
		{
				int flag = 0;
			if(!BIJ[i][j].RGBD.empty())
			{
		
				float r,g,b,h,s,v;
				r = (float)frame.at<Vec3b>(i,j)[2]; g = (float)frame.at<Vec3b>(i,j)[1]; b = (float)frame.at<Vec3b>(i,j)[0]; 
				RGB2HSV(r,g,b,h,s,v);
				
					for(int k = 0;k< BIJ[i][j].RGBD.size();k++)
					{
				float br,bg,bb,bh,bs,bv;
				br = (float)BIJ[i][j].RGBD[k].r; bb = (float)BIJ[i][j].RGBD[k].b; bg = (float)BIJ[i][j].RGBD[k].g;
				RGB2HSV(br,bg,bb,bh,bs,bv);
		
				if((alpha <= v/bv <= beta)&&( h - bh <= tauh)&&( s - bs <= taus))
				{
				flag++;
				}
										
					}
				
			}
			if (flag == BIJ[i][j].RGBD.size())
			{
			
				M.at<uchar>(i,j) = 1;
				
			}
			
		}
	}
	
	return M;
}

void UpdateFGmodel(BGtuple BIJ[ROW_NO][COL_NO],Mat& fore,int A,graytuple FGtu[ROW_NO][COL_NO],Mat frame,int Tage)
{

	for(int i = 0;i< fore.rows;i++)
	{
		for(int j = 0;j<fore.cols;j++)
		{
			int flag = 0;
			for(int k = 0;k< BIJ[i][j].RGBD.size();k++)
			{
				if(fore.at<uchar>(i,j) == ( max_three( (int)(frame.at<Vec3b>(i,j)[2]) - BIJ[i][j].RGBD[k].r , (int)(frame.at<Vec3b>(i,j)[0]) -BIJ[i][j].RGBD[k].b , (int)(frame.at<Vec3b>(i,j)[1]) - BIJ[i][j].RGBD[k].g ) <= A && 1 ))
				{ 
				FGtu[i][j].age++;
				break;
				}
			}
						
		}
	}
	
		for(int i = 0;i< fore.rows;i++)
	{
		for(int j = 0;j<fore.cols;j++)
		{
				if(FGtu[i][j].age > Tage)
				{
				fore.at<uchar>(i,j) = 0;
				}
						
		}
	}
	
	
	
}

void imbs()
{

int n = 0;
int ts = 0;
int P = 10;		// Killswitch for sampling rate [Set P = 0]
int tp = 20;
int N = 30,D = 2,A = 5;
BGtuple BIJ[ROW_NO][COL_NO];
graytuple FGtu[ROW_NO][COL_NO];
n = 0;
Mat foreground;
Mat frame;
Mat sha;
Mat nsha;
int Tage = 10;
	
	/////////////////////////////////////
	// VideoCapture for testing
	
	VideoCapture cap;
	cap.open("dol.avi");
	
	
	
	/////////////////////////////////////
while(tp < cap.get(CV_CAP_PROP_FRAME_COUNT))
	{
	cap>>frame;
	resize(frame,frame,Size(640,360));
	// Main frame reader condition here!
		if(tp - ts > P)
		{
		
		RegisterBackground(n,frame,BIJ,N,D,A);
		ts  = tp;								 
		n++;
		if (n == N)
		{
		n = 0;
		}

		}


		foreground = RegisterForeground(frame,BIJ,N,D,A);
	
// 	string S1("/home/ml/motion/fore_mor_dolphin/foreground"); 
// 	string S2("/home/ml/motion/frame_dolphin/frame"); 
// 	stringstream SS1;
// 	SS1<<tp;
// 	
// 	S1.append(SS1.str());
// 	S1.append(".jpeg");
// 
// 	S2.append(SS1.str());
// 	S2.append(".jpeg");
// 	
// 
// 	imwrite(S2.data(),frame);
		sha = 	shadow(frame,BIJ);
	//	foreground = foreground*255 + sha;
//		bitwise_not(sha,nsha);

//		bitwise_and(foreground,sha,foreground);

	
		morphologyEx(foreground,foreground,MORPH_OPEN,Mat());
		morphologyEx(foreground,foreground,MORPH_CLOSE,Mat());
	
// 		imwrite(S1.data(),foreground*255);
		UpdateFGmodel(BIJ,foreground,A,FGtu,frame,Tage);
		imshow("Fore",foreground*255);
		imshow("Shadow",sha);
	//		medianBlur(foreground,foreground,11);
	//	imshow("Curr",frame);
	//	moveWindow("Fore", 0,0);
//		moveWindow("Sha", 300,400);
		waitKey(1);
		tp++;
	}

}

int main()
{

	imbs();
	
	return 1;
	
}
