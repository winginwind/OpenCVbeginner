#include "stereocalib.h"
#include "picturegrub.h"
#define SAVE_PIC 0
#define STEREO_CALIB 1
int main(void)
{
	StereoCalibrate StCab;
	PictureGrub PicGrub;
	//�������ڱ궨����Ƭ
	if(SAVE_PIC){
		PicGrub.SaveCaliPicture();
	}
	//���б궨����ʾУ�����
	if (STEREO_CALIB){
		StCab.StereoCalibrateRectify();
	}
	return 0;
}