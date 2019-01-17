/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC23xx with MCB2300
 * Version: V1.00
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#include <LPC23xx.H>                             /* LPC23xx definitions */
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "clk.h"
#include "CAN.h"                                 /* LPC23xx CAN adaption layer */

extern void wait_ms( unsigned int msecs );

unsigned int can_error_count = 0;

#define RBUF_SIZE   8      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

#if RBUF_SIZE < 2
#error RBUF_SIZE is too small.  It must be larger than 1.
#elif ((RBUF_SIZE & (RBUF_SIZE-1)) != 0)
#error RBUF_SIZE must be a power of 2.
#endif

struct can_buf_st {
  unsigned int in;          /* Next In Index */
  unsigned int out;         /* Next Out Index */
  CAN_msg buf [RBUF_SIZE];     /* Buffer */
};

static struct can_buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))

CAN_msg       CAN_TxMsg;                      /* CAN message for sending */                              

unsigned int  CAN_TxRdy = 0;              /* CAN HW ready to transmit a message */


/* Values of bit time register for different baudrates
   NT = Nominal bit time = TSEG1 + TSEG2 + 3
   SP = Sample point     = ((TSEG2 +1) / (TSEG1 + TSEG2 + 3)) * 100%
                                            SAM,  SJW, TSEG1, TSEG2, NT,  SP */
const uint32_t CAN_BIT_TIME[] = {          0, /*             not used             */
                                           0, /*             not used             */
                                           0, /*             not used             */
                                           0, /*             not used             */
                                  0x0001C000, /* 0+1,  3+1,   1+1,   0+1,  4, 75% */
                                           0, /*             not used             */
                                  0x0012C000, /* 0+1,  3+1,   2+1,   1+1,  6, 67% */
                                           0, /*             not used             */
                                  0x0023C000, /* 0+1,  3+1,   3+1,   2+1,  8, 63% */
                                           0, /*             not used             */
                                  0x0025C000, /* 0+1,  3+1,   5+1,   2+1, 10, 70% */
                                           0, /*             not used             */
                                  0x0036C000, /* 0+1,  3+1,   6+1,   3+1, 12, 67% */
                                           0, /*             not used             */
                                           0, /*             not used             */
                                  0x0048C000, /* 0+1,  3+1,   8+1,   4+1, 15, 67% */
                                  0x0049C000, /* 0+1,  3+1,   9+1,   4+1, 16, 69% */
                                };


/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
static void CAN_ISR (void) __irq;

/*----------------------------------------------------------------------------
  configure the requested baudrate.  CAN controller (1..2)
 *----------------------------------------------------------------------------*/
static void CAN_cfgBaudrate (uint32_t baudrate)  {
  CAN_TypeDef *pCAN = CAN1;
  unsigned long PCLK = CAN1_CLK;
  uint32_t result = 0;
  uint32_t nominal_time;

  /* Determine which nominal time to use for PCLK and baudrate */
  if (baudrate <= 500000)  {
    nominal_time = 12;
  }  else if (((PCLK / 1000000) % 15) == 0)  {
    nominal_time = 15;
  }  else if (((PCLK / 1000000) % 16) == 0)  {
    nominal_time = 16;
  }  else if (((PCLK / 1000000) % 12) == 0)  {
    nominal_time = 12;
  }  else if (((PCLK / 1000000) % 10) == 0)  {
    nominal_time = 10;
  }  else if (((PCLK / 1000000) %  8) == 0)  {
    nominal_time =  8;
  }  else if (((PCLK / 1000000) %  6) == 0)  {
    nominal_time =  6;
  }  else if (((PCLK / 1000000) %  4) == 0)  {
    nominal_time =  4;
  }  else  {
    nominal_time = 10;
  }

  /* Prepare value appropriate for bit time register */
  result  = (PCLK / nominal_time) / baudrate - 1;
  result &= 0x000003FF;
  result |= CAN_BIT_TIME[nominal_time];

  pCAN->BTR  = result;                           /* Set bit timing */
}


/*----------------------------------------------------------------------------
  setup CAN interface.  CAN controller (1..2)
 *----------------------------------------------------------------------------*/
