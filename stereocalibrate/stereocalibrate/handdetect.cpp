#include "handdetect.h"



//·ôÉ«¼ì²â
void HandDetector::HandDetect(Mat &frame){
	vector< vector<Point> >  contours;
	vector< vector<Point> > filterContours;
	medianBlur(frame, frame, 5); //ÖÐÖµÂË²¨
	cvtColor(frame, frameHSV, CV_BGR2HSV);
	inRange(frameHSV, Scalar(0,30,30), Scalar(20,170,256), handMask);  
	Mat element = getStructuringElement(MORPH_RECT, Size(3,3));  
	erode(handMask, handMask, element);  
	morphologyEx(handMask, handMask, MORPH_OPEN, element);  
	dilate(handMask, handMask, element);  
	morphologyEx(handMask, handMask, MORPH_CLOSE, element); 
	findContours(handMask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++){
		if (contourArea(contours[i]) > 8000){
			filterContours.push_back(contours[i]);
		}
	}
	vector<Point> hull;
	if (filterContours.size() == 1){
		drawContours(frame, filterContours, 0, Scalar(255, 0, 0), 3);
		convexHull(Mat(filterContours[0]), hull, true);  
		int hullcount = (int)hull.size();
		int minX = 480;
		int maxX = 0;
		int minY = 640;
		int maxY = 0;
		for (int i = 0; i < hullcount; i++){
			line(frame, hull[i], hull[(i+1) % hullcount], Scalar(255,255,255),3);
			if(hull[i].x > maxX){
				maxX = hull[i].x;
			}
			if(hull[i].x < minX){
				minX = hull[i].x;
			}
			if(hull[i].y > maxY){
				maxY = hull[i].y;
			}
			if(hull[i].y < minY){
				minY = hull[i].y;
			}
		}
		handLoc.x = (minX + maxX) / 2;
		handLoc.y = (minY + maxY) / 2;
	}

}