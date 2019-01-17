#ifndef _COM_H_
#define _COM_H_

#define COM_IS_SERIALLINE_CONTROL_ENABLED ( 0x1 == COMCTRL )
#define COM_IS_ETHERNET_CONTROL_ENABLED ( 0x2 == COMCTRL )
#define COM_IS_TELNET_CONTROL_ENABLED ( 0x3 == COMCTRL )

void com_setCOMCTRL( unsigned int value );

/**
Message format:
Byte Meaning
0    Message ID, must be >0, special ID 'R' is reserved
1    Message length (3+dataSize)
2    Checksum
3    Data (if message length is >3)
.
.
.
N

Get information:
Byte Value
0    20
1    3
2    0xFF - 23 = 0xE8

Get information answer:
Byte Value
0    20
1    7
2    
3    Major
4    Minor
5    Patch
6    Ethernet available

Get IP address:
Byte Value
0    21
1    3
2    0xFF - 24 = 0xE7

Get IP address answer:
Byte Value
0    21
1    11
2    
3    IP1
4    IP2
5    IP3
6    IP4
7    MSK1
8    MSK2
9    MSK3
10   MSK4

Set IP address:
Byte Value
0    22
1    11
2    
3    IP1
4    IP2
5    IP3
6    IP4
7    MSK1
8    MSK2
9    MSK3
10   MSK4

Set IP address answer = Get IP address answer

Get MAC address:
Byte Value
0    23
1    9
2    
3    MAC1
4    MAC2
5    MAC3
6    MAC4
7    MAC5
8    MAC6

Get MAC address answer:
Byte Value
0    23
1    9
2    
3    MAC1
4    MAC2
5    MAC3
6    MAC4
7    MAC5
8    MAC6

Set MAC address:
Byte Value
0    24
1    9
2    
3    MAC1
4    MAC2
5    MAC3
6    MAC4
7    MAC5
8    MAC6

Set MAC address answer = Get MAC address answer

Get local host name:
Byte Value
0    25
1    3
2    0

Get local host name answer:
Byte Value
0    25
1    3+name length
2    
3    name first byte
.
.
.
N    name last byte

Set local host name:
Byte Value
0    26
1    3+name length
2    
3    name first byte
.
.
.
N    name last byte

Set local host name answer = Get local host name answer

Set FPGA power and FPGA power response:
Byte Value
0    27
1    4
2
3    0: power off, 1: power on

Power off:
Byte Value
0    28
1    3
2    0xFF - 31 = 0xE0

Error answer:
Byte Value
0    255
1    4
2    
3    ID of message that caused this error

PocControl and PosControl response
Byte Value
0    29
1	 16
2    
3    0: off, 1: on
4    speed
5    imax MSB
6    imax LSB
7    for later usage
8    for later usage
9    for later usage
10   for later usage
11   for later usage
12   for later usage
13   for later usage
14   for later usage
15   for later usage
*/

void com_process_message( const unsigned char* message, unsigned char* response );

#ifndef _COM_NO_EXTERNAL_
extern const unsigned int COMCTRL;
#endif //_COM_NO_EXTERNAL_

#endif //_COM_H_
