#ifndef _STEREOMATCH_H_
#define _STEREOMATCH_H_
#include "cv.h"
#include "cxmisc.h"
#include "highgui.h"
#include "cvaux.h"
#include "cxcore.h"
#include "iostream"

using namespace cv;
using namespace std;

class StereMatch{
private:

public:
	StereMatch();
	~StereMatch();
	StereoBM bm;
	StereoSGBM sgbm;
	Mat disp;   // ”≤ÓÕº
	Mat vdisp;   // ”≤ÓÕº
	void InitBMMatch(void);
	void BMMatch(IplImage *img1, IplImage *img2, Mat &vdisp);
	void InitSGBMMatch(void);
	void SGBMMatch(IplImage *img1, IplImage *img2, Mat &vdisp);
	void StereMatch::GCMatch(IplImage *img1, IplImage *img2, CvMat *vdisp);
};

#endif