#include "stereocalib.h"
#include "picturegrub.h"
#include "stereomatch.h"
#include "pointcloud.h"
using namespace std;

#define SAVE_PIC 0
#define STEREO_CALIB 0
#define STEREO_RECTIFY 0
#define STEREO_BMMATCH 0
#define STEREO_SGBMMATCH 0
#define STEREO_GCMATCH 0
#define DISPLAY 0
#define SINGLE_FRAME

void mouseHandler(int event, int x, int y, int flags, void* param);	//handling mouse events
StereoCalibrate StCab;
PictureGrub PicGrub;
StereMatch Stmatch;
PointCloud PointCld;

Mat gdisp;
Mat gvdisp;

#ifdef SINGLE_FRAME
	Mat leftg = imread("stereoData\\left.jpg",0);
	Mat rightg = imread("stereoData\\right.jpg",0);
	//Mat leftg = imread("stereoData\\tsukuba_l.png",0);
	//Mat rightg = imread("stereoData\\tsukuba_r.png",0);
#endif



int main(void)
{
	//保存用于标定的照片
	if(SAVE_PIC){
		PicGrub.SaveCaliPicture();
	}
	//进行标定
	if (STEREO_CALIB){
		StCab.StereoCalib("cali_list.txt", 9, 6, 0);
	}
	
	StCab.LoadCameraPara();
#ifdef SINGLE_FRAME
	Stmatch.InitBMMatch();
	cvNamedWindow("disparity",0);
	cvSetMouseCallback("disparity", mouseHandler, NULL);
	Stmatch.bm(leftg, rightg, gdisp);
	normalize(gdisp, gvdisp, 0, 255, NORM_MINMAX, CV_8UC1);
	PointCld.getPointClouds(gdisp);
	PointCld.savePointClouds();
	
	while (1)
	{
		//PointCld.detectObject();
		imshow("disparity", gvdisp);	
		if (cvWaitKey(10) == 27)
		{
			return 0;
		}
	}
#endif
	//显示结果
	if (DISPLAY)
	{
		//打开摄像头
		CvCapture*capture1 = cvCreateCameraCapture(0); 
		CvCapture*capture2 = cvCreateCameraCapture(1);

		IplImage*left = cvQueryFrame(capture1);
		IplImage*right = cvQueryFrame(capture2);
		//保证开始可以获得图像
		while (!left || !right)
		{
			left = cvQueryFrame(capture1);
			right = cvQueryFrame(capture2);
			cout << "正在重新获得图像" << endl;
		}
		cout << "图像获取成功" << endl;
		CvSize imgSize = cvGetSize(left);
		CvMat* RefPair;
		CvMat part;
		RefPair = cvCreateMat(imgSize.height, imgSize.width * 2, CV_8UC3);
		cvNamedWindow("RefPair", 1);
		cvMoveWindow("RefPair", 60, 60);
		//进行校正
		if (STEREO_RECTIFY)
		{
			StCab.InitStereoRectify();
			cout << "进行校正" << endl;
		}
		//进行BM算法计算视差
		if (STEREO_BMMATCH)
		{
			Stmatch.InitBMMatch();
			cout << "BM算法计算视差" << endl;
			cvNamedWindow("disparity");
			cvMoveWindow("disparity", 60, 60);
			cvSetMouseCallback("disparity", mouseHandler, NULL);
			
		}
	
		if (STEREO_SGBMMATCH)
		{
			Stmatch.InitSGBMMatch();
			cvNamedWindow("disparity");
			cvMoveWindow("disparity", 60, 60);
		}
		while (1)
		{
			
			left = cvQueryFrame(capture1);
			right = cvQueryFrame(capture2);

			//cvShowImage("left", left);
			//cvShowImage("right", right);
			//if (cvWaitKey(10) == 's')
			//{
			//	cvSaveImage("stereoData\\left.jpg",left);
			//	cvSaveImage("stereoData\\right.jpg",right);
			//}
			if (STEREO_RECTIFY){
				StCab.StereoRectify(left, right);
			
			}
			if (STEREO_BMMATCH)
			{
				clock_t tStart = clock();
				Mat vdisp(imgSize.height,
					imgSize.width, CV_8U);
				Stmatch.BMMatch(left, right, vdisp);
				clock_t tPerFrame = clock() - tStart;
				//cout << tPerFrame << endl;			
				//imshow("disparity", vdisp);
				Stmatch.vdisp = vdisp;
				imshow("disparity", vdisp);	

			}
			if (STEREO_SGBMMATCH)
			{
				Mat vdisp(imgSize.height,
					imgSize.width, CV_8U);
				Stmatch.SGBMMatch(left,right,vdisp);
				imshow("disparity", vdisp);	
			}
			if (STEREO_GCMATCH)
			{
				CvMat* vdisp = cvCreateMat(imgSize.height,
					imgSize.width, CV_8U);
				Stmatch.GCMatch(left, right, vdisp);
				cvShowImage("disparity", vdisp);
				cvReleaseMat(&vdisp);
			}
			
			cvGetCols(RefPair, &part, 0, imgSize.width);  
			cvConvert(left, &part);
			cvGetCols(RefPair, &part, imgSize.width,
				imgSize.width * 2);
			cvConvert(right, &part);
			
			for (int j = 0; j < imgSize.height; j += 16)
				cvLine(RefPair, cvPoint(0, j),
				cvPoint(imgSize.width * 2, j),
				CV_RGB(0, 255, 0));
			
			cvShowImage("RefPair", RefPair);

			
			int c = cvWaitKey(10);
			if (c == 'p')
			{
				c = 0;
				while (c != 'p'&&c != 27)
				{
					c = cvWaitKey(250);
				}
			}
			if (c == 27)
			break;
		}
		cvReleaseImage(&left);
		cvReleaseImage(&right);
		cvReleaseMat(&RefPair);
		
	}
	return 0;
}

bool left_mouse = false;
void mouseHandler(int event, int x, int y, int flags, void *param){
	Mat_<short>::iterator it;
	it = gdisp.begin<short>();

	if (event == CV_EVENT_LBUTTONDOWN)
	{
		cout << "x:" << x<< "y:" << y << endl;
		double dis;
		dis = (StCab.baseline * StCab.focal) /  (double)(*(it + y * gdisp.cols + x)) * 16.0;

		printf("distance of this object is: %lf \n", dis);

		left_mouse = true;
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		left_mouse = false;
	}
	else if ((event == CV_EVENT_MOUSEMOVE) && (left_mouse == true))
	{
	}
}