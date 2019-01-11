#ifndef _ACTION_EA09FirmwareUpdate_H_
#define _ACTION_EA09FirmwareUpdate_H_

#include "action/Menu.h"

namespace action
{
	class EA09FirmwareUpdate : public action::Menu
	{
		Q_OBJECT
	public:
		EA09FirmwareUpdate( Action* parent );

		void customActivate( Action* before );

		QString title() const;

	private Q_SLOTS:
		void on_ea_version_changed( const QString& board, quint16 firmware_major, quint16 firmware_minor, quint16 firmware_patch );

	private:
		void updateFromToStr();

		QString _packageFileName;

		quint16 _firmware_major;
		quint16 _firmware_minor;
		quint16 _firmware_patch;
		quint16 _package_major;
		quint16 _package_minor;
		quint16 _package_patch;
		QString _board;
	};
}

#endif //_ACTION_EA09FirmwareUpdate_H_
