#include "picturegrub.h"

using namespace std;
using namespace cv;

PictureGrub::PictureGrub(){

}

PictureGrub::~PictureGrub(){

}
int PictureGrub::SaveCaliPicture(void)
{
	VideoCapture cap1(0);
	VideoCapture cap2(1);

	double w = 640, h = 480;
	cap1.set(CV_CAP_PROP_FRAME_WIDTH,w);
	cap1.set(CV_CAP_PROP_FRAME_HEIGHT,h);
	cap2.set(CV_CAP_PROP_FRAME_WIDTH,w);
	cap2.set(CV_CAP_PROP_FRAME_HEIGHT,h);

	cout << "img width " << cap1.get(CV_CAP_PROP_FRAME_WIDTH) << " ";
	cout << "img height " << cap1.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
	if(!cap1.isOpened()) {cout<<"-1"<<endl;return -1;}
	if(!cap2.isOpened()) {cout<<"-2"<<endl;return -2;}
	namedWindow("L_image",1);
	namedWindow("R_image",1);
	//显示角点
	namedWindow("Corner_L",0);
	namedWindow("Corner_R",0);
	Mat frameL,frameR;
	Mat fdcrimgL,fdcrimgR;
	Mat dwcrimgL,dwcrimgR;
	int iresL,iresR;

	int nx = 9;
	int ny = 6;
	int cornum = nx * ny;
	//Mat cornersL, cornersR;
	vector<Point2f> cornersL,cornersR;
	int framenum = 1;
	while (1)
	{
		cap1>>frameL;
		cap2>>frameR;
		if(!frameL.empty() && !frameR.empty()){

			cvtColor(frameL,fdcrimgL,CV_BGR2GRAY);
			cvtColor(frameR,fdcrimgR,CV_BGR2GRAY);
			if(waitKey(100)== 's'){
				iresL = findChessboardCorners(fdcrimgL, cvSize(nx, ny),
					cornersL ,CV_CALIB_CB_ADAPTIVE_THRESH |CV_CALIB_CB_NORMALIZE_IMAGE);  //归一化图像后进行自适应二值化
				iresR = findChessboardCorners(fdcrimgR, cvSize(nx, ny),
					cornersR ,CV_CALIB_CB_ADAPTIVE_THRESH |CV_CALIB_CB_NORMALIZE_IMAGE);  //归一化图像后进行自适应二值化
				if (iresL == 1 && iresR == 1)
				{
					dwcrimgL = frameL.clone();
					dwcrimgR = frameR.clone();
					drawChessboardCorners(dwcrimgL, cvSize(nx, ny), cornersL, iresL);
					drawChessboardCorners(dwcrimgR, cvSize(nx, ny), cornersR, iresR);
					imshow("Corner_L", dwcrimgL);
					imshow("Corner_R", dwcrimgR);
					//保存图片
					char imgname[64];
					sprintf_s(imgname,"stereoData\\L%02d.jpg",framenum);
					imwrite(imgname, frameL);
					sprintf_s(imgname,"stereoData\\R%02d.jpg",framenum);
					imwrite(imgname, frameR);
					framenum++;
				}

			}
			imshow("L_image",frameL);
			imshow("R_image",frameR);
		}			
		if(framenum > 20){
			break;
		}
		int c = waitKey(15);
		if (c == 'p')
		{
			c = 0;
			while (c != 'p'&&c != 27)
			{
				c = waitKey(250);
			}
		}
		if (c == 27)
			break;

	}	
	return 0;
}

void PictureGrub::SaveSingeFrame(IplImage* pframe1, IplImage* pframe2){
	if (cvWaitKey(10) == 's')
	{
		cvSaveImage("stereoData\\left.jpg",pframe1);
		cvSaveImage("stereoData\\right.jpg",pframe2);
	}
}
