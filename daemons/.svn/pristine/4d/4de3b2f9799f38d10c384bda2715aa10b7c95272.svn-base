//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _DISPLAY_HW_SETTINGS_H_
#define _DISPLAY_HW_SETTINGS_H_

#define BIT0  0x01
#define BIT1  0x02
#define BIT2  0x04
#define BIT3  0x08
#define BIT4  0x10
#define BIT5  0x20
#define BIT6  0x40
#define BIT7  0x80

#define HW_INV_STATUS (0x80)
#define HW_INV_CONTROL (0x0B)

// Settings for hardware cable setup
#define BIT_BACKLIGHT (ParPort::SLCT)
#define BIT_EN        (ParPort::STRB)
#define BIT_RS        (ParPort::INIT)

#define DISPLAY_WIDTH    16
#define DISPLAY_HEIGHT   4

// settings from datasheet

// commands
#define DISPLAY_CLEAR    BIT0
#define RETURN_HOME      BIT1
#define ENTRY_MODE_SET   BIT2
#define DISPLAY_ONOFF    BIT3
#define SHIFT            BIT4
#define SET_FUNCTION     BIT5
#define SET_CG_RAM       BIT6
#define SET_DD_RAM       BIT7

// command detail: ENTRY_MODE_SET
#define EMS_CURSOR_MOVE_INC  BIT1
#define EMS_CURSOR_MOVE_DEC  0
#define EMS_DISPLAY_SHIFT    BIT0
#define EMS_NO_DISPLAY_SHIFT 0

// command detail: DISPLAY_ONOFF
#define DOO_ON           BIT2
#define DOO_OFF          0
#define DOO_CURSOR_ON    BIT1
#define DOO_CURSOR_OFF   0
#define DOO_BLINKING_ON  BIT0
#define DOO_BLINKING_OFF 0

// command detail: SHIFT
#define SHI_DISPLAY_SHIFT     BIT3
#define SHI_DISPLAY_CUR_MOVE  0
#define SHI_RIGHT_SHIFT       BIT2
#define SHI_LEFT_SHIFT        0

// command detail: SET_FUNCTION
#define SFN_INTERFACE_8_BIT  BIT4
#define SFN_INTERFACE_4_BIT  0x00
#define SFN_2_LINES          BIT3
#define SFN_1_LINE           0
#define SFN_5x10_DOTS        BIT2
#define SFN_5x7_DOTS         0


#define TIME_ENTRY_MODE_SET 1600
#define TIME_RETURN_HOME   43
#define TIME_DISPLAY_CLEAR 43
#define TIME_DISPLAY_ONOFF 43
#define TIME_SET_FUNCTION 43
#define TIME_SET_DD_RAM  43
#define TIME_SET_CG_RAM  43
#define TIME_READ_DATA   46
#define TIME_WRITE_DATA  43


#endif

