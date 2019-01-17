#include <LPC23XX.H>
#include <rtl.H>
#include <stdio.h>
#include "util.h"
#include "com.h"
#include "usb.h"
#include "usbhw.h"
#include "fpga.h"
#include "gyro.h"
#include "udp.h"
#include "version.h"
#include "charger.h"
#include "led.h"

extern void init_serial (void);
extern int sendchar (int ch);
extern void sendline(const char* str);

extern void mainTimer_start( void );
extern unsigned int mainTimer_elapsed( void );
extern void tickTimer_start( void );
extern unsigned int tickTimer_elapsed( void );
extern void timer_init( void );
extern void timer_start_motor_timer( void );
extern void timer_stop_motor_timer( void );

extern void power_init( void );
extern unsigned char cover_pwr_btn_is_pressed( void );
extern void set_power_ok_high( void );
extern void set_power_ok_low( void );

extern void io_init( void );
extern unsigned char io_pwr_ok_state( void );
extern void io_set_pwrbtn_low( void );
extern void io_set_pwrbtn_high( void );
extern unsigned char io_sus_s3( void );
extern unsigned char io_sus_s4( void );
extern unsigned char io_sus_s5( void );
extern unsigned char io_bumper( void );
extern void io_power_on( void );
extern void io_power_off( void );
extern void io_power_off( void );
extern unsigned char io_config_reset_button_pressed( void );
extern unsigned char io_version0( void );

extern void spi_init( void );
extern void spi_all_motors_on( void );
extern void spi_all_motors_off( void );

extern void motor_init( void );
extern void motor_setDir( unsigned char motor, unsigned char dir );
extern void motor_dir_output_init( void );
extern void motor_update( void );
extern void motor_setControlPoint( unsigned char motor );
extern void motor_stopAll( void );
extern short motor_speed_setpoint[4];

extern void odometry_init( void );

extern void pwm_init( void );
extern void pwm_set( unsigned char channel, unsigned char value );

extern void adc_init( void );
extern void adc_update( void );

extern void com_init( void );
extern void com_update( void );
extern const unsigned int com_rx_counter;

extern void init_TcpNet (void);
extern BOOL main_TcpNet (void);
extern void timer_tick (void);
extern void myudp_init( void );

extern void charger_init( void );
extern void charger_update( void );

unsigned char emergency_bumper = 0;
unsigned int emergency_bumper_counter = 0;

unsigned char config_reset_signal = 0;

extern const unsigned int motorTimerElapsed;

extern void watchdog_feed( void );

void resetDevice( unsigned char enterUSBBootloader )
{
	fpga_power_on();

	if ( enterUSBBootloader )
		setBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );
	else
		clearBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );

	USB_Connect( 0 );
	USB_Reset();

	// Use watchdog with a very small timeout value
	//WDTC = 0;
	//WDMOD = 3; // Set WDEN and WDRESET bits
	//WDFEED = 0xAA; // Start the watchdog timer. Reset will be triggered at timeout (which will be very very soon...).
	//WDFEED = 0x55;

	printf("Reset\n\r");

	for(;;);
}

void shutdown( void )
{
	motor_stopAll();

	mainTimer_start();
	while( mainTimer_elapsed() < 2 ){}

	timer_stop_motor_timer();

	fpga_power_off();
	fpga_reset();

	io_power_off();

	set_power_ok_low();

	printf( "Shutdown\n\r" );
	
	//enter Power-Down Mode
	setBit( PCON, 1 );
	
	printf( "This should never be visible\n\r" );
}

