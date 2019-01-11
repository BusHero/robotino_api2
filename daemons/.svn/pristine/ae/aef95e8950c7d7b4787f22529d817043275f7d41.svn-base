#ifndef _SELECTIONSETTINGSITEM_H_
#define _SELECTIONSETTINGSITEM_H_

#include "SettingsItem.h"

class SelectionSettingsItem: public SettingsItem
{
public:
	explicit SelectionSettingsItem();
	QString getXMLElement() const;

	void setOptions(const QStringList& options){this->options = options;}

	const QString& getValue() const {return value;}
	void setValue(const QString& value) {this->value = value;}
private:
	QStringList options;
};

#endif //_SELECTIONSETTINGSITEM_H_
