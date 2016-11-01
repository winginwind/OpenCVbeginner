#ifndef  _PICTUREGRUB_H_
#define  _PICTURNGRUB_H_
#include "opencv.hpp"
#include "highgui/highgui.hpp"

class PictureGrub{
public:
	PictureGrub();
	~PictureGrub();
	int SaveCaliPicture(void);
};


#endif