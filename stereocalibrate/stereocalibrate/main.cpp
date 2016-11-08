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
#define POINTCLOUD 0
//#define FRAMEDIFF
//#define BACKGROUNGDIFF
#define DISPLAY 1
//#define SINGLE_FRAME

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
	imshow("disparity", gvdisp);	
	//得到3D电云
	PointCld.getPointClouds(gdisp,StCab.projectq);
	PointCld.savePointClouds();
    //检测物体
	PointCld.detectObject();	
	if(cvWaitKey(0) == 27) return 0;
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

#ifdef FRAMEDIFF
		Mat matLeft,matleftPre,matDiffFrame,matLeftGray;
		bool firstFrame = true;
		Mat element = getStructuringElement(MORPH_RECT,Size(3,3));  
#endif

#ifdef BACKGROUNGDIFF
		Mat matLeft;
		Mat matLeftGray;
		Mat matFr;
		Mat matBk,matshowbk;
		bool firstFrame = true;
#endif

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
#ifdef FRAMEDIFF
			matLeft = Mat(left);
			cvtColor(matLeft, matLeftGray, CV_RGB2GRAY);
			if (true == firstFrame){
				matleftPre = matLeftGray.clone();
				firstFrame = false;
			}else{
				absdiff(matLeftGray, matleftPre, matDiffFrame);
				threshold(matDiffFrame, matDiffFrame, 20, 255, CV_THRESH_BINARY);
				matleftPre = matLeftGray.clone();
				medianBlur(matDiffFrame,matDiffFrame,3);     //中值滤波法  
				//dilate(matDiffFrame,matDiffFrame,element);  //膨胀
				imshow("diff",matDiffFrame);
			}
#endif

#ifdef BACKGROUNGDIFF
			matLeft = Mat(left);
			cvtColor(matLeft, matLeftGray, CV_RGB2GRAY);
			imshow("frame", matLeftGray);
			matLeftGray.convertTo(matLeftGray, CV_32F);
			if (true == firstFrame){
				matBk = matLeftGray.clone();  //记录背景							
				firstFrame = false;
			}else{
				GaussianBlur(matLeftGray,matLeftGray,Size(5,5),0);  //高斯滤波
				absdiff(matLeftGray, matBk, matFr);  //减去背景得到前景
				threshold(matFr, matFr, 50, 255, CV_THRESH_BINARY); 
				//accumulateWeighted(matLeftGray, matBk, 0.001);  //更新背景
				namedWindow("background",0);
				namedWindow("foreground",0);
				matBk.convertTo(matshowbk,CV_8U);
				imshow("background", matshowbk);    //显示背景
				imshow("foreground", matFr);    //显示前景
			}
#endif

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
			if (POINTCLOUD)
			{
				PointCld.getPointClouds(Stmatch.disp, StCab.projectq);
				//检测物体
				Mat mleft(left);
				PointCld.detectObject(mleft);	
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
			
			//cvShowImage("RefPair", RefPair);
		
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
	it = Stmatch.disp.begin<short>();
	//Mat_<float>::iterator itrd,itrx,itry;
	//itrd = PointCld.mat_depth.begin<float>();
	//itrx = PointCld.mat_phyx.begin<float>();
	//itry = PointCld.mat_phyy.begin<float>();
	//int pcols =  PointCld.mat_depth.cols;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		cout << "pixel_x:" << x<< "pixel_y:" << y << endl;
		double dis;
		dis = (StCab.baseline * StCab.focal) /  (double)(*(it + y * Stmatch.disp.cols + x)) * 16.0;
		printf("distance of this object is: %lf \n", dis);
		double phyx,phyy,phyz;
		//cout << "x:" << *(itrx + y*pcols + x) << " ";
		//cout << "y:" << *(itry + y*pcols + x) << " ";
		//cout << "y:" << *(itrd + y*pcols + x) << endl;
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