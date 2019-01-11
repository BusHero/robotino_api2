#include "SelectionSettingsItem.h"
#include "rec/robotino/daemons/Log.h"

SelectionSettingsItem::SelectionSettingsItem()
{
}

QString SelectionSettingsItem::getXMLElement() const
{
	QString result;

	QXmlStreamWriter xmlWriter(&result);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartElement("settingItem");

	writeXMLElement(xmlWriter, "itemName", itemName);
	writeXMLElement(xmlWriter, "category", category);
	writeXMLElement(xmlWriter, "type", "selection");
	writeXMLElement(xmlWriter, "value", value);
	writeXMLElement(xmlWriter, "valuemap", options.join(","));

	xmlWriter.writeEndElement();

	rec::robotino::daemons::Log::singleton()->log( result, 0 );

	return result;
}
