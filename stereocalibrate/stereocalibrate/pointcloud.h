#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_
#include "cv.h"
#include "highgui.h"
using namespace cv;

class PointCloud{
public:
	Mat pointClouds;

	PointCloud();
	~PointCloud();
	void getPointClouds(Mat disp);
	void savePointClouds(void);
	void detectObject(void);
};

#endif