void CAN_setup (uint32_t baudrate)
{
  CAN_TypeDef *pCAN = CAN1;
    
	PINSEL1 |= 0x3C00; //P0.21 is RD1 and P0.22 is TD1

  *(&VICVectAddr23) = (unsigned long) CAN_ISR; /* Set interrupt vector for CAN */
  *(&VICVectCntl23) = 1;
    
  VICIntEnable = (1 << 23);                    /* Enable CAN Interrupt */


  CANAF->AFMR = 2;                               /* By default filter is not used */
  pCAN->MOD   = 1;                               /* Enter reset mode */
  pCAN->IER   = 0;                               /* Disable all interrupts */
  pCAN->GSR   = 0;                               /* Clear status register */
  CAN_cfgBaudrate(baudrate);                 /* Set bit timing */
  pCAN->IER   = 0x0003;                          /* Enable Tx and Rx interrupt */
}


/*----------------------------------------------------------------------------
  leave initialisation mode.  CAN controller (1..2)
 *----------------------------------------------------------------------------*/
void CAN_start (void)  {
  CAN_TypeDef *pCAN = CAN1;

  pCAN->MOD = 0;                                 /* Enter normal operating mode */
}

/*----------------------------------------------------------------------------
  check if transmit mailbox is empty
 *----------------------------------------------------------------------------*/
void CAN_waitReady () {
  CAN_TypeDef *pCAN = CAN1;

  while ((pCAN->SR & (1<<2)) == 0);              /* Transmitter ready for transmission */
  CAN_TxRdy = 1;
}

unsigned char CAN_error(void)
{
	CAN_TypeDef *pCAN = CAN1;
	return bitIsSet(pCAN->GSR,6);
}

void CAN_clear_error(void)  {
  CAN_TypeDef *pCAN = CAN1;
	
	if( bitIsSet(pCAN->GSR,6)) //Bus error
	{
		/*
		The Tx and Rx error counters can be written if RM in CANxMOD is 1. Writing 255 to the
	Tx Error Counter forces the CAN Controller to Bus-Off state. If Bus-Off (BS in CANxSR) is
	1, writing any value 0 through 254 to the Tx Error Counter clears Bus-Off. When software
	clears RM in CANxMOD thereafter, only one Bus Free condition (11 consecutive
	recessive bits) is needed before operation resumes.
		*/
		
		//printf("Clear CAN error\n\r");

		pCAN->MOD = 1;
		//printf("  MOD =  0x%X\n\r", pCAN->MOD );
		pCAN->GSR = 0xFF000000;
		//printf("  GSR =  0x%X\n\r", pCAN->GSR );
		pCAN->GSR = 0;
		//printf("  GSR =  0x%X\n\r", pCAN->GSR );
		pCAN->MOD = 0;
	}
}

/*----------------------------------------------------------------------------
  wite a message to CAN peripheral and transmit it.  CAN controller (1..2)
 *----------------------------------------------------------------------------*/
void CAN_wrMsg (CAN_msg *msg)  {
  CAN_TypeDef *pCAN = CAN1;
  uint32_t CANData;

  CANData = (((uint32_t) msg->len) << 16)     & 0x000F0000 | 
            (msg->format == EXTENDED_FORMAT ) * 0x80000000 |
            (msg->type   == REMOTE_FRAME)     * 0x40000000;

  if (pCAN->SR & (1<<2))  {                      /* Transmit buffer 1 free */
    pCAN->TFI1  = CANData;                       /* Write frame informations */
    pCAN->TID1 = msg->id;                        /* Write CAN message identifier */
    pCAN->TDA1 = *(uint32_t *) &msg->data[0];    /* Write first 4 data bytes */
    pCAN->TDB1 = *(uint32_t *) &msg->data[4];    /* Write second 4 data bytes */
    //pCAN->CMR  = 0x31;                           /* Select Tx1 for Self Tx/Rx */
    pCAN->CMR  = 0x21;                           /* Start transmission without loop-back */
  }
}

/*----------------------------------------------------------------------------
  setup acceptance filter.  CAN controller (1..2)
 *----------------------------------------------------------------------------*/
