#ifndef _CONFIGEDITOR_H_
#define _CONFIGEDITOR_H_

#include <QtCore>

class ConfigEditor
{
public:
	ConfigEditor();

	void setValue( const QString& key, const QString& value );

	bool write( const QString& fileName );

private:
	QMap<QString,QString> _values;
};

#endif //_CONFIGEDITOR_H_
