#pragma warning( disable: 4996 )
/* *************** License:**************************
Oct. 3, 2008
Right to use this code in any way you want without warrenty, support or any guarentee of it working.

BOOK: It would be nice if you cited it:
Learning OpenCV: Computer Vision with the OpenCV Library
by Gary Bradski and Adrian Kaehler
Published by O'Reilly Media, October 3, 2008

AVAILABLE AT:
http://www.amazon.com/Learning-OpenCV-Computer-Vision-Library/dp/0596516134
Or: http://oreilly.com/catalog/9780596516130/
ISBN-10: 0596516134 or: ISBN-13: 978-0596516130

OTHER OPENCV SITES:
* The source code is on sourceforge at:
http://sourceforge.net/projects/opencvlibrary/
* The OpenCV wiki page (As of Oct 1, 2008 this is down for changing over servers, but should come back):
http://opencvlibrary.sourceforge.net/
* An active user group is at:
http://tech.groups.yahoo.com/group/OpenCV/
* The minutes of weekly OpenCV development meetings are at:
http://pr.willowgarage.com/wiki/OpenCV
************************************************** */


#include "stereocalib.h"
//#include "cvaux.h"
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <ctype.h>

#include <iostream>
using namespace std;

StereoCalibrate::StereoCalibrate()
{

}

StereoCalibrate::~StereoCalibrate()
{

}

