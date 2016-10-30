#include "stereocalib.h"
#include "savepicture.h"

int main(void)
{
	StereoCalibrate StCab;
	//保存用于标定的照片
	//SaveCaliPicture();
	//进行标定并显示校正结果
	StCab.StereoCalibrateRectify();
	return 0;
}