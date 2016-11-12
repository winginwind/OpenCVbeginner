#ifndef _OPTICALTRACK_H_
#define _OPTICALTRACK_H_
#include "cv.h"
using namespace cv;

class OpticalTrack{
private:

	Mat gray, prevGray;
	vector<Point2f> points[2]; //ÌØÕ÷µã
public:
	bool needInit;
	OpticalTrack();
	~OpticalTrack();
	void SingleLKTrack(Mat &mgA, Mat &imgB, Mat &imgC);
	void CtnLKTrack(Mat &frame, Mat mask);
};

#endif