void CAN_wrFilter (uint32_t id, uint8_t format)  {
  static int CAN_std_cnt = 0;
  static int CAN_ext_cnt = 0;
         uint32_t buf0, buf1;
         int cnt1, cnt2, bound1;

  /* Acceptance Filter Memory full */
  if ((((CAN_std_cnt + 1) >> 1) + CAN_ext_cnt) >= 512)
    return;                                       /* error: objects full */

  /* Setup Acceptance Filter Configuration 
    Acceptance Filter Mode Register = Off  */                                 
  CANAF->AFMR = 0x00000001;

  if (format == STANDARD_FORMAT)  {              /* Add mask for standard identifiers */
    //id |= (ctrl-1) << 13;                        /* Add controller number */
    id &= 0x0000F7FF;                            /* Mask out 16-bits of ID */

    /* Move all remaining extended mask entries one place up                 
       if new entry will increase standard ID filters list   */
    if ((CAN_std_cnt & 0x0001) == 0 && CAN_ext_cnt != 0) {
      cnt1   = (CAN_std_cnt >> 1);
      bound1 = CAN_ext_cnt;
      buf0   = CANAF_RAM->mask[cnt1];
      while (bound1--)  {
        cnt1++;
        buf1 = CANAF_RAM->mask[cnt1];
        CANAF_RAM->mask[cnt1] = buf0;
        buf0 = buf1;
      }        
    }

    if (CAN_std_cnt == 0)  {                     /* For entering first  ID */
      CANAF_RAM->mask[0] = 0x0000FFFF | (id << 16);
    }  else if (CAN_std_cnt == 1)  {             /* For entering second ID */
      if ((CANAF_RAM->mask[0] >> 16) > id)
        CANAF_RAM->mask[0] = (CANAF_RAM->mask[0] >> 16) | (id << 16);
      else
        CANAF_RAM->mask[0] = (CANAF_RAM->mask[0] & 0xFFFF0000) | id;
    }  else  {
      /* Find where to insert new ID */
      cnt1 = 0;
      cnt2 = CAN_std_cnt;
      bound1 = (CAN_std_cnt - 1) >> 1;
      while (cnt1 <= bound1)  {                  /* Loop through standard existing IDs */
        if ((CANAF_RAM->mask[cnt1] >> 16) > id)  {
          cnt2 = cnt1 * 2;
          break;
        }
        if ((CANAF_RAM->mask[cnt1] & 0x0000FFFF) > id)  {
          cnt2 = cnt1 * 2 + 1;
          break;
        }
        cnt1++;                                  /* cnt1 = U32 where to insert new ID */
      }                                          /* cnt2 = U16 where to insert new ID */

      if (cnt1 > bound1)  {                      /* Adding ID as last entry */
        if ((CAN_std_cnt & 0x0001) == 0)         /* Even number of IDs exists */
          CANAF_RAM->mask[cnt1]  = 0x0000FFFF | (id << 16);
        else                                     /* Odd  number of IDs exists */
          CANAF_RAM->mask[cnt1]  = (CANAF_RAM->mask[cnt1] & 0xFFFF0000) | id;
      }  else  {
        buf0 = CANAF_RAM->mask[cnt1];            /* Remember current entry */
        if ((cnt2 & 0x0001) == 0)                /* Insert new mask to even address */
          buf1 = (id << 16) | (buf0 >> 16);
        else                                     /* Insert new mask to odd  address */
          buf1 = (buf0 & 0xFFFF0000) | id;
     
        CANAF_RAM->mask[cnt1] = buf1;            /* Insert mask */

        bound1 = CAN_std_cnt >> 1;
        /* Move all remaining standard mask entries one place up */
        while (cnt1 < bound1)  {
          cnt1++;
          buf1  = CANAF_RAM->mask[cnt1];
          CANAF_RAM->mask[cnt1] = (buf1 >> 16) | (buf0 << 16);
          buf0  = buf1;
        }

        if ((CAN_std_cnt & 0x0001) == 0)         /* Even number of IDs exists */
          CANAF_RAM->mask[cnt1] = (CANAF_RAM->mask[cnt1] & 0xFFFF0000) | (0x0000FFFF);
      }
    }
    CAN_std_cnt++;
  }  else  {                                     /* Add mask for extended identifiers */
    //id |= (ctrl-1) << 29;                        /* Add controller number */

    cnt1 = ((CAN_std_cnt + 1) >> 1);
    cnt2 = 0;
    while (cnt2 < CAN_ext_cnt)  {                /* Loop through extended existing masks */
      if (CANAF_RAM->mask[cnt1] > id)
        break;
      cnt1++;                                    /* cnt1 = U32 where to insert new mask */
      cnt2++;
    }

    buf0 = CANAF_RAM->mask[cnt1];                /* Remember current entry */
    CANAF_RAM->mask[cnt1] = id;                  /* Insert mask */

    CAN_ext_cnt++;

    bound1 = CAN_ext_cnt - 1;
    /* Move all remaining extended mask entries one place up */
    while (cnt2 < bound1)  {
      cnt1++;
      cnt2++;
      buf1 = CANAF_RAM->mask[cnt1];
      CANAF_RAM->mask[cnt1] = buf0;
      buf0 = buf1;
    }        
  }
  
  /* Calculate std ID start address (buf0) and ext ID start address (buf1) */
  buf0 = ((CAN_std_cnt + 1) >> 1) << 2;
  buf1 = buf0 + (CAN_ext_cnt << 2);

  /* Setup acceptance filter pointers */
  CANAF->SFF_sa     = 0;
  CANAF->SFF_GRP_sa = buf0;
  CANAF->EFF_sa     = buf0;
  CANAF->EFF_GRP_sa = buf1;
  CANAF->ENDofTable = buf1;

  CANAF->AFMR = 0x00000000;                      /* Use acceptance filter */
}

