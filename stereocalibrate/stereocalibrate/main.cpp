#include "stereocalib.h"
#include "savepicture.h"

int main(void)
{
	StereoCalibrate StCab;
	//�������ڱ궨����Ƭ
	//SaveCaliPicture();
	//���б궨����ʾУ�����
	StCab.StereoCalibrateRectify();
	return 0;
}