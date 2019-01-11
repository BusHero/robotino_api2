#ifndef _BOOLEANSETTINGSITEM_H_
#define _BOOLEANSETTINGSITEM_H_

#include "SettingsItem.h"

class BooleanSettingsItem: public SettingsItem
{
public:
	explicit BooleanSettingsItem();
	QString getXMLElement() const;

	const float getValue() const {return value == "true";}
	void setValue(bool value) {this->value = value?"true":"false";}
};

#endif //_BOOLEANSETTINGSITEM_H_
