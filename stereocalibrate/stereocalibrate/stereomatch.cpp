#include "stereomatch.h"


StereMatch::StereMatch()
{

}

StereMatch::~StereMatch()
{

}

void StereMatch::InitBMMatch(void)
{
	BMState = cvCreateStereoBMState();
	assert(BMState != 0);
	BMState->preFilterSize = 21;
	BMState->preFilterCap = 30;
	BMState->SADWindowSize = 13;
	BMState->minDisparity = 0;
	BMState->numberOfDisparities = 128;
	BMState->textureThreshold = 30;
	BMState->uniquenessRatio = 15;
}
/**
*基于BM算法计算视差图
* img1左视图
* img2右视图
* disp视差图
* vdisp归一化视差图
*/

void StereMatch::BMMatch(IplImage *img1, IplImage *img2, CvMat *disp, CvMat *vdisp)
{
	CvSize imageSize = cvGetSize(img1);
	CvMat* img1gray = cvCreateMat(imageSize.height,
		imageSize.width, CV_8U);
	CvMat* img2gray = cvCreateMat(imageSize.height,
		imageSize.width, CV_8U);
	
	cvCvtColor(img1, img1gray, CV_RGB2GRAY);
	cvCvtColor(img2, img2gray, CV_RGB2GRAY);
	cvFindStereoCorrespondenceBM(img1gray, img2gray, disp, BMState);
	cvNormalize(disp, vdisp, 0, 256, CV_MINMAX);	
	cvReleaseMat(&img1gray);
	cvReleaseMat(&img2gray);
}

void StereMatch::InitSGBMMatch(void)
{
	int cn = 1;
	sgbm.preFilterCap = 63;
	sgbm.SADWindowSize = 11;
	sgbm.P1 = 4*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.P2 = 32*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;  
	sgbm.minDisparity = 0;  
	sgbm.numberOfDisparities = 32;  
	sgbm.uniquenessRatio = 10;  
	sgbm.speckleWindowSize = 100;  
	sgbm.speckleRange = 32;  
	sgbm.disp12MaxDiff = 1;  
}

void StereMatch::SGBMMatch(IplImage *img1, IplImage *img2, Mat disp, Mat vdisp)
{
	CvSize imageSize = cvGetSize(img1);
	Mat img1gray;
	Mat img2gray;

	cvCvtColor(img1, &img1gray, CV_RGB2GRAY);
	cvCvtColor(img2, &img2gray, CV_RGB2GRAY);

	sgbm(img1gray, img2gray, disp);
	cvNormalize(&disp, &vdisp, 0, 256, CV_MINMAX);	
}