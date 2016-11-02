#ifndef _STEREOMATCH_H_
#define _STEREOMATCH_H_
#include "cv.h"
#include "cxmisc.h"
#include "highgui.h"
using namespace cv;

class StereMatch{
private:
	CvStereoBMState *BMState;
	StereoSGBM sgbm;
public:
	StereMatch();
	~StereMatch();
	void InitBMMatch(void);
	void BMMatch(IplImage *img1, IplImage *img2, CvMat *disp, CvMat *vdisp);
	void InitSGBMMatch(void);
	void SGBMMatch(IplImage *img1, IplImage *img2, Mat disp, Mat vdisp);
};

#endif