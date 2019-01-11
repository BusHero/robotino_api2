#ifndef _SETTINGSITEMSXMLGEN_H_
#define _SETTINGSITEMSXMLGEN_H_

#include "SettingsItem.h"

/**
 * For generating XML of settingItems.
 */
class SettingsItemsXMLGen : public QObject
{
	Q_OBJECT
public:
	explicit SettingsItemsXMLGen(QObject *parent = 0);

	void appendItem( SettingsItem* item );

	QString getSettingsItemsXML();

	void clear();

private:
	SettingsItemPointerList _items;
};

#endif //_SETTINGSITEMSXMLGEN_H_
