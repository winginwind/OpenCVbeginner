#include "stereocali.h"
#include "savepicture.h"

int main(void)
{
	//保存用于标定的照片
	SaveCaliPicture();
	//进行标定并显示校正结果
	//StereoCalibrateRectify();
	return 0;
}