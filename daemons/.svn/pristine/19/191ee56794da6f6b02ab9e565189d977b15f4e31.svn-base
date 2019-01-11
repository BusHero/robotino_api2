#ifndef _FLOATSETTINGSITEM_H_
#define _FLOATSETTINGSITEM_H_

#include "SettingsItem.h"

class FloatSettingsItem: public SettingsItem
{
public:
	explicit FloatSettingsItem();
	QString getXMLElement() const;

	const float getValue() const {return value.toFloat();}
	void setValue(float value) {this->value = QString("%1").arg(value);}

	float getMax() const {return max;}
	void setMax(float max) {this->max = max;}

	float getMin() const {return min; }
	void setMin(float min) {this->min = min;}

	float getPrecision() const {return precision;}
	void setPrecision(float precision) {this->precision = precision;}

private:
	float min;
	float max;
	float precision;
};

#endif //_FLOATSETTINGSITEM_H_
