/*
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <stdio.h>
#include <stdlib.h>
#include "Cpu.h"
#include "derivative.h" /* include peripheral declarations */

#include "freemaster.h"
#include "PFC_statemachine.h"

//-------------------- Global variables ------------------------
extern uint16_t vba_vector_addr;
uint16_t gu16MsgIntervalCnt=0;

//-------------------- Local variables ------------------------
uint8_t  RxMsg_num=0, RxMsg_head, RxMsg_1, RxMsg_2, RxMsg_3, RxMsg_4, RxMsg_5, RxMsg_6, RxMsg_7, RxMsg_8, RxMsg_checksum;
uint8_t  LLC_State;
short    LLCVout, LLCIout;

void main(void)
{
   
	uint16_t i,j;

	asm(nop); // for loop is not allowed as the first instruction
			
	/* delay for steady power supply, avoid false hardware over-voltage protection */
	for(i=2500;i>0;i--)
	{
		for(j=500;j>0;j--){asm(nop);}
	}
	
	INTC->VBA = (uint16_t)(&vba_vector_addr);
	
	Peripheral_init();
	FMSTR_Init();
  
	__EI(0); // enable interrupts of all priorities

    while(1) 
    {
    	FMSTR_Poll();

#if COMMUNICATION_EN
    	/* If the message time interval is too long, restart to verify from the message head */
    	if(gu16MsgIntervalCnt >MESSAGE_MAX_INTERVAL)   RxMsg_num = 0;
    	    	
    	/* message received from second end, two kind of message can be identified:  
    	*the LLC state, faultId, VOUT and IOUT are transmitted about every 200ms: head=0x15 
    	*LLC received control command confirmation: head=0x12
    	* */
    	if(QSCI0->STAT & QSCI_STAT_RDRF_MASK) /* receiver full */
    	{
    	    switch(RxMsg_num)
    	    {
    	    	case 0: /* message head, cooperate with the message defined in LLC, 0x12&0x15 is the effective message head */
    	    	{
    	    		RxMsg_head = QSCI0->DATA;
    	    		if(RxMsg_head == 0x12 || RxMsg_head == 0x15) 
    	    		{ 
    	    			RxMsg_num ++;  
    	    		  	gu16MsgIntervalCnt = 0;
    	    		}
    	    		break;
    	    	}
    	    	case 1: /* read message and calculate the checksum */
    	    	{
    	    		RxMsg_1 = QSCI0->DATA;
    	    		RxMsg_checksum = RxMsg_head ^ RxMsg_1;
    	    		RxMsg_num++; 
    	    		gu16MsgIntervalCnt = 0;
    	    		break;
    	    	}
    	    	case 2:
    	    	{
    	    		RxMsg_2 = QSCI0->DATA; 
    	    		  		
    	    		/* when msg_head = 0x12, the length of the message is 3, message complete */   		  		   
    	    		if(RxMsg_head == 0x12) 
    	    		{
    	    			RxMsg_num = 0;
    	    		  	if(RxMsg_checksum == RxMsg_2) /* check if the received data are right, if no, discard it */
    	    		  	{
    	    		  		if(RxMsg_1 == 0x01) /* it informs PFC the output voltage is acceptable and LLC works now */  
    	    		  		{
    	    		  		    gu16LLCVprimOK = 1;
    	    		  		    gu16LLCRun = 1;
    	    		  		} 
    	    		  		else if(RxMsg_1 == 0x03) /* it informs PFC the LLC is stop */
    	    		  		{
    	    		  		    gu16LLCRun = 0;
    	    		  		}
    	    		  	}
    	    		}
    	    		else /* when msg_head = 0x15, more messages will be sent */
    	    		{ 
    	    			RxMsg_num++; 
    	    		  	RxMsg_checksum ^= RxMsg_2; 
    	    		  	gu16MsgIntervalCnt = 0;
    	    		 } 
    	    		 break;
    	    	}
    	    	case 3:  {RxMsg_3 = QSCI0->DATA; RxMsg_checksum ^= RxMsg_3; RxMsg_num++; gu16MsgIntervalCnt = 0; break;}
    	    	case 4:  {RxMsg_4 = QSCI0->DATA; RxMsg_checksum ^= RxMsg_4; RxMsg_num++; gu16MsgIntervalCnt = 0; break;}
    	    	case 5:  {RxMsg_5 = QSCI0->DATA; RxMsg_checksum ^= RxMsg_5; RxMsg_num++; gu16MsgIntervalCnt = 0; break;}
    	    	case 6:  {RxMsg_6 = QSCI0->DATA; RxMsg_checksum ^= RxMsg_6; RxMsg_num++; gu16MsgIntervalCnt = 0; break;}
    	    	case 7:  {RxMsg_7 = QSCI0->DATA; RxMsg_checksum ^= RxMsg_7; RxMsg_num++; gu16MsgIntervalCnt = 0; break;}
    	    	default: 
    	    	{
    	    		RxMsg_8 = QSCI0->DATA;
    	    		RxMsg_num = 0;
    	    		  		
    	    		/* when msg_head = 0x15, the length of the message is 9, it transfer LLC states to PFC */
    	    		if(RxMsg_checksum == RxMsg_8)
    	    		{
    	    		  	LLC_State = RxMsg_1;
    	    		  	gu16LLCFaultID = (uint16_t)RxMsg_2<<8 | RxMsg_3;
    	    		  	LLCVout = (uint16_t)RxMsg_4<<8 | RxMsg_5;
    	    		  	LLCIout = (uint16_t)RxMsg_6<<8 | RxMsg_7;
    	    		}
    	    		break;
    	    	}
    	    }		
    	}
#endif
    	
    	/* Call state-machine */
    	SM_StateMachine(&gsPFC_Ctrl); //PFC state machine
    }
}
