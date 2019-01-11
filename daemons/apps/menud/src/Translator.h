#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#define TT(x) Translator::translate(x)

#include <QtCore>
#include <QtXml>

class Translator
{
public:
	static void init();

	static void done();

	static QString translate( const QString& str );

	static void setLanguage( const QString& key );

private:
	Translator( QDomElement& e );

	void loadTranslation( QDomElement& e );

	static Translator* _impl;
	static QString _currentLanguage;

	QMap< QString, QMap< QString, QString > > _transMap;
};

#endif //_TRANSLATOR_H_

