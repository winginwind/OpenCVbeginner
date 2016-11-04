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
