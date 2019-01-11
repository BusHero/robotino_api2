#ifndef CONFIGPAGEBUILDER_H_
#define CONFIGPAGEBUILDER_H_

#include <QtCore>
#include "ConfigReader.h"

class SettingCategory
{
public:
	SettingCategory()
	{
	}

	SettingCategory( const QString& filepath_ )
		: filepath( filepath_ )
	{
	}

	SettingCategory( const QString& filepath_, const ConfigRestartItem& restartItem_ )
		: filepath( filepath_ )
		, restartItem( restartItem_ )
	{
	}

	QString filepath;
	ConfigRestartItem restartItem;
};

typedef QList<SettingCategory> SettingCategoryList;

/**
 * Real all .ini files from specified dirs
 */
class ConfigPageBuilder
{
public:
	ConfigPageBuilder();
	virtual ~ConfigPageBuilder();

	QString getCategoryXML() const { return _categoryXML; }
	QString getSettingItemXML() const { return _settingItemXML; }
	ConfigRestartItem getConfigRestartItem( const QString& filepath ) const;

	void setINI_DIR( const QStringList& dirs );

	/**
	 * Parse all Ini files in the specified DIRs
	 */
	void rebuild();


private:
	SettingCategoryList _categories;

	QList<ConfigItem> _settingItems;

	QStringList _dirs;

	QString _categoryXML;
	QString _settingItemXML;

	QMap<QString, ConfigRestartItem> _restartItems;
};

#endif /* CONFIGPAGEBUILDER_H_ */
