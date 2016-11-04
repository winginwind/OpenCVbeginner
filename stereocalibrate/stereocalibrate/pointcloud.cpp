#include "pointcloud.h"
#include <iostream>
using namespace std;

PointCloud::PointCloud()
{

}

PointCloud::~PointCloud()
{

}

void PointCloud::getPointClouds(Mat disp)
{
	CvMat *cm_CalibQ;
	cm_CalibQ = (CvMat*)cvLoad("Q.xml");
	Mat CalibQ(cm_CalibQ);
	reprojectImageTo3D(disp, pointClouds, CalibQ, true);
	pointClouds *= 16;  //×ª³Émm
}


void PointCloud::savePointClouds(void)
{
	FILE* fp = fopen("pointclouds.txt", "wt");
	for(int y = 0; y < pointClouds.rows; y++)
	{
		for(int x = 0; x < pointClouds.cols; x++)
		{
			Vec3f point = pointClouds.at<Vec3f>(y, x);
			fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
		}
	}
	fclose(fp);
}

void PointCloud::detectObject(void)
{
	vector<Mat> xyzSet;
	split(pointClouds, xyzSet);
	Mat depth;
	double maxVal = 0, minVal = 0;
	xyzSet[2].copyTo(depth);

	
	Mat depthThresh = Mat::zeros(depth.rows, depth.cols, CV_8UC1);
	minMaxLoc(depth, &minVal, &maxVal);
	double thrVal = minVal * 1.5;
	threshold(depth, depthThresh, thrVal, 255, CV_THRESH_BINARY_INV);
 
	Mat element = getStructuringElement(MORPH_RECT, Size(7,7));

	dilate(depthThresh, depthThresh, element,Point(-1,-1),3);	// Í¼ÏñÅòÕÍ
	erode(depthThresh, depthThresh, element,Point(-1,-1),3);	// Í¼Ïñ¸¯Ê´

	vector<vector<cv::Point> > contours;	// ÎïÌåÂÖÀªµãÁ´
	findContours(depthThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//·ÖÎöÂÖÀª
	Mat mask = cv::Mat::zeros(depth.size(), CV_8UC1);
	for (int objid = 0; objid < contours.size(); objid++)
	{
		Mat contour(contours[objid]);
		double area = contourArea( contour );
		if (area > 100)
		{
			drawContours(mask, contours, objid, cv::Scalar(255), -1);
			double minVal = 0, maxVal = 0;
			Point minPos,maxPos;
			minMaxLoc(depth, &minVal, &maxVal, &minPos, &maxPos, mask);
			imshow("mask", mask);
		}
	}

}
