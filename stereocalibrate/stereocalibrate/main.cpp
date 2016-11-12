#include "stereocalib.h"
#include "picturegrub.h"
#include "stereomatch.h"
#include "pointcloud.h"
#include "segmentation.h"
#include "opticaltrack.h"
#include "nonfree/features2d.hpp"
using namespace std;

#define SAVE_PIC 0
#define STEREO_CALIB 0
#define STEREO_RECTIFY 0
#define STEREO_BMMATCH 0
#define POINTCLOUD 0   //��õ�������
//#define FRAMEDIFF    //ʹ��֡�
//#define BACKGROUNGDIFF //ʹ�ñ�����ַ�
//#define OPTICALFLOW   //ʹ�ù�����
#define DISPLAY 0
#define SINGLE_FRAME  //���򵥲���



void mouseHandler(int event, int x, int y, int flags, void* param);	//handling mouse events
StereoCalibrate StCab;
PictureGrub PicGrub;
StereMatch Stmatch;
PointCloud PointCld;

Mat gdisp;
Mat gvdisp;

#ifdef SINGLE_FRAME
	Mat leftg = imread("stereoData\\left.jpg",-1);
	Mat rightg = imread("stereoData\\right.jpg",-1);
	//Mat leftg = imread("stereoData\\tsukuba_l.png",0);
	//Mat rightg = imread("stereoData\\tsukuba_r.png",0);
	//Mat imgA = imread("stereoData\\OpticalFlow0.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	//Mat imgB = imread("stereoData\\OpticalFlow1.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	//Mat imgC = imread("stereoData\\OpticalFlow1.jpg",CV_LOAD_IMAGE_UNCHANGED);

#endif



