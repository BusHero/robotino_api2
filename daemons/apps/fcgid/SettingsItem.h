#ifndef _SETTINGSITEM_H_
#define _SETTINGSITEM_H_

#include <QObject>
#include <QStringList>
#include <QXmlStreamWriter>

/**
 * Abstract base class for setting Items.
 */
class SettingsItem
{
public:
	explicit SettingsItem();
	virtual ~SettingsItem(){};

	virtual QString getXMLElement() const = 0;

	const QString& getCategory() const { return category;}
	void setCategory(const QString& category) {this->category = category;}

	int getItemId() const {return itemID;}
	void setItemId(int itemId) {itemID = itemId;}

	const QString& getItemName() const {return itemName;}
	void setItemName(const QString& itemName) {this->itemName = itemName;}

	const QString& getType() const {return type;}
	void setType(const QString& type) {this->type = type;}

	void writeXMLElement(QXmlStreamWriter& writer, QString tag, QString value) const;

protected:
	int     itemID;
	QString itemName;
	QString type;
	QString category;
	QString value;
};

typedef QList<SettingsItem*> SettingsItemPointerList;

#endif //_SETTINGSITEM_H_
