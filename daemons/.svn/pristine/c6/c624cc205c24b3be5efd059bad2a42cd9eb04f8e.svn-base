#ifndef _CONFIGEDITOR_H_
#define _CONFIGEDITOR_H_

#include <QtCore>

class ConfigRestartItem;

class ConfigEditor
{
public:
	ConfigEditor();

	void setValue( const QString& key, const QString& value );
	void setValues( const QMap<QString,QString>&  values );

	bool write( const QString& fileName, ConfigRestartItem& restartItem );

private:
	QMap<QString,QString> _values;
};

#endif //_CONFIGEDITOR_H_
