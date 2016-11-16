#include "features.h"


Features::Features(){

}

Features::~Features(){

}

void Features::suftDetectMatch(Mat &frame1, Mat &frame2, Mat mask1, Mat mask2){
	//检测suft特征点
	SurfFeatureDetector detector(1000);
	vector<KeyPoint> keypoints[2];	
	detector.detect(frame1, keypoints[0], mask1);
	detector.detect(frame2, keypoints[1], mask2);

	//描述suft特征点
	if (keypoints[0].size() == 0 || keypoints[1].size() == 0){
		return;
	}
	SurfDescriptorExtractor descriptor;
	Mat descripfeature[2];	
	descriptor.compute(frame1, keypoints[0], descripfeature[0]);
	descriptor.compute(frame2, keypoints[1], descripfeature[1]);
	//计算匹配点
	//BruteForceMatcher<L2<float>> matcher;
	FlannBasedMatcher matcher;
	vector< vector<DMatch> > matchpoint;
	vector<DMatch> goodMatchs;
	//matcher.match(descripfeature[0], descripfeature[1], matchpoint);
	matcher.knnMatch(descripfeature[0], descripfeature[1], matchpoint, 2);
	//法二
	for(int i = 0; i < (int) matchpoint.size(); i++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
	{
		if((matchpoint[i][0].distance < 0.2*(matchpoint[i][1].distance))
			&& ((int) matchpoint[i].size()<=2 && (int) matchpoint[i].size()>0))
		{
			goodMatchs.push_back(matchpoint[i][0]);
		}
	}
	//找到好的匹配点法一
	/*
	double mindis = 100;
	for (int i = 0; i < matchpoint.size(); i++)
	{
		if (matchpoint[i].distance < mindis)
		{
			mindis = matchpoint[i].distance;
		}
	}
	for (int i = 0; i < matchpoint.size(); i++)
	{
		if (matchpoint[i].distance < 2 * mindis)
		{
			goodMatchs.push_back(matchpoint[i]);
		}
	}
	*/
	//画出匹配图
	drawMatches(frame1, keypoints[0], frame2, keypoints[1], goodMatchs, imgMatch, Scalar(255, 0, 0));

}