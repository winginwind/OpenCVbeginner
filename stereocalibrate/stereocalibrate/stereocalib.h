#ifndef  _STEREOCALI_H_
#define  _STEREOCALI_H_
#include "cv.h"
#include "cxmisc.h"
#include "highgui.h"
struct CameraParams
{
	CvMat		cameraMatrix;			// ���������
	CvMat		distortionCoefficients;	// ������������
};

struct StereoParams
{
	CameraParams	cameraParams1;	// ��������궨����
	CameraParams	cameraParams2;	// ��������궨����
	CvMat			rotation;		// ��ת����
	CvMat			translation;	// ƽ������
	CvMat			essential;		// ���ʾ���
	CvMat			foundational;	// ��������
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
