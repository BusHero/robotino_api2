#ifndef _CONFIGEDITOR_H_
#define _CONFIGEDITOR_H_

#include <QtCore>

class ConfigEditor
{
public:
	ConfigEditor();

	void setValues( const QMap<QString, QString>& values);

	bool write( const QString& fileName );

	int verbosity;

private:
	QMap<QString,QString> _values;
};

#endif //_CONFIGEDITOR_H_
