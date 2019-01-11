#include "SettingsItemsXMLGen.h"

SettingsItemsXMLGen::SettingsItemsXMLGen(QObject *parent) :
    						QObject(parent)
{
}

void SettingsItemsXMLGen::appendItem(SettingsItem* item)
{
	_items.append( item );
}

QString SettingsItemsXMLGen::getSettingsItemsXML()
{
	QStringList elements;
	for( SettingsItemPointerList::const_iterator iter = _items.constBegin(); iter != _items.constEnd(); ++iter )
	{
		const SettingsItem* item = *iter;

		elements.append( item->getXMLElement() );
	}
	return "<List>" + elements.join("") + "\n</List>";
}

void SettingsItemsXMLGen::clear()
{
	qDeleteAll( _items );
	_items.clear();
}
