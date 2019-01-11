#ifndef _CONFIGREADER_H_
#define _CONFIGREADER_H_

#include <QtCore>

class ConfigItem
{
public:
	typedef enum {
		ItemType_text,
		ItemType_ip,
		ItemType_float,
		ItemType_int,
		ItemType_selection,
		ItemType_bool
	} ConfigItemType;

	ConfigItem()
		: type( ItemType_text )
	{
	}

	void clear()
	{
		type = ItemType_text;
		category.clear();
		name.clear();
		value.clear();
		parameters.clear();
	}

	bool isEmpty() const
	{
		return name.isEmpty();
	}

	ConfigItemType type;
	QString category;
	QString group;
	QString name;
	QVariant value;
	QVariantList parameters;
};

class ConfigRestartItem
{
public:
	ConfigRestartItem()
	{
	}

	bool isEmpty() const
	{
		return ( script.isEmpty() );
	}

	void clear()
	{
		script.clear();
		name.clear();
	}

	QString script;
	QString name;
};

class ConfigReader
{
public:
	ConfigReader();

	bool read( const QString& configFileName );

	QString value(const QString key, const QString defaultValue = QString() ) const;
	
	void setValue(const QString key, const QString value);
	
	const QMap<QString,QString>& items() const;

	const QList<ConfigItem>& webItems() const;

	ConfigRestartItem restartItem() const { return _restartItem; }

private:
	QMap<QString, QString> _items;
	QList<ConfigItem> _webItems;
	ConfigRestartItem _restartItem;
};

#endif