int main (void)
{
	unsigned char sus_s3 = 0;
	unsigned char sus_s4 = 0;
	unsigned char sus_s5 = 0;
	unsigned int power_pressed_counter = 0;
	unsigned int power_off_state = 0;
	unsigned int last_com_rx_counter = 0;
	unsigned char com_active = 0;

	const unsigned int sX_counter_max_default = 10;
	unsigned int sX_counter_max = 300;
	unsigned int sX_counter = 0;

#define com_inactive_counter_max 3
	unsigned int com_inactive_counter = com_inactive_counter_max;

	unsigned int config_reset_button_pressed_counter = 0;
	unsigned int config_reset_idle_counter = 0;

	unsigned int charger_battery_low_shutdown_counter = 0;
	const int charger_battery_low_shutdown_counter_max = 200;
	
	unsigned char hasGyrosope = 1;
	unsigned char hwVersion = 0;
	
	PINSEL10 = 0;                                      /* Disable ETM interface */

	clearBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );

	WDTC = 20000000; //20s
	WDMOD = 3; // Set WDEN and WDRESET bits
	WDFEED = 0xAA;
	WDFEED = 0x55;

	//Brown-Out Global Disable
	//setBit( PCON, 3 );

	io_init();
	
	if( io_version0() )
	{
		hwVersion = 1;
	}
	
	printf( "\n\rHardware version %d\n\r", hwVersion );
	
	if( hwVersion > 0 )
	{
		hasGyrosope = 0;
	}

	init_serial();

	timer_init();
	
	printf( "Robotino 3 firmware version " );
	sendline( versionstr );
	
	printf( "Init fpga ... " );
	fpga_init();
	printf( "Done\n\r" );
	if( 0 == fpga_done() )
	{
		printf( "Waiting 500ms\n\r" );
		while( mainTimer_elapsed() < 500 )
		{
			//printf("Watchdog %u\n\r", WDTV );
		}
		printf( "Done\n\r" );
	}

	power_init();
	set_power_ok_high();

	if( hasGyrosope ) gyro_init();

	odometry_init();

	spi_init();

	fpga_configure();

	if( 0 == fpga_done() )
	{
		printf( "Configuration of FPGA failed\n\r" );
		//shutdown();
	}

	fpga_power_on();

	adc_init();

	motor_init();

	charger_init();

	init_TcpNet();
	myudp_init();

	timer_start_motor_timer();
	tickTimer_start();

	printf( "\n\rRobotino 3 firmware version " );
	sendline( versionstr );

	com_init();

	WDTC = 100000; //100ms

	led_set_pattern( LED_SINUS );

	for( ;; )
	{
		if( mainTimer_elapsed() >= 100 )
		{
			mainTimer_start();
			
			timer_tick();

			//update_gyro_calibration();

			if( 0 == charger_battery_low_count() )
			{
				io_set_pwrbtn_low();
				++charger_battery_low_shutdown_counter;
			}

			if( charger_battery_low_shutdown_counter > 0 )
			{
				if( 0 == charger_battery_low_shutdown_counter%10 )
				{
					printf("Battery low. Hard shutdown in %ds\n\r", charger_battery_low_shutdown_counter_max - charger_battery_low_shutdown_counter );
				}
				++charger_battery_low_shutdown_counter;
				if( charger_battery_low_shutdown_counter == charger_battery_low_shutdown_counter_max )
				{
					shutdown();
				}
			}


			if( config_reset_idle_counter > 0 )
			{
				--config_reset_idle_counter;
			}
			else
			{
				if( io_config_reset_button_pressed() )
				{
					config_reset_button_pressed_counter++;
					fpga_toggle_led( 1 );
				}
				else
				{
					config_reset_button_pressed_counter = 0;
					fpga_led_off( 1 );
				}
			}

			if( config_reset_button_pressed_counter > 20 )
			{
				config_reset_button_pressed_counter = 0;
				config_reset_idle_counter = 100;
				config_reset_signal = 1;
			}

			fpga_toggle_led( 0 );

			if( emergency_bumper_counter > 0 )
			{
				printf( "emergency bumper %u\n\r", emergency_bumper_counter );
				--emergency_bumper_counter;
			}

			if( sX_counter_max != sX_counter_max_default && com_rx_counter > 0 )
			{
				printf("Setting sX_counter_max to %u\n\r", sX_counter_max_default );
				sX_counter_max = sX_counter_max_default;
			}

			if( io_sus_s3() != sus_s3
				|| io_sus_s4() != sus_s4
				|| io_sus_s5() != sus_s5 )
			{
				printf( "S3:%d S4:%d S5:%d\n\r", (int)io_sus_s3(), (int)io_sus_s4(), (int)io_sus_s5() );
				sus_s3 = io_sus_s3();
				sus_s4 = io_sus_s4();
				sus_s5 = io_sus_s5();
			}

			if( 1 == sus_s3
				&& 1 == sus_s4 
				&& 1 == sus_s5 )
			{
				sX_counter = 0;
			}

			if( 0 == sus_s3 )
			{
				++sX_counter;
				printf( "Entered S3 count %d\n\r", sX_counter );
			}
			if( 0 == sus_s4 )
			{
				++sX_counter;
				printf( "Entered S4 count %d\n\r", sX_counter );
			}
			if( 0 == sus_s5 )
			{
				++sX_counter;
				printf( "Entered S5 count %d\n\r", sX_counter );
			}

			if( sX_counter > sX_counter_max )
			{
				shutdown();
			}

			switch( power_off_state )
			{
			case 0:
				if( cover_pwr_btn_is_pressed() )
				{
					++power_pressed_counter;
				}
				else
				{
					power_pressed_counter = 0;
				}
				if( power_pressed_counter > 20 )
				{
					io_set_pwrbtn_low();
					printf( "Going into power off state 1\n\r" );
					power_pressed_counter = 0;
					power_off_state = 1;
				}
				break;

			case 1:
				led_set_pattern( LED_DOWN );
				++power_pressed_counter;

				if( power_pressed_counter > 200 )
				{
					printf( "Does not go into S3. Hard power off\n\r" );
					shutdown();
				}
				break;

			default:
				break;
			}

			if( last_com_rx_counter != com_rx_counter )
			{
				last_com_rx_counter = com_rx_counter;
				com_inactive_counter = 0;
			}
			else
			{
				++com_inactive_counter;
			}

			if( com_inactive_counter > com_inactive_counter_max )
			{
				com_active = 0;
				motor_stopAll();
			}
			else
			{
				com_active = 1;
			}

			if( 0 == power_off_state )
			{
				if( config_reset_idle_counter > 0 )
				{
					led_set_pattern( LED_RECT );
				}
				else
				{
					if( com_active )
					{
						led_set_pattern( LED_FAST_SINUS );
					}
					else
					{
						led_set_pattern( LED_SINUS );
					}
				}
			}
		}

		main_TcpNet();

		com_update();
		adc_update();
		if( hasGyrosope ) gyro_update();
		charger_update();
		udp_update();

		if( emergency_bumper && io_bumper() )
		{
			motor_stopAll();
			emergency_bumper_counter = 20;
		}

		watchdog_feed();
	}
}
