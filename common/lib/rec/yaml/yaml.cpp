#include "rec/yaml/yaml.h"

using namespace rec::yaml;

Reader::Reader()
{

}

bool Reader::parse(const QString& fileName)
{
	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (false == file.atEnd())
		{
			QString line = file.readLine();
			QStringList l = line.split(":", QString::SkipEmptyParts);
			if (2 == l.count())
			{
				QString key = l.at(0).trimmed();
				QString valueStr = l.at(1).trimmed();
				QStringList valueList;
				if (valueStr.startsWith("[") && valueStr.endsWith("]"))
				{
					valueStr.remove("[");
					valueStr.remove("]");
					valueList = valueStr.split(",", QString::SkipEmptyParts);
				}
				else
				{
					valueList << valueStr;
				}
				
				QStringList::iterator iter = valueList.begin();
				while (valueList.end() != iter)
				{
					*iter = (*iter).trimmed();
					(*iter).remove("\"");
					++iter;
				}
					
				_entries[key] = valueList;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

QStringList Reader::keys() const
{
	return _entries.keys();
}

entryType Reader::entryTypeForKey(const QString& key) const
{
	QStringList l = listEntryForKey(key);
	if (l.count() > 1)
	{
		return ListEntry;
	}
	else
	{
		return StringEntry;
	}
}

QString Reader::entryForKey(const QString& key) const
{
	QStringList l = listEntryForKey(key);
	if (l.count() > 0)
	{
		return l.at(0);
	}
	else
	{
		return QString();
	}
}

qreal Reader::realEntryForKey(const QString& key) const
{
	QString str = entryForKey(key);
	bool ok;
	qreal value = str.toDouble(&ok);
	if (ok)
	{
		return value;
	}
	else
	{
		return 0;
	}
}

QStringList Reader::listEntryForKey(const QString& key) const
{
	QStringList l;
	entries_t::const_iterator iter = _entries.find(key);
	if (_entries.end() != iter)
	{
		l = iter.value();
	}

	return l;
}

QVector<qreal> Reader::realVectorEntryForKey(const QString& key) const
{
	QStringList l = listEntryForKey(key);
	QVector<qreal> vec;

	Q_FOREACH(const QString& str, l)
	{
		bool ok;
		qreal value = str.toDouble(&ok);
		if (ok)
		{
			vec << value;
		}
		else
		{
			vec << 0;
		}
	}

	return vec;
}