void CAN_printMsg (CAN_msg *msg)
{
	unsigned char i;

	printf( "CAN message:\n\r" );
	printf( "id: 0x%02X  len: %d  format: %d  type: %d\n\r", msg->id, (int)msg->len, (int)msg->format, (int)msg->type ); 
	for( i=0; i<msg->len; ++i )
	{
		printf( "0x%02X ", msg->data[i] );
	}
	printf( "\n\r" );
}

void CAN_copyMsg( CAN_msg* dst, const CAN_msg* src )
{
	//memcpy( dst, src, sizeof( CAN_msg ) );

	dst->format   = src->format;
	dst->type     = src->type;
	dst->len      = src->len;

	dst->id = src->id;

	if (dst->type == DATA_FRAME)  {                /* Read the data if received message was DATA FRAME  */ 
		*(uint32_t *) &dst->data[0] = *(uint32_t *) &src->data[0];
		*(uint32_t *) &dst->data[4] = *(uint32_t *) &src->data[4];
	}
}

void CAN_rdMsg ()  {
  CAN_TypeDef *pCAN = CAN1;
  uint32_t CANData;
	struct can_buf_st *p;
	CAN_msg* dst;
	
	p = &rbuf;
		
	if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0)
	{
	 dst = &p->buf[p->in & (RBUF_SIZE-1)];
		
	 /* Read frame informations */
   CANData = pCAN->RFS;
   dst->format   = (CANData & 0x80000000) == 0x80000000;
   dst->type     = (CANData & 0x40000000) == 0x40000000;
   dst->len      = ((uint8_t)(CANData >> 16)) & 0x0F;

   dst->id = pCAN->RID;                           /* Read CAN message identifier */

   if (dst->type == DATA_FRAME)  {                /* Read the data if received message was DATA FRAME  */ 
     *(uint32_t *) &dst->data[0] = pCAN->RDA;
     *(uint32_t *) &dst->data[4] = pCAN->RDB;
   }
	
	  p->in++;
	}
}

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
static void CAN_ISR (void) __irq  {
  volatile uint32_t temp;

  if (CAN1->GSR & (1 << 0)) {                    /* CAN Controller #1 meassage is received */
		 
	CAN_rdMsg();                /*  read the message */
	CAN1->CMR = (1 << 2);                        /* Release receive buffer */
  }

  if (CAN1->GSR & (1 << 3)) {                    /* CAN Controller #1 meassage is transmitted */
	CAN_TxRdy = 1; 
  }

  temp = CAN1->ICR;                              /* clear interrupts */
  VICVectAddr = 0xFFFFFFFF;                      /* Acknowledge Interrupt */
}

int __swi(12) can_getmsg (CAN_msg* dst);
int __SWI_12              (CAN_msg* dst)
{
	CAN_msg* src;
  struct can_buf_st *p = &rbuf;

  if (SIO_RBUFLEN == 0)
    return (-1);

  src = &(p->buf [(p->out++) & (RBUF_SIZE - 1)]);
	
	CAN_copyMsg(dst,src);
	
	return SIO_RBUFLEN;
}

int getCanMsg(CAN_msg* dst)
{
	return can_getmsg(dst);
}
