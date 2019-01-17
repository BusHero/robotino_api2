#include "can.h"

#include "stm32f10x_can.h"

#include <inttypes.h>
#include <string.h>

static CanRxMsg rxMsg;
static CanTxMsg txMsg;

extern void new_dac_value( unsigned short value );

void CAN1_RX0_Istr(void)
{
	CAN_Receive( CAN1, CAN_FIFO0, &rxMsg );
	if ( rxMsg.StdId == 0x0000 && rxMsg.DLC == 2 )
	{
		unsigned short dac_value = 0;
		memcpy( &dac_value, rxMsg.Data, sizeof( dac_value ) );
		dac_value %= 0x400;
		new_dac_value( dac_value );
	}
	else
	{
		int i;
		printf( "CAN msg rx: " );
		printf( "ID 0x%04x Length %u Data", rxMsg.StdId, rxMsg.DLC );
		for( i = 0; i < rxMsg.DLC; ++i )
			printf( " 0x%02x", rxMsg.Data[i] );
		printf( "\n\r" );
	}
}

void can_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_0 );
  
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	/* GPIO clock enable */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE );

	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	/* CANx Periph clock enable */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1, ENABLE );

	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit( &CAN_InitStructure );

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

	/* CAN Baudrate = 100kBps*/
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStructure.CAN_Prescaler = 40; // change to 4 for 1MBps
	CAN_Init( CAN1, &CAN_InitStructure );

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_ITConfig( CAN1, CAN_IT_FMP0 | CAN_IT_FMP1, ENABLE );
	
	/* Transmit */
	txMsg.StdId = 0x0001;
	txMsg.ExtId = 0x01;
	txMsg.RTR = CAN_RTR_DATA;
	txMsg.IDE = CAN_ID_STD;
}

void can_send( unsigned char length, unsigned char* data )
{
	int i;
	if ( length == 0 )
		return;
	if ( length > 8 )
		length = 8;
	txMsg.DLC = length;
	for( i = 0; i < length; ++i )
		txMsg.Data[i] = data[i];
	for( ; i < 8; ++i )
		txMsg.Data[i] = 0;
	CAN_Transmit( CAN1, &txMsg );
}
