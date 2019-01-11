#include "BooleanSettingsItem.h"

BooleanSettingsItem::BooleanSettingsItem()
{
	value = "false";
}

QString BooleanSettingsItem::getXMLElement() const
{
	QString result;

	QXmlStreamWriter xmlWriter(&result);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartElement("settingItem");

	writeXMLElement(xmlWriter, "itemName", itemName);
	writeXMLElement(xmlWriter, "category", category);
	writeXMLElement(xmlWriter, "type", "boolean");
	writeXMLElement(xmlWriter, "value", value);

	xmlWriter.writeEndElement();

	return result;
}
