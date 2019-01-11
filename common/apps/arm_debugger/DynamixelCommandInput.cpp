#include "DynamixelCommandInput.h"
#include "rec/grappler/serial/USBProtocol.h"

class HexInput : public QSpinBox
{
public:
	HexInput( QWidget* parent )
		: QSpinBox( parent )
	{
		setRange( 0, 0xff );
	}

	QString textFromValue( int value ) const
	{
		return QString("%1").arg( value, 2, 16, QChar( '0' ) );
	}

	int valueFromText( const QString& text ) const
	{
		bool ok;
		return text.toInt( &ok, 16 );
	}

	QValidator::State validate( QString& input, int & pos ) const
	{
		if( input.isEmpty() )
		{
			return QValidator::Acceptable;
		}

		bool ok;
		int num = input.toInt( &ok, 16 );

		if( ok && num <= maximum() && num >= minimum() )
		{
			return QValidator::Acceptable;
		}
		else
		{
			return QValidator::Invalid;
		}
	}
};

DynamixelCommandInput::DynamixelCommandInput( QWidget* parent )
: QWidget( parent )
, _channelSelector( new QComboBox( this ) )
, _id( new HexInput( this ) )
, _instr( new HexInput( this ) )
, _params( new QLineEdit( this ) )
{
	_channelSelector->addItems( QStringList() << "RX64" << "RX28" << "RX10" );

	_id->setValue( 1 );
	_instr->setValue( 3 );
	_params->setText( "1e 0 0 0 2" );

	QGridLayout* layout = new QGridLayout;
	setLayout( layout );

	layout->addWidget( new QLabel( "Channel" ), 0, 0 );
	layout->addWidget( new QLabel( "ID" ), 0, 1 );
	layout->addWidget( new QLabel( "INSTR" ), 0, 2 );
	layout->addWidget( new QLabel( "PARAMS" ), 0, 3 );

	layout->addWidget( _channelSelector, 1, 0 );
	layout->addWidget( _id, 1, 1 );
	layout->addWidget( _instr, 1, 2 );
	layout->addWidget( _params, 1, 3 );

	QPushButton* sendButton = new QPushButton( this );
	sendButton->setText( "Send" );
	(bool)connect( sendButton, SIGNAL( clicked() ), SLOT( on_sendButton_clicked() ) );
	layout->addWidget( sendButton, 2, 0, 1, 4 );
}

void DynamixelCommandInput::on_sendButton_clicked()
{
	QStringList params = _params->text().split( QRegExp( "\\s" ), QString::SkipEmptyParts );

	QByteArray paramdata;

	bool ok;

	Q_FOREACH( const QString& str, params )
	{
		unsigned int num = str.toUInt( &ok, 16 );

		if( !ok && num > 255 )
		{
			Q_EMIT error( "RX64 wrong parameter syntax" );
			return;
		}

		paramdata.append( (const char)num );
	}

	unsigned int id = _id->text().toUInt( &ok, 16 );
	unsigned int instr = _instr->text().toUInt( &ok, 16 );

	rec::grappler::serial::Channel channel = rec::grappler::serial::RX64Channel;
	if( _channelSelector->currentText() == "RX28" )
	{
		channel = rec::grappler::serial::RX28Channel;
	}
	else if( _channelSelector->currentText() == "RX10" )
	{
		channel = rec::grappler::serial::RX10Channel;
	}

	QByteArray commandData = rec::grappler::serial::USBProtocol::encodeDynamixelCommand( channel, id, instr, paramdata );

	Q_EMIT sendCommand( commandData );
}