//
// Given a list of chessboard images, the number of corners (nx, ny)
// on the chessboards, and a flag: useCalibrated for calibrated (0) or
// uncalibrated (1: use cvStereoCalibrate(), 2: compute fundamental
// matrix separately) stereo. Calibrate the cameras and display the
// rectified results along with the computed disparity images.
//
void StereoCalibrate::StereoCalib(const char* imageList, int nx, int ny, int useUncalibrated)
{

	int displayCorners = 0;
	int showUndistorted = 0;
	bool isVerticalStereo = false;//OpenCV can handle left-right
	//or up-down camera arrangements
	const int maxScale = 1;
	const float squareSize = 26.0f; //Set this to your actual square size
	FILE* f = fopen(imageList, "rt");
	int i, j, lr, nframes, n = nx*ny, N = 0;  //lr代表左视图或右视图索引
	vector<string> imageNames[2];
	vector<CvPoint3D32f> objectPoints;
	vector<CvPoint2D32f> points[2];
	vector<int> npoints;  //每一帧中的角点数
	vector<uchar> active[2];
	vector<CvPoint2D32f> temp(n);
	// ARRAY AND VECTOR STORAGE:
	double M1[3][3], M2[3][3], D1[5], D2[5];
	double R[3][3], T[3], E[3][3], F[3][3];
	stereoParams.cameraParams1.cameraMatrix = cvMat(3, 3, CV_64F, M1);
	stereoParams.cameraParams2.cameraMatrix = cvMat(3, 3, CV_64F, M2);
	stereoParams.cameraParams1.distortionCoefficients = cvMat(1, 5, CV_64F, D1);
	stereoParams.cameraParams2.distortionCoefficients = cvMat(1, 5, CV_64F, D2);
	stereoParams.rotation = cvMat(3, 3, CV_64F, R);
	stereoParams.translation = cvMat(3, 1, CV_64F, T);
	stereoParams.essential = cvMat(3, 3, CV_64F, E);
	stereoParams.foundational = cvMat(3, 3, CV_64F, F);
	if (displayCorners)
		cvNamedWindow("corners", 1);
	// READ IN THE LIST OF CHESSBOARDS:
	if (!f)
	{
		fprintf(stderr, "can not open file %s\n", imageList);
		return;
	}
	for (i = 0;; i++)
	{
		char buf[1024];
		int count = 0, result = 0;
		lr = i % 2;
		vector<CvPoint2D32f>& pts = points[lr];
		if (!fgets(buf, sizeof(buf)-3, f))
			break;
		size_t len = strlen(buf);
		while (len > 0 && isspace(buf[len - 1]))  //遇到空格就转为结束符
			buf[--len] = '\0';
		if (buf[0] == '#')
			continue;
		IplImage* img = cvLoadImage(buf, 0);  //读取照片，强转为灰度图
		if (!img)
			break;
		imageSize = cvGetSize(img);
		imageNames[lr].push_back(buf);//在vector尾部加入一个数据。
		//FIND CHESSBOARDS AND CORNERS THEREIN:
		for (int s = 1; s <= maxScale; s++)
		{
			IplImage* timg = img;
			if (s > 1)
			{
				timg = cvCreateImage(cvSize(img->width*s, img->height*s),
					img->depth, img->nChannels);
				cvResize(img, timg, CV_INTER_CUBIC);
			}
			result = cvFindChessboardCorners(timg, cvSize(nx, ny),
				&temp[0], &count,
				CV_CALIB_CB_ADAPTIVE_THRESH |
				CV_CALIB_CB_NORMALIZE_IMAGE);  //归一化图像后进行自适应二值化
			if (timg != img)
				cvReleaseImage(&timg);
			if (result || s == maxScale)
			for (j = 0; j < count; j++)
			{
				temp[j].x /= s;
				temp[j].y /= s;
			}
			if (result)
				break;
		}
		if (displayCorners)
		{
			printf("%s\n", buf);
			IplImage* cimg = cvCreateImage(imageSize, 8, 3);
			cvCvtColor(img, cimg, CV_GRAY2BGR);
			cvDrawChessboardCorners(cimg, cvSize(nx, ny), &temp[0],
				count, result);
			cvShowImage("corners", cimg);
			cvReleaseImage(&cimg);
			if (cvWaitKey(1000) == 27) //Allow ESC to quit
				exit(-1);
		}else{
			putchar('.');
		}
		N = pts.size();
		pts.resize(N + n, cvPoint2D32f(0, 0));//n角点个数
		active[lr].push_back((uchar)result);
		//assert( result != 0 );
		if (result)
		{
			//Calibration will suffer without subpixel interpolation
			cvFindCornerSubPix(img, &temp[0], count,
				cvSize(11, 11), cvSize(-1, -1),
				cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,
				30, 0.01));
			copy(temp.begin(), temp.end(), pts.begin() + N);
		}
		cvReleaseImage(&img);
	}
	fclose(f);
	printf("\n");
	// HARVEST CHESSBOARD 3D OBJECT POINT LIST:
	nframes = active[0].size();//Number of good chessboads found左
	objectPoints.resize(nframes*n);
	for (i = 0; i < ny; i++)
	for (j = 0; j < nx; j++)
		objectPoints[i*nx + j] =
		cvPoint3D32f(i*squareSize, j*squareSize, 0);
	for (i = 1; i < nframes; i++)
		copy(objectPoints.begin(), objectPoints.begin() + n,
		objectPoints.begin() + i*n);
	npoints.resize(nframes, n);
	N = nframes*n;
	CvMat _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0]);  //物理空间坐标
	CvMat _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0]);    //左视图坐标
	CvMat _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0]);    //右视图坐标
	CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0]);
	printf("Running stereo calibration ...");
	//先进行单目标定
	cvCalibrateCamera2(
		&_objectPoints, &_imagePoints1,
		&_npoints,  imageSize,
		&stereoParams.cameraParams1.cameraMatrix,
		&stereoParams.cameraParams1.distortionCoefficients,
		NULL, NULL,0  //CV_CALIB_FIX_ASPECT_RATIO
		);
	cvCalibrateCamera2(
		&_objectPoints, &_imagePoints2,
		&_npoints,  imageSize,
		&stereoParams.cameraParams2.cameraMatrix,
		&stereoParams.cameraParams2.distortionCoefficients,
		NULL, NULL,0  //CV_CALIB_FIX_ASPECT_RATIO
		);

	//cvSetIdentity(&_M1);//设置为单位阵
	//cvSetIdentity(&_M2);
	//cvZero(&_D1);
	//cvZero(&_D2);

	// CALIBRATE THE STEREO CAMERAS	
	fflush(stdout);  //清空输出缓存区
	cvStereoCalibrate(&_objectPoints, &_imagePoints1,
		&_imagePoints2, &_npoints,
		&stereoParams.cameraParams1.cameraMatrix,
		&stereoParams.cameraParams1.distortionCoefficients,
		&stereoParams.cameraParams2.cameraMatrix,
		&stereoParams.cameraParams2.distortionCoefficients,
		imageSize,
		&stereoParams.rotation,
		&stereoParams.translation, 
		&stereoParams.essential,
		&stereoParams.foundational,
		cvTermCriteria(CV_TERMCRIT_ITER +
		CV_TERMCRIT_EPS, 100, 1e-5),
		//CV_CALIB_FIX_ASPECT_RATIO +
		CV_CALIB_ZERO_TANGENT_DIST +
		//CV_CALIB_FIX_PRINCIPAL_POINT+
		//CV_CALIB_USE_INTRINSIC_GUESS+
		CV_CALIB_FIX_ASPECT_RATIO+
		CV_CALIB_SAME_FOCAL_LENGTH
		//CV_CALIB_FIX_INTRINSIC+
		//CALIB_FIX_K3
		//CALIB_FIX_K4 + 
		//CALIB_FIX_K5
		);
	printf(" done\n");
	//保存标定参数
	cvSave("M1.xml",&stereoParams.cameraParams1.cameraMatrix);
	cvSave("D1.xml",&stereoParams.cameraParams1.distortionCoefficients);
	cvSave("M2.xml",&stereoParams.cameraParams2.cameraMatrix);
	cvSave("D2.xml",&stereoParams.cameraParams2.distortionCoefficients);
	cvSave("R.xml",&stereoParams.rotation);
	cvSave("T.xml",&stereoParams.translation);
	cvSave("E.xml",&stereoParams.essential);
	cvSave("F.xml",&stereoParams.foundational);
	// CALIBRATION QUALITY CHECK
	// because the output fundamental matrix implicitly
	// includes all the output information,
	// we can check the quality of calibration using the
	// epipolar geometry constraint: m2^t*F*m1=0  像素坐标
	vector<CvPoint3D32f> lines[2];
	points[0].resize(N);
	points[1].resize(N);
	_imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0]);
	_imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0]);  
	lines[0].resize(N);
	lines[1].resize(N);
	CvMat _L1 = cvMat(1, N, CV_32FC3, &lines[0][0]);
	CvMat _L2 = cvMat(1, N, CV_32FC3, &lines[1][0]);
	//计算校正前的误差
	cvComputeCorrespondEpilines(&_imagePoints1, 1, &stereoParams.foundational, &_L1);
	cvComputeCorrespondEpilines(&_imagePoints2, 2, &stereoParams.foundational, &_L2);
	double avgErr = 0;
	for (i = 0; i < N; i++)//判断对应点与极线的偏差
	{
		double err = fabs(points[0][i].x*lines[1][i].x +
			points[0][i].y*lines[1][i].y + lines[1][i].z)
			+ fabs(points[1][i].x*lines[0][i].x +
			points[1][i].y*lines[0][i].y + lines[0][i].z);
		avgErr += err;
	}
	printf("distorted avg err = %g\n", avgErr / (nframes*n));
	//Always work in undistorted space
	cvUndistortPoints(&_imagePoints1, &_imagePoints1,
		&stereoParams.cameraParams1.cameraMatrix,
		&stereoParams.cameraParams1.distortionCoefficients, 0,
		&stereoParams.cameraParams1.cameraMatrix);
	cvUndistortPoints(&_imagePoints2, &_imagePoints2,
		&stereoParams.cameraParams2.cameraMatrix,
		&stereoParams.cameraParams2.distortionCoefficients, 0,
		&stereoParams.cameraParams2.cameraMatrix);
	cvComputeCorrespondEpilines(&_imagePoints1, 1, &stereoParams.foundational, &_L1);
	cvComputeCorrespondEpilines(&_imagePoints2, 2, &stereoParams.foundational, &_L2);
	avgErr = 0;
	for (i = 0; i < N; i++)//判断对应点与极线的偏差
	{
		double err = fabs(points[0][i].x*lines[1][i].x +
			points[0][i].y*lines[1][i].y + lines[1][i].z)
			+ fabs(points[1][i].x*lines[0][i].x +
			points[1][i].y*lines[0][i].y + lines[0][i].z);
		avgErr += err;
	}
	printf("avg err = %g\n", avgErr / (nframes*n));
	//COMPUTE AND DISPLAY RECTIFICATION
	GetStereoRectifyMat();

}

