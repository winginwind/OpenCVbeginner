#include "stereocalib.h"
#include "picturegrub.h"
#include "stereomatch.h"
using namespace std;

#define SAVE_PIC 0
#define STEREO_CALIB 0
#define STEREO_RECTIFY 1
#define STEREO_BMMATCH 0
#define STEREO_SGBMMATCH 1
#define DISPLAY 1
int main(void)
{
	StereoCalibrate StCab;
	PictureGrub PicGrub;
	StereMatch Stmatch;
	//保存用于标定的照片
	if(SAVE_PIC){
		PicGrub.SaveCaliPicture();
	}
	//进行标定
	if (STEREO_CALIB){
		StCab.StereoCalib("cali_list.txt", 9, 6, 0);
	}
	

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

			if (STEREO_RECTIFY){
				StCab.StereoRectify(left, right);
			}
			if (STEREO_BMMATCH)
			{
				clock_t tStart = clock();
				CvMat* disp = cvCreateMat(imgSize.height,
					imgSize.width, CV_16S);
				CvMat* vdisp = cvCreateMat(imgSize.height,
					imgSize.width, CV_8U);
				Stmatch.BMMatch(left, right, disp, vdisp);
				clock_t tPerFrame = clock() - tStart;
				cout << tPerFrame << endl;			
				cvShowImage("disparity", vdisp);
				cvReleaseMat(&disp);
				cvReleaseMat(&vdisp);
			}
			if (STEREO_SGBMMATCH)
			{
				Mat disp(imgSize.height,
					imgSize.width, CV_16S);
				Mat vdisp(imgSize.height,
					imgSize.width, CV_16S);
				Stmatch.SGBMMatch(left,right,disp,vdisp);
				imshow("disparity", vdisp);
			}
			
			cvGetCols(RefPair, &part, 0, imgSize.width);  
			cvConvert(left, &part);
			cvGetCols(RefPair, &part, imgSize.width,
				imgSize.width * 2);
			cvConvert(right, &part);
			/*
			for (int j = 0; j < imgSize.height; j += 16)
				cvLine(RefPair, cvPoint(0, j),
				cvPoint(imgSize.width * 2, j),
				CV_RGB(0, 255, 0));
			*/
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