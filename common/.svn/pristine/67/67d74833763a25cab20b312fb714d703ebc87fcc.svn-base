#ifndef _REC_ROBOTINO_SERVER_V1_MESSAGES_INFO_H_
#define _REC_ROBOTINO_SERVER_V1_MESSAGES_INFO_H_

#include <QByteArray>
#include <QString>

namespace rec
{
	namespace robotino
	{
		namespace server
		{
			namespace v1
			{
				namespace messages
				{
					class Info
					{
					public:
						Info();

						void setMessage( const QString& message );

						void setPassiveMode( bool isPassiveMode );

						QByteArray encode() const;

					private:
						QString _message;
						bool _isPassiveMode;
					};
				}
			}
		}
	}
}

#endif //_REC_ROBOTINO_SERVER_V1_MESSAGES_INFO_H_