//获得校正需要的映射矩阵
void StereoCalibrate::GetStereoRectifyMat(void)
{
	CvMat* mx1 = cvCreateMat(imageSize.height,
		imageSize.width, CV_32F);
	CvMat* my1 = cvCreateMat(imageSize.height,
		imageSize.width, CV_32F);
	CvMat* mx2 = cvCreateMat(imageSize.height,

		imageSize.width, CV_32F);
	CvMat* my2 = cvCreateMat(imageSize.height,
		imageSize.width, CV_32F);
	
	double R1[3][3], R2[3][3], P1[3][4], P2[3][4], Q[4][4];
	CvMat _R1 = cvMat(3, 3, CV_64F, R1);
	CvMat _R2 = cvMat(3, 3, CV_64F, R2);
	// IF BY CALIBRATED (BOUGUET'S METHOD)

	CvMat _P1 = cvMat(3, 4, CV_64F, P1);
	CvMat _P2 = cvMat(3, 4, CV_64F, P2);
	CvMat _Q = cvMat(4, 4, CV_64F, Q);
	cvStereoRectify(&stereoParams.cameraParams1.cameraMatrix, 
					&stereoParams.cameraParams2.cameraMatrix,
					&stereoParams.cameraParams1.distortionCoefficients,
					&stereoParams.cameraParams2.distortionCoefficients, 
					imageSize,
					&stereoParams.rotation, 
					&stereoParams.translation,
					&_R1, &_R2, &_P1, &_P2, &_Q,
					0,0);

	//Precompute maps for cvRemap()
	cvInitUndistortRectifyMap(&stereoParams.cameraParams1.cameraMatrix,
							  &stereoParams.cameraParams1.distortionCoefficients,
							  &_R1, &_P1, mx1, my1);
	cvInitUndistortRectifyMap(&stereoParams.cameraParams2.cameraMatrix,
							  &stereoParams.cameraParams2.distortionCoefficients,
							  &_R2, &_P2, mx2, my2);

	cvSave("mx1.xml", mx1);
	cvSave("my1.xml", my1);
	cvSave("mx2.xml", mx2);
	cvSave("my2.xml", my2);
	cvSave("p1.xml", &_P1);
	cvSave("p2.xml", &_P2);
	cvSave("Q.xml",&_Q);

	cvReleaseMat(&mx1);
	cvReleaseMat(&my1);
	cvReleaseMat(&mx2);
	cvReleaseMat(&my2);
}

