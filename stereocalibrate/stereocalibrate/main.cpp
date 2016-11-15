#include "stereocalib.h"
#include "picturegrub.h"
#include "stereomatch.h"
#include "pointcloud.h"
#include "segmentation.h"
#include "opticaltrack.h"
#include "handdetect.h"
#include "features.h"

using namespace std;

#define SAVE_PIC        0             //����ͼƬ���ڱ궨
#define STEREO_CALIB    0             //��������궨
#define STEREO_RECTIFY  0             //��������У��
#define STEREO_BMMATCH  0             //���п�����ƥ��
#define POINTCLOUD      0             //��õ�������
#define FRAMEDIFF       0             //ʹ��֡�
#define BACKGROUNGDIFF  0             //ʹ�ñ�����ַ�
#define OPTICALFLOW     0             //ʹ�ù�����
#define HANDDETECT      0            //����ʶ��
#define SUFT            1
#define DISPLAY         1             //��ȡ����ͷ����ʾ
#define SINGLE_FRAME                  //���򵥲���



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

#endif



int main(void)
{
	Segmentation Segmtn(20,16,false);
	OpticalTrack OptFlowTrack;
	HandDetector HandLeft;
	HandDetector HandRight;
	Features Featurepro;
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

        if (FRAMEDIFF){
			Segmtn.InitFrameDiff();
			cout << "֡�" << endl;
			cvNamedWindow("diffframe");
			cvMoveWindow("diffframe", 60, 60);
		}
		
		if (BACKGROUNGDIFF){
			Segmtn.InitBkgSub();
			namedWindow("foreback");
			moveWindow("foreback", 60, 60);
		}

		if (OPTICALFLOW){
			namedWindow("flow");
			moveWindow("flow", 60, 60);
		}

		if (HANDDETECT){
			namedWindow("handleft");
			moveWindow("handleft", 60, 60);
			namedWindow("handright");
			moveWindow("handright", 100, 60);
		}

		if (SUFT)
		{
			namedWindow("suft");
			moveWindow("suft", 60, 60);
		}
		while (1)
		{		
			left = cvQueryFrame(capture1);
			right = cvQueryFrame(capture2);	
			Mat matLeft = Mat(left);
			Mat matright = Mat(right);
			//cvShowImage("left", left);
			//cvShowImage("right", right);
			//���ڱ��浥��ͼƬ
			//PicGrub.SaveSingeFrame(left,right);
			if (FRAMEDIFF){
				Mat diffframe = matLeft.clone();
				Segmtn.CalDiffFrame(diffframe);
				if(!Segmtn.diffFrame.empty()){
					imshow("diffframe", Segmtn.diffFrame);
				}
			}

			if (BACKGROUNGDIFF){				
				Mat forebackframe = matLeft.clone();
				Segmtn.BkgSuntract(forebackframe);
				//imshow("foreback",forebackframe);
				if (!Segmtn.FrFrameb.empty())
				{
					imshow("foreground", Segmtn.FrFrameb);
				}			
			}

			if (OPTICALFLOW){
				Mat optframe = matLeft.clone();	
				OptFlowTrack.CtnLKTrack(optframe, Segmtn.FrFrameb); //ʹ��ǰ����Ϊmask
				imshow("flow", optframe);
			}

			if (HANDDETECT){
				Mat handLeft = matLeft.clone();
				Mat handRight = matright.clone();
				HandLeft.HandDetect(handLeft);
				HandRight.HandDetect(handRight);
				imshow("handleft", handLeft);
				imshow("handright", handRight);
			}

			if (SUFT)
			{
				Mat frameleft = matLeft.clone();
				Mat frameright = matright.clone();
				Featurepro.suftDetectMatch(frameleft, frameright, HandLeft.handMask, HandRight.handMask);
				imshow("suft", Featurepro.imgMatch);
			}

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