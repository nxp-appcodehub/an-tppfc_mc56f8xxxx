/******************************************************************************
* 
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause
*                      
*
******************************************************************************* 
*
* @file      PFC_statemachine.h
*
* @author    
* 
* @version   1.0.0.0

* @date      Dec-02-2016
* 
* @brief     PFC state machine head
*
*******************************************************************************/

#ifndef PFC_STATEMACHINE_H_
#define PFC_STATEMACHINE_H_

/******************************************************************************
* Includes
******************************************************************************/

#include "state_machine.h"
#include "Cpu.h"
#include "peripherals.h"
#include "PFC_structure.h"
#include "PFC_Ctrl.h"
#include "spll_1ph_sogi.h"

/******************************************************************************
* Types
******************************************************************************/
typedef enum {
	SOFTSTART       = 0,
	NORMAL          = 1,
	LIGHTLOAD       = 2
} PFC_RUN_SUBSTATE_T;

/******************************************************************************
* Global variables
******************************************************************************/
extern SM_APP_CTRL_T           gsPFC_Ctrl;
extern PFC_STRUC_T             gsPFC_Drive;
extern PFC_RUN_SUBSTATE_T      gsPFC_Runsub;
__pmem extern const PFCN_VOID_VOID mPFC_STATE_RUN_TABLE[3];

extern SPLL_1PH_SOGI_T spll_obj;
extern uint16_t  gu16TimeBaseCnt, gu16MsgIntervalCnt;
extern uint16_t  gu16LLCRun, gu16LLCVprimOK, gu16LLCFaultID;
extern frac16_t  gf16SwRevDistoritonCur;
/******************************************************************************
* Global functions
******************************************************************************/

#endif /* PFC_STATEMACHINE_H_ */
