#include "features.h"


Features::Features():detector(400){

}

Features::~Features(){

}

void Features::suftDetectMatch(Mat &frame1, Mat &frame2, Mat mask1, Mat mask2){
	//���suft������
	detector.detect(frame1, keypoints[0], mask1);
	detector.detect(frame2, keypoints[1], mask2);
	//����suft������
	if (keypoints[0].size() == 0 || keypoints[1].size() == 0){
		return;
	}
	descriptor.compute(frame1, keypoints[0], descripfeature[0]);
	descriptor.compute(frame2, keypoints[1], descripfeature[1]);
	//����ƥ���
	//BruteForceMatcher<L2<float>> matcher;

	matcher.match(descripfeature[0], descripfeature[1], matchpoint);
	//�ҵ��õ�ƥ���
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
	//������Ӧ��
	vector<Point2f> matchesp[2];
	for (int i = 0; i < goodMatchs.size(); i++)
	{
		matchesp[0].push_back(keypoints[0][goodMatchs[i].queryIdx].pt);
		matchesp[1].push_back(keypoints[1][goodMatchs[i].trainIdx].pt);
	}
	//����ƥ��ͼ
	drawMatches(frame1, keypoints[0], frame2, keypoints[1], goodMatchs, imgMatch, Scalar(255, 0, 0));

}