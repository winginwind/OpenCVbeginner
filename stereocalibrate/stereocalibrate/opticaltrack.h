#ifndef _OPTICALTRACK_H_
#define _OPTICALTRACK_H_
#include "cv.h"
using namespace cv;

class OpticalTrack{
private:
	bool firstFrame;
	Mat gray, prevGray;
	vector<Point2f> points[2]; //ÌØÕ÷µã
public:
	OpticalTrack();
	~OpticalTrack();
	void SingleLKTrack(Mat &mgA, Mat &imgB, Mat &imgC);
	void InitLKTrack(void);
	void CtnLKTrack(Mat &frame);
};

#endif
