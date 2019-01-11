#include "rec/dataexchange_lt/messages/Configuration.h"
#include "rec/dataexchange_lt/messages/Message.h"

#include <string>

using namespace rec::dataexchange_lt::messages;

QByteArray Configuration::encode( const rec::dataexchange_lt::configuration::Configuration& config )
{
	QByteArray configData = config.save();

	unsigned int configDataSize = configData.size();

	QByteArray data( configDataSize + 5, 0 );

	data[0] = ConfigurationId;

	quint8* p = (quint8*)&configDataSize;
	data[1] = *(p++);
	data[2] = *(p++);
	data[3] = *(p++);
	data[4] = *(p++);

	memcpy( data.data() + 5, configData.constData(), configDataSize );

	return data;
}

bool Configuration::decode( const QByteArray& data, rec::dataexchange_lt::configuration::Configuration* configuration )
{
	return configuration->load( data );
}

