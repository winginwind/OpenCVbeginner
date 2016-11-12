#include "opticaltrack.h"
#define MAX_CORNERS 200
OpticalTrack::OpticalTrack(){

}

OpticalTrack::~OpticalTrack(){

}

void OpticalTrack::SingleLKTrack(Mat &imgA, Mat &imgB, Mat &imgC){
	//得到跟踪的角点
	;
	vector<Point2f> points[2];
	goodFeaturesToTrack(imgA, points[0], MAX_CORNERS,0.1, 10.0, Mat(), 3, 0, 0.04);
	cornerSubPix(imgA,  points[0], Size(10,10), Size(-1,-1), 
				TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
	//使用LK光流法
	vector<uchar> status;
	vector<float> err;
	calcOpticalFlowPyrLK(imgA, imgB, points[0], points[1], status, err, Size(31,31), 3,
						TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), 0, 0.001);

	for (int i = 0; i < points[0].size(); i++)
	{
		if( status[i] == 0|| err[i]> 50 ){
			continue;
		}

		line( imgC, points[0][i], points[1][i], CV_RGB(255,0,0),2 );
		circle(imgA, points[0][i], 2, Scalar(0), 3);
		circle(imgB, points[1][i], 2, Scalar(0), 3);
	}
}


void OpticalTrack::CtnLKTrack(Mat &frame, Mat mask){
	cvtColor(frame, gray, COLOR_RGB2GRAY);
	if (true == needInit)
	{
		prevGray = gray.clone();
		goodFeaturesToTrack(prevGray, points[0], MAX_CORNERS,0.1, 10.0, mask, 3, 0, 0.04);
		cornerSubPix(prevGray,  points[0], Size(10,10), Size(-1,-1), 
			TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
		needInit = false;
	}else if (!prevGray.empty() && !points[0].empty()){
		vector<uchar> status;
		vector<float> err;
		calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, Size(21,21), 3,
			TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), 0, 0.001);
		int k = 0;
		for( int i = 0; i < points[1].size(); i++ )
		{
			if( !status[i] )
				continue;
			points[0][k++] = points[1][i];   //保存至上一帧
			circle( frame, points[1][i], 3, Scalar(255,0,0), -1); //显示特征点
		}
		points[0].resize(k);
		prevGray = gray.clone();
	}
}