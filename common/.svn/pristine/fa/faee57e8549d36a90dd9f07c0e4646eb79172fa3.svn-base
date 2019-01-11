#include "ConnectJTAGPage.h"
#include "LogList.h"
#include "Config.h"

ConnectJTAGPage::ConnectJTAGPage()
{
	setTitle( "JTAG Verbindung herstellen." );

	QLabel* label = new QLabel;
	label->setText("Verbinden Sie das Keil ULINK2 JTAG Programmiergerät mit dem JTAG Stecker neben dem Microcontroller.\n"
		"Verbinden Sie den RS232-USB-Adapter mit X7.\n"
		"Stecken Sie einen Jumper auf Pin4,6 des Anschaltkopfsteckers X6."
		);
	label->setWordWrap( true );

	QLabel* imageLabel = new QLabel;
	imageLabel->setPixmap( QPixmap( ":/images/firmware_connection.png" ) );

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);
	layout->addWidget(imageLabel);
	
	setLayout(layout);
}
