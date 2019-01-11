#include "IntegerSettingsItem.h"

IntegerSettingsItem::IntegerSettingsItem()
{
}

QString IntegerSettingsItem::getXMLElement() const
{
	QString result;

	QXmlStreamWriter xmlWriter(&result);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartElement("settingItem");

	writeXMLElement(xmlWriter, "itemName", itemName);
	writeXMLElement(xmlWriter, "category", category);
	writeXMLElement(xmlWriter, "type", "string");
	writeXMLElement(xmlWriter, "value", value);

	xmlWriter.writeEndElement();

	return result;
}
