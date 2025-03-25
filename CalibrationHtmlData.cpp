/**
 * calibrationHtmlData.cpp
 * MALT Web interface to NV_Params.CalDataNV
 * @author gavin.murray and leszek.zarzycki
 * @version Single_MALT1.1
 * @date 2015-01-13
 */

#include <stdio.h>
#include "CalibrationHtmlData.h"

CalibrationHtmlData::CalibrationHtmlData() {
	// TODO Auto-generated constructor stub
}

CalibrationHtmlData::~CalibrationHtmlData() {
	// TODO Auto-generated destructor stub
}

void CalibrationHtmlData::resetCalibrationData() {
	calib.valueMax=0;
	calib.countMax=0;
	calib.countMin=0;
	calib.valueMin=0;
	calib.gradient=0.0;
	calib.offset = 0.0;
}

CalibrationData CalibrationHtmlData::getCalib() const {
	return calib;
}

void CalibrationHtmlData::setCalib(CalibrationData calib) {
	this->calib = calib;
}

int CalibrationHtmlData::getCountMax() const {
	return calib.countMax;
}

void CalibrationHtmlData::setCountMax(int countMax) {
	calib.countMax = countMax;
}

int CalibrationHtmlData::getCountMin() const {
	return calib.countMin;
}

void CalibrationHtmlData::setCountMin(int countMin) {
	calib.countMin = countMin;
}

float CalibrationHtmlData::getGradient() const {
	return calib.gradient;
}

void CalibrationHtmlData::setGradient(float gradient) {
	calib.gradient = gradient;
}

float CalibrationHtmlData::getOffset() const {
	return calib.offset;
}

void CalibrationHtmlData::setOffset(float offset) {
	calib.offset = offset;
}

int CalibrationHtmlData::getValueMax() const {
	return calib.valueMax;
}

void CalibrationHtmlData::setValueMax(int valueMax) {
	calib.valueMax = valueMax;
}

int CalibrationHtmlData::getValueMin() const {
	return calib.valueMin;
}

void CalibrationHtmlData::setValueMin(int valueMin) {
	calib.valueMin = valueMin;
}

void CalibrationHtmlData::printCalibrationData() const {
	printf("Calibration Data:[%d, %d, %d, %d, , %.3f, %.3f]\r\n",
		calib.valueMax,
		calib.countMax,
		calib.valueMin,
		calib.countMin,
		calib.gradient,
		calib.offset
		);

}
