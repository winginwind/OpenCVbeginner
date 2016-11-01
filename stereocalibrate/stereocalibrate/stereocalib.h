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
class StereoCalibrate{
//variable
private:
	StereoParams stereoParams;
	CvSize imageSize;
//function
private:
	void StereoCalib(const char* imageList, int nx, int ny, int useUncalibrated);
public:
	StereoCalibrate();
	virtual ~StereoCalibrate();
	int StereoCalibrateRectify(void);
	void GetStereoRectifyMat(void);

};


#endif
