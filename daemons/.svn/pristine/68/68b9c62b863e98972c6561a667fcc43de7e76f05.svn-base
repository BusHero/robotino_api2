#ifndef _PARPORT_H_
#define _PARPORT_H_

class ParPort
{
public:
	typedef enum { STRB = 0x01, AFD = 0x02, INIT = 0x04, SLCT = 0x08 } controlBit;
	typedef enum { ERR = 0x08, SELIN = 0x10, PE = 0x20, ACK = 0x40, BUSY = 0x80 } statusBit;

	ParPort( unsigned short baseAddress );

	void setData( unsigned char ch );
	unsigned char readStatus();

	void setControl( controlBit cb, bool status );

	void out( unsigned short portAddress, unsigned char data );
	unsigned char in( unsigned short portAddress );

	void waitMus( unsigned int execTimeMus );

protected:
	unsigned char _control;
	unsigned short _baseAddress;
};

#endif //_PARPORT_H_