int main(void)
{
	Segmentation Segmtn(20,16,false);
	OpticalTrack OptFlowTrack;
	//�������ڱ궨����Ƭ
	if(SAVE_PIC){
		PicGrub.SaveCaliPicture();
	}
	//���б궨
	if (STEREO_CALIB){
		StCab.StereoCalib("cali_list.txt", 9, 6, 0);
	}
	
	StCab.LoadCameraPara();

#ifdef SINGLE_FRAME
	/*
	Stmatch.InitBMMatch();
	cvNamedWindow("disparity",0);
	cvSetMouseCallback("disparity", mouseHandler, NULL);
	Stmatch.bm(leftg, rightg, gdisp);
	normalize(gdisp, gvdisp, 0, 255, NORM_MINMAX, CV_8UC1);
	imshow("disparity", gvdisp);	
	//�õ�3D����
	PointCld.getPointClouds(gdisp,StCab.projectq);
	PointCld.savePointClouds();
    //�������
	PointCld.detectObject();

	OptFlowTrack.SingleLKTrack(imgA,imgB,imgC);
	imshow("cornerA", imgA);
	imshow("cornerB", imgB);
	imshow("cornerline", imgC);
	*/
	vector<KeyPoint> keypoints[2];
	SurfFeatureDetector detector(400);
	//���suft������
	detector.detect(leftg, keypoints[0]);
	detector.detect(rightg, keypoints[1]);
	//����suft������
	SurfDescriptorExtractor descriptor;
	Mat descripfeature[2];
	descriptor.compute(leftg, keypoints[0], descripfeature[0]);
	descriptor.compute(rightg, keypoints[1], descripfeature[1]);
	//����ƥ���
	BruteForceMatcher<L2<float>> matcher;
	vector<DMatch> matchpoint;
	matcher.match(descripfeature[0], descripfeature[1], matchpoint);
	//ȡ�����С��25����
	nth_element(matchpoint.begin(), matchpoint.begin()+24, matchpoint.end());
	matchpoint.erase(matchpoint.begin()+25 , matchpoint.end());
	//����ƥ��ͼ
	Mat imgMatch;
	drawMatches(leftg, keypoints[0], rightg, keypoints[1], matchpoint, imgMatch, Scalar(255, 0, 0));

	imshow("suft", imgMatch);
	if(waitKey(0) == 27) return 0;
#endif


	//��ʾ���
	if (DISPLAY)
	{
		//������ͷ
		CvCapture*capture1 = cvCreateCameraCapture(0); 
		CvCapture*capture2 = cvCreateCameraCapture(1);

		IplImage*left = cvQueryFrame(capture1);
		IplImage*right = cvQueryFrame(capture2);
		//��֤��ʼ���Ի��ͼ��
		while (!left || !right)
		{
			left = cvQueryFrame(capture1);
			right = cvQueryFrame(capture2);
			cout << "�������»��ͼ��" << endl;
		}
		cout << "ͼ���ȡ�ɹ�" << endl;
		CvSize imgSize = cvGetSize(left);
		CvMat* RefPair;
		RefPair = cvCreateMat(imgSize.height, imgSize.width * 2, CV_8UC3);
		CvMat part;		
		//����У��
		if (STEREO_RECTIFY)
		{
			StCab.InitStereoRectify();
			cout << "����У��" << endl;
			cvNamedWindow("RefPair", 1);
			cvMoveWindow("RefPair", 60, 60);
		}
		//����BM�㷨�����Ӳ�
		if (STEREO_BMMATCH)
		{
			Stmatch.InitBMMatch();
			cout << "BM�㷨�����Ӳ�" << endl;
			cvNamedWindow("disparity");
			cvMoveWindow("disparity", 60, 60);
			//cvSetMouseCallback("disparity", mouseHandler, NULL);			
		}

#ifdef FRAMEDIFF
		Segmtn.InitFrameDiff();
		cout << "֡�" << endl;
		cvNamedWindow("diffframe");
		cvMoveWindow("diffframe", 60, 60);
#endif

#ifdef BACKGROUNGDIFF
		Segmtn.InitBkgSub();
		namedWindow("foreground");
		moveWindow("foreground", 60, 60);
#endif

#ifdef OPTICALFLOW
		namedWindow("flow");
		moveWindow("flow", 60, 60);
#endif
		while (1)
		{		
			left = cvQueryFrame(capture1);
			right = cvQueryFrame(capture2);			
			//cvShowImage("left", left);
			//cvShowImage("right", right);
			//���ڱ��浥��ͼƬ
			//PicGrub.SaveSingeFrame(left,right);

#ifdef FRAMEDIFF
			Mat matleft = Mat(left);
			Segmtn.CalDiffFrame(matleft);
			if(!Segmtn.diffFrame.empty()){
				imshow("diffframe", Segmtn.diffFrame);
			}
#endif

#ifdef BACKGROUNGDIFF
			Mat matLeft = Mat(left);			
			Segmtn.BkgSuntract(matLeft);
			//imshow("test",matLeft);
			if (!Segmtn.FrFrameb.empty())
			{
				imshow("foreground", Segmtn.FrFrameb);
			}			
#endif

#ifdef OPTICALFLOW
			OptFlowTrack.CtnLKTrack(matLeft, Segmtn.FrFrameb); //ʹ��ǰ����Ϊmask
			imshow("flow", matLeft);

#endif

			if (STEREO_RECTIFY){
				StCab.StereoRectify(left, right);
				cvGetCols(RefPair, &part, 0, imgSize.width);  
				cvConvert(left, &part);
				cvGetCols(RefPair, &part, imgSize.width,
					imgSize.width * 2);
				cvConvert(right, &part);

				for (int j = 0; j < imgSize.height; j += 16){
					cvLine(RefPair, cvPoint(0, j),
					cvPoint(imgSize.width * 2, j),
					CV_RGB(0, 255, 0));
				}
				cvShowImage("RefPair", RefPair);		
			}
			if (STEREO_BMMATCH)
			{
				clock_t tStart = clock();
				Mat vdisp(imgSize.height, imgSize.width, CV_8U);
				Stmatch.BMMatch(left, right, vdisp);
				clock_t tPerFrame = clock() - tStart;
				//cout << tPerFrame << endl;			
				//imshow("disparity", vdisp);
				Stmatch.vdisp = vdisp;
				imshow("disparity", vdisp);	
			}
			if (POINTCLOUD)
			{
				PointCld.getPointClouds(Stmatch.disp, StCab.projectq);
				//�������
				Mat mleft(left);
				PointCld.detectObject(mleft);	
			}	
			int c = cvWaitKey(10);
			if (c == 'p')
			{
				c = 0;
				while (c != 'p'&&c != 27)
				{
					c = cvWaitKey(250);
				}
			}
			if (c == 'r')
			{
				OptFlowTrack.needInit = true;
			}
			if (c == 27)
			break;
		}
		cvReleaseImage(&left);
		cvReleaseImage(&right);
		cvReleaseMat(&RefPair);
		
	}
	return 0;
}


bool left_mouse = false;
void mouseHandler(int event, int x, int y, int flags, void *param){
	Mat_<short>::iterator it;
	it = Stmatch.disp.begin<short>();
	//Mat_<float>::iterator itrd,itrx,itry;
	//itrd = PointCld.mat_depth.begin<float>();
	//itrx = PointCld.mat_phyx.begin<float>();
	//itry = PointCld.mat_phyy.begin<float>();
	//int pcols =  PointCld.mat_depth.cols;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		cout << "pixel_x:" << x<< "pixel_y:" << y << endl;
		double dis;
		dis = (StCab.baseline * StCab.focal) /  (double)(*(it + y * Stmatch.disp.cols + x)) * 16.0;
		printf("distance of this object is: %lf \n", dis);
		double phyx,phyy,phyz;
		//cout << "x:" << *(itrx + y*pcols + x) << " ";
		//cout << "y:" << *(itry + y*pcols + x) << " ";
		//cout << "y:" << *(itrd + y*pcols + x) << endl;
		left_mouse = true;
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		left_mouse = false;
	}
	else if ((event == CV_EVENT_MOUSEMOVE) && (left_mouse == true))
	{
	}
}