//立体校正初始化(载入校正矩阵)
void StereoCalibrate::InitStereoRectify(void)
{
	//导入校正矩阵
	rectifyParams.mx1 = *(CvMat*)cvLoad("mx1.xml");  
	rectifyParams.my1 = *(CvMat*)cvLoad("my1.xml");
	rectifyParams.mx2 = *(CvMat*)cvLoad("mx2.xml");
	rectifyParams.my2 = *(CvMat*)cvLoad("my2.xml");
}

void StereoCalibrate::StereoRectify(IplImage *left, IplImage *right)
{
	IplImage*t1 = cvCloneImage(left);
	IplImage*t2 = cvCloneImage(right);
	cvRemap(t1, left, &rectifyParams.mx1, &rectifyParams.my1);
	cvRemap(t2, right, &rectifyParams.mx2, &rectifyParams.my2);
	cvReleaseImage(&t1);
	cvReleaseImage(&t2);
}

//载入测距参数
void StereoCalibrate::LoadCameraPara(void)
{

	stereoParams.cameraParams1.cameraMatrix = *(CvMat*)cvLoad("M1.xml");
	stereoParams.cameraParams1.distortionCoefficients = *(CvMat*)cvLoad("D1.xml");
	stereoParams.cameraParams2.cameraMatrix = *(CvMat*)cvLoad("M2.xml");
	stereoParams.cameraParams2.distortionCoefficients = *(CvMat*)cvLoad("D2.xml");
	stereoParams.rotation = *(CvMat*)cvLoad("R.xml");
	stereoParams.translation =  *(CvMat*)cvLoad("T.xml");
	stereoParams.essential = *(CvMat*)cvLoad("E.xml");
	stereoParams.foundational = *(CvMat*)cvLoad("F.xml");
	CvMat* q = (CvMat*)cvLoad("Q.xml");
	projectq = Mat(q);
	focal = CV_MAT_ELEM(stereoParams.cameraParams1.cameraMatrix, double, 0, 0);  //pixel
	baseline = -CV_MAT_ELEM(stereoParams.translation, double, 0, 0);  //mm
}