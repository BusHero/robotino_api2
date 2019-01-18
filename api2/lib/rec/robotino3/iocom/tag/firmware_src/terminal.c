#include "util.h"
#include "charger.h"
#include <stdio.h>
#include <string.h>

extern int getkey (void);
extern int  sendchar(int ch);  /* in serial.c */

extern int uart3_putchar (int ch); //charger serial
extern int charger_isp_getkey (void);

extern unsigned int globalTime( void );

extern void io_nrst_high( void );
extern void io_nrst_low( void );
extern unsigned char io_nrst( void );
extern void io_set_boot( unsigned char num );
extern void io_clear_boot( void );
extern unsigned char io_boot( void );

extern void charger_isp_synchronize( void );

extern void wait_ms( unsigned int msecs );

extern void resetDevice( unsigned char enterUSBBootloader );

#define COMMAND_MAX_LENGTH 16
char command[COMMAND_MAX_LENGTH];
char commandIndex=0;

char echoOn=1;

enum
{
	NORMAL_MODE,
	CHARGER_MODE
};

void terminal_update(void)
{
	static char terminal_mode=NORMAL_MODE;
	int ch;
	int i;
	
	if(CHARGER_MODE==terminal_mode)
	{
		ch = charger_isp_getkey();
		while(ch!=-1)
		{
			sendchar(ch);
			ch = charger_isp_getkey();
		}
	}
		
	ch = getkey();
	
	if(-1 == ch) return;
	
	while( ch != -1 )
	{
		if(echoOn)
		{
			if('\n'==ch)
			{
				sendchar('\r');
				sendchar('\n');
			}
			else if('\r'==ch)
			{
			}
			else
			{
				sendchar(ch);
			}
		}
		
		if(ch != '\n' && ch != '\r')
		{
			command[commandIndex] = (char)ch;
			
			if(commandIndex<COMMAND_MAX_LENGTH-1)
			{
				commandIndex++;
			}
		}
		else
		{
			command[commandIndex]=0;
			commandIndex=0;
			break;
		}
		
		ch = getkey();
	}
	
	if(0==commandIndex)
	{
		if(0 == strncmp(command,"exit",4) )
		{
			printf("Leave charger mode\r\n");
			terminal_mode=NORMAL_MODE;
		}
		else if(CHARGER_MODE==terminal_mode)
		{
			for(i=0;i<COMMAND_MAX_LENGTH;++i)
			{
				if( command[i] > 0 )
				{
					uart3_putchar(command[i]);
				}
				else
				{
					break;
				}
			}
			if(i>0)
			{
				uart3_putchar('\n');
			}
		}
		else
		{
			if( 0 == strlen(command) )
			{
				printf("> ");
			}
			else if(0 == strncmp(command,"echo",4) )
			{
				if(0 == strncmp(command+5,"on",2) )
				{
					echoOn=1;
					printf("Echo is on\r\n");
				}
				else
				{
					echoOn=0;
					printf("Echo is off\r\n");
				}
			}
			else if(0 == strncmp(command,"time",4) )
			{
				printf("Time %ums\r\n",globalTime());
			}
			else if(0 == strncmp(command,"reset",5) )
			{
				resetDevice(0);
			}
			else if(0 == strncmp(command,"enterbl",7) )
			{
				resetDevice(1);
			}
			else if(0 == strncmp(command,"charger",6) )
			{
				printf("Enter charger mode\r\n");
				terminal_mode=CHARGER_MODE;
			}
			else if(0 == strncmp(command,"cver",4) )
			{
				for(i=0;i<3;++i)
				{
					printf("%d: %d.%d.%d\r\n", i+1, charger_version_array[i].major, charger_version_array[i].minor, charger_version_array[i].patch);
				}
			}
			else if(0 == strncmp(command,"cgver",4) )
			{
				charger_getVersion();
			}
			else if(0 == strncmp(command,"pbinfo",6))
			{
				i = command[7]-0x30;
				if(i>0 && i<4)
				{
					printf("Charger %d\r\n", i+1);
					printf("  time      %us\r\n", pb_charger_info_array[i].time);
					printf("  voltage   %.2fV\r\n", pb_charger_info_array[i].batteryVoltage);
					printf("  current   %.2fA\r\n", pb_charger_info_array[i].current);
					printf("  batt1temp %.2f%cC\r\n", pb_charger_info_array[i].battery1temp,248);
					printf("  batt2temp %.2f%cC\r\n", pb_charger_info_array[i].battery2temp,248);
					printf("  state      %d\r\n", pb_charger_info_array[i].state);
					printf("  %s\r\n",charger_error_message(i));
				}
				else
				{
					printf("Arg out of range\r\n");
				}
			}
			else if(0 == strncmp(command,"finfo",5))
			{
				printf("Festool charger\r\n");
				
				printf("  time              ");
				for(i=0;i<2;i++)
				{
					printf("|%ums", festool_charger_info.time[i]);
				}
				printf("\r\n");
				
				printf("  Akku Sel          ");
				for(i=0;i<2;i++)
				{
					printf("|%d",(int)festool_charger_info.akku_sel[i]);
				}
				printf("\r\n");
				
				printf("  Akku load         ");
				for(i=0;i<8;i++)
				{
					printf("|%d",(int)festool_charger_info.akku_load[i]);
				}
				printf("\r\n");
				
				printf("  Ext power         ");
				for(i=0;i<2;i++)
				{
					printf("|%u", festool_charger_info.ext_power[i]);
				}
				printf("\r\n");
				
				printf("  Charger present   ");
				for(i=0;i<2;i++)
				{
					printf("|%u", festool_charger_info.charger_present[i]);
				}
				printf("\r\n");
				
				printf("  MOS_ON_BATT1      ");
				for(i=0;i<2;i++)
				{
					printf("|%u", festool_charger_info.mos_on_batt1[i]);
				}
				printf("\r\n");
				
				printf("  Akku on           ");
				for(i=0;i<8;i++)
				{
					printf("|%d",(int)festool_charger_info.akku_on[i]);
				}
				printf("\r\n");
				
				printf("  BatLow           ");
				for(i=0;i<2;i++)
				{
					printf("|%u", festool_charger_info.isBatteryLow[i]);
				}
				printf("\r\n");

				for(i=0;i<8;++i)
				{
					printf("   Bat%d %.2fV -> %d%%\r\n",i,0.001f*festool_charger_info.batteryVoltage[i],(int)festool_charger_info.batteryCapacity[i]);
				}
			}
			else if(0 == strncmp(command,"crst",4) )
			{
				i = command[5]-0x30;
				switch(i)
				{
					case 0:
						io_nrst_low();
					break;
					
					case 1:
						io_nrst_high();
					break;
					
					default:
					printf("Invalid value\r\n");
					break;
				}
				wait_ms(1);
				printf("nrst %d\r\n",io_nrst());
			}
			else if(0 == strncmp(command,"cboot",5) )
			{
				i = command[6]-0x30;
				io_set_boot( i );
				wait_ms(1);
				printf("cboot %d\r\n",(int)io_boot());
			}
			else if(0 == strncmp(command,"cup",3) )
			{
				i = command[4]-0x30;
				if(i>0 && i<4)
				{
					printf("Update charger %d\r\n",i);
					charger_program(i);
				}
				else
				{
					printf("Arg out of range\r\n");
				}
			}
			else if(0 == strncmp(command,"hotswap",7) )
			{
				charger_startHotSwap();
			}
			else
			{
				printf("Commands\r\n");
				printf("  echo on|off   - Enable echo\r\n");
				printf("  time          - print time\r\n");
				printf("  reset         - reset device\r\n");
				printf("  enterbl       - enter bootloader\r\n");
				printf("  charger       - enter charger mode\r\n");
				printf("  exit          - leave charger mode\r\n");
				printf("  cver          - print charger FW version\r\n");
				printf("  cgver         - get charger FW version\r\n");
				printf("  pbinfo 1|2|3  - print PB charger info\r\n");
				printf("  finfo         - print Festool charger info\r\n");
				printf("  crst 0|1      - Set charger reset\r\n");
				printf("  cboot 0|1|2|3 - Set charger boot\r\n");
				printf("  cup 1|2|3     - Update charger\r\n");
				printf("  hotswap       - Hotswap accus\r\n");
			}
		}
	}
}
