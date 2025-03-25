/**
 * calibrationHtmlData.h
 * MALT Web interface to NV_Params.CalDataNV
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.0
 * @date 2015-01-13
 */

#ifndef CALIBRATIONHTMLDATA_H_
#define CALIBRATIONHTMLDATA_H_

#include "MLT_globalTypes.h"

class CalibrationHtmlData {
private:
private:
	CalibrationData calib;

public:
	CalibrationHtmlData();
	virtual ~CalibrationHtmlData();

	CalibrationData getCalib() const;
	void setCalib(CalibrationData calib);

	int getCountMax() const;
	void setCountMax(int countMax);

	int getCountMin() const;
	void setCountMin(int countMin);

	float getGradient() const;
	void setGradient(float gradient);

	float getOffset() const;
	void setOffset(float offset);

	int getValueMax() const;
	void setValueMax(int valueMax);

	int getValueMin() const;
	void setValueMin(int valueMin);

	void resetCalibrationData();

	void printCalibrationData() const;

};



#endif /* CALIBRATIONHTMLDATA_H_ */
