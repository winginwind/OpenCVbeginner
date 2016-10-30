#ifndef  _STEREOCALI_H_
#define  _STEREOCALI_H_

class StereoCalibrate{
private:
	void StereoCalib(const char* imageList, int nx, int ny, int useUncalibrated);
public:
	StereoCalibrate();
	virtual ~StereoCalibrate();
	int StereoCalibrateRectify(void);
};
//function

#endif
