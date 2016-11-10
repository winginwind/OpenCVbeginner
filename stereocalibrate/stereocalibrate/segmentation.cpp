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

//��ʼ��֡���
void Segmentation::InitFrameDiff(){
	firstFrame = true;
	element = getStructuringElement(MORPH_RECT,Size(3,3));  
}
//����֡�����
void Segmentation::CalDiffFrame(Mat frame){
	cvtColor(frame, grayFrame, CV_RGB2GRAY);
	if (true == firstFrame){
		preFrame = grayFrame.clone();
		firstFrame = false;
	}else{
		absdiff(grayFrame, preFrame, diffFrame);
		threshold(diffFrame, diffFrame, 20, 255, CV_THRESH_BINARY_INV);
		preFrame = grayFrame.clone();
		medianBlur(diffFrame,diffFrame,3);     //��ֵ�˲�ȥ���� 
		//dilate(diffFrame,diffFrame,element);  //����
	}
}



void Segmentation::InitBkgSub(){
	firstFrame = true;
}

//֡��뱳����ַ��ں�
void Segmentation::BkgSuntract(Mat &frame){
	cvtColor(frame, grayFrame, CV_RGB2GRAY);
	FileStorage fs("grayf.xml", FileStorage::WRITE);  
	fs<<"vocabulary"<<grayFrame;  
	fs.release();  
	grayFrame.convertTo(grayFramef, CV_32F);
	
	if (true == firstFrame){
		BkFramef = grayFramef.clone();  //��¼����
		preFrame = grayFrame.clone();   //��¼��һ֡
		firstFrame = false;
	}else{
		//ʹ��֡��õ�ǰ�������뱳��
		absdiff(grayFrame, preFrame, diffFrame);
		threshold(diffFrame, diffFrame, 20, 255, CV_THRESH_BINARY_INV);
		preFrame = grayFrame.clone();
		medianBlur(diffFrame,diffFrame,3);     //��ֵ�˲�ȥ���� 

		GaussianBlur(grayFramef,grayFramef,Size(5,5),0);  //��˹�˲�
		absdiff(grayFramef, BkFramef, FrFramef);  //��ȥ�����õ�ǰ��
		threshold(FrFramef, FrFrameb, 50, 255, CV_THRESH_BINARY); 
		//��Ҫ��ǰ�����뱳��
		//accumulateWeighted(grayFramef, BkFramef, 0.003, diffFrame);  //���±�����ֹ���ձ仯
		accumulateWeighted(grayFramef, BkFramef, 0.003);  //���±�����ֹ���ձ仯
		//������е�Բ
		Mat dec;
		FrFrameb.convertTo(FrFrameb,CV_8UC1);
		dec = FrFrameb.clone();
		DetectCircle(dec, frame);
	}
}

void Segmentation::Gussbkgsub(Mat frame){
	bgSubtractor(frame,FrFrameb,0.001);
}

void Segmentation::DetectCircle(Mat frameb, Mat &framesh){
	vector< vector<Point> > contours;	// ������������
	Point2f center;
	float radius;
	float circleArea;
	findContours(frameb, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area > 800 || area < 10) continue;
		drawContours(framesh, contours, i, cv::Scalar(0,255,0), 2);
		minEnclosingCircle(Mat(contours[i]), center, radius);
		circleArea = PI * radius * radius;
		if (circleArea / area < 2 && circleArea / area > 0.5)
		{
			circle(framesh, Point(center), static_cast<int>(radius), Scalar(255,0,0), 3);
		}	
	}
}

