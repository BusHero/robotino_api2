#include "rec/dataexchange_lt/messages/Data.h"
#include "rec/dataexchange_lt/messages/Message.h"

#include <QtDebug>

using namespace rec::dataexchange_lt::messages;

Data::Data()
: type( NOTYPE )
, uint_value( 0 )
, int_value( 0 )
, float_value( 0.0f )
{
}

Data::Data( const QString& name_, rec::dataexchange_lt::DataType type_ )
: name( name_ )
, type( type_ )
, uint_value( 0 )
, int_value( 0 )
, float_value( 0.0f )
{
}

QByteArray rec::dataexchange_lt::messages::Data::encode() const
{
	Q_ASSERT( rec::dataexchange_lt::NOTYPE != type );

	QByteArray nameData = name.toUtf8();

	if( nameData.size() > 0xFFFF )
	{
		nameData.resize( 0xFFFF );
	}

	QByteArray valueData;

	switch( type )
	{
	case rec::dataexchange_lt::U32:
		{
			valueData.resize( 4 );

			quint32* p = reinterpret_cast<quint32*>( valueData.data() );
			*p = uint_value;
		}
		break;

	case rec::dataexchange_lt::I32:
		{
			valueData.resize( 4 );

			qint32* p = reinterpret_cast<qint32*>( valueData.data() );
			*p = int_value;
		}
		break;

	case rec::dataexchange_lt::FLOAT32:
		{
			valueData.resize( 4 );
	
			float* p = reinterpret_cast<float*>( valueData.data() );
			*p = float_value;
		}
		break;

	case rec::dataexchange_lt::STRING:
		{
			std::string str = string_value.toStdString();
			if( str.size() > 0xFFFFFFFF )
			{
				str.resize( 0xFFFFFFFF );
			}
			valueData.resize( str.size() );
			memcpy( valueData.data(), str.c_str(), valueData.size() );
		}
		break;

	case rec::dataexchange_lt::FLOATARRAY:
		{
			QVector<float> vec = floatvector_value;
			if( vec.size() > ( 0xFFFFFFFF >> 2 ) )
			{
				vec.resize( 0xFFFFFFFF >> 2 );
			}
			valueData.resize( vec.size() * sizeof( float ) );
			if ( !vec.isEmpty() )
			{
				memcpy( valueData.data(), vec.constData(), vec.size() * sizeof( float ) );
			}
		}
		break;

	case rec::dataexchange_lt::BYTEARRAY:
		{
			QByteArray ba = bytearray_value;
			if( ba.size() > 0xFFFFFFFF )
			{
				ba.resize( 0xFFFFFFFF );
			}
			valueData.resize( ba.size() );
			memcpy( valueData.data(), ba.constData(), valueData.size() );
		}
		break;
	}

	quint32 valueDataSize = valueData.size();

	quint32 dataSizeWithoutHeader = 2 + nameData.size() + 1 + 4 + valueData.size();

	QByteArray data( dataSizeWithoutHeader + 5, 0 );

	char* datap = data.data();

	//header************************
	//message id
	*(datap++) = rec::dataexchange_lt::messages::DataId;

	//gesamte nachrichtenlänge ohne header
	quint32* puint32 = reinterpret_cast<quint32*>( datap );
	*puint32 = dataSizeWithoutHeader;
	datap += 4;
	//quint8* p = (quint8*)&dataSizeWithoutHeader;
	//*(datap++) = *(p++);
	//*(datap++) = *(p++);
	//*(datap++) = *(p++);
	//*(datap++) = *(p++);

	//body*************************
	//länge des names
	quint16* puint16 = reinterpret_cast<quint16*>( datap );
	*puint16 = static_cast<quint16>( nameData.size() );
	datap += 2;
	//*(datap++) = ( nameData.size() & 0xFF );
	//*(datap++) = ( ( nameData.size() >> 8 ) & 0xFF );

	//der name
	memcpy( datap, nameData.constData(), nameData.size() );
	datap += nameData.size();

	//der type
	*(datap++) = type;

	//länge der daten
	puint32 = reinterpret_cast<quint32*>( datap );
	*puint32 = valueDataSize;
	datap += 4;
	//*(datap++) = ( valueDataSize & 0xFF );
	//*(datap++) = ( ( valueDataSize >> 8 ) & 0xFF );
	//*(datap++) = ( ( valueDataSize >> 16 ) & 0xFF );
	//*(datap++) = ( ( valueDataSize >> 24 ) & 0xFF );

	//die daten
	memcpy( datap, valueData.constData(), valueData.size() );

	return data;
}

