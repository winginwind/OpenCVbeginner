#include "segmentation.h"
#include "highgui.h"

#define PI 3.141592653f

Segmentation::Segmentation(){

}

Segmentation::~Segmentation(){

}

Segmentation::Segmentation(int history, float varthreshod, bool shadowdetection)
	:bgSubtractor(history,varthreshod,shadowdetection){

}

//初始化帧差函数
void Segmentation::InitFrameDiff(){
	firstFrame = true;
	element = getStructuringElement(MORPH_RECT,Size(3,3));  
}
//进行帧差计算
void Segmentation::CalDiffFrame(Mat frame){
	cvtColor(frame, grayFrame, CV_RGB2GRAY);
	if (true == firstFrame){
		preFrame = grayFrame.clone();
		firstFrame = false;
	}else{
		absdiff(grayFrame, preFrame, diffFrame);
		threshold(diffFrame, diffFrame, 20, 255, CV_THRESH_BINARY_INV);
		preFrame = grayFrame.clone();
		medianBlur(diffFrame,diffFrame,3);     //中值滤波去噪声 
		//dilate(diffFrame,diffFrame,element);  //膨胀
	}
}



void Segmentation::InitBkgSub(){
	firstFrame = true;
}

//背景差分法
void Segmentation::BkgSuntract(Mat &frame){
	cvtColor(frame, grayFrame, CV_RGB2GRAY);
	grayFrame.convertTo(grayFramef, CV_32F);
	
	if (true == firstFrame){
		BkFramef = grayFramef.clone();  //记录背景
		firstFrame = false;
	}else{
		GaussianBlur(grayFramef,grayFramef,Size(5,5),0);  //高斯滤波
		absdiff(grayFramef, BkFramef, FrFramef);  //减去背景得到前景
		threshold(FrFramef, FrFramebInv, 50, 255, CV_THRESH_BINARY_INV); 
		threshold(FrFramef, FrFrameb, 50, 255, CV_THRESH_BINARY); 
		FrFrameb.convertTo(FrFrameb,CV_8UC1);
		erode(FrFrameb, FrFrameb, getStructuringElement(MORPH_RECT,Size(5,5)));   //腐蚀
		dilate(FrFrameb, FrFrameb, getStructuringElement(MORPH_RECT,Size(5,5)));  //膨胀
		//不要把前景加入背景
		FrFramebInv.convertTo(FrFramebInv,CV_8UC1);
		accumulateWeighted(grayFramef, BkFramef, 0.01 , FrFramebInv);  //更新背景防止光照变化
		//检测其中的圆
		Mat dec;
		dec = FrFrameb.clone();
		DetectCircle(dec, frame);
	}
}

void Segmentation::Gussbkgsub(Mat frame){
	bgSubtractor(frame,FrFrameb,0.001);
}

void Segmentation::DetectCircle(Mat frameb, Mat &framesh){
	vector< vector<Point> > contours;	// 物体轮廓点链
	Point2f center;
	float radius;
	float circleArea;
	findContours(frameb, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area > 8000 || area < 200) continue;
		drawContours(framesh, contours, i, cv::Scalar(0,255,0), 2);
		minEnclosingCircle(Mat(contours[i]), center, radius);
		circleArea = PI * radius * radius;
		if (circleArea / area < 2 && circleArea / area > 0.5)
		{
			circle(framesh, Point(center), static_cast<int>(radius), Scalar(0,0,255), -1);
		}	
	}
}

