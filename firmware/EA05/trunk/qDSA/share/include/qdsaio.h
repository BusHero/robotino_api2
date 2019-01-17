#ifndef _QDSAIO_H
#define _QDSAIO_H

#define M_SCK_PORT PORTB
#define M_SCK_DDR DDRB
#define M_SCK_BIT 7

#define M_MISO_PORT PORTB
#define M_MISO_BIT 6

#define M_MOSI_PORT PORTB
#define M_MOSI_DDR DDRB
#define M_MOSI_BIT 5

#define M_SS_PORT PORTB
#define M_SS_DDR DDRB
#define M_SS_BIT 4

#define M_DISPLAY_MISC PORTB
#define M_DISPLAY_DDR DDRB
#define M_DISPLAY_PIN PINB
#define M_DISPLAY_BIT 2

#define M_RESET_PORT PORTB
#define M_REST_DDR DDRB
#define M_RESET_BIT  0

#define M_SS3_PORT PORTC
#define M_SS3_DDR DDRC
#define M_SS3_BIT 6

#define M_SS2_PORT PORTC
#define M_SS2_DDR DDRC
#define M_SS2_BIT 5

#define M_SS1_PORT PORTC
#define M_SS1_DDR DDRC
#define M_SS1_BIT 4

#define M_SS0_PORT PORTC
#define M_SS0_DDR DDRC
#define M_SS0_BIT 3

#define M_LED0_PORT PORTC
#define M_LED0_PIN PINC
#define M_LED0_DDR DDRC
#define M_LED0_BIT 7

#define M_LED1_PORT PORTC
#define M_LED1_PIN PINC
#define M_LED1_DDR DDRC
#define M_LED1_BIT 2

#define M_POWERLED_PORT PORTB
#define M_POWERLED_PIN PINB
#define M_POWERLED_DDR DDRB
#define M_POWERLED_BIT 2

#define M_SDA_PORT PORTC
#define M_SDA_DDR DDRC
#define M_SDA_PIN PINC
#define M_SDA_BIT 1

#define M_SCL_PORT PORTC
#define M_SCL_DDR DDRC
#define M_SCL_PIN PINC
#define M_SCL_BIT 0

#define M_PIDCLOCK_PORT PORTD
#define M_PIDCLOCK_DDR DDRD
#define M_PIDCLOCK_BIT 7

#define M_CTS_PORT PORTD
#define M_CTS_DDR DDRD
#define M_CTS_BIT 6

#define M_RTS_PORT PORTD
#define M_RTS_PIN PIND
#define M_RTS_BIT 5

#define M_POWER_PORT PORTD
#define M_POWER_DDR DDRD
#define M_POWER_BIT  4

#define M_TXD_PORT PORTD
#define M_TDX_DDR DDRD
#define M_TXD_BIT 1

#define M_RXD_PORT PORTD
#define M_RXD_PIN PIND
#define M_RXD_BIT 0

#define S_SCK_PORT PORTB
#define S_SCK_DDR DDRB
#define S_SCK_BIT 7

#define S_MISO_PORT PORTB
#define S_MISO_DDR DDRB
#define S_MISO_BIT 6

#define S_MOSI_PORT PORTB
#define S_MOSI_DDR DDRB
#define S_MOSI_BIT 5

#define S_SS_PORT PORTB
#define S_SS_PIN PINB
#define S_SS_BIT 4

#define S_DIR_PORT PORTB
#define S_DIR_DDR DDRB
#define S_DIR_BIT 3

#define S_BRAKE_PORT PORTB
#define S_BRAKE_DDR DDRB
#define S_BRAKE_BIT  2

#define S_UPCLK_PORT PORTB
#define S_UPCLK_PIN PINB
#define S_UPCLK_BIT 1

#define S_DOWNCLK_PORT PORTB
#define S_DOWNCLK_PIN PINB
#define S_DOWNCLK_BIT 0

#define S_LED0_PORT  PORTD
#define S_LED0_DDR   DDRD
#define S_LED0_PIN   PIND
#define S_LED0_BIT   6

#define S_LED1_PORT  PORTD
#define S_LED1_DDR   DDRD
#define S_LED1_PIN   PIND
#define S_LED1_BIT   5

#define S_DO0_PORT   PORTC
#define S_DO0_DDR    DDRC
#define S_DO0_PIN    PINC
#define S_DO0_BIT    4

#define S_DO1_PORT   PORTC
#define S_DO1_DDR    DDRC
#define S_DO1_PIN    PINC
#define S_DO1_BIT    5

#define S_DO2_PORT   PORTC
#define S_DO2_DDR    DDRC
#define S_DO2_PIN    PINC
#define S_DO2_BIT    6

#define S_DO3_PORT   PORTC
#define S_DO3_DDR    DDRC
#define S_DO3_PIN    PINC
#define S_DO3_BIT    7

#define S_R0_PORT   PORTD
#define S_R0_DDR    DDRD
#define S_R0_PIN    PIND
#define S_R0_BIT    0

#define S_DI0_PORT PORTC
#define S_DI0_PIN PINC
#define S_DI0_BIT  0

#define S_DI1_PORT PORTC
#define S_DI1_PIN PINC
#define S_DI1_BIT  1

#define S_DI2_PORT PORTC
#define S_DI2_PIN PINC
#define S_DI2_BIT  2

#define S_DI3_PORT PORTC
#define S_DI3_PIN PINC
#define S_DI3_BIT  3

#define S_PWM_PORT PORTD
#define S_PWM_DDR DDRD
#define S_PWM_BIT 7

#define S_THFLAG_PORT PORTD
#define S_THFLAG_PIN PIND
#define S_THFLAG_BIT 4

#define S_BUMPER_PORT PORTD
#define S_BUMPER_PIN PIND
#define S_BUMPER_BIT  1

#endif
