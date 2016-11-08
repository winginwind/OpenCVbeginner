#include "pointcloud.h"
#include <iostream>
using namespace std;

PointCloud::PointCloud()
{

}

PointCloud::~PointCloud()
{

}

void PointCloud::getPointClouds(Mat disp, Mat calibq)
{
	reprojectImageTo3D(disp, pointClouds, calibq, true);
	pointClouds *= 16;  //转成mm
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

void PointCloud::detectObject(Mat &image)
{
	vector<Mat> xyzSet;
	split(pointClouds, xyzSet);

	double maxVal = 0, minVal = 0;
	xyzSet[2].copyTo(mat_depth);
	xyzSet[1].copyTo(mat_phyy);
	xyzSet[0].copyTo(mat_phyx);
	
	Mat depthThresh;
	//minMaxLoc(mat_depth, &minVal, &maxVal);
	//double thrVal = minVal * 1.5;
	threshold(mat_depth, depthThresh, 1500, 255, CV_THRESH_BINARY_INV);//1.5m之内
	//threshold(mat_depth, mat_depth, thrVal, 0,CV_THRESH_TRUNC );
	Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
	dilate(depthThresh, depthThresh, element,Point(-1,-1),3);	// 图像膨胀
	erode(depthThresh, depthThresh, element,Point(-1,-1),3);	// 图像腐蚀
	depthThresh.convertTo(depthThresh, CV_8UC1);
	namedWindow("depthThresh",0);
	imshow("depthThresh",depthThresh);
	vector< vector<Point> > contours;	// 物体轮廓点链
	findContours(depthThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//分析轮廓
	Mat mask = cv::Mat::zeros(mat_depth.size(), CV_8UC1);
	for (int objid = 0; objid < contours.size(); objid++)
	{
		Mat contour(contours[objid]);
		double area = contourArea( contour );
		if (area > 2000)
		{
			double phyx, phyy, depth;
			drawContours(mask, contours, objid, cv::Scalar(255), -1);
			//imshow("mask",mask);
			double minVal = 0, maxVal = 0;
			Point minPos,maxPos;
			minMaxLoc(mat_depth, &minVal, &maxVal, &minPos, &maxPos,mask);
			phyx = mat_phyx.at<float>(minPos.y, minPos.x);
			phyy = mat_phyy.at<float>(minPos.y, minPos.x);
			depth = mat_depth.at<float>(minPos.y, minPos.x);
			//phyx = mat_phyx.at<float>(maxPos.y, maxPos.x);
			//phyy = mat_phyy.at<float>(maxPos.y, maxPos.x);
			//depth = mat_depth.at<float>(maxPos.y, maxPos.x);
			//cout << objid  << " " << phyx << " " << phyy << " " << depth << endl;
			//画出物体轮廓
			RotatedRect minRect;
			minRect = minAreaRect( contour );
			Point2f rect_points[4]; 
			minRect.points(rect_points);
			for (int i = 0; i < 4; i++)
			{
				line( image, rect_points[i], rect_points[(i+1)%4], CV_RGB(255,0,0), 4 );
			}
		}
	}

}
