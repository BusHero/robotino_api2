#ifndef _STRINGSETTINGSITEM_H_
#define _STRINGSETTINGSITEM_H_

#include "SettingsItem.h"

class StringSettingsItem: public SettingsItem
{
public:
	explicit StringSettingsItem();
	QString getXMLElement() const;

	const QString getValue() const {return value;}
	void setValue(QString value) {this->value = value;}
};

#endif //_STRINGSETTINGSITEM_H_
