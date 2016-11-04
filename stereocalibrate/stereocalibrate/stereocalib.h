#ifndef  _STEREOCALI_H_
#define  _STEREOCALI_H_
#include "cv.h"
#include "cxmisc.h"
#include "highgui.h"
struct CameraParams
{
	CvMat		cameraMatrix;			// 摄像机矩阵
	CvMat		distortionCoefficients;	// 摄像机畸变参数
};

struct StereoParams
{
	CameraParams	cameraParams1;	// 左摄像机标定参数
	CameraParams	cameraParams2;	// 右摄像机标定参数
	CvMat			rotation;		// 旋转矩阵
	CvMat			translation;	// 平移向量
	CvMat			essential;		// 本质矩阵
	CvMat			foundational;	// 基础矩阵
};

struct RectifyParams
{
	CvMat mx1;
	CvMat my1;
	CvMat mx2;
	CvMat my2;
};
class StereoCalibrate{
//variable
public:
	double focal;
	double baseline;
	double cxl;
	double cxr;
private:
	StereoParams stereoParams;
	CvSize imageSize;
	RectifyParams rectifyParams;

//function
private:
	
	void GetStereoRectifyMat(void);

public:
	StereoCalibrate();
	virtual ~StereoCalibrate();
	void InitStereoRectify(void);
	void StereoRectify(IplImage *left, IplImage *right);
	void StereoCalib(const char* imageList, int nx, int ny, int useUncalibrated);
	void LoadCameraPara(void);
};


#endif
