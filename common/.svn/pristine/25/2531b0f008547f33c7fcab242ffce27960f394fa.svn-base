#ifndef _REC_DATAEXCHANGE_UDP_BYTEARRAYMESSAGE_HPP_
#define _REC_DATAEXCHANGE_UDP_BYTEARRAYMESSAGE_HPP_

#include <QtCore>

namespace rec
{
	namespace dataexchange_lt
	{
		namespace udp
		{
			class ByteArrayMessage : public QObject
			{
				Q_OBJECT
			public:
				ByteArrayMessage();

				virtual ~ByteArrayMessage();

				virtual QByteArray encode() const;

				void decode( const QByteArray& data );

				QByteArray data() const;

				void setData( const char* data, unsigned int dataSize );

				void setData( const QByteArray& data );

			Q_SIGNALS:
				void dataChanged();

			private:
				QByteArray _data;
			};
		}
	}
}

#endif //_REC_DATAEXCHANGE_UDP_BYTEARRAYMESSAGE_HPP_
