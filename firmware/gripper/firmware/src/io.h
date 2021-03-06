#ifndef _IO_H_
#define _IO_H_

#ifdef NO_FAST_GPIO
#define PORT0PIN  IO0PIN
#define PORT0DIR  IO0DIR
#define PORT0SET  IO0SET
#define PORT0CLR  IO0CLR
#define PORT0MASK
#define PORT1PIN  IO1PIN
#define PORT1DIR  IO1DIR
#define PORT1SET  IO1SET
#define PORT1CLR  IO1CLR
#define PORT1MASK
#else
#define PORT0PIN  FIO0PIN
#define PORT0DIR  FIO0DIR
#define PORT0SET  FIO0SET
#define PORT0CLR  FIO0CLR
#define PORT0MASK FIO0MASK
#define PORT1PIN  FIO1PIN
#define PORT1DIR  FIO1DIR
#define PORT1SET  FIO1SET
#define PORT1CLR  FIO1CLR
#define PORT1MASK FIO1MASK
#endif

#define LED1_BIT 21
#define LED2_BIT 22
#define LEDS_PIN PORT0PIN
#define LEDS_DIR PORT0DIR
#define LEDS_SET PORT0SET
#define LEDS_CLR PORT0CLR

//P0.30
#define AD03_BIT 30
#define AD03_PIN PORT0PIN
#define AD03_DIR PORT0DIR
#define AD03_SET PORT0SET
#define AD03_CLR PORT0CLR

//P0.25
#define AD04_BIT 25
#define AD04_PIN PORT0PIN
#define AD04_DIR PORT0DIR
#define AD04_SET PORT0SET
#define AD04_CLR PORT0CLR

#endif
