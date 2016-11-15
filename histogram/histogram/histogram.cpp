
#include "cv.h"
#include "highgui.h"
#include "imgproc/imgproc.hpp"

using namespace cv;

int main() {

    //IplImage* src;
	//src = cvLoadImage("lena.jpg", 1);
	Mat src;
	src = imread("lena.jpg", 1);
    // Compute the HSV image, and decompose it into separate planes.
    //
    //IplImage* hsv = cvCreateImage( cvGetSize(src), 8, 3 ); 
    //cvCvtColor( src, hsv, CV_BGR2HSV );
	Mat hsv;
	cvtColor(src, hsv, CV_BGR2HSV);

	vector<Mat> hsvset;
	split(hsv, hsvset);


    // Build the histogram and compute its contents.

	Mat hist;  
    int h_bins = 30;
	int hist_size[] = {h_bins};
	int channels[] = {0};    
	float h_range[] = { 0, 180};
    const float *ranges[] = {h_range};

	calcHist(&hsvset[0], 1, channels, Mat(), hist, 1, hist_size, ranges, true, false );  

    // Create an image to use to visualize our histogram.

    int scale = 20;
	int hist_height = 256;
	Mat hist_img = Mat::zeros(hist_height, h_bins * scale, CV_8UC1);
    // populate our visualization with little gray squares.
    double max_value = 0;
    minMaxLoc( hist, 0, &max_value, 0, 0 );

    for( int h = 0; h < h_bins; h++ ) {
		float bin_val = hist.at<float>(h);
		int intensity = cvRound( bin_val * 255 / max_value );
		rectangle( 
			hist_img, 
			Point( h*scale, hist_height-1 ),
			Point( (h+1)*scale - 1, hist_height - intensity),
			CV_RGB(255,255,255),
			CV_FILLED
			);
    }

    namedWindow( "Source", 1 );
    imshow("Source", src );

    namedWindow( "H Histogram", 1 );
    imshow("H Histogram", hist_img );

    cvWaitKey(0);

}

