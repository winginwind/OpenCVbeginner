#include "savepicture.h"

using namespace std;
using namespace cv;

int SaveCaliPicture(void)
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
	Mat frame1,frame2;
	while (1)
	{

		cap1>>frame1;
		cap2>>frame2;
		if(!frame1.empty() && !frame2.empty()){

			imshow("L_image",frame1);
			imshow("R_image",frame2);
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
