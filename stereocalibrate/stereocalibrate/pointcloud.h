#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_
#include "cv.h"
#include "highgui.h"
using namespace cv;

class PointCloud{
public:
	Mat pointClouds;
	Mat mat_depth;
	Mat mat_phyx;
	Mat mat_phyy;
	PointCloud();
	~PointCloud();
	void getPointClouds(Mat disp, Mat calibq);
	void savePointClouds(void);
	void detectObject(Mat &image);
};

#endif