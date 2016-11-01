#include "stereocalib.h"
#include "picturegrub.h"
#define SAVE_PIC 0
#define STEREO_CALIB 1
int main(void)
{
	StereoCalibrate StCab;
	PictureGrub PicGrub;
	//保存用于标定的照片
	if(SAVE_PIC){
		PicGrub.SaveCaliPicture();
	}
	//进行标定并显示校正结果
	if (STEREO_CALIB){
		StCab.StereoCalibrateRectify();
	}
	return 0;
}