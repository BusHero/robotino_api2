#ifndef _INTEGERSETTINGSITEM_H_
#define _INTEGERSETTINGSITEM_H_

#include "SettingsItem.h"

class IntegerSettingsItem: public SettingsItem
{
public:
	explicit IntegerSettingsItem();
	QString getXMLElement() const;

	const float getValue() const {return value.toInt();}
	void setValue(float value) {this->value = QString("%1").arg(value);}
};

#endif //_INTEGERSETTINGSITEM_H_
