#include "SettingsItem.h"

SettingsItem::SettingsItem()
{
}

void SettingsItem::writeXMLElement(QXmlStreamWriter& writer, QString tag, QString value) const
{
	writer.writeStartElement(tag);
	writer.writeCharacters(value);
	writer.writeEndElement();
}
