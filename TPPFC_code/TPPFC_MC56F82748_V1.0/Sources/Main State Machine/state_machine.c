/******************************************************************************
* 
* Copyright : 2016 Freescale Semiconductor, Inc.  
* SPDX-License-Identifier: BSD-3-Clause                       
*
******************************************************************************* 
*
* @file      state_machine.c
*
* @author    
* 
* @version   1.0.0.0

* @date      Dec-02-2016
* 
* @brief     state machine
*
*******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include "state_machine.h"

/******************************************************************************
* Constants
******************************************************************************/

/******************************************************************************
* Macros 
******************************************************************************/

/******************************************************************************
* Types
******************************************************************************/

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/

/******************************************************************************
* Local functions
******************************************************************************/

/*------------------------------------
 * Application state machine functions
 * ----------------------------------*/
static void SM_StateFault(SM_APP_CTRL_T *psAppCtrl);
static void SM_StateInit(SM_APP_CTRL_T *psAppCtrl);
static void SM_StateStop(SM_APP_CTRL_T *psAppCtrl);
static void SM_StateRun(SM_APP_CTRL_T *psAppCtrl);

/******************************************************************************
* Global functions
******************************************************************************/
/* State machine functions field (in pmem) */
__pmem const PFCN_VOID_PSM gSM_STATE_TABLE[4] = {SM_StateFault, SM_StateInit, SM_StateStop, SM_StateRun};


/***************************************************************************//*!
*
* @brief   FAULT state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void SM_StateFault(SM_APP_CTRL_T *psAppCtrl)
{
    /* User Fault function */
    psAppCtrl->psState->Fault();
    
    /* if clear fault command flag */
    if ((psAppCtrl->uiCtrl & SM_CTRL_FAULT_CLEAR) > 0 )
    {
	    /* Clear INIT_DONE, FAULT, FAULT_CLEAR flags */
	    psAppCtrl->uiCtrl = SM_CTRL_NONE;
	    //~(SM_CTRL_INIT_DONE | SM_CTRL_FAULT | SM_CTRL_FAULT_CLEAR);

        /* User Fault to Init transition function */
        psAppCtrl->psTrans->FaultInit();

		/* Init state */
		psAppCtrl->eState = INIT;
    }
}

/***************************************************************************//*!
*
* @brief   Init state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void SM_StateInit(SM_APP_CTRL_T *psAppCtrl)
{
    /* User Init function */
    psAppCtrl->psState->Init();

	/* if fault flag */
	if ((psAppCtrl->uiCtrl & SM_CTRL_FAULT) > 0)
	{
	    /* User Init to Fault transition function */
	    psAppCtrl->psTrans->InitFault();
	    
		/* Fault state */
		psAppCtrl->eState = FAULT;
	}
	/* if INIT_DONE flag */
	else if ((psAppCtrl->uiCtrl & SM_CTRL_INIT_DONE) > 0)
	{
	    /* Clear INIT_DONE, START_STOP, OM_CHANGE, STOP_ACK, RUN_ACK flags */
	    //psAppCtrl->uiCtrl &= ~(SM_CTRL_INIT_DONE | SM_CTRL_STOP | SM_CTRL_START | SM_CTRL_STOP_ACK | SM_CTRL_RUN_ACK);
		psAppCtrl->uiCtrl &= ~(SM_CTRL_INIT_DONE);
 		/* User Init to Stop transition function */
	    psAppCtrl->psTrans->InitStop();
	    //psAppCtrl->psTrans->InitRun();
		/* Stop state */
		psAppCtrl->eState = STOP;
		//psAppCtrl->eState = RUN;
	}
}

/***************************************************************************//*!
*
* @brief   STOP state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void SM_StateStop(SM_APP_CTRL_T *psAppCtrl)
{
    /* User Stop function */
    psAppCtrl->psState->Stop();

    /* if fault */
    if ((psAppCtrl->uiCtrl & SM_CTRL_FAULT) > 0)
    {
	    /* User Stop to Fault transition function */
	    psAppCtrl->psTrans->StopFault();

		/* Fault state */
		psAppCtrl->eState = FAULT;	
    }
	else if ((psAppCtrl->uiCtrl & SM_CTRL_START) > 0)
	{
		/* User Stop to Run transition function, user must set up the SM_CTRL_RUN_ACK
		flag to allow the RUN state */
		psAppCtrl->psTrans->StopRun();
		
		/* Clears the START command */
//		psAppCtrl->uiCtrl &= ~SM_CTRL_START;		

		if ((psAppCtrl->uiCtrl & SM_CTRL_RUN_ACK) > 0)
		{
			/* Clears the RUN_ACK flag */
			psAppCtrl->uiCtrl &= ~(SM_CTRL_RUN_ACK | SM_CTRL_START);
			
			/* Run state */
			psAppCtrl->eState = RUN;	
		}
	}
}

/***************************************************************************//*!
*
* @brief   RUN state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void SM_StateRun(SM_APP_CTRL_T *psAppCtrl)
{
    psAppCtrl->psState->Run();


	if ((psAppCtrl->uiCtrl & SM_CTRL_FAULT) > 0)
    {
	    /* User Run to Fault transition function */
	    psAppCtrl->psTrans->RunFault();

		/* Fault state */
		psAppCtrl->eState = FAULT;	
    }
    else if ((psAppCtrl->uiCtrl & SM_CTRL_STOP) > 0)
	{
		/* User Run to Stop transition function, user must set up the SM_CTRL_STOP_ACK
		flag to allow the STOP state */
		psAppCtrl->psTrans->RunStop();
		
		/* Clears the STOP command */
//		psAppCtrl->uiCtrl &= ~SM_CTRL_STOP;

		if ((psAppCtrl->uiCtrl & SM_CTRL_STOP_ACK) > 0)
		{
			/* Clears the STOP_ACK flag */
			psAppCtrl->uiCtrl &= ~(SM_CTRL_STOP_ACK | SM_CTRL_STOP);
			
			/* Run state */
			psAppCtrl->eState = STOP;	
		}
	}
}

/******************************************************************************
* Inline functions
******************************************************************************/


