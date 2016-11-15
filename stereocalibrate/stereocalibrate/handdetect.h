#ifndef _HANDDETECT_H_
#define _HANDDETECT_H_

#include "cv.h"
using namespace cv;


class HandDetector{
public:
	Mat handMask;
	Mat frameHSV;
	Point2f handLoc;
	HandDetector();
	~HandDetector();
	void HandDetect(Mat &frame);
};

#endif
