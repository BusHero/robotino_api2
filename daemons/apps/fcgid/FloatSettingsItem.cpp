#include "FloatSettingsItem.h"

FloatSettingsItem::FloatSettingsItem():
    						min(0),
    						max(1),
    						precision(2)
{
	value = "0";
}

QString FloatSettingsItem::getXMLElement() const
{
	QString result;

	QXmlStreamWriter xmlWriter(&result);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartElement("settingItem");

	writeXMLElement(xmlWriter, "itemName", itemName);
	writeXMLElement(xmlWriter, "category", category);
	writeXMLElement(xmlWriter, "type", "float");
	writeXMLElement(xmlWriter, "value", value);
	writeXMLElement(xmlWriter, "min", QString("%1").arg(min));
	writeXMLElement(xmlWriter, "max", QString("%1").arg(max));
	writeXMLElement(xmlWriter, "precision", QString("%1").arg(precision));

	xmlWriter.writeEndElement();

	return result;
}