bool rec::dataexchange_lt::messages::Data::decode( const QByteArray& data )
{
	//daten ohne header

	if( data.size() < 7 )
	{
		return false;
	}

	const char* datap = data.constData();

	quint16 nameSize= *( reinterpret_cast<const quint16*>( datap ) );
	datap += 2;

	if( data.size() < nameSize + 7 )
	{
		return false;
	}

	name = QString::fromUtf8( datap, nameSize );
	datap += nameSize;

	type = static_cast<rec::dataexchange_lt::DataType>( *(datap++) );

	if( type < rec::dataexchange_lt::NOTYPE || type >= rec::dataexchange_lt::DataType_Max )
	{
		return false;
	}

	quint32 valueDataSize = *( reinterpret_cast<const quint32*>( datap ) );
	datap += 4;

	if( data.size() != nameSize + 7 + valueDataSize )
	{
		qDebug() << data.size();
		return false;
	}

	switch( type )
	{
	case rec::dataexchange_lt::U32:
		{
			if( valueDataSize != 4 )
			{
				return false;
			}
			uint_value = *( reinterpret_cast<const quint32*>( datap ) );
		}
		break;

	case rec::dataexchange_lt::I32:
		{
			if( valueDataSize != 4 )
			{
				return false;
			}

			int_value = *( reinterpret_cast<const qint32*>( datap ) );
		}
		break;

	case rec::dataexchange_lt::FLOAT32:
		{
			if( valueDataSize != 4 )
			{
				return false;
			}

			float_value = *( reinterpret_cast<const float*>( datap ) );
		}
		break;

	case rec::dataexchange_lt::STRING:
		{
			std::string str( datap, valueDataSize );
			string_value = QString::fromStdString( str );
		}
		break;

	case rec::dataexchange_lt::FLOATARRAY:
		{
			floatvector_value.resize( valueDataSize / sizeof( float ) );
			if ( valueDataSize > 0 )
			{
				memcpy( floatvector_value.data(), datap, valueDataSize );
			}
		}
		break;

	case rec::dataexchange_lt::BYTEARRAY:
		{
			bytearray_value = QByteArray( datap, valueDataSize );
		}
		break;
	}

	return true;
}

const char* rec::dataexchange_lt::dataTypeName( rec::dataexchange_lt::DataType type )
{
	switch( type )
	{
	case rec::dataexchange_lt::U32:
		return "U32";

	case rec::dataexchange_lt::I32:
		return "I32";

	case rec::dataexchange_lt::FLOAT32:
		return "FLOAT32";

	case rec::dataexchange_lt::STRING:
		return "STRING";

	case rec::dataexchange_lt::FLOATARRAY:
		return "FLOATARRAY";

	case rec::dataexchange_lt::BYTEARRAY:
		return "BYTEARRAY";

	default:
		return "NOTYPE";
	}
};

rec::dataexchange_lt::DataType rec::dataexchange_lt::dataTypeFromName( const char* name )
{
	if( 0 == strncmp( name, "U32", 3 ) )
	{
		return rec::dataexchange_lt::U32;
	}
	else if( 0 == strncmp( name, "I32", 3 ) )
	{
		return rec::dataexchange_lt::I32;
	}
	else if( 0 == strncmp( name, "FLOAT32", 7 ) )
	{
		return rec::dataexchange_lt::FLOAT32;
	}
	else if( 0 == strncmp( name, "STRING", 6 ) )
	{
		return rec::dataexchange_lt::STRING;
	}
	else if( 0 == strncmp( name, "FLOATARRAY", 10 ) )
	{
		return rec::dataexchange_lt::FLOATARRAY;
	}
	else if( 0 == strncmp( name, "BYTEARRAY", 10 ) )
	{
		return rec::dataexchange_lt::BYTEARRAY;
	}
	else
	{
		return rec::dataexchange_lt::NOTYPE;
	}
}
