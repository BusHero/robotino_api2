#include "ConnectUSBPage.h"

ConnectUSBPage::ConnectUSBPage()
{
	setTitle( "USB Verbindung vorbereiten." );

	QLabel* label = new QLabel;
	label->setText("1) Entfernen Sie den Jumper vom Anschaltknopfstecker. Schließen Sie dort den Anschaltknopf an.\n"
		"2) Verbinden Sie das USB-Kabel mit der Mini-USB-Buchse neben dem Microcontroller.\n"
		"3) Stecken Sie den USB-Connect Jumper.\n"
		"4) Stecken Sie den Motor Jumper.\n"
		"Schalten Sie das System durch drücken des Anschaltknopfes ein."
		);
	label->setWordWrap( true );

	QLabel* imageLabel = new QLabel;
	imageLabel->setPixmap( QPixmap( ":/images/usb_connection.png" ) );

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);
	layout->addWidget(imageLabel);
	
	setLayout(layout);
}
