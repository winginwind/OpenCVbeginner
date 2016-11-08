#include "stereomatch.h"


StereMatch::StereMatch()
{

}

StereMatch::~StereMatch()
{

}

void StereMatch::InitBMMatch(void)
{
	bm.state->preFilterCap = 13;
	bm.state->preFilterSize = 13;
	bm.state->SADWindowSize = 13;
	bm.state->minDisparity = 0;
	bm.state->numberOfDisparities = 144;
	bm.state->textureThreshold = 10;
	bm.state->uniquenessRatio = 15;
	bm.state->speckleWindowSize = 100;
	bm.state->speckleRange = 32;
	bm.state->disp12MaxDiff = 1;
}
/**
*基于BM算法计算视差图
* img1左视图
* img2右视图
* disp视差图
* vdisp归一化视差图
*/

void StereMatch::BMMatch(IplImage *img1, IplImage *img2, Mat &vdisp)
{
	Mat image1(img1);
	Mat image2(img2);
	Mat imggray1;
	Mat imggray2;
	cvtColor(image1, imggray1, CV_RGB2GRAY);
	cvtColor(image2, imggray2, CV_RGB2GRAY);
	bm(imggray1, imggray2, disp);
	normalize(disp, vdisp, 0, 255, NORM_MINMAX, CV_8UC1);	
}

void StereMatch::InitSGBMMatch(void)
{
	int cn = 1;
	sgbm.preFilterCap = 15;
	sgbm.SADWindowSize = 17;
	sgbm.P1 = 8*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.P2 = 32*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;  
	sgbm.minDisparity = 0;  
	sgbm.numberOfDisparities = 128;  
	sgbm.uniquenessRatio = 13;  
	sgbm.speckleWindowSize = 100;  
	sgbm.speckleRange = 32;  
	sgbm.disp12MaxDiff = 1;  
}

void StereMatch::SGBMMatch(IplImage *img1, IplImage *img2, Mat &vdisp)
{
	Mat image1(img1);
	Mat image2(img2);
	Mat imggray1;
	Mat imggray2;
	cvtColor(image1, imggray1, CV_RGB2GRAY);
	cvtColor(image2, imggray2, CV_RGB2GRAY);
	sgbm(imggray1, imggray2, disp);
	normalize(disp, vdisp, 0, 255, NORM_MINMAX, CV_8UC1);	
}

void StereMatch::GCMatch(IplImage *img1, IplImage *img2, CvMat *vdisp)
{
	CvSize imageSize = cvGetSize(img1);
	CvMat* img1gray = cvCreateMat(imageSize.height,
		imageSize.width, CV_8U);
	CvMat* img2gray = cvCreateMat(imageSize.height,
		imageSize.width, CV_8U);

	CvMat* ldisp = cvCreateMat(imageSize.height,
		imageSize.width, CV_16S);
	CvMat* rdisp = cvCreateMat(imageSize.height,
		imageSize.width, CV_16S);

	cvCvtColor(img1, img1gray, CV_RGB2GRAY);
	cvCvtColor(img2, img2gray, CV_RGB2GRAY);

	CvStereoGCState* state = cvCreateStereoGCState( 16, 2 );   
	cvFindStereoCorrespondenceGC( img1gray, img2gray, vdisp, rdisp, state, 0 );  
	//cvNormalize(ldisp, vdisp, 0, 256, CV_MINMAX);	
	cvReleaseStereoGCState( &state );  
}