#ifndef _REC_DATAEXCHANGE_UDP_MESSAGE0_HPP_
#define _REC_DATAEXCHANGE_UDP_MESSAGE0_HPP_

#include <QtCore>

namespace rec
{
	namespace dataexchange_lt
	{
		namespace udp
		{
			class Message0 : public QObject
			{
				Q_OBJECT
			public:
				Message0();

				virtual ~Message0();

				virtual QByteArray encode() const;

				void decode( const QByteArray& data );

				int data( int n ) const;

				void setData( int n, int value );

				void setData( const QVector< int >& data );

			Q_SIGNALS:
				void dataChanged();

			protected:
				QByteArray encode_i( unsigned char number ) const;

			private:
				mutable QMutex _mutex;

				static const int dataSize = 8;
				int _data[8];
			};
		}
	}
}

#endif //_REC_DATAEXCHANGE_UDP_MESSAGE0_HPP_
