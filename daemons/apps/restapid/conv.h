#ifndef _CONF_H_
#define _CONF_H_

#include <cpprest/asyncrt_utils.h>
#include <QtCore>

#ifdef _UTF16_STRINGS
static utility::string_t toUtilityString(std::string str)
{
	return utility::conversions::to_utf16string(str);
}
static std::string utilityToStdString(utility::string_t str)
{
	return utility::conversions::to_utf8string(str);
}
#else
#define toUtilityString(x) x
#define utilityToStdString(x) x
#endif

static utility::string_t qByteArrayToUtilityString(const QByteArray& ba)
{
	std::string str = ba.constData();
	return toUtilityString(str);
}

static QByteArray utilityToQByteArray(utility::string_t str)
{
	std::string sstr = utilityToStdString(str);
	return QByteArray::fromStdString(sstr);
}

#ifdef _UTF16_STRINGS
static utility::string_t qStringToUtilityString(const QString& str)
{
	return str.toStdWString();
}

static QString utilityToQString(utility::string_t str)
{
	return QString::fromStdWString(str);
}

#else //_UTF16_STRINGS

static utility::string_t qStringToUtilityString(const QString& str)
{
	return str.toStdString();
}

static utility::string_t qByteArrayToUtilityString(const QByteArray& ba)
{
	std::string str = ba.constData();
	return toUtilityString(str);
}

static QString utilityToQString(utility::string_t str)
{
	return QString::fromStdString(str);
}
#endif //_UTF16_STRINGS

#endif //_CONF_H_
