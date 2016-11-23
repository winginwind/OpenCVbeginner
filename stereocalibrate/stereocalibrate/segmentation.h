#ifndef _SEGMENTATION_H_
#define _SEGMENTATION_H_
#include "cv.h"
#include "video/background_segm.hpp"
using namespace cv;
class Segmentation{
public:
//vaviable
	Mat preFrame;
	Mat grayFrame;
	Mat diffFrame;
	bool firstFrame;
	Mat element;

	Mat grayFramef;
	Mat FrFramef;
	Mat FrFramebInv;
	Mat FrFrameb;
	Mat BkFramef;

	BackgroundSubtractorMOG2 bgSubtractor;
//function
	Segmentation();
	Segmentation(int history, float varthreshod, bool shadowdetection);
	~Segmentation();
	void InitFrameDiff(void);
	void CalDiffFrame(Mat frame);

	void InitBkgSub(void);
	void BkgSuntract(Mat &frame);
	void Gussbkgsub(Mat frame);
	void DetectCircle(Mat frameb,Mat &framesh);
};



#endif