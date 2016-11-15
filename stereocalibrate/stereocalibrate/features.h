#ifndef _FEATURES_H_
#define _FEATURES_H_
#include "cv.h"
#include "nonfree/features2d.hpp"
using namespace cv;

class Features{
private:
	SurfFeatureDetector detector;
	SurfDescriptorExtractor descriptor;
	FlannBasedMatcher matcher;
public:
	vector<KeyPoint> keypoints[2];	
	Mat descripfeature[2];	
	vector<DMatch> matchpoint;
	vector<DMatch> goodMatchs;
	Mat imgMatch;
	Features();
	~Features();

	void suftDetectMatch(Mat &frame1, Mat &frame2, Mat mask1, Mat mask2);
};


#endif