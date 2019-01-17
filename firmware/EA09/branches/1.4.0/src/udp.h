#ifndef _UDP_H_
#define _UDP_H_

/**
Messages consist of a 3 byte header and data

byte      description
0         message id
1         message data length high byte ( n >> 8 )
2         message data length low byte	( n & 0xFF )
3         message data byte 0 byte
...
n         message data byte n byte


Messages

Set COMCTRL
byte      value
0		  0
1         0
2         4
3         COMCTRL MSB ( (p >> 24) & 0xFF ) (COMCTRL is of type uint32)
4		  COMCTRL	  ( (p >> 16) & 0xFF )
5         COMCTRL	  ( (p >> 8) & 0xFF )
6		  COMCTRL LSB ( p & 0xFF )

Set MCTRL
byte      value
0         1
1         0
2         5
3         motor
4         MCTRL MSB
5		  MCTRL
6         MCTRL
7		  MCTRL LSB

Set MMONITOR
byte      value
0         2
1         0
2         5
3         motor
4         MMONITOR MSB
5		  MMONITOR
6         MMONITOR
7		  MMONITOR LSB


Monitor message Type A
byte			value
0					3
1					0
2					17
3         motor
4         time high byte ( (p >> 24) & 0xFF ) (time is of type uint32)
5         time mid byte ( (p >> 16) & 0xFF )
6         time mid byte ( (p >> 8) & 0xFF )
7         time low byte ( p & 0xFF )
8		  speed set-point high byte ( v >> 8 ) (setVelocity is of type int16)
9		  speed set-point low byte ( v & 0xFF )
10		  actual speed high byte (velocity is of type int16)
11		  actual speed low byte
12        current high byte	(current is of type uint16)
13        current low byte
14        control point high byte (is of type int16)
15        control point low byte
16         position high byte ( (p >> 24) & 0xFF ) (time is of type uint32)
17         position mid byte ( (p >> 16) & 0xFF )
18         position mid byte ( (p >> 8) & 0xFF )
19         position low byte ( p & 0xFF )

Monitor message Type B
byte      value
0         4
1         0
2         25
3         motor
4         time high byte ( (p >> 24) & 0xFF ) (time is of type uint32)
5         time mid byte ( (p >> 16) & 0xFF )
6         time mid byte ( (p >> 8) & 0xFF )
7         time low byte ( p & 0xFF )
8		  speed set-point high byte ( v >> 8 ) (setVelocity is of type int16)
9		  speed set-point low byte ( v & 0xFF )
10		  actual speed high byte (velocity is of type int16)
11		  actual speed low byte
12        current high byte	(current is of type uint16)
13        current low byte
14        kp
15        ki
16        deviation from set-point high byte (is of type int16)
17        deviation from set-point low byte
18        proportional term (kp*err) high byte ( (p >> 24) & 0xFF ) (proportional is of type int32)
19		  proportional term mid byte ( (p >> 16) & 0xFF )
20 		  proportional term mid byte ( (p >> 8) & 0xFF )
21        proportional term low byte ( p & 0xFF )
22        integral term (ki*ehsum) high byte ( (p >> 24) & 0xFF ) (integral is of type int32)
23		  integral term mid byte ( (p >> 16) & 0xFF )
24 		  integral term mid byte ( (p >> 8) & 0xFF )
25        integral term low byte ( p & 0xFF )
26        control point high byte (is of type int16)
27        control point low byte


Request firmware version (RFV)
This also registers the sender as receiver of messages
Send an UR message to unregister
byte      value
0         5
1         0
2         0


Firmware version (FV)
byte      value
0         6
1		  n high byte
2  		  n low byte
3         first character of version string
4         second character of version string
...
n         last character of version string (not null terminated)

Set control point
byte			value
0					7
1					0
2					8
3        control point motor 0 high byte	(current is of type int16)
4        control point motor 0 low byte
5        control point motor 1 high byte	(current is of type int16)
6        control point motor 1 low byte
7        control point motor 2 high byte	(current is of type int16)
8        control point motor 2 low byte
9        control point motor 3 high byte	(current is of type int16)
10       control point motor 3 low byte

Set internal PI controller constants
byte			value
0					8
1					0
2					3
3         motor
4        kp
5        ki

Unregister receiver (UR)
byte			value
0					9
1					0
2					0

Receiver Rejected (RR)
byte			value
0					10
1					0
2					0
*/

void udp_init( void );

void udp_send_monitor_message_type_a( unsigned char motor );

void udp_send_monitor_message_type_b( unsigned char motor );

void udp_send_SetCOMCTRL( void );

void udp_send_SetMCTRL( unsigned char motor );

void udp_send_SetMMONITOR( unsigned char motor );

void udp_unregister_all_receivers( void );

void udp_unregister_receiver( unsigned int receiver );

struct udp_receiver
{
	unsigned short rem_Port;
	unsigned char rem_IP[4];
};

#define MAX_UDP_RECEIVER 2

#ifndef _UDP_NOEXTERN_
extern struct udp_receiver udp_receiver_buffer[MAX_UDP_RECEIVER];
#endif


#endif //_UDP_H_
